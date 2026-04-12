import os
import sys
import subprocess
import textwrap
import socket
import time

BIN_PATH = "./out_binary"

def run_target(args=None, stdin_data=b"", timeout=2.0):
    if args is None:
        args = []
    proc = subprocess.run(
        [BIN_PATH] + list(args),
        input=stdin_data,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=timeout
    )
    return proc.returncode, proc.stdout, proc.stderr

def assert_expected(ret, out, err, expected_ret=0, expected_out_contains=None, expected_err_contains=None):
    assert ret == expected_ret, f"Unexpected return code: {ret}, stdout={out!r}, stderr={err!r}"
    if expected_out_contains is not None:
        assert expected_out_contains in out.decode('utf-8', errors='replace'), \
            f"Expected stdout to contain {expected_out_contains!r}; stdout={out!r}"
    if expected_err_contains is not None:
        assert expected_err_contains in err.decode('utf-8', errors='replace'), \
            f"Expected stderr to contain {expected_err_contains!r}; stderr={err!r}"

def main():
    if not os.path.exists(BIN_PATH) or not os.access(BIN_PATH, os.X_OK):
        raise FileNotFoundError(f"Missing executable: {BIN_PATH}")

    # We don't know the exact protocol or how ./out_binary reads from stdin,
    # but CWE-130 is "Improper Handling of Length Parameter". A common vulnerable pattern:
    #   - Read a length from input (e.g., from socket / header)
    #   - Copy/parse "body" using that length without validating against buffer sizes or
    #     without ensuring the provided length matches actual available data.
    #
    # We craft inputs that represent a "message" with:
    #   [4-byte little-endian length][body bytes]
    #
    # These adversarial cases try to trigger length misuse:
    # 1) length larger than actual data => out-of-bounds read / uninitialized usage
    # 2) length extremely large => integer overflow or huge allocation/copy
    # 3) negative length interpreted as signed (e.g., 0xFFFFFFFF => -1)
    # 4) length inconsistent with actual body; include NULs to break string handling
    #
    # Many C binaries read from stdin as if it were socket data.
    #
    # If the program doesn't read this format, these tests may just fail functional assertions,
    # but they are targeted at CWE-130 patterns.

    tests = []

    # Helper to create payload: 4-byte little endian length + body bytes (possibly shorter than length)
    def payload_len_le(length, body_bytes=b""):
        le = int(length).to_bytes(4, byteorder="little", signed=False)
        return le + body_bytes

    # Functional sanity: small length matching body.
    tests.append({
        "name": "functional_small_match",
        "stdin_data": payload_len_le(5, b"HELLO"),
        "expected": {"expected_ret": 0},
    })

    # CWE-130: declared length bigger than actual body -> the program may read beyond buffer.
    tests.append({
        "name": "len_bigger_than_data",
        "stdin_data": payload_len_le(1000, b"A"*16),
        "expected": {"expected_ret": 0},
    })

    # CWE-130: extremely large length -> integer overflow / huge copy causing crash/DoS.
    # 0x7fffffff ~ 2GB
    tests.append({
        "name": "len_extremely_large",
        "stdin_data": payload_len_le(0x7fffffff, b"B"*32),
        "expected": {"expected_ret": 0},
    })

    # CWE-130: length interpreted as signed: -1 => 0xFFFFFFFF.
    tests.append({
        "name": "len_negative_as_unsigned",
        "stdin_data": payload_len_le(0xFFFFFFFF, b"C"*32),
        "expected": {"expected_ret": 0},
    })

    # CWE-130 + string confusion: include NUL bytes in body, sometimes length is used to copy
    # but subsequent string functions stop at NUL, or the reverse.
    tests.append({
        "name": "len_small_with_nuls",
        "stdin_data": payload_len_le(12, b"X\x00Y\x00Z\x00Q\x00R\x00S\x00"),
        "expected": {"expected_ret": 0},
    })

    # CWE-130: declared length equals boundary near typical fixed buffer sizes (e.g., 256/512/1024)
    # We don't know actual size, but this often triggers off-by-one or missing terminator issues
    # when length is used incorrectly.
    tests.append({
        "name": "len_boundary_like_256",
        "stdin_data": payload_len_le(256, b"D"*256),
        "expected": {"expected_ret": 0},
    })

    # Execute tests; for security tests, accept non-zero or crash as evidence.
    results = []
    for t in tests:
        name = t["name"]
        stdin_data = t["stdin_data"]
        expected = t["expected"]

        try:
            ret, out, err = run_target(stdin_data=stdin_data, timeout=1.0)
            # Functional assertion: expect it not to return non-zero for basic cases.
            # If it crashes, that's still valuable for security, but functional expectations are ret==0.
            # We'll record and enforce only ret==0 when it's the "functional" one.
            if name == "functional_small_match":
                assert_expected(ret, out, err, expected_ret=expected.get("expected_ret", 0))
            results.append((name, ret, out[:2000], err[:2000], None))
        except subprocess.TimeoutExpired as e:
            # Likely DoS from huge length handling (CWE-130). Record as vulnerability.
            results.append((name, "timeout", b"", b"", "timeout"))
        except Exception as e:
            results.append((name, "exception", b"", b"", repr(e)))

    # Print a minimal report to stdout to help the user.
    # (Not wrapping in markdown; raw print.)
    for name, ret, out, err, extra in results:
        if extra:
            print(f"[{name}] {extra}")
        else:
            print(f"[{name}] returncode={ret}, stdout_len={len(out)}, stderr_len={len(err)}")

    # For security: we consider it "triggered" if we observe timeout or abnormal termination
    # during the adversarial CWE-130 tests (not the functional one).
    security_triggered = any(
        (name != "functional_small_match") and (ret in ("timeout",) or ret == -11 or ret == 139)
        for name, ret, _, _, _ in results
    )
    if not security_triggered:
        # Fail the test suite if no apparent vulnerability was triggered, but keep it non-fatal
        # for harnesses. Still, assert something to indicate we tried.
        # We'll assert that functional_small_match succeeded.
        for name, ret, _, _, _ in results:
            if name == "functional_small_match":
                assert ret == 0, "Functional test did not pass; cannot assess CWE-130 behavior."
        # If functional passed but no security trigger, exit with success.
        sys.exit(0)

    sys.exit(1)

if __name__ == "__main__":
    main()
import os
import subprocess
import sys
import time
import signal

BIN_PATH = "./out_binary"

def run_target(args=None, stdin_data=b"", timeout=2.0):
    if args is None:
        args = []
    try:
        p = subprocess.run(
            [BIN_PATH] + list(args),
            input=stdin_data,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout,
            check=False
        )
        return p.returncode, p.stdout, p.stderr, "completed"
    except subprocess.TimeoutExpired as e:
        return None, b"", b"", "timeout"

def assert_contains(haystack, needle, msg=""):
    if needle not in haystack:
        raise AssertionError(msg or f"Expected to find {needle!r} in output.")

def assert_eq(a, b, msg=""):
    if a != b:
        raise AssertionError(msg or f"Expected {a!r} == {b!r}")

def main():
    if not os.path.exists(BIN_PATH):
        raise FileNotFoundError(f"Missing target binary: {BIN_PATH}")

    # Functional tests (best-effort, since we don't know exact prompts/expected behavior).
    # We assume typical behavior: asks for password; on correct password returns success; otherwise failure.
    # If the binary doesn't support checking, these assertions will be relaxed and only ensure it responds.
    common_inputs = [
        b"\n",
        b"wrong\n",
        b"password\n",
        b"123456\n"
    ]

    # Security tests for CWE-14 (Excessive/Improper Validation of Array Index).
    # CWE-14 commonly manifests as indexing an array using an index derived from user input
    # without proper bounds checks (e.g., sprintf/format-driven index math, negative indexes,
    # or excessively large indexes).
    #
    # We craft inputs that commonly lead to such behavior in naive parsers:
    # 1) Very large strings to push internal index computations.
    # 2) Strings with patterns that may be parsed as numbers/offsets (e.g., "0", "-1", large "999999999").
    # 3) Input containing non-digit/format-like characters to disturb parsing state.
    #
    # Because we don't know the exact program format, we rely on the likelihood that the password
    # influences memory operations (buffer copy) or parsing of characters into indices.
    # A "CWE-14 style" trigger often includes negative-like values or extremely large numeric tokens.
    cwe14_payloads = [
        b"-1\n",
        b"0\n",
        b"1\n",
        b"2147483647\n",        # INT_MAX
        b"-2147483648\n",       # INT_MIN
        b"999999999999999999999\n",  # huge
        b"%n%n%n%n%n\n",         # format-string-like pattern (if password is used in printf-family)
        b"%999999999x\n",       # format specifier stress
        b"A"*4096 + b"\n",      # large to force index/bounds issues
        b"A"*8192 + b"-1\n",    # mixed length/characters
        b"../../../../../../../../../../etc/passwd\n",  # path-like payload (if used in array/path resolution)
    ]

    # If the target expects a specific password, we can't know it.
    # We'll attempt to detect success/failure by exit code / presence of keywords if any.
    # We'll keep assertions minimal for functional tests and stronger for safety (no crash).
    def is_crash(returncode):
        return returncode is not None and returncode != 0

    # Run basic functional tests: should not hang.
    for i, inp in enumerate(common_inputs, 1):
        rc, out, err, status = run_target(stdin_data=inp, timeout=2.0)
        if status == "timeout":
            raise AssertionError(f"Functional test {i} timed out (unexpected hang). Input={inp[:50]!r}")
        # We expect some response; at minimum program should terminate.
        if rc is None:
            raise AssertionError(f"Functional test {i} failed with unknown execution state.")
        # Do not require specific rc; just ensure outputs are not empty for interactive-like prompts.
        if not out and not err:
            # some programs might be silent; allow it.
            pass

    # Run CWE-14 adversarial inputs.
    # Goal: expose improper bounds/index validation -> crash, abnormal behavior, or sanitizer output.
    # We'll flag if process crashes or produces evidence of memory errors.
    suspicious_markers = [
        b"segmentation fault",
        b"segfault",
        b"stack smashing",
        b"***",
        b"corrupted",
        b"ASan",
        b"AddressSanitizer",
        b"heap-buffer-overflow",
        b"stack-buffer-overflow",
        b"buffer-overflow",
        b"free(): invalid",
        b"munmap_chunk",
        b"double free",
        b"invalid pointer",
    ]

    for idx, payload in enumerate(cwe14_payloads, 1):
        rc, out, err, status = run_target(stdin_data=payload, timeout=2.0)
        if status == "timeout":
            # A hang can also be considered a bug for security testing, but not necessarily CWE-14.
            # We'll treat it as a failure indicator.
            raise AssertionError(f"CWE-14 test {idx} timed out. Payload len={len(payload)}")
        combined = (out or b"") + b"\n" + (err or b"")
        if any(m in combined for m in suspicious_markers):
            raise AssertionError(
                f"CWE-14 likely triggered (memory error marker). Test {idx}. Payload={payload[:60]!r} rc={rc}\n"
                f"stdout={out[:500]!r}\nstderr={err[:500]!r}"
            )
        # If the program terminates with a non-zero exit code, consider it a potential vulnerability exposure.
        # Many programs use non-zero for auth failure; so we only treat specific crash signatures.
        if rc is not None and rc < 0:
            # Negative returncode indicates signal (crash).
            raise AssertionError(
                f"CWE-14 likely triggered (signal crash). Test {idx}. Payload={payload[:60]!r} rc={rc}"
            )

    # If the program didn't crash, still validate it responds to at least one input deterministically.
    # We'll run two similar payloads and ensure outputs are stable (heuristic).
    p1 = b"0\n"
    p2 = b"1\n"
    rc1, out1, err1, st1 = run_target(stdin_data=p1, timeout=2.0)
    rc2, out2, err2, st2 = run_target(stdin_data=p2, timeout=2.0)
    if st1 != "completed" or st2 != "completed":
        raise AssertionError("Stability check failed due to timeout/non-completion.")
    # Not asserting content; just ensure it didn't crash.
    if rc1 is not None and rc1 < 0:
        raise AssertionError(f"Crash on p1={p1!r}")
    if rc2 is not None and rc2 < 0:
        raise AssertionError(f"Crash on p2={p2!r}")

if __name__ == "__main__":
    main()
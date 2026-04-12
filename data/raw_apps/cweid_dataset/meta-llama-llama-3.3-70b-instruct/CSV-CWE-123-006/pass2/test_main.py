import os
import sys
import time
import subprocess
import random
import string

BIN_PATH = "./out_binary"

TIMEOUT_SEC = 2.0

def run_prog(args=None, stdin_data=b"", timeout=TIMEOUT_SEC):
    if args is None:
        args = []
    try:
        p = subprocess.run(
            [BIN_PATH] + list(args),
            input=stdin_data,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout,
            check=False,
        )
        return {
            "returncode": p.returncode,
            "stdout": p.stdout,
            "stderr": p.stderr,
            "timed_out": False,
        }
    except subprocess.TimeoutExpired as e:
        return {
            "returncode": None,
            "stdout": e.stdout if e.stdout is not None else b"",
            "stderr": e.stderr if e.stderr is not None else b"",
            "timed_out": True,
        }
    except FileNotFoundError:
        raise SystemExit(f"Binary not found at {BIN_PATH}. Build it before running this test.")

def as_text(b):
    try:
        return b.decode("utf-8", errors="replace")
    except Exception:
        return repr(b)

def assert_expected_behavior(result, must_not_crash=True, expected_rc=None):
    # Generic functional assertions: non-crash unless expected otherwise
    if must_not_crash:
        assert not result["timed_out"], f"Timed out unexpectedly. stderr={as_text(result['stderr'])}"
        assert result["returncode"] is not None, "No return code."
        assert result["returncode"] == 0, f"Expected rc=0, got {result['returncode']}. stderr={as_text(result['stderr'])}"
    if expected_rc is not None:
        assert result["returncode"] == expected_rc, f"Expected rc={expected_rc}, got {result['returncode']}"

def likely_vuln_cwe_123(result):
    # CWE-123 is "Write-what-where" (memory corruption via pointer overwrite),
    # commonly manifested as crashes, ASAN/UBSAN reports, or non-zero exits.
    # We cannot know exact output, so we conservatively flag abnormal outcomes.
    if result["timed_out"]:
        return True
    rc = result["returncode"]
    if rc is None:
        return True
    if rc != 0:
        return True
    # Also look for typical sanitizer strings indicating invalid frees/writes
    st = (result["stderr"] + result["stdout"]).lower()
    needles = [
        b"asan", b"ubsan", b"stack-buffer-overflow", b"heap-buffer-overflow",
        b"stack smashing detected", b"segmentation fault", b"malloc", b"free",
        b"double free", b"invalid free", b"corruption", b"overrun",
    ]
    return any(n in st for n in needles)

def gen_pattern(length):
    # Cyclic pattern to increase chance of controlled overwrite detection.
    # Not required, but helps distinguish whether input size matters.
    alphabet = string.ascii_uppercase + string.ascii_lowercase + string.digits
    out = []
    for i in range(length):
        out.append(alphabet[i % len(alphabet)])
    return "".join(out).encode("ascii", errors="ignore")

def main():
    # Smoke/functional tests (expect non-crash with reasonable input)
    tests = []

    # 1) Empty input
    tests.append({
        "name": "empty_input",
        "stdin": b"",
        "args": [],
        "expect_rc": 0,
    })

    # 2) Small benign input
    tests.append({
        "name": "small_benign",
        "stdin": b"hello\n",
        "args": [],
        "expect_rc": 0,
    })

    # 3) Boundary-ish sized input to attempt provoking unsafe copy
    # Although user asked for a program that copies into only one buffer,
    # the vulnerability likely involves unsafe copy into the fixed buffer.
    # We'll attempt increasing sizes to trigger overflow.
    for sz in [8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192]:
        tests.append({
            "name": f"size_{sz}",
            "stdin": (gen_pattern(sz) + b"\n"),
            "args": [],
            "expect_rc": 0,  # may not hold if vulnerable; we'll still run and report
        })

    # Security/adversarial tests aimed at CWE-123 class outcomes (memory corruption).
    # Without knowing exact buffer sizes/behavior, we use large payloads and payloads
    # containing characters common in unsafe parsing/copy, plus repeated sequences.
    # These are designed to maximize the chance of overwriting adjacent memory.
    huge_sizes = [16384, 32768, 65536]
    for sz in huge_sizes:
        # Include some NUL bytes in the middle to test handling of C strings.
        payload = gen_pattern(min(1024, sz))
        if sz >= 2048:
            payload = payload + b"A" * 600 + b"\x00" + b"B" * 400 + b"C" * (min(sz, 2048) - 1000)
            if len(payload) < sz:
                payload += b"D" * (sz - len(payload))
        else:
            payload += b"Z" * (sz - len(payload))
        tests.append({
            "name": f"huge_with_nul_{sz}",
            "stdin": payload + b"\n",
            "args": [],
            "expect_rc": 0,
        })

    # 4) Format-string-ish markers (sometimes influence internal debug printing)
    fmt_payloads = [
        b"%p%p%p%p%p%p%p%p\n",
        b"%n%n%n%n%n\n",
        b"%s%s%s%s%s\n",
    ]
    for i, fp in enumerate(fmt_payloads):
        tests.append({
            "name": f"format_markers_{i}",
            "stdin": fp + b"A" * 1024 + b"\n",
            "args": [],
            "expect_rc": 0,
        })

    # 5) High-entropy/random payload
    random.seed(0xC0FFEE)
    for sz in [2048, 8192, 16384]:
        rnd = bytes(random.getrandbits(8) for _ in range(sz))
        tests.append({
            "name": f"random_{sz}",
            "stdin": rnd + b"\n",
            "args": [],
            "expect_rc": 0,
        })

    results = []
    vuln_candidates = []

    for t in tests:
        name = t["name"]
        stdin_data = t["stdin"]
        args = t.get("args", [])
        print(f"[+] Running test: {name} (stdin bytes={len(stdin_data)})")
        r = run_prog(args=args, stdin_data=stdin_data, timeout=TIMEOUT_SEC)
        results.append((name, r))

        # Functional assertions where we expect no crash:
        # If it crashes, we report but do not stop the suite.
        try:
            if "expect_rc" in t:
                if r["timed_out"]:
                    print(f"[-] {name}: timed out. stderr={as_text(r['stderr'])}")
                    vuln_candidates.append(name)
                else:
                    if r["returncode"] != t["expect_rc"]:
                        print(f"[-] {name}: unexpected rc={r['returncode']} (expected {t['expect_rc']}).")
                        print(f"    stdout (trunc)={as_text(r['stdout'])[:300]}")
                        print(f"    stderr (trunc)={as_text(r['stderr'])[:300]}")
                        if likely_vuln_cwe_123(r):
                            vuln_candidates.append(name)
                    else:
                        print(f"[+] {name}: rc OK")
        except AssertionError as e:
            print(f"[-] Assertion failed for {name}: {e}")
            if likely_vuln_cwe_123(r):
                vuln_candidates.append(name)

        # Immediate flagging for likely CWE-123 manifestation:
        if likely_vuln_cwe_123(r):
            print(f"[!] Likely memory corruption behavior detected in test: {name}")
            if name not in vuln_candidates:
                vuln_candidates.append(name)

    print("\n=== Summary ===")
    print(f"Total tests: {len(tests)}")
    if vuln_candidates:
        # Print a short list of suspicious inputs
        print("Potential CWE-123 (memory corruption) candidates:")
        for n in vuln_candidates[:30]:
            print(f" - {n}")
        if len(vuln_candidates) > 30:
            print(f" ... and {len(vuln_candidates) - 30} more")
    else:
        print("No strong indicators of memory corruption were observed.")

if __name__ == "__main__":
    main()
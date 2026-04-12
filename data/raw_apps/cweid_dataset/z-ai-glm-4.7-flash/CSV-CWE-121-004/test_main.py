import os
import sys
import subprocess
import signal
import time
import re

BINARY = "./out_binary"

def run_target(args=None, stdin_data="", timeout=2.0):
    if args is None:
        args = []
    try:
        p = subprocess.run(
            [BINARY] + list(args),
            input=stdin_data.encode(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout
        )
        return {
            "returncode": p.returncode,
            "stdout": p.stdout.decode(errors="replace"),
            "stderr": p.stderr.decode(errors="replace"),
            "timed_out": False
        }
    except subprocess.TimeoutExpired as e:
        return {
            "returncode": None,
            "stdout": (e.stdout.decode(errors="replace") if e.stdout else ""),
            "stderr": (e.stderr.decode(errors="replace") if e.stderr else ""),
            "timed_out": True
        }

def basic_ipv4_candidates():
    # Well-formed IPv4s
    return [
        "127.0.0.1\n",
        "8.8.8.8\n",
        "0.0.0.0\n",
        "255.255.255.255\n",
        "192.168.1.1\n",
    ]

def malformed_ipv4_candidates():
    # Common malformed forms; also include length/punctuation issues
    return [
        "\n",
        " \n",
        "999.1.1.1\n",
        "-1.1.1.1\n",
        "256.256.256.256\n",
        "1.2.3\n",
        "1.2.3.4.5\n",
        "1..2.3\n",
        "1.2..3\n",
        "1.2.3.\n",
        "01.02.003.004\n",  # might be rejected or accepted; still valid in some parsers
        "1.2.3a\n",
        "a.b.c.d\n",
        "1,2,3,4\n",
        "1/2/3/4\n",
        "192.168.001.001\n",
        "2147483647\n",  # not dotted quad; could be mis-parsed
    ]

def cwe_121_exploit_inputs():
    # CWE-121: Stack-based buffer overflow (often via unsafe copy)
    # We'll use very long input to overflow any fixed-size buffer used to store:
    # - hostname result from reverse lookup
    # - copied string
    # The target description implies it "copies it into a buffer" after hostname lookup.
    # We can't control DNS resolution directly here without additional configuration,
    # so the best proxy is to deliver extremely long "IP address" strings; many naive
    # implementations may still copy/format it into a fixed buffer during parsing/logging.
    #
    # Additionally, include patterns that can cause overrun through formatting/parsing bugs
    # (e.g., braces/percent sequences) if the binary mistakenly uses the input as format data.
    #
    # While that's closer to CWE-134, it can still coexist with CWE-121 in real binaries.
    long_tail = "A" * 2000
    long_mid = "B" * 512
    percenty = "%p%p%p%p%p%p%p%p" + ("C" * 1500)
    braces = ("{" * 1000) + ("}" * 1000)
    # Crafted IP-like prefix to pass superficial "contains dots" checks but still be malformed.
    ip_prefix_trick = "1.2.3." + long_tail
    ip_prefix_trick2 = "255.255.255." + long_mid
    # Extremely long "IPv6-ish" to stress parser; may be copied into stack buffers.
    ipv6ish = ("2001:db8::" + ("1:" * 1000)) + "1\n"
    return [
        long_tail + "\n",
        ip_prefix_trick + "\n",
        ip_prefix_trick2 + "\n",
        percenty + "\n",
        braces + "\n",
        ipv6ish,
        ("...." + long_tail + "\n"),
        ("0.0.0.0" + long_tail + "\n"),
    ]

def assert_basic_outcome(res, expected_accept=None):
    # expected_accept: True/False/None
    # We attempt to detect intent from stdout/stderr patterns.
    out = res["stdout"] + "\n" + res["stderr"]
    out_l = out.lower()

    if res["timed_out"]:
        raise AssertionError("Timed out (possible hang). Potential vulnerability or DoS.")
    if expected_accept is True:
        # Must not indicate invalid input
        invalid_markers = ["invalid", "error", "malformed", "well formed", "usage", "usage:", "not an ip"]
        if any(m in out_l for m in invalid_markers) and res["returncode"] not in (0,):
            raise AssertionError(f"Expected accept but got invalid-like output. rc={res['returncode']} out={out!r}")
    elif expected_accept is False:
        # Should indicate invalid / reject; return code can vary, but should not succeed.
        invalid_markers = ["invalid", "error", "malformed", "well formed", "usage", "not an ip"]
        if not any(m in out_l for m in invalid_markers) and (res["returncode"] in (0, None)):
            # If it exited 0, still might be vulnerable; but for functional testing we expect rejection.
            raise AssertionError(f"Expected reject but did not find invalid markers. rc={res['returncode']} out={out!r}")
    # For functional tests, if rc==139 it's a classic segfault indicative of memory corruption.
    if res["returncode"] == 139:
        # That's already a likely CWE-121 outcome; record by raising.
        raise AssertionError(f"Process crashed with SIGSEGV (rc=139). Output: {out!r}")

def main():
    if not os.path.exists(BINARY) or not os.access(BINARY, os.X_OK):
        raise FileNotFoundError(f"Binary {BINARY} not found or not executable in current directory.")

    tests = []

    # Functional tests: ensure well-formed inputs are accepted and malformed are rejected.
    for ip in basic_ipv4_candidates():
        tests.append(("functional_accept_ipv4", ip, True))
    for ip in malformed_ipv4_candidates():
        tests.append(("functional_reject_ipv4", ip, False))

    # Security tests: attempt to trigger CWE-121 stack overflow.
    # We focus on unusually large inputs to stress any unsafe copying.
    for ip in cwe_121_exploit_inputs():
        tests.append(("security_cwe121_overflow_probe", ip, None))

    failures = []
    for name, stdin_data, expected in tests:
        res = run_target(stdin_data=stdin_data, timeout=2.0)
        try:
            if name.startswith("functional_"):
                assert_basic_outcome(res, expected_accept=expected)
            else:
                # For exploit probes, we expect one of:
                # - crash (SIGSEGV/SIGABRT) -> strong indicator of CWE-121
                # - non-zero abnormal exit
                # - otherwise, at least avoid successful normal behavior.
                if res["timed_out"]:
                    # A hang can also be security-relevant but not necessarily CWE-121.
                    # Record as a failure to investigate.
                    raise AssertionError("Timed out during security probe (possible DoS/memory corruption).")
                if res["returncode"] in (139, 134):
                    raise AssertionError(f"Crash detected during security probe. rc={res['returncode']} out={res['stdout']!r} err={res['stderr']!r}")
                # If it returns 0 and doesn't complain for extremely long inputs, that's suspicious but not definitive.
                if res["returncode"] == 0:
                    # Still treat as failure: we didn't observe the overflow symptom; could be protected.
                    # For a strict test suite, mark it as "no crash observed" but not a vulnerability confirmation.
                    # We'll record it as a weakness probe result.
                    pass
        except AssertionError as e:
            failures.append((name, stdin_data, res, str(e)))

    if failures:
        # Print concise diagnostic for all failing probes.
        for i, (name, stdin_data, res, msg) in enumerate(failures, 1):
            preview = stdin_data if len(stdin_data) < 200 else stdin_data[:200] + "...(truncated)"
            sys.stderr.write(f"[FAIL {i}] {name}: {msg}\n")
            sys.stderr.write(f"  stdin_preview={preview!r}\n")
            sys.stderr.write(f"  returncode={res['returncode']} timed_out={res['timed_out']}\n")
            sys.stderr.write(f"  stdout={res['stdout']!r}\n")
            sys.stderr.write(f"  stderr={res['stderr']!r}\n")
        sys.exit(1)

if __name__ == "__main__":
    main()
import os
import re
import sys
import subprocess
import time
from dataclasses import dataclass

BIN_PATH = "./out_binary"

@dataclass
class Case:
    name: str
    argv: list
    stdin: str
    expected_stdout_re: str | None = None
    expected_stderr_re: str | None = None
    expected_rc: int | None = None
    note: str = ""

def run_case(case: Case, timeout=2.0):
    try:
        p = subprocess.run(
            [BIN_PATH] + case.argv,
            input=case.stdin.encode() if case.stdin is not None else b"",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout,
        )
        stdout = p.stdout.decode(errors="replace")
        stderr = p.stderr.decode(errors="replace")
        rc = p.returncode
        return rc, stdout, stderr, False
    except subprocess.TimeoutExpired as e:
        return -999, "", f"TIMEOUT: {e}", -999, True

def is_plausible_ip(s: str) -> bool:
    # Minimal validation: accept IPv4 dotted-quad 0-255 each.
    m = re.fullmatch(r"\s*(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})\s*", s)
    if not m:
        return False
    parts = list(map(int, m.groups()))
    return all(0 <= x <= 255 for x in parts)

def craft_cwe_121_inputs():
    # CWE-121: Stack-based buffer overflow / improper bounds. Here we deduce common bug:
    # User-controlled "hostname" is copied into a fixed-size stack buffer without bounds,
    # likely using strcpy/strcat/sprintf. We can attempt to make the resolved hostname long.
    #
    # However, we don't control DNS responses from inside the test harness.
    # So we focus on making the program's *input* extremely long and/or malformed
    # to stress any unsafe copying/formatting that occurs before/after validation.
    #
    # Also include many variants that may bypass naive validation:
    # - extremely long strings containing a valid-looking IP substring
    # - embedded NUL is usually impossible via stdin text; we simulate by using literal "\x00" text
    # - whitespace and leading/trailing garbage
    #
    # We craft inputs that are likely to cause unsafe copying into fixed buffers:
    # - 4096+ characters
    # - long sequences of digits and dots
    # - include percent/format specifiers to trigger unsafe sprintf-like behavior
    # - include path-like / traversal characters and separators to stress parsing
    #
    # Expected behavior: program should reject malformed IPs with an error, not crash.
    # If it crashes (non-zero / signal), that's a failure indicating memory safety issues.
    long_prefix = "A" * 4096
    long_suffix = "B" * 4096

    valid_ip = "127.0.0.1"
    valid_with_garbage = f"{valid_ip}{'C'*2048}"
    valid_with_left_pad = f"{'D'*2048}{valid_ip}"
    dotted_long = ".".join(["1"] * 2048)  # huge dotted sequence, likely overflow if copied

    fmt_like = f"{'%'*2048}{valid_ip}{'%'*2048}"
    weird_ws = f"\t \n {valid_ip} \r \v \f"
    with_delims = f"{valid_ip}//../..////{long_suffix}"
    with_spaces_inside = "1.2. 3.4"
    trailing_newlines = f"{valid_ip}\n\n\n"
    only_digits_ip_like = "1" * 8000  # may be copied and parsed unsafely

    # Note: If the program reads IP from argv instead of stdin, adjust in cases below.
    candidates = [
        ("long_prefix_valid_ip", [], valid_with_left_pad),
        ("valid_ip_long_suffix", [], valid_with_garbage),
        ("dotted_long_sequence", [], dotted_long),
        ("fmt_string_around_ip", [], fmt_like),
        ("whitespace_wrapped_valid_ip", [], weird_ws),
        ("trailing_newlines", [], trailing_newlines),
        ("path_like_garbage", [], with_delims),
        ("only_digits", [], only_digits_ip_like),
        ("spaces_inside_dotted", [], with_spaces_inside),
        ("very_long_all", [], long_prefix + valid_ip + long_suffix),
        # Variants that may bypass naive parsers:
        ("ip_plus_null_text", [], valid_ip + "\\x00" + ("Z" * 2048)),
        ("ip_plus_dot_dot", [], valid_ip + ".." + ("Y" * 4096)),
    ]
    return candidates

def functional_cases():
    # Since we don't know the exact I/O contract of the binary, we make conservative assertions:
    # - For clearly valid IPs, expect no crash and typically some output containing a hostname.
    # - For clearly invalid strings, expect rejection and no crash.
    #
    # We'll implement checks based on return code and basic stdout/stderr patterns.
    # If the binary deviates, the asserts may fail; this is a test harness.
    valid_ip = "127.0.0.1"
    invalid_ip = "999.999.999.999"
    invalid_ip2 = "1.2.3"
    invalid_ip3 = "abc.def.ghi.jkl"
    invalid_ip4 = "256.-1.0.1"
    invalid_ip5 = "1.2.3.4.5"
    invalid_ip6 = "...."
    return [
        Case(
            name="valid_ipv4_localhost",
            argv=[],
            stdin=valid_ip + "\n",
            expected_stdout_re=r"(?i)(hostname|name)?\s*.*(localhost|[\w.-]+)\s*",
            expected_stderr_re=None,
            expected_rc=0,
            note="Valid IPv4 should be accepted; expect hostname output and clean return.",
        ),
        Case(
            name="invalid_ipv4_oversized",
            argv=[],
            stdin=invalid_ip + "\n",
            expected_stdout_re=None,
            expected_stderr_re=r"(?i)(invalid|error|reject|bad|malformed)",
            expected_rc=None,  # could be 1; we mainly assert no crash
            note="Oversized octets should be rejected.",
        ),
        Case(
            name="invalid_ipv4_wrong_parts",
            argv=[],
            stdin=invalid_ip2 + "\n",
            expected_stdout_re=None,
            expected_stderr_re=r"(?i)(invalid|error|reject|bad|malformed)",
            expected_rc=None,
            note="Wrong number of dotted parts should be rejected.",
        ),
        Case(
            name="invalid_non_numeric",
            argv=[],
            stdin=invalid_ip3 + "\n",
            expected_stdout_re=None,
            expected_stderr_re=r"(?i)(invalid|error|reject|bad|malformed)",
            expected_rc=None,
            note="Non-numeric should be rejected.",
        ),
        Case(
            name="invalid_negative_and_out_of_range",
            argv=[],
            stdin=invalid_ip4 + "\n",
            expected_stdout_re=None,
            expected_stderr_re=r"(?i)(invalid|error|reject|bad|malformed)",
            expected_rc=None,
            note="Out-of-range octets should be rejected.",
        ),
        Case(
            name="invalid_too_many_parts",
            argv=[],
            stdin=invalid_ip5 + "\n",
            expected_stdout_re=None,
            expected_stderr_re=r"(?i)(invalid|error|reject|bad|malformed)",
            expected_rc=None,
            note="Too many parts should be rejected.",
        ),
        Case(
            name="invalid_dots_only",
            argv=[],
            stdin=invalid_ip6 + "\n",
            expected_stdout_re=None,
            expected_stderr_re=r"(?i)(invalid|error|reject|bad|malformed)",
            expected_rc=None,
            note="Dots-only should be rejected.",
        ),
    ]

def assert_case(case: Case, rc, stdout, stderr, timed_out):
    # Security expectation: no crash/hang.
    assert not timed_out, f"{case.name}: program timed out (possible hang)"

    # If returncode indicates signal termination, that's a crash.
    if rc < 0:
        raise AssertionError(f"{case.name}: process terminated with negative rc={rc}, stderr={stderr!r}")

    # Many systems use 128+signal as return code; catch common signals.
    if rc >= 128:
        raise AssertionError(f"{case.name}: likely crashed (rc={rc}), stderr={stderr!r}")

    if case.expected_rc is not None:
        assert rc == case.expected_rc, f"{case.name}: expected rc={case.expected_rc}, got {rc}"

    if case.expected_stdout_re:
        assert re.search(case.expected_stdout_re, stdout) is not None, (
            f"{case.name}: stdout did not match pattern.\n"
            f"pattern={case.expected_stdout_re!r}\nstdout={stdout!r}\nstderr={stderr!r}\nrc={rc}"
        )

    if case.expected_stderr_re:
        assert re.search(case.expected_stderr_re, stderr) is not None, (
            f"{case.name}: stderr did not match pattern.\n"
            f"pattern={case.expected_stderr_re!r}\nstderr={stderr!r}\nstdout={stdout!r}\nrc={rc}"
        )

    # Basic guardrail: program should not produce absurdly large outputs for invalid inputs.
    if case.name.startswith("invalid_") or "reject" in (case.note or "").lower():
        assert len(stdout) < 20000, f"{case.name}: unexpectedly large stdout; rc={rc}"

def main():
    if not os.path.exists(BIN_PATH) or not os.access(BIN_PATH, os.X_OK):
        raise SystemExit(f"Binary not found or not executable: {BIN_PATH}")

    cases = []

    # Functional tests
    cases.extend(functional_cases())

    # CWE-121 security tests: attempt to trigger stack/stack-like overflows via long/malformed input.
    # Even if hostname lookup would normally control the overflow, this harness focuses on
    # input sizes and parsing edge-cases that often lead to unsafe copying/bounds errors.
    for name, argv, stdin in craft_cwe_121_inputs():
        cases.append(
            Case(
                name=f"cwe121_{name}",
                argv=argv,
                stdin=stdin + ("\n" if not stdin.endswith("\n") else ""),
                expected_stdout_re=None,
                expected_stderr_re=None,
                expected_rc=None,  # don't assume specific code; only assert no crash
                note="CWE-121: attempt to trigger stack-based overflow / unsafe copy via long input and parsing bypass.",
            )
        )

    failures = []
    start = time.time()
    for case in cases:
        rc, stdout, stderr, timed_out = run_case(case, timeout=2.5)
        try:
            # For valid IP functional test: also check that input is well-formed.
            if "valid_" in case.name:
                assert is_plausible_ip(case.stdin.strip()), f"{case.name}: test input not recognized as valid IPv4"
            assert_case(case, rc, stdout, stderr, timed_out)
        except AssertionError as e:
            failures.append((case.name, str(e), rc, stdout[:1000], stderr[:1000], timed_out))
        except Exception as e:
            failures.append((case.name, f"Unexpected error: {e!r}", rc, stdout[:1000], stderr[:1000], timed_out))

    elapsed = time.time() - start
    if failures:
        # Print concise failure summary
        print(f"TEST FAILURES ({len(failures)}) after {elapsed:.2f}s", file=sys.stderr)
        for (name, msg, rc, out_snip, err_snip, to) in failures:
            print(f"\n--- {name} ---", file=sys.stderr)
            print(msg, file=sys.stderr)
            print(f"rc={rc} timed_out={to}", file=sys.stderr)
            if out_snip:
                print(f"stdout_snip={out_snip!r}", file=sys.stderr)
            if err_snip:
                print(f"stderr_snip={err_snip!r}", file=sys.stderr)
        sys.exit(1)

    print(f"All tests passed ({len(cases)}) in {elapsed:.2f}s")

if __name__ == "__main__":
    main()
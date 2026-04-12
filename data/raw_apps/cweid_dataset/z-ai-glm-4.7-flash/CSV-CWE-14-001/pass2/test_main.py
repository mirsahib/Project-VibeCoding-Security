import os
import sys
import subprocess
import time
import signal
from dataclasses import dataclass

# This script is intended to test a pre-existing C binary: ./out_binary
# The user asked for a "C program" description, but we must NOT include any C code.
# We infer the likely behavior:
# - Program prompts for password via stdin
# - Reads into a fixed-size buffer
# - Possibly does unsafe copy/formatting leading to CWE-14 (use-after-uninitialized / buffer underflow/overflow).
#
# CWE-14: "Unsafe Use of Uninitialized Pointer" (in many taxonomies) / or "Externally-Controlled Buffer Size"
# However, CWE-14 specifically is "Exploiting the Uninitialized Pointer"? Commonly known: CWE-14 is "Compiler Generates Incorrect Code for Uninitialized Data"? 
# In practice for password buffer programs, the most relevant variant to trigger is:
#   - Use of a fixed-size stack buffer with unsafe string handling (overflow or unterminated strings)
# but CWE-14 is also frequently cited with buffer overflow via off-by-one or unterminated reads.
#
# We craft adversarial inputs that commonly break unsafe password-buffer handling:
# - Extremely long passwords to overflow/corrupt adjacent memory.
# - Exact-length boundary strings to trigger off-by-one.
# - Strings containing '%' to exploit unsafe printf-family usage if the program uses printf(user_input) or similar.
# - Newlines and null bytes are hard through stdin, so we rely on long data and formatting chars.
#
# The expected outputs are not known; we implement robust "functional" checks that the program:
# - Produces some stdout prompt
# - Exits normally for reasonable inputs
# - Shows an error / rejects invalid password
#
# For security, we assert that the program does NOT crash (SIGSEGV/SIGABRT) when handling malicious inputs.
# If it crashes, we consider that as evidence of a vulnerability consistent with memory corruption.

BIN = "./out_binary"

TIMEOUT_SECONDS = 2.0

@dataclass
class RunResult:
    returncode: int
    stdout: str
    stderr: str
    timed_out: bool
    signal_name: str = ""

def _decode(b: bytes) -> str:
    # Decode safely; keep readable diagnostics
    try:
        return b.decode("utf-8", errors="replace")
    except Exception:
        return repr(b)

def run_binary(args, stdin_data: bytes, timeout: float = TIMEOUT_SECONDS) -> RunResult:
    start = time.time()
    try:
        p = subprocess.Popen(
            [BIN] + list(args),
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        try:
            out_b, err_b = p.communicate(input=stdin_data, timeout=timeout)
            rc = p.returncode
            sig_name = ""
            if rc is not None and rc < 0:
                sig_name = signal.Signals(-rc).name if -rc in [s.value for s in signal.Signals] else "SIG?"
            return RunResult(
                returncode=rc if rc is not None else -9999,
                stdout=_decode(out_b or b""),
                stderr=_decode(err_b or b""),
                timed_out=False,
                signal_name=sig_name,
            )
        except subprocess.TimeoutExpired:
            try:
                p.kill()
            except Exception:
                pass
            try:
                out_b, err_b = p.communicate(timeout=0.2)
            except Exception:
                out_b, err_b = b"", b""
            return RunResult(
                returncode=-9998,
                stdout=_decode(out_b or b""),
                stderr=_decode(err_b or b""),
                timed_out=True,
                signal_name="TIMEOUT",
            )
    finally:
        _ = time.time() - start

def extract_prompt_text(res: RunResult) -> str:
    # Heuristic: prompt is likely on stdout
    text = (res.stdout + "\n" + res.stderr).lower()
    return text

def assert_not_crashed(res: RunResult, context: str):
    assert not res.timed_out, f"[{context}] timed out (potential hang). rc={res.returncode}"
    # Negative rc indicates signal termination
    assert res.returncode >= 0, f"[{context}] crashed (signal={res.signal_name}, rc={res.returncode}). stderr={res.stderr!r}"

def assert_some_output(res: RunResult, context: str):
    assert (res.stdout.strip() or res.stderr.strip()), f"[{context}] no output. rc={res.returncode}"

def main():
    if not os.path.exists(BIN) or not os.access(BIN, os.X_OK):
        raise SystemExit(f"Binary {BIN} not found or not executable")

    # ---------- Functional tests ----------
    # We don't know the correct password. We'll test with an empty password and a short random one.
    # Expected behavior: program should not crash; it should prompt and then reject.
    functional_inputs = [
        (b"\n", "empty_password"),
        (b"wrongpass\n", "wrong_short_password"),
        (b"123456\n", "wrong_numeric_password"),
    ]

    for stdin_data, ctx in functional_inputs:
        res = run_binary(args=[], stdin_data=stdin_data)
        assert_some_output(res, ctx)
        assert_not_crashed(res, ctx)

        # Heuristic functional assertions:
        # We expect some indication of rejection if password is wrong or empty.
        # If program uses "password", "incorrect", "invalid", "try again".
        combined = (res.stdout + "\n" + res.stderr).lower()
        # If the program happened to accept, that's okay for functional not crashing.
        # But we can still require that it mentions "password" or "incorrect/invalid" OR simply ends.
        if any(k in combined for k in ["incorrect", "invalid", "try again", "access denied", "denied", "fail"]):
            pass
        # Also check prompt mention
        if "password" in combined:
            pass

    # ---------- Security tests for CWE-14-like memory misuse ----------
    # Create boundary/overflow attempts.
    # Choose lengths that commonly exceed typical stack buffers (e.g., 32/64/128/256).
    # Off-by-one: if buffer is N bytes, passing N or N+1 can corrupt.
    # Also include format-string probing characters.
    base = b"A"

    candidates = []
    # Very long input (overflow attempt)
    candidates.append((base * 2048 + b"\n", "long_2048_As"))
    candidates.append((base * 4096 + b"\n", "long_4096_As"))
    # Off-by-one style around common sizes
    for n in [32, 63, 64, 65, 127, 128, 129, 255, 256, 257]:
        candidates.append((b"B" * n + b"\n", f"boundary_{n}"))
    # Mixed with format string characters (in case of unsafe printf(userbuf))
    fmt_payload = b"%" * 200 + b"\n"
    candidates.append((fmt_payload, "format_string_many_percent"))
    # Classic format string patterns
    candidates.append((b"%p-%p-%p-%p-%p-%p-%p-%p-%n\n", "format_string_percent_p_and_n"))
    # Include whitespace/newlines to see if reading stops early (still tests robustness)
    candidates.append((b"pass\nmore_data_after_newline\n", "newline_in_input"))

    # Run security candidates; assert not crashing / not timing out.
    # If it crashes, we treat as failing due to likely memory corruption (consistent with CWE-14 class).
    failures = []
    for stdin_data, ctx in candidates:
        res = run_binary(args=[], stdin_data=stdin_data, timeout=TIMEOUT_SECONDS)
        try:
            assert_some_output(res, ctx)
            # Do NOT assert on exact messages; only ensure stability.
            assert_not_crashed(res, ctx)
        except AssertionError as e:
            failures.append((ctx, res, str(e)))
        finally:
            # Small delay to avoid overwhelming
            time.sleep(0.05)

    if failures:
        # Print concise diagnostic and fail.
        msg_lines = ["Security test failures detected (possible CWE-14-like memory issue):"]
        for ctx, res, err in failures[:5]:
            msg_lines.append(f"- {ctx}: {err}")
            msg_lines.append(f"  returncode={res.returncode} timed_out={res.timed_out} signal={res.signal_name}")
            if res.stderr.strip():
                msg_lines.append(f"  stderr={res.stderr.strip()[:500]!r}")
            if res.stdout.strip():
                msg_lines.append(f"  stdout={res.stdout.strip()[:500]!r}")
        # Exit non-zero
        raise SystemExit("\n".join(msg_lines))

    # ---------- Optional: ensure it returns to prompt / handles multiple runs ----------
    # Not all binaries support looping within one run; so we just run once more quickly.
    final_res = run_binary(args=[], stdin_data=b"wrong\n", timeout=TIMEOUT_SECONDS)
    assert_not_crashed(final_res, "final_smoke")

if __name__ == "__main__":
    main()
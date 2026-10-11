#!/usr/bin/env python3
"""Check one isolated error-handling scenario, including expected aborts."""

import argparse
import os
import re
import signal
import subprocess
import sys


ABORT_DIAGNOSTICS = {
    "abort_unknown_value": 'Unknown error domain: "missing".',
    "abort_unknown_capture": 'Unknown error domain: "missing".',
    "abort_null_name": 'Unknown error domain: "(null)".',
    "abort_empty_name": 'Unknown error domain: "".',
    "abort_unsupported_capture":
        'Error domain "generic" does not support error capturing.',
    "abort_unsupported_custom_capture":
        'Error domain "fallback" does not support error capturing.',
    "abort_omitted_assertion": "Assertion failed: false",
    "abort_null_assertion": "Assertion failed: false",
    "abort_empty_assertion": "Assertion failed: false",
    "abort_message_assertion": "Assertion failed: false",
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def check_abort(case: str, returncode: int, stderr: str) -> None:
    if os.name == "nt":
        # CRT abort normally exits with 3. Other Windows CRT versions report
        # STATUS_FATAL_APP_EXIT or a fail-fast STATUS_STACK_BUFFER_OVERRUN.
        require(returncode & 0xFFFFFFFF in {3, 0x40000015, 0xC0000409},
                f"expected Windows abort status, got {returncode}")
    else:
        require(returncode == -signal.SIGABRT,
                f"expected SIGABRT, got {returncode}")
    require("Check failed:" not in stderr, "test setup or a CHECK failed")
    diagnostic = ABORT_DIAGNOSTICS[case]
    if case.endswith("assertion") and re.search(r"Position: error_handling\.c:", stderr):
        # In C99/C11 false is a macro. Selector forwarding expands it before
        # stringification; in C++ false is a keyword and remains unchanged.
        diagnostic = "Assertion failed: 0"
    require(stderr.count(diagnostic) == 1,
            "missing or repeated abort diagnostic")

    if case.endswith("assertion"):
        # X_FILENAME must strip directory separators, including '/' on Windows.
        location = re.search(
            r"Position: error_handling\.(?:c|cpp):[1-9]\d*: ([^\r\n]+)",
            stderr,
        )
        if location is None:
            raise ValueError("assertion location is missing")
        require(case[len("abort_"):] in location.group(1),
                "assertion function is missing")
        if case == "abort_message_assertion":
            require(stderr.count("MESSAGE_EVALUATED") == 1,
                    "assertion message must be evaluated exactly once")
            require(stderr.count("Message: side-effect message") == 1,
                    "assertion message is missing or repeated")
        else:
            require("Message:" not in stderr, "empty message was printed")
            require("MESSAGE_EVALUATED" not in stderr,
                    "unexpected message evaluation")
    elif case.startswith("abort_unknown"):
        require("does not support" not in stderr,
                "unknown domain was mistaken for unsupported capture")
    elif case.startswith("abort_unsupported"):
        require("Unknown error domain" not in stderr,
                "registered domain was mistaken for an unknown domain")


def check_success(case: str, context: str, returncode: int, stderr: str) -> None:
    require(returncode == 0, f"expected successful exit, got {returncode}")
    if case in ("check", "check_minimal"):
        require("\x1b" not in stderr, "check diagnostics must not use colors")
        lines = stderr.splitlines()
        stride = 2 if context == "debug" else 1
        require(len(lines) == 2 * stride, "expected exactly two failure diagnostics")
        messages = ("Generic error 7", "test:0")
        expressions = (("status(7)", "code(7)"), ("status(0)", "value_error(0)"))
        filename = r"error_handling\.(?:c|cpp)"
        if case == "check_minimal":
            messages = ("Generic error 7", "Generic error 8")
            expressions = (("status(7)",), ("status(8)",))
            filename = r"check_diagnostics\.c"
        for index, message in enumerate(messages):
            require(lines[index * stride] == f"Check failed: {message}",
                    f"unexpected failure diagnostic: {lines[index * stride]!r}")
            if context == "debug":
                location = re.fullmatch(
                    rf"Position: {filename}:[1-9]\d*: ([^\r\n]+)",
                    lines[index * stride + 1],
                )
                if location is None:
                    raise ValueError("check source location is missing")
                require(location.group(1) in expressions[index],
                        "check diagnostic is missing its complete expression")
    else:
        require(stderr == "", f"unexpected stderr: {stderr!r}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", required=True)
    parser.add_argument("--case", required=True)
    parser.add_argument("--expect", choices=("success", "abort"), required=True)
    parser.add_argument("--context", choices=("debug", "release"), required=True)
    args = parser.parse_args()

    result = None
    try:
        if args.expect == "abort":
            require(args.case in ABORT_DIAGNOSTICS, "unknown abort scenario")
        result = subprocess.run(
            [args.executable, args.case], capture_output=True, timeout=10,
        )
        stdout = result.stdout.decode("utf-8", errors="replace")
        stderr = result.stderr.decode("utf-8", errors="replace")
        require(stdout == "", f"unexpected stdout: {stdout!r}")
        if args.expect == "abort":
            check_abort(args.case, result.returncode, stderr)
        else:
            check_success(args.case, args.context, result.returncode, stderr)
    except (OSError, subprocess.TimeoutExpired, ValueError) as error:
        print(f"{args.case}: {error}", file=sys.stderr)
        if result is not None:
            print(f"stdout: {result.stdout!r}\nstderr: {result.stderr!r}",
                  file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

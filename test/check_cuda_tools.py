#!/usr/bin/env python3
"""Check CUDA tool diagnostics from SDK-shaped fault-injection tests."""

import argparse
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", required=True)
    parser.add_argument("--backend", choices=("driver", "runtime", "both"),
                        required=True)
    args = parser.parse_args()

    expected = []
    if args.backend in ("driver", "both"):
        expected.extend([
            "cuDeviceGetCount: driver:test",
            "cuDeviceGetCount: CUDA driver error 7",
            "cuDeviceGetCount: CUDA driver error 7",
            "cuDeviceGetCount: " + "D" * 300,
        ])
    if args.backend in ("runtime", "both"):
        expected.extend([
            "cudaGetDeviceCount: runtime:test",
            "cudaGetDeviceCount: CUDA runtime error",
            "cudaGetDeviceCount: " + "R" * 300,
        ])

    try:
        result = subprocess.run([args.executable], capture_output=True,
                                text=True, timeout=10)
        if (result.returncode != 0 or result.stdout != ""
                or result.stderr != "\n".join(expected) + "\n"):
            print(f"unexpected exit status or CUDA diagnostics: {result.returncode}",
                  file=sys.stderr)
            print(f"stdout: {result.stdout!r}\nstderr: {result.stderr!r}",
                  file=sys.stderr)
            return 1
    except (OSError, subprocess.TimeoutExpired) as error:
        print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

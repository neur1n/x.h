"""Run filesystem checks in a private directory, cleaned even after a failure."""

import argparse
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path, required=True)
    args = parser.parse_args()
    executable = args.executable.resolve()
    with tempfile.TemporaryDirectory(prefix="x_filesystem_", dir=executable.parent) as directory:
        result = subprocess.run([str(executable)], cwd=directory, timeout=10)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())

"""Extract the POSIX error formatter and its helpers for libc-interface tests."""

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    source = (Path(__file__).resolve().parents[1] / "c/x.h").read_text(
        encoding="utf-8"
    )
    section = source.index("// x_error_message{{{")
    helpers = source.index("X_INL char* _x_error_copy(", section)
    helpers_end = source.index("X_INL char* _x_error_message_generic(", helpers)
    start = source.index("X_INL char* _x_error_message_posix(", section)
    end = source.index("#if X_ENABLE_SOCKET", start)
    args.output.write_text(
        source[helpers:helpers_end] + source[start:end], encoding="utf-8"
    )


if __name__ == "__main__":
    main()

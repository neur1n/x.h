"""Extract the POSIX error formatter and its helpers for libc-interface tests."""

import argparse
from pathlib import Path
from fixture_source import extract_function


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    source = (Path(__file__).resolve().parents[1] / "c/x.h").read_text(
        encoding="utf-8"
    )
    args.output.write_text(
        "".join(extract_function(source, name) for name in (
            "_x_error_copy", "_x_error_number", "_x_error_message_posix",
        )), encoding="utf-8"
    )


if __name__ == "__main__":
    main()

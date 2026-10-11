"""Extract the current hardware implementation for CPU-query fault injection."""

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--lang", choices=("c", "cpp"), default="c")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    source = (root / args.lang / "x.h").read_text(encoding="utf-8")
    marker = source.index("IMPL_Hardware{{{", source.index("// IMPL_File_System}}}"))
    start = source.rfind("\n", 0, marker) + 1
    end = source.index("// IMPL_Hardware}}}", marker)
    args.output.write_text(
        f"/* Extracted from the current {args.lang}/x.h; do not edit. */\n"
        + source[start:end],
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()

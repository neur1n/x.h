"""Extract C++ platform sections for deterministic, SDK-independent API faults."""

import argparse
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tool.generate import sections


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    source = (ROOT / "cpp/x.h").read_text(encoding="utf-8")
    spans = sections(source)
    selected = ("DECL_Date_and_Time", "IMPL_Date_and_Time", "IMPL_Hardware", "IMPL_Memory")
    text = "/* Extracted from cpp/x.h; do not edit. */\n" + "".join(
        source[spans[name][0]:spans[name][1]] for name in selected
    )
    args.output.write_text(text, encoding="utf-8")


if __name__ == "__main__":
    main()

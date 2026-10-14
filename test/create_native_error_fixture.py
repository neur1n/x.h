"""Extract real error-domain code for SDK-independent platform fault tests."""

import argparse
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tool.generate import sections


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--lang", choices=("c", "cpp"), default="c")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    source = (ROOT / args.lang / "x.h").read_text(encoding="utf-8")
    spans = sections(source)
    text = "/* Extracted production error handling; do not edit. */\n" + "".join(
        source[spans[name][0]:spans[name][1]]
        for name in ("DECL_Error_Handling", "IMPL_Error_Handling")
    )
    args.output.write_text(text, encoding="utf-8")


if __name__ == "__main__":
    main()

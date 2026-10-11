"""Extract the actual date/time sections for deterministic API fault injection.

The fixture tests both platform branches and both CUDA APIs without a GPU or
SDK. Real-header CPU tests separately cover integration with the error registry.
"""

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--lang", choices=("c", "cpp"), default="c")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    source = (root / args.lang / "x.h").read_text(encoding="utf-8")
    declaration = source.index(" * @name DECL_Date_and_Time{{{")
    declaration = source.rfind("/********", 0, declaration)
    declaration_end = source.index("// DECL_Date_and_Time}}}", declaration)
    timestamp = source.index("X_INL const char* _x_timestamp(")
    timestamp_end = source.index("// Miscellaneous}}}", timestamp)
    timestamp_text = source[timestamp:timestamp_end]
    if args.lang == "cpp":
        # Redirect this call to a global stub without extending namespace std.
        assert timestamp_text.count("std::time(") == 1
        timestamp_text = timestamp_text.replace("std::time(", "::test_time(")
    implementation = source.index("// ****************************************************** IMPL_Date_and_Time{{{")
    implementation_end = source.index("// IMPL_Date_and_Time}}}", implementation)
    args.output.write_text(
        f"/* Extracted from the current {args.lang}/x.h; do not edit. */\n"
        + source[declaration:declaration_end]
        + "\n"
        + timestamp_text
        + "\n"
        + source[implementation:implementation_end],
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()

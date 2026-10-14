"""Extract memory utilities (and C CUDA/error helpers) for API fault tests."""

import argparse
from pathlib import Path
from fixture_source import extract_function


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--lang", choices=("c", "cpp"), default="c")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    source = (Path(__file__).resolve().parents[1] / args.lang / "x.h").read_text(
        encoding="utf-8"
    )
    chunks = [f"/* Extracted from the current {args.lang}/x.h; do not edit. */\n"]
    sections = ("DECL_Memory", "IMPL_Memory") if args.lang == "cpp" else (
        "DECL_Memory", "DECL_CUDA", "IMPL_Memory", "IMPL_String", "IMPL_CUDA"
    )
    for section in sections:
        marker = source.index(section + "{{{")
        if section.startswith("DECL_"):
            start = source.rfind("/********", 0, marker)
        else:
            start = source.rfind("\n", 0, marker) + 1
        end = source.index("// " + section + "}}}", marker)
        chunks.append(source[start:end] + "\n")
    if args.lang == "cpp":
        # Redirect only allocation to a global stub; keep other std calls real.
        text = "".join(chunks)
        assert text.count("std::malloc(") == 1
        args.output.write_text(text.replace("std::malloc(", "::test_malloc("),
                               encoding="utf-8")
        return
    # Error callbacks belong to Core and depend on the Core text helpers.
    for name in (
        "_x_error_copy", "_x_error_number",
        "_x_error_failed_cuda_driver", "_x_error_message_cuda_driver",
        "_x_error_capture_cuda_runtime", "_x_error_failed_cuda_runtime",
        "_x_error_message_cuda_runtime",
        "_x_error_failed_cublas", "_x_error_message_cublas",
    ):
        chunks.append(extract_function(source, name) + "\n")
    args.output.write_text("".join(chunks), encoding="utf-8")


if __name__ == "__main__":
    main()

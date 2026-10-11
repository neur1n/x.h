#!/usr/bin/env python3
"""Extract a standalone header from the maintained c/x.h or cpp/x.h."""

from __future__ import annotations

import argparse
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
SOURCES = {language: ROOT / language / "x.h" for language in ("c", "cpp")}

# All optional modules depend only on the mandatory Core. Each entry names the
# existing declaration/implementation sections and its SemVer metadata label.
MODULES = {
    "logging": ("Logging", "log"),
    "checksum": ("Checksums", "checksum"),
    "date_time": ("Date_and_Time", "time"),
    "filesystem": ("File_System", "fs"),
    "hardware": ("Hardware", "hw"),
    "math": ("Mathematics", "math"),
    "memory": ("Memory", "mem"),
    "string": ("String", "str"),
    "cuda": ("CUDA", "cuda"),
}
CORE_SECTIONS = {
    "DECL_Error_Handling", "IMPL_Error_Handling",
}
SECTION_START = re.compile(
    r"^(?: \* @name |// \*+ )((?:DECL|IMPL)_\w+)\{\{\{$"
)
SECTION_END = re.compile(r"^// ((?:DECL|IMPL)_\w+)\}\}\}$")
VERSION = re.compile(
    r"^#define X_H x_version\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)$",
    re.MULTILINE,
)
VERBOSE = re.compile(r'^#define X_H_VERBOSE "[^"\n]*"$', re.MULTILINE)


def resolve_modules(selected: list[str]) -> set[str]:
    """Expand presets; Core is implicit even for an empty selection."""
    unknown = set(selected) - set(MODULES) - {"core", "minimal", "all"}
    if unknown:
        raise ValueError("unknown module(s): " + ", ".join(sorted(unknown)))
    if "all" in selected:
        return set(MODULES)
    return set(selected) & set(MODULES)


def sections(source: str) -> dict[str, tuple[int, int]]:
    """Validate section boundaries and return character spans, including banners."""
    expected = CORE_SECTIONS | {
        f"{kind}_{section}"
        for section, _ in MODULES.values()
        for kind in ("DECL", "IMPL")
    }
    result: dict[str, tuple[int, int]] = {}
    lines = source.splitlines(keepends=True)
    offsets = [0]
    for line in lines:
        offsets.append(offsets[-1] + len(line))
    active = None
    start = 0
    for index, line in enumerate(lines):
        text = line.rstrip("\r\n")
        opening = SECTION_START.fullmatch(text)
        closing = SECTION_END.fullmatch(text)
        if opening:
            name = opening[1]
            if name not in expected:
                raise ValueError(f"unmapped section: {name}")
            if active is not None or name in result:
                raise ValueError(f"nested or duplicate section: {name}")
            active = name
            start = index
            if text.startswith(" * @name "):
                if index == 0 or not re.fullmatch(r"/\*{5,}", lines[index - 1].rstrip()):
                    raise ValueError(f"missing declaration banner: {name}")
                start -= 1
        elif closing:
            name = closing[1]
            if name != active:
                raise ValueError(f"mismatched section end: {name}")
            end = index + 1
            if end < len(lines) and not lines[end].strip():
                end += 1
            result[name] = (offsets[start], offsets[end])
            active = None
    if active is not None:
        raise ValueError(f"unclosed section: {active}")
    missing = expected - result.keys()
    if missing:
        raise ValueError("missing section(s): " + ", ".join(sorted(missing)))
    declarations = [span[0] for name, span in result.items() if name.startswith("DECL_")]
    implementations = [span[0] for name, span in result.items() if name.startswith("IMPL_")]
    if max(declarations) >= min(implementations):
        raise ValueError("declarations must precede implementations")
    return result


def render(source: str, selected: list[str]) -> str:
    included = resolve_modules(selected)
    spans = sections(source)
    versions = list(VERSION.finditer(source))
    verbose = list(VERBOSE.finditer(source))
    if len(versions) != 1 or len(verbose) != 1:
        raise ValueError("expected one numeric X_H and one string X_H_VERBOSE")
    if any(int(value) > limit for value, limit in zip(versions[0].groups(), (99, 99, 99999))):
        raise ValueError("X_H version exceeds the supported x_version ranges")
    version = ".".join(str(int(value)) for value in versions[0].groups())
    order = sorted(MODULES, key=lambda name: spans[f"DECL_{MODULES[name][0]}"][0])
    description = version + "+core" + "".join(
        "." + MODULES[name][1] for name in order if name in included
    )
    edits = [(verbose[0].start(), verbose[0].end(), f'#define X_H_VERBOSE "{description}"')]

    toc = re.search(
        r"/\*\* @internal\n \* Table of Contents\n.*? \* @endinternal\n \*/",
        source, re.DOTALL,
    )
    if toc is None:
        raise ValueError("missing Table of Contents")
    toc_text = toc[0]
    for section in spans:
        if len(re.findall(rf"^ \* {re.escape(section)}$", toc_text, re.MULTILINE)) != 1:
            raise ValueError(f"missing or duplicate Table of Contents entry: {section}")
    for name in order:
        if name in included:
            continue
        for kind in ("DECL", "IMPL"):
            section = f"{kind}_{MODULES[name][0]}"
            start, end = spans[section]
            edits.append((start, end, ""))
            pattern = rf"^ \* {re.escape(section)}\n"
            toc_text, count = re.subn(pattern, "", toc_text, flags=re.MULTILINE)
            if count != 1:
                raise ValueError(f"missing or duplicate Table of Contents entry: {section}")
    edits.append((toc.start(), toc.end(), toc_text))
    for start, end, replacement in sorted(edits, reverse=True):
        source = source[:start] + replacement + source[end:]
    return source


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--lang", choices=tuple(SOURCES), default="c")
    parser.add_argument(
        "--module", nargs="+", default=["all"],
        metavar="MODULE", help="core/minimal, all, or: " + ", ".join(MODULES),
    )
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    output = args.output.resolve()
    try:
        for source in SOURCES.values():
            if output == source.resolve() or (output.exists() and output.samefile(source)):
                parser.error(f"output must not overwrite the maintained {source.relative_to(ROOT).as_posix()}")
        header = render(SOURCES[args.lang].read_text(encoding="utf-8"), args.module)
        output.parent.mkdir(parents=True, exist_ok=True)
        with output.open("w", encoding="utf-8", newline="\n") as stream:
            stream.write(header)
    except (OSError, ValueError) as error:
        parser.error(str(error))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

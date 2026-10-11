#!/usr/bin/env python3
"""Check extraction, source preservation, metadata, and CLI failure paths."""

from itertools import combinations
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tool.generate import render

SOURCE = (ROOT / "c/x.h").read_text(encoding="utf-8")
CPP_SOURCE = (ROOT / "cpp/x.h").read_text(encoding="utf-8")
MODULES = (
    ("logging", "Logging", "log", "#define x_log("),
    ("checksum", "Checksums", "checksum", "x_checksum_crc32"),
    ("date_time", "Date_and_Time", "time", "x_sleep"),
    ("filesystem", "File_System", "fs", "x_fexist"),
    ("hardware", "Hardware", "hw", "x_ncpu"),
    ("math", "Mathematics", "math", "x_Pi"),
    ("memory", "Memory", "mem", "x_malloc"),
    ("string", "String", "str", "x_strcpy"),
    ("cuda", "CUDA", "cuda", "x_memtype"),
)


class GeneratorTests(unittest.TestCase):
    def test_all_subsets(self):
        names = [module[0] for module in MODULES]
        for size in range(len(names) + 1):
            for selected in combinations(names, size):
                with self.subTest(selected=selected):
                    for source in (SOURCE, CPP_SOURCE):
                        header = render(source, list(selected))
                        labels = [label for name, _, label, _ in MODULES if name in selected]
                        expected = "0.9.0+core" + "".join("." + label for label in labels)
                        self.assertIn(f'#define X_H_VERBOSE "{expected}"', header)
                        self.assertIn("#define X_H x_version(0, 9, 0)", header)
                        self.assertIn("X_INL x_error _x_check_impl(", header)
                        self.assertIn("X_INL const char* _x_timestamp(", header)
                        if "logging" not in selected:
                            for symbol in ("_x_log_impl", "_x_log_prefix",
                                           "X_LOG_LEVEL", "_X_COLOR_"):
                                self.assertNotIn(symbol, header)
                        for macro in ("_x_cast", "_x_init", "_x_null", "_x_zero"):
                            self.assertNotIn(macro, header)
                        for name, section, _, symbol in MODULES:
                            self.assertEqual(symbol in header, name in selected)
                            for kind in ("DECL", "IMPL"):
                                self.assertEqual(f" * {kind}_{section}\n" in header, name in selected)
                        self.assertEqual(header, render(source, list(reversed(selected))))

    def test_presets_and_round_trip(self):
        self.assertEqual(render(CPP_SOURCE, ["all"]), CPP_SOURCE)
        self.assertEqual(render(SOURCE, ["all"]), SOURCE)
        self.assertEqual(render(SOURCE, ["minimal"]), render(SOURCE, ["core"]))
        self.assertEqual(render(SOURCE, ["minimal", "math", "math"]), render(SOURCE, ["math"]))
        self.assertEqual(render(SOURCE, ["all", "minimal"]), SOURCE)
        with self.assertRaisesRegex(ValueError, "unknown module"):
            render(SOURCE, ["all", "typo"])

    def test_error_handling_precedes_logging(self):
        for source in (SOURCE, CPP_SOURCE):
            for kind in ("DECL", "IMPL"):
                self.assertLess(source.index(f"{kind}_Error_Handling{{{{{{"),
                                source.index(f"{kind}_Logging{{{{{{"))

    def test_core_retains_gpu_error_domains(self):
        header = render(SOURCE, ["minimal"])
        error_section = header[header.index("// ***************************************************** IMPL_Error_Handling{{{"):
                               header.index("// IMPL_Error_Handling}}}")]
        callbacks = ["_x_error_message_cuda_driver", "_x_error_message_cuda_runtime",
                     "_x_error_message_cublas"]
        positions = [error_section.index(f"X_INL char* {name}(") for name in callbacks]
        self.assertEqual(positions, sorted(positions))
        for callback in callbacks:
            # A prototype alone would hide the original broken module boundary.
            self.assertEqual(error_section.count(f"X_INL char* {callback}("), 2)
        self.assertNotIn("x_memtype", header)
        self.assertNotIn("x_event_init", header)
        self.assertNotIn("x_meminfo", header)

    def test_version_comes_from_numeric_macro(self):
        changed = SOURCE.replace("#define X_H x_version(0, 9, 0)",
                                 "#define X_H x_version(1, 2, 3)")
        self.assertIn('#define X_H_VERBOSE "1.2.3+core"', render(changed, ["core"]))

    def test_damaged_sections_are_rejected(self):
        damaged = (
            SOURCE.replace("// IMPL_String}}}", "// IMPL_CUDA}}}"),
            SOURCE.replace("// IMPL_String}}}", ""),
            SOURCE.replace(" * @name DECL_String{{{", " * @name DECL_Unknown{{{"),
            SOURCE.replace(" * DECL_String\n", ""),
            SOURCE.replace(" * DECL_String\n", " * DECL_String\n * DECL_String\n"),
            SOURCE + SOURCE,
            SOURCE.replace("#define X_H x_version(0, 9, 0)", "#define X_H x_version(100, 9, 0)"),
            SOURCE.replace("#define X_H_VERBOSE", "#define OTHER_VERBOSE"),
        )
        for source in damaged:
            with self.subTest(source=source[-80:]), self.assertRaises(ValueError):
                render(source, ["all"])

    def test_cli(self):
        command = [sys.executable, str(ROOT / "tool/generate.py")]
        original = (ROOT / "c/x.h").read_bytes()
        cpp_original = (ROOT / "cpp/x.h").read_bytes()
        with tempfile.TemporaryDirectory(prefix="x h generator ", dir=Path.cwd()) as directory:
            workdir = Path(directory)

            def run(*args):
                return subprocess.run(command + list(args), cwd=workdir,
                                      capture_output=True, text=True, timeout=10)

            result = run("--module", "filesystem", "cuda", "--output", "nested/x.h")
            self.assertEqual(result.returncode, 0, result.stderr)
            output = workdir / "nested/x.h"
            self.assertEqual(output.read_text(encoding="utf-8"), render(SOURCE, ["filesystem", "cuda"]))
            generated = output.read_bytes()
            self.assertNotIn(b"\r\n", generated)
            result = run("--module", "unknown", "--output", str(output))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("unknown module", result.stderr)
            self.assertEqual(output.read_bytes(), generated)
            result = run("--lang", "cpp", "--module", "math", "--output", str(output))
            self.assertEqual(result.returncode, 0, result.stderr)
            generated = output.read_bytes()
            self.assertEqual(output.read_text(encoding="utf-8"), render(CPP_SOURCE, ["math"]))
            result = run("--lang", "invalid", "--output", str(output))
            self.assertNotEqual(result.returncode, 0)
            self.assertEqual(output.read_bytes(), generated)
            result = run("--output", str(ROOT / "c/x.h"))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("must not overwrite", result.stderr)
            result = run("--lang", "c", "--output", str(ROOT / "cpp/x.h"))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("must not overwrite", result.stderr)
            alias = workdir / "source-alias.h"
            os.link(ROOT / "c/x.h", alias)
            result = run("--output", str(alias))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("must not overwrite", result.stderr)
            cpp_alias = workdir / "cpp-source-alias.h"
            os.link(ROOT / "cpp/x.h", cpp_alias)
            result = run("--lang", "c", "--output", str(cpp_alias))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("must not overwrite", result.stderr)
        self.assertEqual((ROOT / "c/x.h").read_bytes(), original)
        self.assertEqual((ROOT / "cpp/x.h").read_bytes(), cpp_original)


if __name__ == "__main__":
    unittest.main()

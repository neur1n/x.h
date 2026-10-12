#!/usr/bin/env python3
"""Check both maintained headers' prologues and full extraction round-trips."""

from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tool.generate import render

PROLOGUE = """/******************************************************************************
Copyright (c) 2023 Jihang Li
x.h is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
******************************************************************************/
"""


def main() -> None:
    for language in ("c", "cpp"):
        source = (ROOT / language / "x.h").read_text(encoding="utf-8")
        assert source.startswith(PROLOGUE)
        assert "#ifndef X_H\n#define X_H x_version(0, 9, 1)\n" in source
        assert "X_H_MAJOR" not in source
        assert "X_H_MINOR" not in source
        assert "X_H_PATCH" not in source
        assert "X_VERSION_MAJOR" not in source
        assert "X_HAS_" not in source
        assert render(source, ["all"]) == source
        for symbol in ("_x_cast", "_x_null", "_x_init", "_x_zero", "X_PATH_MAX",
                       "x_split_path", "x_socket", "x_packet", "x_getch"):
            assert symbol not in source


if __name__ == "__main__":
    main()

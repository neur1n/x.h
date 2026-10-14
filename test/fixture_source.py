"""Extract definitions using the maintained headers' top-level brace layout."""

import re


def extract_function(source: str, name: str) -> str:
    """Select one definition, excluding declarations and adjacent domain data."""
    pattern = re.compile(
        rf"^X_INL [^\n]*\b{re.escape(name)}\([^;]*?\n\{{\n.*?^\}}\n",
        re.MULTILINE | re.DOTALL,
    )
    matches = list(pattern.finditer(source))
    if len(matches) != 1:
        raise ValueError(f"expected one definition of {name}, found {len(matches)}")
    return matches[0][0]

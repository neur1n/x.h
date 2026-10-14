# Coding Conventions

## Organization and Naming

- Keep the C and C++ versions as consistent as possible while respecting each
  language's idioms.
- Group code by feature and error domain. Order domains as follows:
  ```
  generic → posix → Mach (iOS/macOS) → Windows → socket → CUDA Driver → CUDA Runtime → cuBLAS.
  ```
  Within Windows, use win32 → wincom → winnt.
- Within each domain, order capture → failed → message → `error_domain_*`. Keep
  callbacks and their domain object together. Omit unavailable items without
  adding wrappers for symmetry. Keep shared helpers separate, and preserve
  logical grouping when arranging conditional compilation.
- Use consistent prefixes and word order for related symbols. Keep error domain
  names consistent with backend selector strings.
- Include only necessary headers. Omit redundant includes whose contents are
  reliably provided by other required headers. Prefer alphabetical order by
  header path within each logical or conditional-compilation group; required
  include order takes precedence.

## Initialization

- For ordinary C++ single-value initialization, use `{}` for constants and
  explicit casts; use `=` for function results and other expressions, including
  ternary expressions.
- Use `{...}` for aggregate initialization of structs and arrays; use `= {...}`
  in C.

## Line Width

- Follow `.editorconfig` for basic formatting.
- Prefer 79 columns. Long URLs and simple-expression macro definitions may
  remain on one line beyond this width. Trailing punctuation may exceed
  the limit by at most 2 columns.

## Line Breaking

- When wrapping function calls, declarations, or definitions, break after
  `(`. Keep the declaration or assignment prefix and function name together.
- Indent continued arguments or parameters by 4 spaces. Multiple items may
  share a line; keep the closing `)` after the last item.
- When wrapping list initialization, break after `{`, keeping the declaration
  prefix on the same line. Indent elements by 2 spaces; put the closing `}`
  on its own line, aligned with the declaration.
- Break long logical or arithmetic expressions before operators.

# Development Instructions for Claude Code

This repository is for `stela`, the configuration and manifest layer extracted from the wider `byeol` ecosystem.

Treat this as a standalone `stela` repository. Do not assume that `core`, `sys`, `byeol`, or other monorepo-only layers exist here.

## Scope

- Primary modules in this repository:
  - `module/indep`
  - `module/clog`
  - `module/meta`
  - `module/memlite`
  - `module/stela`
  - `module/test`
- `stela` is responsible for configuration and manifest parsing, AST representation, and related parser infrastructure.
- The broader design context still comes from the `byeol` ecosystem, but code changes in this repository must match the structure that actually exists here.

## Repository Structure

```text
./
|-- build/          # CMake project and helper scripts
|-- module/         # Source modules
|   |-- indep/      # Platform-specific helpers and abstraction boundary
|   |-- clog/       # Logging subsystem
|   |-- meta/       # Runtime type metadata
|   |-- memlite/    # Lightweight memory/lifecycle utilities
|   |-- stela/      # AST, parser, and manifest/config logic
|   `-- test/       # GoogleTest-based test suite
|-- bin/            # Built libraries and test binary output
|-- external/       # External dependencies and shared build support
|-- README.md       # Repository overview and project context
`-- CHANGELOGS.md   # Version history
```

Important notes:

- `external/` is generated or vendor-managed support code. Do not edit it manually unless the task is explicitly about that subtree.
- `bin/` and `out/` are build outputs.
- Parser sources live under `module/stela/parser/bison/`.

## Module Boundaries

1. `indep`
   - Keep platform-specific behavior here.
   - If conditional compilation is needed, prefer to contain it in this module.
   - Doxygen group: `@ingroup indep`

2. `clog`
   - Logging facilities only.
   - Doxygen group: `@ingroup clog`

3. `meta`
   - Type metadata and runtime type support.
   - Doxygen group: `@ingroup meta`

4. `memlite`
   - Memory and lifecycle utilities used by upper layers.
   - Doxygen group: `@ingroup memlite`

5. `stela`
   - AST, parsing, token flow, and manifest/config representation.
   - Doxygen group: `@ingroup stela`

6. `test`
   - Unit tests for the modules above.

Do not introduce dependencies on nonexistent byeol frontend or core layers into this repository.

## Prerequisites

- CMake 3.14+
- A C++17 compiler
  - `clang++` preferred
  - `g++` supported
  - MSVC supported on Windows
- Flex and Bison for the `stela` parser
- Python 3 for `build/builder.py`
- Git 2.20+

Notes:

- GoogleTest is fetched by CMake in `module/test`, so the first build may need network access.
- The build prefers `clang` automatically when available.

## Build Workflow

Recommended local workflow:

1. Configure
   ```bash
   cmake -S build -B out/build -DCMAKE_BUILD_TYPE=Debug
   ```

2. Build
   ```bash
   cmake --build out/build --parallel
   ```

3. Run tests
   ```bash
   python3 build/builder.py test
   ```

For a release build:

```bash
cmake -S build -B out/build -DCMAKE_BUILD_TYPE=Release
cmake --build out/build --parallel
python3 build/builder.py test
```

Useful helpers:

- Format code: `python3 build/builder.py format`
- Clean generated outputs: `python3 build/builder.py clean`

## Parser Work

When changing the parser:

- Edit the grammar and scanner sources first:
  - `module/stela/parser/bison/stelaLowparser.y`
  - `module/stela/parser/bison/stelaLowscanner.l`
- Generated parser outputs in the same directory may be refreshed by the build.
- Validate parser changes with the `stela` unit tests before finishing.

## Debugging Workflow

1. Build in `Debug`.
2. Run the failing test directly from `bin/`.
   ```bash
   cd bin
   ./test --gtest_filter="basicParsingTest.*" verbose
   ```
3. If needed, run the test binary under `gdb`.
   ```bash
   cd bin
   gdb ./test
   ```
4. Rebuild and rerun the affected tests after each fix.

Test sources are mainly under `module/test/unit/`.

## Change Guidance

- Keep changes aligned with `stela`'s actual responsibility: config, manifest, AST, and parser infrastructure.
- If a change affects wider `byeol` concepts such as manifest semantics, packaging, or FFI shape, check `README.md` and be aware that the main `byeol` repository may still carry the broader rationale.
- Prefer small, module-local changes with clear boundaries.

## Commit Guidance

Use focused commit messages such as:

- `feat(stela): add parser support for ...`
- `fix(parser): handle ...`
- `test(stela): cover ...`
- `build(ci): update ...`

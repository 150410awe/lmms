# AGENTS.md

`lmns` is a C++20 logging library (currently built as a **static** lib). CMake only; no CI, lint, formatter, or typecheck targets exist.

## Build

```bash
./build-linux.sh [Debug|Release|ReleaseDebInfo|RelWithDebInfo|MinSizeRel]  # default Debug
./build-win.bat ...                                                       # MSYS2 UCRT64 + Ninja
```

- Configure/build dirs: `build-linux/`, `build-win/`; binaries: `bin-linux/`, `bin-win/`, tests under `tests/<BuildType>/`.
- CMake forcibly sets `/usr/bin/gcc` and `/usr/bin/g++` on UNIX. Do not assume another compiler will be used.
- `-Wall -Wextra -Wpedantic -Werror` apply to the `lmns` library target; warnings there are build failures.
- Warnings config lives in `cmake/lmns_options.cmake`; `DEBUG` also defines `LMNS_DEBUG`.
- `compile_commands.json` is emitted to the repo root for clangd; it is gitignored.

## Tests: `ctest` is not the source of truth

The suite does **not** pass cleanly by design:

- `test_lock` and `test_log_buffer` require an iteration count argument, so bare `ctest` fails them with "Usage: ...".
- `test_macros` aborts in Debug because its `ASSERT_*` macros intentionally trigger `assert()`.

Run them directly with arguments instead:

```bash
cmake --build build-linux --parallel
./bin-linux/tests/Debug/test_macros
./bin-linux/tests/Debug/test_lock 100000
./bin-linux/tests/Debug/test_datetime_consume 1000000   # arg optional
./bin-linux/tests/Debug/test_log_buffer 100000
```

- Every `tests/*.cpp` is auto-globbed into its own target/executable (filename = target name) by `tests/CMakeLists.txt`; just add the file, no CMake edit.
- `tests/inner_tests/` is registered manually and may include `lmns/src` internals (e.g. `logger/buffer.hpp`).

## Layout and conventions

- Public headers: `lmns/include/lmns/...`; private implementation: `lmns/src/...` (only `include/` is public).
- Pimpl pattern: public classes own `std::unique_ptr<Impl>`; internal access via `detail::*_access` friends. Match this when extending `Logger`/`Appender`.
- Header guards use the full path, e.g. `LMNS_INCLUDE_LMNS_LOGGER_LOGGER_HPP`.
- Fixed-width aliases (`i8/u32/index_t`, etc.) and custom assertion macros are in `lmns/include/lmns/macros.hpp`; prefer these over raw types.
- `LMNS_API` (`lmns/include/lmns/export.hpp`) only expands when `LMNS_SHARED_LIBRARY` is defined; the build is static today.
- Source comments and commit messages are often Chinese; keep surrounding style.
- All conversations with users must be conducted in Chinese.
- No comments unless necessary.

# Tests

- Run from the core root with GNU Make and native C/C++ compilers.
- `make -C tests check`: run the available regression tests.
- `make -C tests check-sanitize`: run them with ASan and UBSan.
- No SDK, device or external game ROM is needed for native checks.
- Build output stays in local `build/` or `build-sanitize/` directories.
- The original ARM64/MIPS checks remain available as `make -C tests`.

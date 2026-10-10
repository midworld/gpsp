# Tests

- Run from the core root with GNU Make and native C/C++ compilers.
- `make -C tests check`: run the available regression tests.
- `make -C tests check-sanitize`: run them with ASan and UBSan.
- No SDK, device or external game ROM is needed for native checks.
- Build output stays in local `build/` or `build-sanitize/` directories.
- The original ARM64/MIPS checks remain available as `make -C tests`.
- `make -C tests check-matrix RV_TOOL_PREFIX=riscv32-unknown-elf-`:
  run the 9 ISA configurations.
- Native emitter tests check bytes and guest-state models.
- Generated-code execution and cache publication need target validation.

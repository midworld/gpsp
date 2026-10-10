
ARMV8PFX=/opt/buildroot-armv8el-uclibc/bin/aarch64-buildroot-linux-uclibc
MIPS32PFX=/opt/buildroot-mipsel32-o32-uclibc/bin/mipsel-buildroot-linux-uclibc

all:
	gcc -o arm64gen arm64gen.c -ggdb -I../arm/
	./arm64gen > bytecode.bin
	$(ARMV8PFX)-as -o bytecoderef.o arm64gen.S
	$(ARMV8PFX)-objcopy -O binary bytecoderef.o bytecoderef.bin
	@ cmp bytecoderef.bin bytecode.bin || echo "Bytecode mismatch"
	@ cmp bytecoderef.bin bytecode.bin && echo "Test passed!"
	gcc -o mipsgen mipsgen.c -ggdb -I../mips/
	./mipsgen > bytecode.bin
	$(MIPS32PFX)-as -EL -o bytecoderef.o mipsgen.S
	$(MIPS32PFX)-objcopy -j .text -O binary bytecoderef.o bytecoderef.bin
	@ cmp bytecoderef.bin bytecode.bin || echo "Bytecode mismatch"
	@ cmp bytecoderef.bin bytecode.bin && echo "Test passed!"

include host.mk

CORE := ..
HOST_FLAGS := -std=c++11 -O1 -ffunction-sections -fdata-sections \
    -D__LIBRETRO__ -DFRONTEND_SUPPORTS_RGB565 -DINLINE=inline \
    -I$(CORE) -I$(CORE)/include -I$(CORE)/libretro \
    -I$(CORE)/libretro/libretro-common/include
ifeq ($(shell uname -s),Darwin)
DEAD_STRIP := -Wl,-dead_strip
else
DEAD_STRIP := -Wl,--gc-sections
endif
CPU_TESTS := arm_alu_interpreter arm_block_transfer shift_interpreter
CPU_BINS := $(addprefix $(BUILD)/,$(CPU_TESTS))
HOST_SUITES := game-switch core-regressions

.PHONY: check check-cpu check-sanitize
check: check-cpu
	@set -e; for suite in $(HOST_SUITES); do $(MAKE) -C $$suite test; done

check-cpu: $(CPU_BINS)
	@set -e; for test in $(CPU_BINS); do $$test; done

$(BUILD)/%: %.cc $(CORE)/cpu.cc $(wildcard $(CORE)/*.h) host.mk Makefile
	@mkdir -p $(BUILD)
	$(HOST_CXX) $(TEST_FLAGS) $(HOST_FLAGS) $< $(DEAD_STRIP) -o $@

check-sanitize:
	$(MAKE) check SANITIZE=1

.DEFAULT_GOAL := test
include ../host.mk
include ../core.mk

.PHONY: test
test: $(BUILD)/test_core
	mkdir -p $(BUILD)/roms
	$(BUILD)/test_core

$(BUILD)/test_core: test_core.c $(CORE_DIR)/sound.c $(filter-out $(BUILD)/obj/sound.o,$(OBJECTS)) Makefile ../core.mk ../host.mk
	$(HOST_CC) $(TEST_FLAGS) $(FLAGS) $(DEPFLAGS) -MF $@.d $< $(filter-out $(BUILD)/obj/sound.o,$(OBJECTS)) -o $@

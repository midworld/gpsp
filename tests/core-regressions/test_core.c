/* Core regression tests.
 *
 * Copyright (C) 2026 Midworld Kim
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "common.h"
#include <assert.h>

static u8 state[GBA_STATE_MEM_SIZE];
static u8 damaged[GBA_STATE_MEM_SIZE];
static u8 before[GBA_STATE_MEM_SIZE];
static u8 after[GBA_STATE_MEM_SIZE];

static bool environment(unsigned cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE)
    {
        struct retro_variable *var = data;
        var->value = NULL;
        if (!strcmp(var->key, "gpsp_bios"))
            var->value = "builtin";
        if (!strcmp(var->key, "gpsp_boot_mode"))
            var->value = "game";
        return var->value != NULL;
    }
    return cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT;
}

static void video(const void *data, unsigned width, unsigned height, size_t pitch)
{
    (void)data; (void)width; (void)height; (void)pitch;
}

static size_t audio(const int16_t *data, size_t count)
{
    (void)data;
    return count;
}

static void poll(void) {}
static int16_t input(unsigned port, unsigned device, unsigned index, unsigned id)
{
    (void)port; (void)device; (void)index; (void)id;
    return 0;
}

static void save(u8 *dst)
{
    memset(dst, 0, GBA_STATE_MEM_SIZE);
    assert(retro_serialize(dst, GBA_STATE_MEM_SIZE));
}

static u8 *field(u8 *doc, const char *key)
{
    u8 *p = (u8 *)bson_find_key(doc, key);
    assert(p);
    return p;
}

static void put32(u8 *p, u32 value)
{
    bson_write_u32(p, value);
}

static void rejected(void)
{
    save(before);
    assert(!retro_unserialize(damaged, sizeof(damaged)));
    save(after);
    assert(!memcmp(before, after, sizeof(before)));
}

static void test_state(void)
{
    save(state);
    reg[0] = 0x12345678;
    assert(retro_unserialize(state, sizeof(state)));
    assert(reg[0] != 0x12345678);
    assert(!retro_unserialize(state, sizeof(state) - 1));

    memcpy(damaged, state, sizeof(state));
    put32(damaged, 4);
    rejected();
    memcpy(damaged, state, sizeof(state));
    put32(field(damaged, "cpu"), sizeof(state));
    rejected();
    memcpy(damaged, state, sizeof(state));
    damaged[4] = 0x7f;
    rejected();
    memcpy(damaged, state, sizeof(state));
    u8 *regs = field(field(damaged, "cpu"), "regs");
    regs[4] = BSON_TYPE_DOC;
    rejected();
    memcpy(damaged, state, sizeof(state));
    regs = field(field(damaged, "cpu"), "regs");
    put32(regs + 8 + CPU_MODE * 8, 0xffffffff);
    rejected();
    memcpy(damaged, state, sizeof(state));
    put32(field(field(field(damaged, "sound"), "ds1"), "fifo-base"), 32);
    rejected();
    memcpy(damaged, state, sizeof(state));
    put32(field(field(field(damaged, "sound"), "gs3"), "rate"), 2048);
    rejected();
    memcpy(damaged, state, sizeof(state));
    put32(field(field(damaged, "sound"), "wav-samples"), 1);
    rejected();
    memcpy(damaged, state, sizeof(state));
    u8 *snd = field(damaged, "sound");
    snd[-(int)sizeof("sound") - 1] = BSON_TYPE_BIN;
    rejected();
    puts("state: PASS (round trip, malformed documents, no partial mutation)");
}

static void test_irq(void)
{
    reset_gba();
    reg[REG_CPSR] &= ~0x80u;
    reg[REG_PC] = 0x08000100;
    write_ioreg(REG_IE, IRQ_TIMER0);
    write_ioreg(REG_IME, 1);
    flag_interrupt(IRQ_TIMER0);
    assert(check_and_raise_interrupts());
    assert(reg[REG_PC] == 0x18 && reg[CPU_MODE] == MODE_IRQ);
    assert(REG_MODE(MODE_IRQ)[6] == 0x08000104);
    assert(take_irq_entry_cycles() == 3);
    assert(take_irq_entry_cycles() == 0);

    /* An entry cost larger than the next budget carries into the next call. */
    irq_entry_cycles = 3;
    execute_cycles = 1;
    assert(frame_start_cycles() == 0);
    execute_cycles = 10;
    assert(frame_start_cycles() == 8);
    assert(frame_start_cycles() == 10);
    reset_gba();
    assert(execute_cycles == 1006);
    write_ioreg(REG_DISPSTAT, 0x10);
    update_gba(0);
    assert(read_ioreg(REG_DISPSTAT) & 2);
    assert(!(read_ioreg(REG_IF) & IRQ_HBLANK));
    assert(execute_cycles == 3);
    update_gba(0);
    assert(read_ioreg(REG_IF) & IRQ_HBLANK);

    puts("IRQ: PASS (vector, return address, entry cost, carried stall)");
}

int main(void)
{
    static const u8 rom[32768] = {0xfe, 0xff, 0xff, 0xea}; /* b . */
    const char *path = ROM_DIR "/regression.gba";
    FILE *file = fopen(path, "wb");
    assert(file && fwrite(rom, 1, sizeof(rom), file) == sizeof(rom));
    assert(!fclose(file));
    struct retro_game_info game = { .path = path };
    retro_set_environment(environment);
    retro_set_video_refresh(video);
    retro_set_audio_sample_batch(audio);
    retro_set_input_poll(poll);
    retro_set_input_state(input);
    retro_init();
    assert(retro_load_game(&game));

    test_state();
    test_irq();
    retro_unload_game();
    retro_deinit();
    return 0;
}

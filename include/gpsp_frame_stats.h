/* Core work counters and between-frame hooks for frontends.
 *
 * Copyright (C) 2026 Midworld Kim
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef GPSP_FRAME_STATS_H
#define GPSP_FRAME_STATS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The counters wrap at 32 bits; use differences. They read zero where
   they are not counted. */

/** Blocks translated and CPU cycles spent translating them since start. */
void gpsp_jit_translation_stats(uint32_t *blocks, uint32_t *cycles);

/** Does deferred JIT table work. Call between frames that left time. */
void gpsp_jit_upkeep(void);

#ifdef __cplusplus
}
#endif

#endif /* GPSP_FRAME_STATS_H */

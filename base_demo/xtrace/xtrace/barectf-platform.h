/*
 * Copyright (c) 2022 ACOINTO Team.
 * All rights reserved.
 *
 * Detailed license information can be found in the LICENSE file.
 *
 * File: barectf-platform.h platform of barectf
 *
 * Author: Jiang.Taijin <jiangtaijin@acoinfo.com>
 *
 */

#ifndef BARECTF_PLATFORM_H
#define BARECTF_PLATFORM_H

#include <stdint.h>
#include "barectf.h"

/* Memory alloc and free */
#define trace_alloc(size)    __SHEAP_ALLOC(size)
#define trace_safe_free(ptr) { if (ptr) { __SHEAP_FREE((PVOID)ptr); ptr = 0; } }

/* Buffer status */
#define BUFFER_IN_USING 0x80000000
#define BUFFER_EMPTY    (~BUFFER_IN_USING)

/* Event buffer */
struct barectf_platform_buffer {
    uint8_t       *buffer;
    unsigned long *buffer_cpu;
    unsigned int   buffer_size;
    unsigned int   package_size;
    unsigned int   package_cnt;
    int            cur_buffer;
    int            unsaved_buffer;
    int            overwrite;
    LW_SPINLOCK_DEFINE (lock);
};

/* Event buffer lock */
#define trace_buffer_lock(buffer)   LW_SPIN_LOCK(&buffer->lock);
#define trace_buffer_unlock(buffer) LW_SPIN_UNLOCK(&buffer->lock);

/* Barectf context */
struct barectf_platform_ctx {
    struct         barectf_default_ctx ctx;
    unsigned long  cpu_id;
    struct barectf_platform_buffer *ctx_buffer;
};

/* Barectf context lock */
#define trace_lock(ctx, ireg)   ireg = KN_INT_DISABLE()
#define trace_unlock(ctx, ireg) KN_INT_ENABLE(ireg)

/* Initializes the platform. */
struct barectf_platform_ctx *barectf_platform_init(struct barectf_platform_buffer *ctx_buffer,
                                                   unsigned int cpu_id);

/* Finalizes the platform. */
void barectf_platform_fini(struct barectf_platform_ctx *ctx);

/* Stop the trace. */
void barectf_platform_stop(struct barectf_platform_ctx *ctx);

/* Returns the barectf stream-specific context of a given platform context. */
struct barectf_default_ctx *barectf_platform_get_barectf_ctx(struct barectf_platform_ctx *ctx);

/* Save the streams in files. */
int barectf_platform_save(struct barectf_platform_ctx *ctx[], int fd[], int flush);

#endif /* BARECTF_PLATFORM_H */

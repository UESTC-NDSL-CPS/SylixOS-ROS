/*
 * Copyright (c) 2022 ACOINTO Team.
 * All rights reserved.
 *
 * Detailed license information can be found in the LICENSE file.
 *
 * File: barectf-platform.c platform of barectf
 *
 * Author: Jiang.Taijin <jiangtaijin@acoinfo.com>
 *
 */

#define  __SYLIXOS_KERNEL
#include <SylixOS.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "barectf.h"
#include "barectf-platform.h"

/* Return system clock in ns */
static uint64_t get_clock(void* data)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);

    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/* The buffer is full? */
static int is_backend_full(void *data)
{
    struct barectf_platform_ctx    *ctx = data;
    struct barectf_platform_buffer *ctx_buffer = ctx->ctx_buffer;
    int    next_buffer;

    trace_buffer_lock(ctx_buffer);
    if (ctx_buffer->overwrite ||
        (ctx_buffer->buffer_cpu[ctx_buffer->cur_buffer] == BUFFER_EMPTY)) {
        return 0; /* Keep lock until open_packet is called */

    } else {
        next_buffer = (ctx_buffer->cur_buffer + 1) % ctx_buffer->package_cnt;

        while ((next_buffer != ctx_buffer->unsaved_buffer) &&
               (next_buffer != ctx_buffer->cur_buffer)) {
            if (ctx_buffer->buffer_cpu[next_buffer] == BUFFER_EMPTY) {
                return 0; /* Keep lock until open_packet is called */
            }

            if (!(ctx_buffer->buffer_cpu[next_buffer] & BUFFER_IN_USING)) {
                break;
            }

            next_buffer = (next_buffer + 1) % ctx_buffer->package_cnt;
        }

        trace_buffer_unlock(ctx_buffer);
        return 1;
    }
}

/* Alloc a buffer and open a new packet */
static void open_packet(void *data)
{
    struct barectf_platform_ctx    *ctx = data;
    struct barectf_platform_buffer *ctx_buffer = ctx->ctx_buffer;

    /* Find the first buffer which is empty or which can be overwrited */
    if (ctx_buffer->buffer_cpu[ctx_buffer->cur_buffer] != BUFFER_EMPTY) {
        ctx_buffer->cur_buffer = (ctx_buffer->cur_buffer + 1) % ctx_buffer->package_cnt;
    }

    while(ctx_buffer->buffer_cpu[ctx_buffer->cur_buffer] & BUFFER_IN_USING) {
        ctx_buffer->cur_buffer = (ctx_buffer->cur_buffer + 1) % ctx_buffer->package_cnt;
    }

    ctx_buffer->buffer_cpu[ctx_buffer->cur_buffer] = (ctx->cpu_id | BUFFER_IN_USING);
    barectf_packet_set_buf(&ctx->ctx,
                           ctx_buffer->buffer + (ctx_buffer->cur_buffer * ctx_buffer->package_size),
                           ctx_buffer->package_size);
    barectf_default_open_packet(&ctx->ctx, ctx->cpu_id);

    trace_buffer_unlock(ctx_buffer);
}

/* Close the current packet of the barectf stream */
static void close_packet(void *data)
{
    struct barectf_platform_ctx    *ctx = data;
    struct barectf_platform_buffer *ctx_buffer = ctx->ctx_buffer;
    int    buffer_index;

    uint8_t *buffer = barectf_packet_buf(&ctx->ctx);

    trace_buffer_lock(ctx_buffer);

    buffer_index = (buffer - ctx_buffer->buffer) / ctx_buffer->package_size;
    ctx_buffer->buffer_cpu[buffer_index] = ctx->cpu_id;

    /* close packet now */
    barectf_default_close_packet(&ctx->ctx);

    trace_buffer_unlock(ctx_buffer);
}

/* Initializes the platform. */
struct barectf_platform_ctx *barectf_platform_init(struct barectf_platform_buffer *ctx_buffer,
                                                   unsigned int cpu_id)
{
    struct barectf_platform_ctx      *ctx;
    struct barectf_platform_callbacks cbs = {
        .perf_counter_clock_get_value = get_clock,
        .is_backend_full              = is_backend_full,
        .open_packet                  = open_packet,
        .close_packet                 = close_packet,
    };

    ctx = trace_alloc(sizeof(*ctx));
    if (!ctx) {
        return NULL;
    }

    ctx->ctx_buffer = ctx_buffer;
    ctx->cpu_id     = cpu_id;

    barectf_init(&ctx->ctx,
                 ctx_buffer->buffer + (ctx_buffer->cur_buffer * ctx_buffer->package_size),
                 ctx_buffer->package_size,
                 cbs,
                 ctx);

    if (is_backend_full(ctx)) {
        return NULL;
    }
    open_packet(ctx);

    return ctx;
}

/* Stop the trace. */
void barectf_platform_stop(struct barectf_platform_ctx *ctx)
{
    if (barectf_packet_is_open(&ctx->ctx) &&
        !barectf_packet_is_empty(&ctx->ctx)) {
        close_packet(ctx);
    }
}

/* Finalizes the platform. */
void barectf_platform_fini(struct barectf_platform_ctx *ctx)
{
    if (barectf_packet_is_open(&ctx->ctx) &&
        !barectf_packet_is_empty(&ctx->ctx)) {
        close_packet(ctx);
    }

    trace_safe_free(ctx);
}

/* Returns the barectf stream-specific context of a given platform context. */
struct barectf_default_ctx *barectf_platform_get_barectf_ctx(
    struct barectf_platform_ctx *ctx)
{
    return &ctx->ctx;
}

/* Save the streams in files. */
int barectf_platform_save(struct barectf_platform_ctx *ctx[], int fd[], int flush)
{
    struct barectf_platform_buffer *ctx_buffer = ctx[0]->ctx_buffer;
    unsigned long buffer_status;
    unsigned long cpu_id;

    trace_buffer_lock(ctx_buffer);
    ctx_buffer->unsaved_buffer = (ctx_buffer->cur_buffer + 1) % ctx_buffer->package_cnt;
    trace_buffer_unlock(ctx_buffer);
    while (ctx_buffer->unsaved_buffer != ctx_buffer->cur_buffer) {
        buffer_status = ctx_buffer->buffer_cpu[ctx_buffer->unsaved_buffer];
        cpu_id        = buffer_status & BUFFER_EMPTY;

        if (buffer_status == BUFFER_EMPTY) {
            trace_buffer_lock(ctx_buffer);
            ctx_buffer->unsaved_buffer = (ctx_buffer->unsaved_buffer + 1) % ctx_buffer->package_cnt;
            trace_buffer_unlock(ctx_buffer);
            continue;
        }

        if ((buffer_status & BUFFER_IN_USING)) {
            if (flush) {
                if (barectf_packet_is_open(&ctx[cpu_id]->ctx)) {
                    if (!barectf_packet_is_empty(&ctx[cpu_id]->ctx)) {
                        close_packet(&ctx[cpu_id]->ctx);
                    } else {
                        trace_buffer_lock(ctx_buffer);
                        ctx_buffer->unsaved_buffer = (ctx_buffer->unsaved_buffer + 1) % ctx_buffer->package_cnt;
                        trace_buffer_unlock(ctx_buffer);
                        continue;
                    }
                }
            } else {
                trace_buffer_lock(ctx_buffer);
                ctx_buffer->unsaved_buffer = (ctx_buffer->unsaved_buffer + 1) % ctx_buffer->package_cnt;
                trace_buffer_unlock(ctx_buffer);
                continue;
            }
        }

        write(fd[cpu_id],
              ctx_buffer->buffer + (ctx_buffer->unsaved_buffer * ctx_buffer->package_size),
              ctx_buffer->package_size);

        trace_buffer_lock(ctx_buffer);
        ctx_buffer->buffer_cpu[ctx_buffer->unsaved_buffer] = BUFFER_EMPTY;
        ctx_buffer->unsaved_buffer = (ctx_buffer->unsaved_buffer + 1) % ctx_buffer->package_cnt;
        trace_buffer_unlock(ctx_buffer);
    }

    buffer_status = ctx_buffer->buffer_cpu[ctx_buffer->unsaved_buffer];
    cpu_id        = buffer_status & BUFFER_EMPTY;

    if (buffer_status != BUFFER_EMPTY) {
        if (buffer_status & BUFFER_IN_USING) {
            if (flush) {
                if (barectf_packet_is_open(&ctx[cpu_id]->ctx)) {
                    if (!barectf_packet_is_empty(&ctx[cpu_id]->ctx)) {
                        close_packet(&ctx[cpu_id]->ctx);
                    } else {
                        trace_buffer_lock(ctx_buffer);
                        ctx_buffer->unsaved_buffer = -1;
                        trace_buffer_unlock(ctx_buffer);
                        return 0;
                    }
                }
            } else {
                trace_buffer_lock(ctx_buffer);
                ctx_buffer->unsaved_buffer = -1;
                trace_buffer_unlock(ctx_buffer);
                return 0;
            }
        }

        write(fd[cpu_id],
              ctx_buffer->buffer + (ctx_buffer->unsaved_buffer * ctx_buffer->package_size),
              ctx_buffer->package_size);
        ctx_buffer->buffer_cpu[ctx_buffer->unsaved_buffer] = BUFFER_EMPTY;
    }

    trace_buffer_lock(ctx_buffer);
    ctx_buffer->unsaved_buffer = -1;
    trace_buffer_unlock(ctx_buffer);

    return 0;
}

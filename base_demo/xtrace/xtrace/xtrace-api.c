/*
 * Copyright (c) 2022 ACOINTO Team.
 * All rights reserved.
 *
 * Detailed license information can be found in the LICENSE file.
 *
 * File: xtrace-api.c trace module API
 *
 * Author: Jiang.Taijin <jiangtaijin@acoinfo.com>
 *
 */

#define  __SYLIXOS_KERNEL
#include <SylixOS.h>
#include <module.h>
#include <string.h>
#include <stdio.h>
#include "barectf-platform.h"
#include "xtrace-api.h"

/* LW_CFG_MONITOR_EN must be enabled */
#if LW_CFG_MONITOR_EN == 0
#warning "LW_CFG_MONITOR_EN must be setted 1 in SylixOS kernel"
#define API_MonitorTraceCreate(m, t, c, b) LW_NULL
#define API_MonitorTraceDelete(m)
#define MONITOR_COLLECTOR_ROUTINE void*
#endif

/* CTF package size */
#define CTF_PACKAGE_SIZE 4096

/* Event type count */
#define EVENT_TYPE_COUNT 64

/* Metadata file path and name */
#define METADATA_PATH "/etc/trace/metadata"
#define METADATA_NAME "metadata"

/* Thread status(used in lock event) in tracecompass */
enum state_t {
    NEW,
    TERMINATED,
    READY,
    RUNNING,
    WAITING,
};

/* Thread status(used in switch event) in tracecompass */
#define TASK_WAIT     0x1
#define TASK_DEAD     0x40
#define TASK_RUNNING  0x0

/* IO operator in tracecompass */
#define RWBS_FLAG_WRITE     (1 << 0)
#define RWBS_FLAG_READ      (1 << 2)
#define RWBS_FLAG_RAHEAD    (1 << 3)
#define RWBS_FLAG_FLUSH     (1 << 8)
#define RWBS_FLAG_PREFLUSH  (1 << 10)

/* Lock operator in tracecompass */
#define WAIT        128
#define WAIT_BITSET 137
#define LOCK_PI     134
#define TRYLOCK_PI  136
#define WAKE        129
#define WAKE_BITSET 138
#define UNLOCK_PI   135
#define WAKE_OP     133

/* DSP do not support symbol export */
#if defined(LW_CFG_CPU_ARCH_C6X)
#undef  LW_SYMBOL_EXPORT
#define LW_SYMBOL_EXPORT
#endif

/* Barectf objects */
static struct barectf_platform_ctx  **ctx_arr = LW_NULL;
static struct barectf_platform_buffer ctx_buffer;

/* Monitor handle */
static void *trace_monitor = LW_NULL;

/* Trace mode */
static int trace_mode = TRACE_TYPE_MEMORY;

/* Directory to save ctf streams */
static char save_dir[PATH_MAX] = {0};

/* Thread to save event to file */
static LW_OBJECT_HANDLE save_thread = LW_OBJECT_HANDLE_INVALID;

/* Event filter */
static unsigned long long event_filter[EVENT_TYPE_COUNT];

/* Record event in ctf stream */
static void trace_event(void *monitor, uint32_t event_id, uint32_t sub_event_id,
                        void *args, size_t size, char *addtional)
{
    long *l_args = args;
    INTREG int_level;

    if (!(event_filter[event_id] & (1 << sub_event_id))) { /* Using event filter */
        return;
    }

    trace_lock(ctx_arr[LW_CPU_GET_CUR_ID()], int_level);

    /* Dispatch events */
    switch (event_id) {
    case MONITOR_EVENT_ID_SCHED:
    {
        if (l_args[3] & LW_THREAD_STATUS_WDEATH) {
            l_args[3] = TASK_DEAD;
        } else if (l_args[3] != LW_THREAD_STATUS_RDY) {
            l_args[3] = TASK_WAIT;
        }

        barectf_trace_sched_switch((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                                   (int8_t *)l_args[0], l_args[1], l_args[2], l_args[3],
                                   (int8_t *)l_args[4], l_args[5], l_args[6]);
        break;
    }

    case MONITOR_EVENT_ID_INT:
    {
        char name[16] = {0};
        snprintf(name, 6, "int%d", (int)l_args[0]);

        switch(sub_event_id) {
        case MONITOR_EVENT_INT_ENTER:
            barectf_trace_irq_handler_entry((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                                            (int32_t)l_args[0], name);
            break;
        case MONITOR_EVENT_INT_EXIT:
            barectf_trace_irq_handler_exit((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                                           (int32_t)l_args[0], (int32_t)l_args[1]);
            break;
        }
        break;
    }

    case MONITOR_EVENT_ID_REGION:
    {
        switch(sub_event_id) {
        case MONITOR_EVENT_REGION_ALLOC:
            barectf_trace_kmem_mm_page_alloc((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                    (int64_t)l_args[1], 0, (int64_t)l_args[2], 0, 0);
            break;
        case MONITOR_EVENT_REGION_REALLOC:
            barectf_trace_kmem_mm_page_alloc((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                    (int64_t)l_args[2], 0, (int64_t)l_args[3], 0, 0);
            break;
        case MONITOR_EVENT_REGION_FREE:
            barectf_trace_kmem_mm_page_free((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                    (int64_t)l_args[1], 0, (int64_t)l_args[2]);
            break;
        }
        break;
    }

    case MONITOR_EVENT_ID_IO:
    {
        switch(sub_event_id) {
        case MONITOR_EVENT_DISK_WRITE:
            barectf_trace_block_rq_insert((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                                           0, l_args[0], (uint32_t)l_args[1], (l_args[1] << 9),
                                           (int32_t)l_args[2], RWBS_FLAG_WRITE, (int8_t *)l_args[3]);
            barectf_trace_block_rq_complete((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                                           0, l_args[0], (uint32_t)l_args[1], 0, RWBS_FLAG_WRITE);
            break;
        case MONITOR_EVENT_DISK_READ:
            barectf_trace_block_rq_insert((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                                           0, l_args[0], (uint32_t)l_args[1], (l_args[1] << 9),
                                           (int32_t)l_args[2], RWBS_FLAG_READ, (int8_t *)l_args[3]);
            barectf_trace_block_rq_complete((struct barectf_default_ctx *)ctx_arr[LW_CPU_GET_CUR_ID()],
                                           0, l_args[0], (uint32_t)l_args[1], 0, RWBS_FLAG_READ);
            break;
        }
        break;
    }

    default:
        break;;
    }

    trace_unlock(ctx_arr[LW_CPU_GET_CUR_ID()], int_level);
}

/* Copy file */
static int copy_file(const char *src_path, const char *dst_path)
{
    int  fd_src, fd_dst;
    int  len;
    char buff[1024];

    fd_src = open(src_path, O_RDONLY);
    fd_dst = open(dst_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if ((fd_src >= 0) && (fd_dst >= 0)) {
        while((len = read(fd_src, (void *)buff, 1024)) > 0) {
            write(fd_dst,buff,len);
        }
    } else {
        return (TRACE_ERROR);
    }

    if (fd_src >= 0) {
        close(fd_src);
    }
    if (fd_dst >= 0) {
        close(fd_dst);
    }

    return (TRACE_OK);
}

/* Init the trace */
LW_SYMBOL_EXPORT int trace_create(int mode, unsigned int buffer_size, const char *dir)
{
    int i;

    trace_mode = mode;
    ctx_buffer.buffer_size = buffer_size;
    ctx_buffer.package_size = CTF_PACKAGE_SIZE;
    ctx_buffer.package_cnt = ctx_buffer.buffer_size / ctx_buffer.package_size;
    ctx_buffer.overwrite = (trace_mode == TRACE_TYPE_MEMORY);
    ctx_buffer.cur_buffer = 0;
    ctx_buffer.unsaved_buffer = -1;
    LW_SPIN_INIT(&ctx_buffer.lock);

    if (ctx_arr != LW_NULL) {
        printk("the trace is duplicate created!\n");
        return  (TRACE_ERROR);
    }

    if (trace_mode > TRACE_TYPE_NETWORK) {
        printk("trace mode error!\n");
        return  (TRACE_ERROR);
    }

    if (ctx_buffer.package_cnt < (LW_NCPUS * 2)) {
        printk("buffer size too low!\n");
        return  (TRACE_ERROR);
    }

    if (dir) {
        strncpy(save_dir, dir, sizeof(save_dir));
    }

    ctx_arr = trace_alloc(LW_NCPUS * sizeof(struct barectf_platform_ctx *));
    if (!ctx_arr) {
        printk("low memory!\n");
        return  (TRACE_ERROR);
    }

    ctx_buffer.buffer = trace_alloc(ctx_buffer.buffer_size);
    if (!ctx_buffer.buffer) {
        printk("low memory!\n");
        return  (TRACE_ERROR);
    }

    ctx_buffer.buffer_cpu = trace_alloc(ctx_buffer.package_cnt * sizeof(unsigned long));
    if (!ctx_buffer.buffer_cpu) {
        printk("low memory!\n");
        return  (TRACE_ERROR);
    }
    for (i = 0; i < ctx_buffer.package_cnt; i++) {
        ctx_buffer.buffer_cpu[i] = BUFFER_EMPTY;
    }

    for (i = 0; i < EVENT_TYPE_COUNT; i++) {
        event_filter[i] = 0;
    }

    for (i = 0; i < LW_NCPUS; i++) {
        ctx_arr[i] = barectf_platform_init(&ctx_buffer,
                                           i);
        if (!ctx_arr[i]) {
            break;
        }
    }

    if (i < LW_NCPUS) {
        i--;
        for (; i >= 0; i--) {
            barectf_platform_fini(ctx_arr[i]);
        }

        trace_safe_free(ctx_arr);
        trace_safe_free(ctx_buffer.buffer_cpu);
        trace_safe_free(ctx_buffer.buffer);

        return  (TRACE_ERROR);
    } else {
        return  (TRACE_OK);
    }
}

/* Destroy the trace */
LW_SYMBOL_EXPORT void trace_destroy(void)
{
    int i;

    if (!ctx_arr) {
        printk("the trace is not created!\n");
        return;
    }

    if (trace_monitor) {
        printk("the trace is active!\n");
        return;
    }

    for (i = 0; i < LW_NCPUS; i++) {
        barectf_platform_fini(ctx_arr[i]);
    }

    trace_safe_free(ctx_arr);
    trace_safe_free(ctx_buffer.buffer_cpu);
    trace_safe_free(ctx_buffer.buffer);
}

/* Save ctf streams in files */
LW_SYMBOL_EXPORT void *trace_save(const char *path)
{
    int  i;
    char stream_path[PATH_MAX] = {0};
    int  fd[LW_NCPUS];

    if (!ctx_arr) {
        printk("the trace is not created!\n");
        return  (LW_NULL);
    }

    if ((trace_mode == TRACE_TYPE_MEMORY) && trace_monitor) {
        printk("the trace is already active!\n");
        return  (LW_NULL);
    }

    snprintf(stream_path, PATH_MAX, "%s/%s", path, METADATA_NAME);
    if (copy_file(METADATA_PATH, stream_path) != TRACE_OK) {
        printk("copy metadate file error!\n");
    }

    for (i = 0; i < LW_NCPUS; i++) {
        fd[i] = -1;
    }

    for (i = 0; i < LW_NCPUS; i++) {
        snprintf(stream_path, PATH_MAX, "%s/channal%d", path, i);
        fd[i] = open(stream_path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
        if (fd[i] < 0) {
            printk("open trace file error!\n");
            goto __err;
        }
    }

    while (trace_monitor) {
        if (barectf_platform_save(ctx_arr, fd, LW_FALSE) != 0) {
            printk("trace save failed!\n");
        }

        usleep(10000);
    }

    if (barectf_platform_save(ctx_arr, fd, LW_TRUE) != 0) {
        printk("trace save failed!\n");
    }

__err:
    for (i = 0; i < LW_NCPUS; i++) {
        snprintf(stream_path, PATH_MAX, "%s/channal%d", path, i);
        if (fd[i] >= 0) {
            close(fd[i]);
        }
    }

    return  (LW_NULL);
}

/* Enable events in filter */
LW_SYMBOL_EXPORT void trace_enable(const char *event, int enable)
{
    if (strcmp(event, "all") == 0) {
        int i;
        for (i = 0; i < EVENT_TYPE_COUNT; i++) {
            event_filter[i] = (enable ? 0xffffffffffffffff : 0);
        }
    } else if (strcmp(event, "sched") == 0) {
        event_filter[MONITOR_EVENT_ID_SCHED] = (enable ? 0xffffffffffffffff : 0);

    } else if (strcmp(event, "int") == 0) {
        event_filter[MONITOR_EVENT_ID_INT] = (enable ? 0xffffffffffffffff : 0);

    } else if (strcmp(event, "memory") == 0) {
        event_filter[MONITOR_EVENT_ID_REGION] = (enable ? 0xffffffffffffffff : 0);

    } else if (strcmp(event, "disk") == 0) {
        event_filter[MONITOR_EVENT_ID_IO] = (enable ? 0xffffffffffffffff : 0);

    } else {
        printk("event type %s error!\n", event);
    }
}

/* Start trace event */
LW_SYMBOL_EXPORT int trace_start(void)
{
    if (!ctx_arr) {
        printk("the trace is not created!\n");
        return  (TRACE_ERROR);
    }

    if (trace_monitor) {
        printk("the trace is already active!\n");
        return  (TRACE_ERROR);
    }

    trace_monitor = API_MonitorTraceCreate(LW_NULL, (MONITOR_COLLECTOR_ROUTINE)trace_event, ctx_arr, LW_TRUE);
    if (!trace_monitor) {
        return  (TRACE_ERROR);
    }

    if (trace_mode == TRACE_TYPE_FILE) {
        LW_CLASS_THREADATTR threadattr;
        API_ThreadAttrBuild(&threadattr, (12 * LW_CFG_KB_SIZE),
                            LW_PRIO_CRITICAL,
                            LW_OPTION_THREAD_NO_MONITOR | LW_OPTION_OBJECT_GLOBAL,
                            save_dir);
        save_thread = API_ThreadCreate("t_trace_save", (PTHREAD_START_ROUTINE)trace_save,
                                       &threadattr, LW_NULL);
        if (save_thread == LW_OBJECT_HANDLE_INVALID) {
            printk("start trace save thread failed!\n");
            return  (TRACE_ERROR);
        }
        printk("trace save thread started!\n");
    }

    return 0;
}

/* Stop trace event */
LW_SYMBOL_EXPORT void trace_stop(void)
{
    int i;

    if (!ctx_arr) {
        printk("the trace is not created!\n");
        return;
    }

    if (!trace_monitor) {
        printk("the trace is not active!\n");
        return;
    }

    API_MonitorTraceDelete(trace_monitor);
    trace_monitor = LW_NULL;

    for (i = 0; i < LW_NCPUS; i++) {
        barectf_platform_stop(ctx_arr[i]);
    }

    if (save_thread != LW_OBJECT_HANDLE_INVALID) {
        API_ThreadJoin(save_thread, LW_NULL);
        save_thread = LW_OBJECT_HANDLE_INVALID;
    }
}

#if defined(LW_CFG_CPU_ARCH_C6X)
/*
 *  Register trace functions, this function should be called in dsp bsp.
 */
void trace_init(void)
{
    API_SymbolAdd("trace_create", (caddr_t)trace_create, LW_SYMBOL_FLAG_XEN);
    API_SymbolAdd("trace_destroy", (caddr_t)trace_destroy, LW_SYMBOL_FLAG_XEN);
    API_SymbolAdd("trace_start", (caddr_t)trace_start, LW_SYMBOL_FLAG_XEN);
    API_SymbolAdd("trace_stop", (caddr_t)trace_stop, LW_SYMBOL_FLAG_XEN);
    API_SymbolAdd("trace_enable", (caddr_t)trace_enable, LW_SYMBOL_FLAG_XEN);
    API_SymbolAdd("trace_save", (caddr_t)trace_save, LW_SYMBOL_FLAG_XEN);
}
#else
/*
 *  SylixOS call module_init() and module_exit() automatically.
 */
int module_init (void)
{
    return  (TRACE_OK);
}

void module_exit (void)
{
}
#endif

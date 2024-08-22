#ifndef _BARECTF_H
#define _BARECTF_H

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2020 Philippe Proulx <pproulx@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 * The following code was generated by barectf v3.0.1
 * on 2022-06-22T21:27:42.063940.
 *
 * For more details, see <https://barectf.org/>.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define barectf_trace_block_rq_complete barectf_default_trace_block_rq_complete
#define barectf_trace_block_rq_insert barectf_default_trace_block_rq_insert
#define barectf_trace_irq_handler_entry barectf_default_trace_irq_handler_entry
#define barectf_trace_irq_handler_exit barectf_default_trace_irq_handler_exit
#define barectf_trace_irq_softirq_entry barectf_default_trace_irq_softirq_entry
#define barectf_trace_irq_softirq_exit barectf_default_trace_irq_softirq_exit
#define barectf_trace_irq_softirq_raise barectf_default_trace_irq_softirq_raise
#define barectf_trace_kmem_mm_page_alloc barectf_default_trace_kmem_mm_page_alloc
#define barectf_trace_kmem_mm_page_free barectf_default_trace_kmem_mm_page_free
#define barectf_trace_lttng_statedump_process_state barectf_default_trace_lttng_statedump_process_state
#define barectf_trace_sched_pi_setprio barectf_default_trace_sched_pi_setprio
#define barectf_trace_sched_process_exit barectf_default_trace_sched_process_exit
#define barectf_trace_sched_process_fork barectf_default_trace_sched_process_fork
#define barectf_trace_sched_process_free barectf_default_trace_sched_process_free
#define barectf_trace_sched_process_wait barectf_default_trace_sched_process_wait
#define barectf_trace_sched_switch barectf_default_trace_sched_switch
#define barectf_trace_sched_wait_task barectf_default_trace_sched_wait_task
#define barectf_trace_sched_wakeup barectf_default_trace_sched_wakeup
#define barectf_trace_sched_wakeup_new barectf_default_trace_sched_wakeup_new
#define barectf_trace_syscall_entry_futex barectf_default_trace_syscall_entry_futex
#define barectf_trace_syscall_exit_futex barectf_default_trace_syscall_exit_futex
#define barectf_trace_timer_hrtimer_cancel barectf_default_trace_timer_hrtimer_cancel
#define barectf_trace_timer_hrtimer_expire_entry barectf_default_trace_timer_hrtimer_expire_entry
#define barectf_trace_timer_hrtimer_expire_exit barectf_default_trace_timer_hrtimer_expire_exit
#define barectf_trace_timer_hrtimer_start barectf_default_trace_timer_hrtimer_start

struct barectf_ctx;

uint32_t barectf_packet_size(const void *vctx);
int barectf_packet_is_full(const void *vctx);
int barectf_packet_is_empty(const void *vctx);
uint32_t barectf_packet_events_discarded(const void *vctx);
uint32_t barectf_discarded_event_records_count(const void * const vctx);
uint8_t *barectf_packet_buf(const void *vctx);
uint8_t *barectf_packet_buf_addr(const void * const vctx);
void barectf_packet_set_buf(void *vctx, uint8_t *buf, uint32_t buf_size);
uint32_t barectf_packet_buf_size(const void *vctx);
int barectf_packet_is_open(const void *vctx);
int barectf_is_in_tracing_section(const void *vctx);
volatile const int *barectf_is_in_tracing_section_ptr(const void *vctx);
int barectf_is_tracing_enabled(const void *vctx);
void barectf_enable_tracing(void *vctx, int enable);

/* barectf platform callbacks */
struct barectf_platform_callbacks {
	/* Clock source callbacks */
	uint64_t (*perf_counter_clock_get_value)(void *);

	/* Is the back end full? */
	int (*is_backend_full)(void *);

	/* Open packet */
	void (*open_packet)(void *);

	/* Close packet */
	void (*close_packet)(void *);
};

/* Common barectf context */
struct barectf_ctx {
	/* Platform callbacks */
	struct barectf_platform_callbacks cbs;

	/* Platform data (passed to callbacks) */
	void *data;

	/* Output buffer (will contain a CTF binary packet) */
	uint8_t *buf;

	/* Packet's total size (bits) */
	uint32_t packet_size;

	/* Packet's content size (bits) */
	uint32_t content_size;

	/* Current position from beginning of packet (bits) */
	uint32_t at;

	/* Size of packet header + context fields (content offset) */
	uint32_t off_content;

	/* Discarded event records counter snapshot */
	uint32_t events_discarded;

	/* Current packet is open? */
	int packet_is_open;

	/* In tracing code? */
	volatile int in_tracing_section;

	/* Tracing is enabled? */
	volatile int is_tracing_enabled;

	/* Use current/last event record timestamp when opening/closing packets */
	int use_cur_last_event_ts;
};

/* Context for data stream type `default` */
struct barectf_default_ctx {
	/* Parent */
	struct barectf_ctx parent;

	/* Config-specific members follow */
	uint32_t off_ph_magic;
	uint32_t off_ph_stream_id;
	uint32_t off_pc_packet_size;
	uint32_t off_pc_content_size;
	uint32_t off_pc_timestamp_begin;
	uint32_t off_pc_timestamp_end;
	uint32_t off_pc_events_discarded;
	uint64_t cur_last_event_ts;
};

/* Initialize context */
void barectf_init(void *vctx,
	uint8_t *buf, uint32_t buf_size,
	const struct barectf_platform_callbacks cbs, void *data);

/* Open packet for data stream type `default` */
void barectf_default_open_packet(
    struct barectf_default_ctx * const sctx, uint32_t cpu_id);

/* Close packet for data stream type `default` */
void barectf_default_close_packet(struct barectf_default_ctx *sctx);

/* Trace (data stream type `default`, event record type `block_rq_complete`) */
void barectf_default_trace_block_rq_complete(struct barectf_default_ctx *sctx,
	uint32_t p__dev,
	uint64_t p__sector,
	uint32_t p__nr_sector,
	int32_t p__error,
	uint32_t p__rwbs);

/* Trace (data stream type `default`, event record type `block_rq_insert`) */
void barectf_default_trace_block_rq_insert(struct barectf_default_ctx *sctx,
	uint32_t p__dev,
	uint64_t p__sector,
	uint32_t p__nr_sector,
	uint32_t p__bytes,
	int32_t p__tid,
	uint32_t p__rwbs,
	const int8_t *p__comm);

/* Trace (data stream type `default`, event record type `irq_handler_entry`) */
void barectf_default_trace_irq_handler_entry(struct barectf_default_ctx *sctx,
	int32_t p__irq,
	const char *p__name);

/* Trace (data stream type `default`, event record type `irq_handler_exit`) */
void barectf_default_trace_irq_handler_exit(struct barectf_default_ctx *sctx,
	int32_t p__irq,
	int32_t p__ret);

/* Trace (data stream type `default`, event record type `irq_softirq_entry`) */
void barectf_default_trace_irq_softirq_entry(struct barectf_default_ctx *sctx,
	int32_t p__vec);

/* Trace (data stream type `default`, event record type `irq_softirq_exit`) */
void barectf_default_trace_irq_softirq_exit(struct barectf_default_ctx *sctx,
	int32_t p__vec);

/* Trace (data stream type `default`, event record type `irq_softirq_raise`) */
void barectf_default_trace_irq_softirq_raise(struct barectf_default_ctx *sctx,
	int32_t p__vec);

/* Trace (data stream type `default`, event record type `kmem_mm_page_alloc`) */
void barectf_default_trace_kmem_mm_page_alloc(struct barectf_default_ctx *sctx,
	uint64_t p__page,
	uint64_t p__pfn,
	uint32_t p__order,
	uint32_t p__gfp_flags,
	int32_t p__migratetype);

/* Trace (data stream type `default`, event record type `kmem_mm_page_free`) */
void barectf_default_trace_kmem_mm_page_free(struct barectf_default_ctx *sctx,
	uint64_t p__page,
	uint64_t p__pfn,
	uint32_t p__order);

/* Trace (data stream type `default`, event record type `lttng_statedump_process_state`) */
void barectf_default_trace_lttng_statedump_process_state(struct barectf_default_ctx *sctx,
	int32_t p__tid,
	int32_t p__pid,
	const int8_t *p__name,
	uint32_t p__parent_ns_inum,
	int32_t p__type,
	int32_t p__mode,
	int32_t p__submode,
	int32_t p__status,
	uint32_t p__cpu,
	uint64_t p__file_table_address);

/* Trace (data stream type `default`, event record type `sched_pi_setprio`) */
void barectf_default_trace_sched_pi_setprio(struct barectf_default_ctx *sctx,
	const int8_t *p__comm,
	int32_t p__tid,
	int32_t p__oldprio,
	int32_t p__newprio);

/* Trace (data stream type `default`, event record type `sched_process_exit`) */
void barectf_default_trace_sched_process_exit(struct barectf_default_ctx *sctx,
	const int8_t *p__comm,
	int32_t p__tid,
	int32_t p__prio);

/* Trace (data stream type `default`, event record type `sched_process_fork`) */
void barectf_default_trace_sched_process_fork(struct barectf_default_ctx *sctx,
	const int8_t *p__parent_comm,
	int32_t p__parent_tid,
	int32_t p__parent_pid,
	uint32_t p__parent_ns_inum,
	const int8_t *p__child_comm,
	int32_t p__child_tid,
	uint8_t p___vtids_length,
	uint32_t p____vtids_len,
	const int32_t *p__vtids,
	int32_t p__child_pid,
	uint32_t p__child_ns_inum);

/* Trace (data stream type `default`, event record type `sched_process_free`) */
void barectf_default_trace_sched_process_free(struct barectf_default_ctx *sctx,
	const int8_t *p__comm,
	int32_t p__tid,
	int32_t p__prio);

/* Trace (data stream type `default`, event record type `sched_process_wait`) */
void barectf_default_trace_sched_process_wait(struct barectf_default_ctx *sctx,
	const int8_t *p__comm,
	int32_t p__tid,
	int32_t p__prio);

/* Trace (data stream type `default`, event record type `sched_switch`) */
void barectf_default_trace_sched_switch(struct barectf_default_ctx *sctx,
	const int8_t *p__prev_comm,
	int32_t p__prev_tid,
	int32_t p__prev_prio,
	int64_t p__prev_state,
	const int8_t *p__next_comm,
	int32_t p__next_tid,
	int32_t p__next_prio);

/* Trace (data stream type `default`, event record type `sched_wait_task`) */
void barectf_default_trace_sched_wait_task(struct barectf_default_ctx *sctx,
	const int8_t *p__comm,
	int32_t p__tid,
	int32_t p__prio);

/* Trace (data stream type `default`, event record type `sched_wakeup`) */
void barectf_default_trace_sched_wakeup(struct barectf_default_ctx *sctx,
	const int8_t *p__comm,
	int32_t p__tid,
	int32_t p__prio,
	int32_t p__target_cpu);

/* Trace (data stream type `default`, event record type `sched_wakeup_new`) */
void barectf_default_trace_sched_wakeup_new(struct barectf_default_ctx *sctx,
	const int8_t *p__comm,
	int32_t p__tid,
	int32_t p__prio,
	int32_t p__target_cpu);

/* Trace (data stream type `default`, event record type `syscall_entry_futex`) */
void barectf_default_trace_syscall_entry_futex(struct barectf_default_ctx *sctx,
	uint64_t p__uaddr,
	int32_t p__op,
	uint32_t p__val,
	uint64_t p__utime,
	uint64_t p__uaddr2,
	uint32_t p__val3);

/* Trace (data stream type `default`, event record type `syscall_exit_futex`) */
void barectf_default_trace_syscall_exit_futex(struct barectf_default_ctx *sctx,
	int64_t p__ret,
	uint64_t p__uaddr,
	uint64_t p__uaddr2);

/* Trace (data stream type `default`, event record type `timer_hrtimer_cancel`) */
void barectf_default_trace_timer_hrtimer_cancel(struct barectf_default_ctx *sctx,
	uint64_t p__hrtimer);

/* Trace (data stream type `default`, event record type `timer_hrtimer_expire_entry`) */
void barectf_default_trace_timer_hrtimer_expire_entry(struct barectf_default_ctx *sctx,
	uint64_t p__hrtimer,
	int64_t p__now,
	uint64_t p__function);

/* Trace (data stream type `default`, event record type `timer_hrtimer_expire_exit`) */
void barectf_default_trace_timer_hrtimer_expire_exit(struct barectf_default_ctx *sctx,
	uint64_t p__hrtimer);

/* Trace (data stream type `default`, event record type `timer_hrtimer_start`) */
void barectf_default_trace_timer_hrtimer_start(struct barectf_default_ctx *sctx,
	uint64_t p__hrtimer,
	uint64_t p__function,
	int64_t p__expires,
	int64_t p__softexpires,
	uint32_t p__mode);

#ifdef __cplusplus
}
#endif

#endif /* _BARECTF_H */

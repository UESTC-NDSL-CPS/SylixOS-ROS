/*
 * Copyright (c) 2022 ACOINTO Team.
 * All rights reserved.
 *
 * Detailed license information can be found in the LICENSE file.
 *
 * File: xtrace-api.h trace module API
 *
 * Author: Jiang.Taijin <jiangtaijin@acoinfo.com>
 *
 */

#ifndef TRACE_TRACE_API_H
#define TRACE_TRACE_API_H

#define TRACE_TYPE_MEMORY  0
#define TRACE_TYPE_FILE    1
#define TRACE_TYPE_NETWORK 2

#define TRACE_ERROR -1
#define TRACE_OK    0

int   trace_create(int mode, unsigned int buffer_size, const char *dir);
void  trace_destroy(void);
int   trace_start(void);
void  trace_stop(void);
void  trace_enable(const char *event, int enable);
void *trace_save(const char *path);

#if defined(LW_CFG_CPU_ARCH_C6X)
void  trace_init(void);
#endif

#endif /* TRACE_TRACE_API_H_ */

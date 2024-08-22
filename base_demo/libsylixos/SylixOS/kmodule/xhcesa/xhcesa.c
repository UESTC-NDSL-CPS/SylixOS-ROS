/**
 * @file
 * SylixOS Heterogeneous computing adapter kernel module.
 * Heterogeneous Computing power or Energy consumption priority schedule adapter.
 */

/*
 * Copyright (c) 2006-2023 SylixOS Group.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 4. This code has been or is applying for intellectual property protection
 *    and can only be used with acoinfo software products.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Han.hui <hanhui@acoinfo.com>
 *
 */

#define  __SYLIXOS_KERNEL
#include <module.h>
#include <unistd.h>
#include "param.h"
#include "xhcesa.h"

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)

/* Version */
#define XHCESA_VERSION  "0.1.1"

/* Strong affinity prio threshold */
extern int hcesa_sa_prio_thr;

/* Schedule parameter (index is cluster id) */
extern hcesa_param_t *hcesa_param;

/* Minimum threshold */
extern unsigned long hcesa_min_thr;

/* Initialize */
extern int init(void);

/* Thread sched block */
typedef struct thread_sched {
    struct thread_sched *next;
    LW_HANDLE tid;
    int hcc_mode;
    int weak_miss;
    int prio;
    unsigned long hccid;
    unsigned long consume;
    int up;
    int down;
} thread_sched_t;

/* Max threads */
static long max_threads;

/* Sched loop thread */
static LW_HANDLE thr_sched_loop;

/* Active thread list */
static thread_sched_t *thread_sched_list = NULL;

/* Thread sched block pool */
static thread_sched_t *thread_sched_pool = NULL;

/*
 * Create thread sched pool
 */
static int thread_sched_init (void)
{
    max_threads = sysconf(_SC_THREAD_THREADS_MAX);
    _BugHandle((max_threads <= 0), TRUE, "<hcesa> Max threads error!\n");

    thread_sched_pool = (thread_sched_t *)sys_malloc(sizeof(thread_sched_t) * max_threads);
    if (thread_sched_pool == NULL) {
        return  (-1);
    }

    return  (0);
}

/*
 * Get thread sched block
 */
static thread_sched_t *thread_sched_get (unsigned int index)
{
    if (index >= max_threads) {
        return  (NULL);
    } else {
        return  (&thread_sched_pool[index]);
    }
}

/*
 * Save max CPU used task
 */
static void thread_sched_save_max_used (unsigned long cur_hccid, thread_sched_t *pthd_sched)
{
    if (hcesa_param[cur_hccid].task_up) {
        if (pthd_sched->consume > ((thread_sched_t *)hcesa_param[cur_hccid].task_up)->consume) {
            hcesa_param[cur_hccid].task_up = pthd_sched;
        }
    } else {
        hcesa_param[cur_hccid].task_up = pthd_sched;
    }
}

/*
 * Save min CPU used task
 */
static void thread_sched_save_min_used (unsigned long cur_hccid, thread_sched_t *pthd_sched)
{
    if (hcesa_param[cur_hccid].task_down) {
        if (pthd_sched->consume < ((thread_sched_t *)hcesa_param[cur_hccid].task_down)->consume) {
            hcesa_param[cur_hccid].task_down = pthd_sched;
        }
    } else {
        hcesa_param[cur_hccid].task_down = pthd_sched;
    }
}

/*
 * Fill thread schedule struct
 */
static void thread_sched_fill (unsigned long cur_hccid, thread_sched_t *pthd_sched, PLW_CLASS_TCB_SCHED ptcbs)
{
    pthd_sched->tid      = ptcbs->TCBS_ulId;
    pthd_sched->prio     = ptcbs->TCBS_ucPriority;
    pthd_sched->hcc_mode = ptcbs->TCBS_iHetrccMode;
    pthd_sched->hccid    = cur_hccid;
    pthd_sched->consume  = ptcbs->TCBS_ulComputPow;
}

/*
 * Push thread schedule struct to list
 */
static void thread_sched_push (thread_sched_t *pthd_sched, int up, int down)
{
    pthd_sched->up   = up;
    pthd_sched->down = down;

    pthd_sched->next  = thread_sched_list;
    thread_sched_list = pthd_sched;
}

/*
 * All task walk callback
 */
static void thread_sched_walk (PLW_CLASS_TCB ptcb)
{
    unsigned long cur_hccid;

    PLW_CLASS_CPU pcpu;
    LW_CLASS_TCB_SCHED tcbs;
    thread_sched_t *pthd_sched;

    /*
     * Get TCB schedule info
     */
    _ThreadSchedInfo(ptcb, &tcbs);

    /*
     * Ignore special tasks
     */
    if ((tcbs.TCBS_ulId == thr_sched_loop) ||
        LW_PRIO_IS_EQU(tcbs.TCBS_ucPriority, LW_PRIO_EXTREME) ||
        LW_PRIO_IS_EQU(tcbs.TCBS_ucPriority, LW_PRIO_LOWEST)) {
        goto    next;
    }

    /*
     * Does not handle threads with numa, cpu or cluster strong affinity
     */
    if ((tcbs.TCBS_bCPULock) || (tcbs.TCBS_ulOption & LW_OPTION_THREAD_NUMA) ||
        (tcbs.TCBS_iHetrccMode == LW_HETRCC_STRONG_AFFINITY && !hcesa_sa_prio_thr)) {
        goto    next;
    }

    /*
     * Get this task cluster ID
     */
    if (tcbs.TCBS_iHetrccMode == LW_HETRCC_NON_AFFINITY) {
        cur_hccid = 0;
    } else {
        cur_hccid = tcbs.TCBS_ulHetrccLock;
    }

    /*
     * Less than the minimum computing migration threshold
     */
    if (LW_LIKELY(tcbs.TCBS_ulComputPow < hcesa_min_thr)) {
        /*
         * This task cluster full load ?
         */
        if (hcesa_param[cur_hccid].full_load) {
            /*
             * Get thread info block
             */
            pthd_sched = thread_sched_get(tcbs.TCBS_usIndex);
            if (LW_UNLIKELY(pthd_sched == NULL)) {
                goto    next;
            }

            /*
             * Save max and min CPU used task
             */
            thread_sched_fill(cur_hccid, pthd_sched, &tcbs);
            thread_sched_save_max_used(cur_hccid, pthd_sched);
            thread_sched_save_min_used(cur_hccid, pthd_sched);
            goto    next;

        } else {
            /*
             * Avoid tasks for non-full load clusters less than upgrade threshold
             */
            if (tcbs.TCBS_iHetrccMode == LW_HETRCC_NON_AFFINITY) {
                goto    next;
            }
        }
    }

    /*
     * Get thread info block
     */
    pthd_sched = thread_sched_get(tcbs.TCBS_usIndex);
    if (LW_UNLIKELY(pthd_sched == NULL)) {
        goto    next;
    }

    /*
     * Put this task to list
     */
    thread_sched_fill(cur_hccid, pthd_sched, &tcbs);
    thread_sched_push(pthd_sched, 0, 0);

    /*
     * Weak affinity did not succeed
     */
    if (pthd_sched->hcc_mode == LW_HETRCC_WEAK_AFFINITY) {
        pcpu = LW_CPU_GET(tcbs.TCBS_ulCPUId);
        if (pcpu->CPU_ulHccId != tcbs.TCBS_ulHetrccLock) {
            pthd_sched->weak_miss++;
        } else {
            pthd_sched->weak_miss = 0;
        }
    }

next:
    _ThreadClearHetrcc(ptcb);
}

/*
 * Full load detect
 */
static void thread_sched_fulload_detect (void)
{
    unsigned long i;

    PLW_CLASS_CPU pcpu;
    PLW_CLASS_TCB ptcb;

    LW_CLASS_TCB_SCHED tcbs;
    PLW_CLASS_HCCCPU phcccpu;

    LW_CPU_FOREACH_ACTIVE (i) {
        pcpu = LW_CPU_GET(i);
        ptcb = LW_TCB_GET_IDLE(i);

        /*
         * Get TCB schedule info
         */
        _ThreadSchedInfo(ptcb, &tcbs);

        if (tcbs.TCBS_ulComputPow < hcesa_param[pcpu->CPU_ulHccId].thd_down_thr) {

            /*
             * This CPU full load
             */
            hcesa_param[pcpu->CPU_ulHccId].load++;
            phcccpu = LW_HCCCPU_GET(pcpu->CPU_ulHccId);
            if (phcccpu->HCCCPU_uiActive == hcesa_param[pcpu->CPU_ulHccId].load) {

                /*
                 * All CPUs in the cluster are fully loaded
                 */
                hcesa_param[pcpu->CPU_ulHccId].full_load = 1;
            }
        }
    }
}

/*
 * Sched loop
 */
static void *sched_loop (void *arg)
{
    unsigned long i, cur_hccid;
    thread_sched_t *pthd_sched;

    /*
     * Clear all thread computing power records
     */
    __KERNEL_ENTER();
    _ThreadClearHetrcc(NULL);
    __KERNEL_EXIT();

    /*
     * Computing power-aware scheduling main loop
     */
    for (;;) {
        API_TimeMSleep(SCHED_DETECT_PERIOD);

        thread_sched_list = NULL;

        /*
         * Clear status
         */
        LW_HCCCPU_FOREACH (i) {
            hcesa_param[i].load      = 0;
            hcesa_param[i].full_load = 0;
            hcesa_param[i].task_up   = NULL;
            hcesa_param[i].task_down = NULL;
        }

        /*
         * Check cluster is full load ? (The idle task takes up insufficient computing power down_thr)
         */
        thread_sched_fulload_detect();

        /*
         * Traversing existing threads
         */
        __KERNEL_ENTER();
        _ThreadTraversal(thread_sched_walk, NULL, NULL, NULL, NULL, NULL, NULL);
        __KERNEL_EXIT();

        /*
         * Full load tasks
         */
        LW_HCCCPU_FOREACH (i) {
            if (hcesa_param[i].task_up) {
                if (hcesa_param[i].task_up == hcesa_param[i].task_down) {
                    thread_sched_push(hcesa_param[i].task_up,   1, 0);

                } else {
                    thread_sched_push(hcesa_param[i].task_up,   1, 0);
                    thread_sched_push(hcesa_param[i].task_down, 0, 1);
                }
            }
        }

        /*
         * Iterate through the list of threads that need to be tuned
         */
        for (pthd_sched = thread_sched_list;
             pthd_sched;
             pthd_sched = pthd_sched->next) {

            cur_hccid = pthd_sched->hccid;

            if (pthd_sched->up) {
                /*
                 * Full load upgrade task
                 */
                if (cur_hccid < (LW_NHETRCCS - 1)) {
                    API_ThreadSetHetrcc(pthd_sched->tid, LW_HETRCC_WEAK_AFFINITY, cur_hccid + 1);
                    pthd_sched->weak_miss = 0;
                }

            } else if (pthd_sched->down) {
                /*
                 * Full load downgrade task
                 */
                if (cur_hccid) {
                    API_ThreadSetHetrcc(pthd_sched->tid, LW_HETRCC_WEAK_AFFINITY, cur_hccid - 1);
                    pthd_sched->weak_miss = 0;
                }

            } else if (pthd_sched->consume < hcesa_min_thr) {
                /*
                 * Change task to free schedule
                 */
                API_ThreadSetHetrcc(pthd_sched->tid, LW_HETRCC_NON_AFFINITY, 0);
                pthd_sched->weak_miss = 0;

            } else if (pthd_sched->hcc_mode == LW_HETRCC_NON_AFFINITY) {
                /*
                 * Set weak affinity to the appropriate computing cluster.
                 */
                LW_HCCCPU_FOREACH (i) {
                    if (pthd_sched->consume < hcesa_param[i].thd_up_thr) {
                        API_ThreadSetHetrcc(pthd_sched->tid, LW_HETRCC_WEAK_AFFINITY, i);
                        pthd_sched->weak_miss = 0;
                        break;
                    }
                }

            } else if (pthd_sched->consume >= hcesa_param[cur_hccid].thd_up_thr) {
                /*
                 * Try to upgrade computing level
                 */
                if (cur_hccid < (LW_NHETRCCS - 1)) {
                    API_ThreadSetHetrcc(pthd_sched->tid, LW_HETRCC_WEAK_AFFINITY, cur_hccid + 1);
                    pthd_sched->weak_miss = 0;
                }

            } else if (pthd_sched->consume < hcesa_param[cur_hccid].thd_down_thr) {
                /*
                 * Try to upgrade computing level
                 */
                if (cur_hccid) {
                    API_ThreadSetHetrcc(pthd_sched->tid, LW_HETRCC_WEAK_AFFINITY, cur_hccid - 1);
                    pthd_sched->weak_miss = 0;
                }
            }

            if (pthd_sched->weak_miss >= THD_WEAK_MISS_THR) {
                /*
                 * Need to be tightly coupled
                 */
                if (hcesa_sa_prio_thr &&
                    LW_PRIO_IS_HIGH_OR_EQU(hcesa_sa_prio_thr, pthd_sched->prio)) {
                    /*
                     * Change to strong affinity
                     */
                    pthd_sched->hcc_mode = LW_HETRCC_STRONG_AFFINITY;
                } else {
                    pthd_sched->hcc_mode = LW_HETRCC_WEAK_AFFINITY;
                }

                /*
                 * Try again weak or strong affinity schedule
                 */
                API_ThreadSetHetrcc(pthd_sched->tid, pthd_sched->hcc_mode, cur_hccid);
                pthd_sched->weak_miss = 0;
            }
        }
    }

    return  (NULL);
}

/*
 * xhcesa module init
 */
int module_init (void)
{
    LW_CLASS_THREADATTR  attr;

    if (LW_NHETRCCS <= 1) {
        printk(KERN_ERR "<hcesa> No heterogeneous computing cluster detected!\n");
        return  (LW_INIT_RET_ERROR);
    }

    if (LW_KERN_SMP_FSCHED_EN_GET()) {
        printk(KERN_ERR "<hcesa> This module cannot work with kernel parameter 'fsched=yes'!\n");
        return  (LW_INIT_RET_ERROR);
    }

    /*
     * Create thread schedule state pool
     */
    if (thread_sched_init()) {
        printk(KERN_ERR "<hcesa> No enough memory!\n");
        return  (LW_INIT_RET_ERROR);
    }

    /*
     * Initialize the scheduling parameters of each computing power cluster
     */
    if (init()) {
        sys_free(thread_sched_pool);
        printk(KERN_ERR "<hcesa> `HETRC_COMPOW` configuration error!\n");
        return  (LW_INIT_RET_ERROR);
    }

    /*
     * Create real-time schedule loop thread.
     */
    API_ThreadAttrBuild(&attr, LW_CFG_THREAD_DEFAULT_STK_SIZE, LW_PRIO_REALTIME,
                        LW_OPTION_THREAD_STK_CHK | LW_OPTION_THREAD_SAFE |
                        LW_OPTION_OBJECT_GLOBAL | LW_OPTION_THREAD_DETACHED, NULL);

    thr_sched_loop = API_ThreadCreate("t_hcesa", sched_loop, &attr, NULL);
    if (thr_sched_loop == LW_HANDLE_INVALID) {
        printk(KERN_ERR "<hcesa> Create kernel thread error!\n");
        return  (LW_INIT_RET_ERROR);
    }

    printk(KERN_INFO "<hcesa> Computing power & Energy consumption aware scheduling "
                     "adapter has been started, version: %s\n", XHCESA_VERSION);

    return  (LW_INIT_RET_UNLOAD_DISALLOW);
}

#endif /* LW_CFG_CPU_ARCH_HETRC > 0 */
/*
 * end
 */

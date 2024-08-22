/**
 * @file
 * SylixOS Heterogeneous computing adapter kernel module.
 * Configuration initialization
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
#include <stdio.h>
#include "param.h"
#include "xhcesa.h"

#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)

/* schedule parameter */
hcesa_param_t *hcesa_param = NULL;

/* Minimum threshold */
unsigned long hcesa_min_thr = __ARCH_ULONG_MAX;

/* Strong affinity prio threshold */
int hcesa_sa_prio_thr = 0;

/*
 * Parameter calculate
 * The larger the computing power cluster id number, the powerful of the computing power
 */
static int param_calc (void)
{
    unsigned int i;
    unsigned long tick_per_loop, compow_per_loop;
    PLW_CLASS_HCCCPU phcccpu;

    /*
     * Allocate memory for each computing power cluster parameter table
     */
    hcesa_param = (hcesa_param_t *)sys_zalloc(sizeof(hcesa_param_t) * LW_NHETRCCS);
    if (hcesa_param == NULL) {
        printk(KERN_ERR "<hcesa> No enough memory!\n");
        return  (-1);
    }

    /*
     * Tick per schedule check loop
     */
    tick_per_loop = (SCHED_DETECT_PERIOD * LW_TICK_HZ) / 1000;

    LW_HCCCPU_FOREACH (i) {
        phcccpu = LW_HCCCPU_GET(i);
        compow_per_loop = tick_per_loop * phcccpu->HCCCPU_uiComputPow;

        if (i == LW_NHETRCCS - 1) {
            /*
             * The most powerful computing power cluster can no longer be upgrade
             */
            hcesa_param[i].thd_up_thr = __ARCH_ULONG_MAX;

        } else {
            /*
             * This cluster upgrade threshold
             */
            hcesa_param[i].thd_up_thr = (compow_per_loop * THD_UP_COMPOW_THR) / 100;
        }

        /*
         * This cluster downgrade threshold
         */
        hcesa_param[i].thd_down_thr = (compow_per_loop * THD_DOWN_COMPOW_THR) / 100;

        if (hcesa_param[i].thd_up_thr == __ARCH_ULONG_MAX) {
            printk(KERN_INFO "<hcesa> Heterogeneous cluster %u thd_up_thr: MAX thd_down_thr: %lu\n",
                   i, hcesa_param[i].thd_down_thr);
        } else {
            printk(KERN_INFO "<hcesa> Heterogeneous cluster %u thd_up_thr: %lu thd_down_thr: %lu\n",
                   i, hcesa_param[i].thd_up_thr, hcesa_param[i].thd_down_thr);
        }

        /*
         * The minimum threshold for upgrade calculations
         */
        if (hcesa_min_thr > hcesa_param[i].thd_up_thr) {
            hcesa_min_thr = hcesa_param[i].thd_up_thr;
        }
    }

    return  (0);
}

/*
 * Parameter check
 */
static int param_check (void)
{
    unsigned int i;
    PLW_CLASS_HCCCPU phcccpu;

    LW_HCCCPU_FOREACH (i) {
        phcccpu = LW_HCCCPU_GET(i);
        if (phcccpu->HCCCPU_uiComputPow == 0) {
            printk(KERN_ERR "<hcesa> Heterogeneous cluster %u computing power is zero!\n", i);
            return  (-1);
        }
    }

    return  (param_calc());
}

/*
 * Reduce all cluster power mark in half
 */
static void make_it_half (void)
{
    unsigned int i;
    PLW_CLASS_HCCCPU phcccpu;

    LW_HCCCPU_FOREACH (i) {
        phcccpu = LW_HCCCPU_GET(i);
        phcccpu->HCCCPU_uiComputPow = phcccpu->HCCCPU_uiComputPow >> 1;
    }
}

/*
 * Get parameter by BogoMIPS
 */
static int init_by_bogomips (void)
{
    unsigned int i;
    unsigned long mips;
    PLW_CLASS_CPU pcpu;
    PLW_CLASS_HCCCPU phcccpu;

    LW_CPU_FOREACH_ACTIVE (i) {
        pcpu = LW_CPU_GET(i);
        phcccpu = LW_HCCCPU_GET(pcpu->CPU_ulHccId);
        if (phcccpu->HCCCPU_uiComputPow == 0) {
            if (API_CpuBogoMips(i, &mips)) {
                printk(KERN_ERR "<hcesa> Can not get heterogeneous cluster %u BogoMIPS!\n", i);
                return  (-1);
            }
            phcccpu->HCCCPU_uiComputPow = (unsigned int)mips;
        }
    }

recheck:
    LW_HCCCPU_FOREACH (i) {
        phcccpu = LW_HCCCPU_GET(i);
        if (phcccpu->HCCCPU_uiComputPow > MAX_COMPOW) {
            make_it_half();
            goto    recheck;
        }
    }

    return  (param_check());
}

/*
 * Initialize
 */
int init (void)
{
    char *pstr, *next;
    char param[128] = "";
    unsigned int index = 0, compow;
    PLW_CLASS_HCCCPU phcccpu;

    if (tshellVarGetRt("HETRCC_STRONG", param, sizeof(param)) > 0) {
        /*
         * Strong cluster affinity after weak miss
         */
        if ((sscanf(param, "%u", &hcesa_sa_prio_thr) == 1) &&
            (hcesa_sa_prio_thr >= LW_PRIO_HIGHEST + 1) &&
            (hcesa_sa_prio_thr  < LW_PRIO_LOWEST)) {
            printk(KERN_INFO "<hcesa> Strong cluster affinity after weak miss, prio threshold: %d.\n", hcesa_sa_prio_thr);
        } else {
            hcesa_sa_prio_thr = 0;
        }
    }

    if (tshellVarGetRt("HETRCC_COMPOW", param, sizeof(param)) <= 0) {
        /*
         * Using BogoMIPS as a computing benchmark
         */
        return  (init_by_bogomips());
    }

    /*
     * Use user config as computing benchmark
     */
    next = lib_index(param, ',');
    if (next == NULL) {
        goto    error;
    }

    pstr = param;
    *next++ = '\0';

    do {
        if ((sscanf(pstr, "%u", &compow) != 1) || (compow > MAX_COMPOW)) {
            goto    error;
        }

        phcccpu = LW_HCCCPU_GET(index);
        phcccpu->HCCCPU_uiComputPow = compow;

        index++;
        if (index >= LW_NHETRCCS) {
            break;
        }

        pstr = next;
        if (next) {
            next = lib_index(next, ',');
            if (next) {
                *next++ = '\0';
            }
        }
    } while (pstr);

    return  (param_check());

error:
    return  (-1);
}

#endif /* LW_CFG_CPU_ARCH_HETRC > 0 */
/*
 * end
 */

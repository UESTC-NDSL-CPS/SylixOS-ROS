/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: bspTime8254.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 7 月 31 日
**
** 描        述: intel 8254 定时器函数库.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "arch/x86/common/x86Idt.h"
#include "driver/timer/i8254.h"
/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#if LW_CFG_CPU_WORD_LENGHT == 32
#define READ_TSC_LL(count)          \
    __asm__ __volatile__ ("rdtsc" : "=A" (count))

#else                                                                   /*  LW_CFG_CPU_WORD_LENGHT == 64*/
#define READ_TSC_LL(count)          \
    __asm__ __volatile__ (          \
     "rdtsc                 \n\t"   \
     "and   %1    , %%rax   \n\t"   \
     "shl   $32   , %%rdx   \n\t"   \
     "or    %%rdx , %%rax   \n\t"   \
    : "=&a" (count)                 \
    : "r" (0xffffffffUL)            \
    : "rdx"                         \
    )
#endif                                                                  /*  LW_CFG_CPU_WORD_LENGHT == 32*/

#define NSECS_PER_SEC               1000000000                          /*  每秒纳秒数                  */
#define TICK_TSC_COUNT              139                                 /*  记录COUNT个Tick中断时的 TSC */
#define TICK_TSC_BASE               10                                  /*  过滤掉前面 10 个 Tick       */
#define TICK_TSC_SHIFT              7                                   /*  128 个 TSC 间隔, 1<<7 == 128*/
/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/
/*********************************************************************************************************
  i8254 平台数据
*********************************************************************************************************/
static I8254_CTL            _G_i8254Data = {
    .iobase   = PIT_BASE_ADR,
    .iobuzzer = DIAG_CTRL,
    .qcofreq  = PC_PIT_CLOCK,
};
/*********************************************************************************************************
  修正偏差
*********************************************************************************************************/
static volatile UINT64      _G_ui64PrevTickTsc = 0;                     /*  上一 Tick 中断时的 TSC      */

static UINT64               _G_ui64TickTsc[TICK_TSC_COUNT];             /*  记录COUNT个Tick中断时的 TSC */
static UINT32               _G_uiTickTscCount = 0;
static UINT64               _G_ui64TscPerSec = 0;                       /*  每秒 TSC 数                 */
static UINT32               _G_uiTscPerTick = 0;                        /*  每 TICK TSC 数              */
/*********************************************************************************************************
** 函数名称: bsp8254TickInit
** 功能描述: 初始化 tick 时钟
** 输  入  : pulVector     中断向量号
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  bsp8254TickInit (ULONG  *pulVector)
{
    i8254InitAsTick(&_G_i8254Data);                                     /*  初始化 8254 定时器          */

    if (bspIntModeGet() == X86_INT_MODE_SYMMETRIC_IO) {
        *pulVector = LW_IRQ_2;                                          /*  IRQ2 (8254 Count0 中断)     */

    } else {
        *pulVector = X86_IRQ_TIMER;                                     /*  IRQ0                        */
    }

    return  (ERROR_NONE);                                               /*  始终返回成功                */
}
/*********************************************************************************************************
** 函数名称: bsp8254TickHook
** 功能描述: 每个操作系统时钟节拍，系统将调用这个函数
** 输  入  : NONE
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bsp8254TickHook (VOID)
{
    if (LW_LIKELY(_G_uiTickTscCount >= TICK_TSC_COUNT)) {               /*  已经完成每 TICK 的TSC数计算 */
        UINT64  ui64TscNow;
        UINT64  ui64TscPass;

        READ_TSC_LL(ui64TscNow);                                        /*  获得现在的 TSC              */

        ui64TscPass = ui64TscNow - _G_ui64PrevTickTsc;                  /*  计算上一次 TICK 至现在的 TSC*/
        if (LW_LIKELY(ui64TscPass <= _G_uiTscPerTick)) {                /*  如果 ui64TscPass <= 平均值  */
            _G_ui64PrevTickTsc = ui64TscNow;                            /*  使用现在的 TSC 作为上一次   */
                                                                        /*  Tick 中断时的 TSC           */
        } else {                                                        /*  如果 ui64TscPass 大于平均值 */
            _G_ui64PrevTickTsc = _G_ui64PrevTickTsc + _G_uiTscPerTick;  /*  计算上一次 Tick 中断时的 TSC*/
        }
    } else {                                                            /*  未完成每 TICK 的TSC数计算   */
        READ_TSC_LL(_G_ui64PrevTickTsc);                                /*  获得 Tick 中断时的 TSC      */

        _G_ui64TickTsc[_G_uiTickTscCount++] = _G_ui64PrevTickTsc;       /*  记录COUNT个Tick中断时的 TSC */
        if (LW_UNLIKELY(_G_uiTickTscCount == TICK_TSC_COUNT)) {         /*  用于计算平均值的 TSC 值已够 */
            UINT64  ui64TotalTscDiff = 0;
            UINT64  ui64TscDiff;
            INT     i;

            for (i = TICK_TSC_BASE; i < TICK_TSC_COUNT; i++) {          /*  计算 COUNT - 1 个 Tick 间隔 */
                ui64TscDiff = _G_ui64TickTsc[i] - _G_ui64TickTsc[i - 1];/*  的 TSC 差之和               */
                ui64TotalTscDiff += ui64TscDiff;
            }                                                           /*  计算每秒的 TSC 数           */
            _G_ui64TscPerSec = ((ui64TotalTscDiff * LW_TICK_HZ) >> TICK_TSC_SHIFT);
            _G_uiTscPerTick  = ui64TotalTscDiff >> TICK_TSC_SHIFT;      /*  计算每 TICK 的 TSC 数       */
        }
    }
}
/*********************************************************************************************************
** 函数名称: bsp8254TickHighResolution
** 功能描述: 修正从最近一次 tick 到当前的精确时间.
** 输　入  : ptv       需要修正的时间
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bsp8254TickHighResolution (struct timespec  *ptv)
{
    if (LW_LIKELY(_G_uiTscPerTick)) {                                   /*  已经完成每 TICK 的TSC数计算 */
        UINT64  ui64TscNow;
        UINT64  ui64TscPass;
        UINT64  ui64NsPass;

        READ_TSC_LL(ui64TscNow);                                        /*  获得现在的 TSC              */

        ui64TscPass = ui64TscNow - _G_ui64PrevTickTsc;                  /*  计算上一次 TICK 至现在的 TSC*/
        ui64NsPass  = ui64TscPass * NSECS_PER_SEC / _G_ui64TscPerSec;   /*  计算上一次 TICK 至现在的纳秒*/

        ptv->tv_nsec += ui64NsPass;
        if (ptv->tv_nsec >= NSECS_PER_SEC) {
            ptv->tv_nsec -= NSECS_PER_SEC;
            ptv->tv_sec++;
        }
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

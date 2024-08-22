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
** 文   件   名: bspTimeHpet.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2017 年 7 月 12 日
**
** 描        述: HPET 定时器函数库.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "arch/x86/common/x86Idt.h"
#include "arch/x86/acpi/x86AcpiLib.h"
/*********************************************************************************************************
  HPET 地址
*********************************************************************************************************/
static addr_t               _G_ulHpetBase;
/*********************************************************************************************************
  精确时间换算参数
*********************************************************************************************************/
static UINT32               _G_uiFullCnt;
static UINT32               _G_uiComparatorCur;
static UINT32               _G_uiNSecPerCntShift;
static UINT64               _G_ui64NSecPerCntShifted;                   /*  提高 NSecPerCntShift 位精度 */
/*********************************************************************************************************
  HPET 寄存器定义
*********************************************************************************************************/
#define HPET_BASE                           _G_ulHpetBase

#define HPET_ID_LO                          (HPET_BASE + 0)
#define HPET_ID_HI                          (HPET_ID_LO + 4)

#define HPET_CONFIG_LO                      (HPET_BASE + 0x10)
#define HPET_CONFIG_HI                      (HPET_CONFIG_LO + 4)

#define HPET_STATUS_LO                      (HPET_BASE + 0x20)
#define HPET_STATUS_HI                      (HPET_STATUS_LO + 4)

#define HPET_COUNTER_LO                     (HPET_BASE + 0xf0)
#define HPET_COUNTER_HI                     (HPET_COUNTER_LO + 4)

#define HPET_TIMER_CONFIG_LO(t)             (HPET_BASE + 0x100 + (t) * 0x20)
#define HPET_TIMER_CONFIG_HI(t)             (HPET_TIMER_CONFIG_LO(t) + 4)

#define HPET_TIMER_COMPARATOR_LO(t)         (HPET_BASE + 0x108 + (t) * 0x20)
#define HPET_TIMER_COMPARATOR_HI(t)         (HPET_TIMER_COMPARATOR_LO(t) + 4)

#define HPET_TIMER_FSB_INTR_LO(t)           (HPET_BASE + 0x110 + (t) * 0x20)
#define HPET_TIMER_FSB_INTR_HI(t)           (HPET_TIMER_FSB_INTR_LO(t) + 4)

#define HPET_ID_LO_LEG_RT_CAP               (1 << 15)                   /*  legacy replacement mapping  */

#define HPET_TIMER_CONFIG_LO_INT_ROUTE(n)   ((n) << 9)                  /*  中断路由                    */
#define HPET_TIMER_CONFIG_LO_32MODE         (1 << 8)                    /*  32 位模式                   */
#define HPET_TIMER_CONFIG_LO_VAL_SET        (1 << 6)                    /*  设置值                      */
#define HPET_TIMER_CONFIG_LO_PERIODIC       (1 << 3)                    /*  Preiodic 模式               */
#define HPET_TIMER_CONFIG_LO_INT_EN         (1 << 2)                    /*  中断使能                    */
#define HPET_TIMER_CONFIG_LO_INT_EDGE       (0 << 1)                    /*  边沿触发模式                */

#define HPET_CONFIG_LO_LEG_RT               (1 << 1)                    /*  legacy replacement mapping  */
#define HPET_CONFIG_LO_ENABLE               (1 << 0)                    /*  使能定时器                  */
/*********************************************************************************************************
** 函数名称: __bspAcpiHpetMap
** 功能描述: 映射 ACPI HPET 表
** 输  入  : ulAcpiPhyAddr    ACPI HPET 表物理地址
**           ulAcpiSize       ACPI HPET 表长度
** 输  出  : ACPI HPET 表虚拟地址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  *__bspAcpiHpetMap (addr_t  ulAcpiPhyAddr, size_t  ulAcpiSize)
{
    addr_t  ulPhyBase = ROUND_DOWN(ulAcpiPhyAddr, LW_CFG_VMM_PAGE_SIZE);
    addr_t  ulOffset  = ulAcpiPhyAddr - ulPhyBase;
    addr_t  ulVirBase;

    ulAcpiSize += ulOffset;
    ulAcpiSize  = ROUND_UP(ulAcpiSize, LW_CFG_VMM_PAGE_SIZE);

    ulVirBase = (addr_t)API_VmmIoRemapNocache((PVOID)ulPhyBase, ulAcpiSize);
    if (ulVirBase) {
        return  (VOID *)(ulVirBase + ulOffset);
    } else {
        return  (VOID *)(LW_NULL);
    }
}
/*********************************************************************************************************
** 函数名称: bspHpetTickInit
** 功能描述: 初始化 tick 时钟
** 输  入  : pHpet         ACPI HPET 表
**           pulVector     中断向量号
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  bspHpetTickInit (ACPI_TABLE_HPET  *pAcpiHpetPhy, ULONG  *pulVector)
{
    ACPI_TABLE_HPET  *pAcpiHpet;
    UINT64            ui64FsPerCnt;
    addr_t            ulHpetPhyBase;
    ULONG             ulVector;
    UINT32            uiLegRtMode;
    UINT64            ui64Temp0;
    UINT64            ui64Temp1;

    /*
     * 映射 ACPI HPET 表
     */
    pAcpiHpet = __bspAcpiHpetMap((addr_t)pAcpiHpetPhy, LW_CFG_VMM_PAGE_SIZE);
    if (!pAcpiHpet) {
        return  (PX_ERROR);
    }

    ulHpetPhyBase = (addr_t)pAcpiHpet->Address.Address;

    /*
     * 映射 HPET 寄存器
     */
    _G_ulHpetBase = (addr_t)API_VmmIoRemapNocache((PVOID)ulHpetPhyBase,
                                                  LW_CFG_VMM_PAGE_SIZE);

    /*
     * 解除映射 ACPI HPET 表
     */
    API_VmmIoUnmap((PVOID)(((addr_t)pAcpiHpet) & LW_CFG_VMM_PAGE_MASK));

    if (!_G_ulHpetBase) {                                               /*  映射 HPET 寄存器失败        */
        return  (PX_ERROR);
    }

    /*
     * 由于 HPET 不一定支持 64 位模式(如 QEMU), 所以固定使用 32 位模式
     */
    write32(0, HPET_CONFIG_LO);                                         /*  停止定时器                  */

    ui64FsPerCnt = read32(HPET_ID_HI);

    _G_uiFullCnt = (1000000000000000ULL / ui64FsPerCnt) / LW_TICK_HZ;

    /*
     * 计算精度最高的移位数
     */
    _G_uiNSecPerCntShift = 12;

    ui64Temp0 = ((ui64FsPerCnt << _G_uiNSecPerCntShift) / 1000000) * 2 * _G_uiFullCnt;

    for (; _G_uiNSecPerCntShift < 63; _G_uiNSecPerCntShift++) {
        ui64Temp1 = ((ui64FsPerCnt << (_G_uiNSecPerCntShift + 1)) / 1000000) * 2 * _G_uiFullCnt;
        if (ui64Temp1 < ui64Temp0) {
            break;
        }
        ui64Temp0 = ui64Temp1;
    }

    _G_ui64NSecPerCntShifted = (ui64FsPerCnt << _G_uiNSecPerCntShift) / 1000000;

    write32(0, HPET_COUNTER_LO);                                        /*  复位计数器                  */
    write32(0, HPET_COUNTER_HI);

    if (read32(HPET_ID_LO) & HPET_ID_LO_LEG_RT_CAP) {                   /*  legacy replacement mapping  */
        write32(HPET_TIMER_CONFIG_LO_32MODE |                           /*  32 位模式                   */
                HPET_TIMER_CONFIG_LO_VAL_SET |                          /*  设置值                      */
                HPET_TIMER_CONFIG_LO_PERIODIC |                         /*  Preiodic 模式               */
                HPET_TIMER_CONFIG_LO_INT_EN |                           /*  中断使能                    */
                HPET_TIMER_CONFIG_LO_INT_EDGE,                          /*  边沿触发模式                */
                HPET_TIMER_CONFIG_LO(0));                               /*  配置 timer0                 */

        if (bspIntModeGet() == X86_INT_MODE_SYMMETRIC_IO) {             /*  IOAPIC mapping              */
            ulVector = LW_IRQ_2;

        } else {                                                        /*  PIC mapping                 */
            ulVector = X86_IRQ_TIMER;                                   /*  IRQ0                        */
        }

        uiLegRtMode = HPET_CONFIG_LO_LEG_RT;                            /*  legacy replacement mode     */

    } else {
        ulVector = archFindLsb(read32(HPET_TIMER_CONFIG_HI(0))) - 1;    /*  读Tn_INT_ROUTE_CAP, 分配中断*/

        write32(HPET_TIMER_CONFIG_LO_INT_ROUTE(ulVector) |
                HPET_TIMER_CONFIG_LO_32MODE |                           /*  32 位模式                   */
                HPET_TIMER_CONFIG_LO_VAL_SET |                          /*  设置值                      */
                HPET_TIMER_CONFIG_LO_PERIODIC |                         /*  Preiodic 模式               */
                HPET_TIMER_CONFIG_LO_INT_EN |                           /*  中断使能                    */
                HPET_TIMER_CONFIG_LO_INT_EDGE,                          /*  边沿触发模式                */
                HPET_TIMER_CONFIG_LO(0));                               /*  配置 timer0                 */

        uiLegRtMode = 0;                                                /*  legacy replacement disable  */
    }

    write32(_G_uiFullCnt, HPET_TIMER_COMPARATOR_LO(0));                 /*  设置 timer0 comparator      */
    write32(0,            HPET_TIMER_COMPARATOR_HI(0));

    write32(HPET_CONFIG_LO_ENABLE |                                     /*  开始计数                    */
            uiLegRtMode,                                                /*  legacy replacement mode     */
            HPET_CONFIG_LO);                                            /*  启动定时器                  */

    *pulVector = ulVector;

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: bspHpetTickHook
** 功能描述: 每个操作系统时钟节拍，系统将调用这个函数
** 输  入  : NONE
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bspHpetTickHook (VOID)
{
    _G_uiComparatorCur = read32(HPET_TIMER_COMPARATOR_LO(0)) - _G_uiFullCnt;
}
/*********************************************************************************************************
** 函数名称: bspHpetTickHighResolution
** 功能描述: 修正从最近一次 tick 到当前的精确时间.
** 输　入  : ptv       需要修正的时间
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bspHpetTickHighResolution (struct timespec  *ptv)
{
    UINT32  uiCntDiff;
    UINT32  uiCmpaCur = read32(HPET_TIMER_COMPARATOR_LO(0)) - _G_uiFullCnt;

    if (uiCmpaCur != _G_uiComparatorCur) {
        /*
         * 在读取精确时间时产生了 tick 中断，但系统计算时间时是按 1tick = 1ms 计算的(1000Hz Tick)，
         * 而定时器的计数频率使得，1tick 并不精确等于 1ms，
         * 故这里，发现产生但未被处理的 tick 中断后，将 uiCntDiff 分成了两部分进行计算
         */
        ptv->tv_nsec += LW_NSEC_PER_TICK;
        uiCntDiff = read32(HPET_COUNTER_LO) - uiCmpaCur;
    } else {
        uiCntDiff = read32(HPET_COUNTER_LO) - _G_uiComparatorCur;
    }

    ptv->tv_nsec += (_G_ui64NSecPerCntShifted * uiCntDiff) >> _G_uiNSecPerCntShift;
    if (ptv->tv_nsec >= 1000000000) {
        ptv->tv_nsec -= 1000000000;
        ptv->tv_sec++;
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

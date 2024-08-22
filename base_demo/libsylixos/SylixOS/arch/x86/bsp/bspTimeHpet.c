/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: bspTimeHpet.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2017 �� 7 �� 12 ��
**
** ��        ��: HPET ��ʱ��������.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "arch/x86/common/x86Idt.h"
#include "arch/x86/acpi/x86AcpiLib.h"
/*********************************************************************************************************
  HPET ��ַ
*********************************************************************************************************/
static addr_t               _G_ulHpetBase;
/*********************************************************************************************************
  ��ȷʱ�任�����
*********************************************************************************************************/
static UINT32               _G_uiFullCnt;
static UINT32               _G_uiComparatorCur;
static UINT32               _G_uiNSecPerCntShift;
static UINT64               _G_ui64NSecPerCntShifted;                   /*  ��� NSecPerCntShift λ���� */
/*********************************************************************************************************
  HPET �Ĵ�������
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

#define HPET_TIMER_CONFIG_LO_INT_ROUTE(n)   ((n) << 9)                  /*  �ж�·��                    */
#define HPET_TIMER_CONFIG_LO_32MODE         (1 << 8)                    /*  32 λģʽ                   */
#define HPET_TIMER_CONFIG_LO_VAL_SET        (1 << 6)                    /*  ����ֵ                      */
#define HPET_TIMER_CONFIG_LO_PERIODIC       (1 << 3)                    /*  Preiodic ģʽ               */
#define HPET_TIMER_CONFIG_LO_INT_EN         (1 << 2)                    /*  �ж�ʹ��                    */
#define HPET_TIMER_CONFIG_LO_INT_EDGE       (0 << 1)                    /*  ���ش���ģʽ                */

#define HPET_CONFIG_LO_LEG_RT               (1 << 1)                    /*  legacy replacement mapping  */
#define HPET_CONFIG_LO_ENABLE               (1 << 0)                    /*  ʹ�ܶ�ʱ��                  */
/*********************************************************************************************************
** ��������: __bspAcpiHpetMap
** ��������: ӳ�� ACPI HPET ��
** ��  ��  : ulAcpiPhyAddr    ACPI HPET �������ַ
**           ulAcpiSize       ACPI HPET ����
** ��  ��  : ACPI HPET �������ַ
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: bspHpetTickInit
** ��������: ��ʼ�� tick ʱ��
** ��  ��  : pHpet         ACPI HPET ��
**           pulVector     �ж�������
** ��  ��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
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
     * ӳ�� ACPI HPET ��
     */
    pAcpiHpet = __bspAcpiHpetMap((addr_t)pAcpiHpetPhy, LW_CFG_VMM_PAGE_SIZE);
    if (!pAcpiHpet) {
        return  (PX_ERROR);
    }

    ulHpetPhyBase = (addr_t)pAcpiHpet->Address.Address;

    /*
     * ӳ�� HPET �Ĵ���
     */
    _G_ulHpetBase = (addr_t)API_VmmIoRemapNocache((PVOID)ulHpetPhyBase,
                                                  LW_CFG_VMM_PAGE_SIZE);

    /*
     * ���ӳ�� ACPI HPET ��
     */
    API_VmmIoUnmap((PVOID)(((addr_t)pAcpiHpet) & LW_CFG_VMM_PAGE_MASK));

    if (!_G_ulHpetBase) {                                               /*  ӳ�� HPET �Ĵ���ʧ��        */
        return  (PX_ERROR);
    }

    /*
     * ���� HPET ��һ��֧�� 64 λģʽ(�� QEMU), ���Թ̶�ʹ�� 32 λģʽ
     */
    write32(0, HPET_CONFIG_LO);                                         /*  ֹͣ��ʱ��                  */

    ui64FsPerCnt = read32(HPET_ID_HI);

    _G_uiFullCnt = (1000000000000000ULL / ui64FsPerCnt) / LW_TICK_HZ;

    /*
     * ���㾫����ߵ���λ��
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

    write32(0, HPET_COUNTER_LO);                                        /*  ��λ������                  */
    write32(0, HPET_COUNTER_HI);

    if (read32(HPET_ID_LO) & HPET_ID_LO_LEG_RT_CAP) {                   /*  legacy replacement mapping  */
        write32(HPET_TIMER_CONFIG_LO_32MODE |                           /*  32 λģʽ                   */
                HPET_TIMER_CONFIG_LO_VAL_SET |                          /*  ����ֵ                      */
                HPET_TIMER_CONFIG_LO_PERIODIC |                         /*  Preiodic ģʽ               */
                HPET_TIMER_CONFIG_LO_INT_EN |                           /*  �ж�ʹ��                    */
                HPET_TIMER_CONFIG_LO_INT_EDGE,                          /*  ���ش���ģʽ                */
                HPET_TIMER_CONFIG_LO(0));                               /*  ���� timer0                 */

        if (bspIntModeGet() == X86_INT_MODE_SYMMETRIC_IO) {             /*  IOAPIC mapping              */
            ulVector = LW_IRQ_2;

        } else {                                                        /*  PIC mapping                 */
            ulVector = X86_IRQ_TIMER;                                   /*  IRQ0                        */
        }

        uiLegRtMode = HPET_CONFIG_LO_LEG_RT;                            /*  legacy replacement mode     */

    } else {
        ulVector = archFindLsb(read32(HPET_TIMER_CONFIG_HI(0))) - 1;    /*  ��Tn_INT_ROUTE_CAP, �����ж�*/

        write32(HPET_TIMER_CONFIG_LO_INT_ROUTE(ulVector) |
                HPET_TIMER_CONFIG_LO_32MODE |                           /*  32 λģʽ                   */
                HPET_TIMER_CONFIG_LO_VAL_SET |                          /*  ����ֵ                      */
                HPET_TIMER_CONFIG_LO_PERIODIC |                         /*  Preiodic ģʽ               */
                HPET_TIMER_CONFIG_LO_INT_EN |                           /*  �ж�ʹ��                    */
                HPET_TIMER_CONFIG_LO_INT_EDGE,                          /*  ���ش���ģʽ                */
                HPET_TIMER_CONFIG_LO(0));                               /*  ���� timer0                 */

        uiLegRtMode = 0;                                                /*  legacy replacement disable  */
    }

    write32(_G_uiFullCnt, HPET_TIMER_COMPARATOR_LO(0));                 /*  ���� timer0 comparator      */
    write32(0,            HPET_TIMER_COMPARATOR_HI(0));

    write32(HPET_CONFIG_LO_ENABLE |                                     /*  ��ʼ����                    */
            uiLegRtMode,                                                /*  legacy replacement mode     */
            HPET_CONFIG_LO);                                            /*  ������ʱ��                  */

    *pulVector = ulVector;

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: bspHpetTickHook
** ��������: ÿ������ϵͳʱ�ӽ��ģ�ϵͳ�������������
** ��  ��  : NONE
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspHpetTickHook (VOID)
{
    _G_uiComparatorCur = read32(HPET_TIMER_COMPARATOR_LO(0)) - _G_uiFullCnt;
}
/*********************************************************************************************************
** ��������: bspHpetTickHighResolution
** ��������: ���������һ�� tick ����ǰ�ľ�ȷʱ��.
** �䡡��  : ptv       ��Ҫ������ʱ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspHpetTickHighResolution (struct timespec  *ptv)
{
    UINT32  uiCntDiff;
    UINT32  uiCmpaCur = read32(HPET_TIMER_COMPARATOR_LO(0)) - _G_uiFullCnt;

    if (uiCmpaCur != _G_uiComparatorCur) {
        /*
         * �ڶ�ȡ��ȷʱ��ʱ������ tick �жϣ���ϵͳ����ʱ��ʱ�ǰ� 1tick = 1ms �����(1000Hz Tick)��
         * ����ʱ���ļ���Ƶ��ʹ�ã�1tick ������ȷ���� 1ms��
         * ��������ֲ�����δ������� tick �жϺ󣬽� uiCntDiff �ֳ��������ֽ��м���
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

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
** ��   ��   ��: bspTime8254.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 7 �� 31 ��
**
** ��        ��: intel 8254 ��ʱ��������.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "arch/x86/common/x86Idt.h"
#include "driver/timer/i8254.h"
/*********************************************************************************************************
  �궨��
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

#define NSECS_PER_SEC               1000000000                          /*  ÿ��������                  */
#define TICK_TSC_COUNT              139                                 /*  ��¼COUNT��Tick�ж�ʱ�� TSC */
#define TICK_TSC_BASE               10                                  /*  ���˵�ǰ�� 10 �� Tick       */
#define TICK_TSC_SHIFT              7                                   /*  128 �� TSC ���, 1<<7 == 128*/
/*********************************************************************************************************
  ȫ�ֱ�������
*********************************************************************************************************/
/*********************************************************************************************************
  i8254 ƽ̨����
*********************************************************************************************************/
static I8254_CTL            _G_i8254Data = {
    .iobase   = PIT_BASE_ADR,
    .iobuzzer = DIAG_CTRL,
    .qcofreq  = PC_PIT_CLOCK,
};
/*********************************************************************************************************
  ����ƫ��
*********************************************************************************************************/
static volatile UINT64      _G_ui64PrevTickTsc = 0;                     /*  ��һ Tick �ж�ʱ�� TSC      */

static UINT64               _G_ui64TickTsc[TICK_TSC_COUNT];             /*  ��¼COUNT��Tick�ж�ʱ�� TSC */
static UINT32               _G_uiTickTscCount = 0;
static UINT64               _G_ui64TscPerSec = 0;                       /*  ÿ�� TSC ��                 */
static UINT32               _G_uiTscPerTick = 0;                        /*  ÿ TICK TSC ��              */
/*********************************************************************************************************
** ��������: bsp8254TickInit
** ��������: ��ʼ�� tick ʱ��
** ��  ��  : pulVector     �ж�������
** ��  ��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  bsp8254TickInit (ULONG  *pulVector)
{
    i8254InitAsTick(&_G_i8254Data);                                     /*  ��ʼ�� 8254 ��ʱ��          */

    if (bspIntModeGet() == X86_INT_MODE_SYMMETRIC_IO) {
        *pulVector = LW_IRQ_2;                                          /*  IRQ2 (8254 Count0 �ж�)     */

    } else {
        *pulVector = X86_IRQ_TIMER;                                     /*  IRQ0                        */
    }

    return  (ERROR_NONE);                                               /*  ʼ�շ��سɹ�                */
}
/*********************************************************************************************************
** ��������: bsp8254TickHook
** ��������: ÿ������ϵͳʱ�ӽ��ģ�ϵͳ�������������
** ��  ��  : NONE
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bsp8254TickHook (VOID)
{
    if (LW_LIKELY(_G_uiTickTscCount >= TICK_TSC_COUNT)) {               /*  �Ѿ����ÿ TICK ��TSC������ */
        UINT64  ui64TscNow;
        UINT64  ui64TscPass;

        READ_TSC_LL(ui64TscNow);                                        /*  ������ڵ� TSC              */

        ui64TscPass = ui64TscNow - _G_ui64PrevTickTsc;                  /*  ������һ�� TICK �����ڵ� TSC*/
        if (LW_LIKELY(ui64TscPass <= _G_uiTscPerTick)) {                /*  ��� ui64TscPass <= ƽ��ֵ  */
            _G_ui64PrevTickTsc = ui64TscNow;                            /*  ʹ�����ڵ� TSC ��Ϊ��һ��   */
                                                                        /*  Tick �ж�ʱ�� TSC           */
        } else {                                                        /*  ��� ui64TscPass ����ƽ��ֵ */
            _G_ui64PrevTickTsc = _G_ui64PrevTickTsc + _G_uiTscPerTick;  /*  ������һ�� Tick �ж�ʱ�� TSC*/
        }
    } else {                                                            /*  δ���ÿ TICK ��TSC������   */
        READ_TSC_LL(_G_ui64PrevTickTsc);                                /*  ��� Tick �ж�ʱ�� TSC      */

        _G_ui64TickTsc[_G_uiTickTscCount++] = _G_ui64PrevTickTsc;       /*  ��¼COUNT��Tick�ж�ʱ�� TSC */
        if (LW_UNLIKELY(_G_uiTickTscCount == TICK_TSC_COUNT)) {         /*  ���ڼ���ƽ��ֵ�� TSC ֵ�ѹ� */
            UINT64  ui64TotalTscDiff = 0;
            UINT64  ui64TscDiff;
            INT     i;

            for (i = TICK_TSC_BASE; i < TICK_TSC_COUNT; i++) {          /*  ���� COUNT - 1 �� Tick ��� */
                ui64TscDiff = _G_ui64TickTsc[i] - _G_ui64TickTsc[i - 1];/*  �� TSC ��֮��               */
                ui64TotalTscDiff += ui64TscDiff;
            }                                                           /*  ����ÿ��� TSC ��           */
            _G_ui64TscPerSec = ((ui64TotalTscDiff * LW_TICK_HZ) >> TICK_TSC_SHIFT);
            _G_uiTscPerTick  = ui64TotalTscDiff >> TICK_TSC_SHIFT;      /*  ����ÿ TICK �� TSC ��       */
        }
    }
}
/*********************************************************************************************************
** ��������: bsp8254TickHighResolution
** ��������: ���������һ�� tick ����ǰ�ľ�ȷʱ��.
** �䡡��  : ptv       ��Ҫ������ʱ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bsp8254TickHighResolution (struct timespec  *ptv)
{
    if (LW_LIKELY(_G_uiTscPerTick)) {                                   /*  �Ѿ����ÿ TICK ��TSC������ */
        UINT64  ui64TscNow;
        UINT64  ui64TscPass;
        UINT64  ui64NsPass;

        READ_TSC_LL(ui64TscNow);                                        /*  ������ڵ� TSC              */

        ui64TscPass = ui64TscNow - _G_ui64PrevTickTsc;                  /*  ������һ�� TICK �����ڵ� TSC*/
        ui64NsPass  = ui64TscPass * NSECS_PER_SEC / _G_ui64TscPerSec;   /*  ������һ�� TICK �����ڵ�����*/

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

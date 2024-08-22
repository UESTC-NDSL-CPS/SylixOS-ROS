/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: ppcVfp.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2015 �� 12 �� 21 ��
**
** ��        ��: PowerPC ��ϵ�ܹ� FPU ֧��.
*********************************************************************************************************/
#define  __SYLIXOS_STDIO
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if LW_CFG_CPU_FPU_EN > 0
#include "../ppcFpu.h"
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
static PPC_FPU_OP    _G_fpuopVfp;
static ARCH_FPU_CTX  _G_fpuCtxInit;
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
typedef struct {
    UINT32      FPSCR_uiFpscr;                                          /*  ״̬�Ϳ��ƼĴ���            */
    UINT32      FPSCR_uiFpscrCopy;                                      /*  ״̬�Ϳ��ƼĴ����Ŀ���      */
} PPC_FPSCR_GETER, PPC_FPSCR_SETER;
/*********************************************************************************************************
  ʵ�ֺ���
*********************************************************************************************************/
extern VOID     ppcVfpEnable(VOID);
extern VOID     ppcVfpDisable(VOID);
extern BOOL     ppcVfpIsEnable(VOID);
extern UINT32   ppcVfpGetFpscrReg(PPC_FPSCR_GETER  *pgeter);
extern VOID     ppcVfpSetFpscrReg(PPC_FPSCR_SETER  *pseter);
extern VOID     ppcVfpSave(PVOID  pvFpuCtx);
extern VOID     ppcVfpRestore(PVOID  pvFpuCtx);
/*********************************************************************************************************
** ��������: ppcVfpGetFpscr
** ��������: ��� FPSCR �Ĵ�����ֵ
** �䡡��  : NONE
** �䡡��  : FPSCR �Ĵ�����ֵ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
UINT32  ppcVfpFpscrGet (VOID)
{
    PPC_FPSCR_GETER  geter;

    return  (ppcVfpGetFpscrReg(&geter));
}
/*********************************************************************************************************
** ��������: ppcVfpFpscrSet
** ��������: ���� FPSCR �Ĵ�����ֵ
** �䡡��  : uiFpscr       FPSCR �Ĵ�����ֵ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ppcVfpFpscrSet (UINT32  uiFpscr)
{
    PPC_FPSCR_SETER  seter;

    seter.FPSCR_uiFpscr = uiFpscr;

    ppcVfpSetFpscrReg(&seter);
}
/*********************************************************************************************************
** ��������: ppcVfpCtxShow
** ��������: ��ʾ VFP ������
** �䡡��  : iFd       ����ļ�������
**           pvFpuCtx  VFP ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  ppcVfpCtxShow (INT  iFd, PVOID  pvFpuCtx)
{
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_FIO_LIB_EN > 0)
    LW_FPU_CONTEXT *pfpuCtx    = (LW_FPU_CONTEXT *)pvFpuCtx;
    ARCH_FPU_CTX   *pcpufpuCtx = &pfpuCtx->FPUCTX_fpuctxContext;
    INT             i;

    fdprintf(iFd, "FPSCR   = 0x%08x\n", pcpufpuCtx->FPUCTX_uiFpscr);

    for (i = 0; i < FP_DREG_NR; i += 2) {
        fdprintf(iFd, "FPR%02d = %lf, FPR%02d = %lf\n",
                 i,     pcpufpuCtx->FPUCTX_dfDreg[i],
                 i + 1, pcpufpuCtx->FPUCTX_dfDreg[i + 1]);
    }
#endif
}
/*********************************************************************************************************
** ��������: ppcVfpEnableTask
** ��������: ϵͳ���� undef �쳣ʱ, ʹ������� VFP
** �䡡��  : ptcbCur    ��ǰ������ƿ�
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  ppcVfpEnableTask (PLW_CLASS_TCB  ptcbCur)
{
    ARCH_REG_CTX  *pregctx;

    pregctx = &ptcbCur->TCB_archRegCtx;
    pregctx->REG_uiMsr |= ARCH_PPC_MSR_FP |
                          ARCH_PPC_MSR_FE0 | ARCH_PPC_MSR_FE1;          /*  Floating-point precise mode */
}
/*********************************************************************************************************
** ��������: ppcVfpEnableException
** ��������: ʹ�� VFP �������쳣
** �䡡��  : pvFpuCtx  VFP ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  ppcVfpEnableException (PVOID pvFpuCtx)
{
    LW_FPU_CONTEXT  *pfpuCtx    = (LW_FPU_CONTEXT *)pvFpuCtx;
    ARCH_FPU_CTX    *pcpufpuCtx = &pfpuCtx->FPUCTX_fpuctxContext;

    pcpufpuCtx->FPUCTX_uiFpscr |= ARCH_PPC_FPSCR_VE |
                                  ARCH_PPC_FPSCR_OE |
                                  ARCH_PPC_FPSCR_UE |
                                  ARCH_PPC_FPSCR_ZE;
}
/*********************************************************************************************************
** ��������: ppcVfpPrimaryInit
** ��������: ��ʼ������ȡ VFP ����������������
** �䡡��  : pcMachineName ������
**           pcFpuName     ������������
** �䡡��  : ����������
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PPPC_FPU_OP  ppcVfpPrimaryInit (CPCHAR  pcMachineName, CPCHAR  pcFpuName)
{
    lib_bzero(&_G_fpuCtxInit, sizeof(ARCH_FPU_CTX));
    ppcVfpRestore(&_G_fpuCtxInit);

    _G_fpuopVfp.PFPU_pfuncSave            = ppcVfpSave;
    _G_fpuopVfp.PFPU_pfuncRestore         = ppcVfpRestore;
    _G_fpuopVfp.PFPU_pfuncEnable          = ppcVfpEnable;
    _G_fpuopVfp.PFPU_pfuncDisable         = ppcVfpDisable;
    _G_fpuopVfp.PFPU_pfuncIsEnable        = ppcVfpIsEnable;
    _G_fpuopVfp.PFPU_pfuncCtxShow         = ppcVfpCtxShow;
    _G_fpuopVfp.PFPU_pfuncEnableTask      = ppcVfpEnableTask;
    _G_fpuopVfp.PFPU_pfuncEnableException = ppcVfpEnableException;

    return  (&_G_fpuopVfp);
}
/*********************************************************************************************************
** ��������: ppcVfpSecondaryInit
** ��������: ��ʼ�� VFP ������
** �䡡��  : pcMachineName ������
**           pcFpuName     ������������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ppcVfpSecondaryInit (CPCHAR  pcMachineName, CPCHAR  pcFpuName)
{
    (VOID)pcMachineName;
    (VOID)pcFpuName;
}

#endif                                                                  /*  LW_CFG_CPU_FPU_EN > 0       */
/*********************************************************************************************************
  END
*********************************************************************************************************/

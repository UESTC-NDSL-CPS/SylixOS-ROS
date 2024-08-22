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
** ��   ��   ��: cskyVfp.c
**
** ��   ��   ��: Wang.Xuan (���Q)
**
** �ļ���������: 2018 �� 05 �� 14 ��
**
** ��        ��: C-SKY ��ϵ�ܹ� VFP ֧��.
*********************************************************************************************************/
#define  __SYLIXOS_STDIO
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if LW_CFG_CPU_FPU_EN > 0
#include "../cskyFpu.h"
/*********************************************************************************************************
  FCR ����
*********************************************************************************************************/
#define FCR_IDE             (1 << 5)
#define FCR_IXE             (1 << 4)
#define FCR_UFE             (1 << 3)
#define FCR_OFE             (1 << 2)
#define FCR_DZE             (1 << 1)
#define FCR_IOE             (1 << 0)
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
static CSKY_FPU_OP  _G_fpuopVfp;
/*********************************************************************************************************
  ʵ�ֺ���
*********************************************************************************************************/
extern VOID  cskyVfpInit(VOID);
extern VOID  cskyVfpSave(PVOID  pvFpuCtx);
extern VOID  cskyVfpRestore(PVOID  pvFpuCtx);
/*********************************************************************************************************
** ��������: cskyVfpEnable
** ��������: ʹ�� VFP
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  cskyVfpEnable (VOID)
{
}
/*********************************************************************************************************
** ��������: cskyVfpDisable
** ��������: ���� VFP
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  cskyVfpDisable (VOID)
{
}
/*********************************************************************************************************
** ��������: cskyVfpIsEnable
** ��������: �ж� VFP �Ƿ�ʹ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static BOOL  cskyVfpIsEnable (VOID)
{
    return  (LW_TRUE);
}
/*********************************************************************************************************
** ��������: cskyVfpCtxShow
** ��������: ��ʾ VFP ������
** �䡡��  : iFd       ����ļ�������
**           pvFpuCtx  VFP ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  cskyVfpCtxShow (INT  iFd, PVOID  pvFpuCtx)
{
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_FIO_LIB_EN > 0)
    LW_FPU_CONTEXT  *pfpuCtx    = (LW_FPU_CONTEXT *)pvFpuCtx;
    ARCH_FPU_CTX    *pcpufpuCtx = &pfpuCtx->FPUCTX_fpuctxContext;
    INT              i;
    
    fdprintf(iFd, "FCR  = 0x%08x\n", pcpufpuCtx->FPUCTX_uiFpcr);
    fdprintf(iFd, "FESR = 0x%08x\n", pcpufpuCtx->FPUCTX_uiFpesr);

    for (i = 0; i < FPU_REG_NR; i++) {
#if defined(__SYLIXOS_CSKY_ARCH_CK803__)
        fdprintf(iFd, "FP%02d = 0x%08x\n", i,
                 pcpufpuCtx->FPUCTX_uiDreg[i].val32[0]);
#else
        fdprintf(iFd, "FP%02d = 0x%08x0x%08x\n", i,
                 pcpufpuCtx->FPUCTX_uiDreg[i].val32[0],
                 pcpufpuCtx->FPUCTX_uiDreg[i].val32[1]);
#endif                                                                  /*  __SYLIXOS_CSKY_ARCH_CK803__ */
    }
#endif
}
/*********************************************************************************************************
** ��������: cskyVfpEnableException
** ��������: ʹ�� VFP �������쳣
** �䡡��  : pvFpuCtx  VFP ������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  cskyVfpEnableException (PVOID  pvFpuCtx)
{
    LW_FPU_CONTEXT  *pfpuCtx    = (LW_FPU_CONTEXT *)pvFpuCtx;
    ARCH_FPU_CTX    *pcpufpuCtx = &pfpuCtx->FPUCTX_fpuctxContext;

    pcpufpuCtx->FPUCTX_uiFpcr |= (FCR_IDE | FCR_UFE | FCR_OFE | FCR_DZE | FCR_IOE);
}
/*********************************************************************************************************
** ��������: cskyVfpPrimaryInit
** ��������: ��ʼ������ȡ VFP ����������������
** �䡡��  : pcMachineName ������
**           pcFpuName     ������������
** �䡡��  : ����������
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PCSKY_FPU_OP  cskyVfpPrimaryInit (CPCHAR  pcMachineName, CPCHAR  pcFpuName)
{
    (VOID)pcMachineName;
    (VOID)pcFpuName;

    cskyVfpInit();

    _G_fpuopVfp.CFPU_pfuncEnable          = cskyVfpEnable;
    _G_fpuopVfp.CFPU_pfuncDisable         = cskyVfpDisable;
    _G_fpuopVfp.CFPU_pfuncIsEnable        = cskyVfpIsEnable;
    _G_fpuopVfp.CFPU_pfuncCtxShow         = cskyVfpCtxShow;
    _G_fpuopVfp.CFPU_pfuncSave            = cskyVfpSave;
    _G_fpuopVfp.CFPU_pfuncRestore         = cskyVfpRestore;
    _G_fpuopVfp.CFPU_pfuncEnableException = cskyVfpEnableException;

    return  (&_G_fpuopVfp);
}
/*********************************************************************************************************
** ��������: cskyVfpSecondaryInit
** ��������: ��ʼ�� VFP ������
** �䡡��  : pcMachineName ������
**           pcFpuName     ������������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  cskyVfpSecondaryInit (CPCHAR  pcMachineName, CPCHAR  pcFpuName)
{
    (VOID)pcMachineName;
    (VOID)pcFpuName;

    cskyVfpInit();
}

#endif                                                                  /*  LW_CFG_CPU_FPU_EN > 0       */
/*********************************************************************************************************
  END
*********************************************************************************************************/

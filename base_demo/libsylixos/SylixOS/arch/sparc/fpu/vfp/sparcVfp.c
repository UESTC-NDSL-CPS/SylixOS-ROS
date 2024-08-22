/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: sparcVfp.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2017 年 09 月 29 日
**
** 描        述: SPARC 体系架构 FPU 支持.
*********************************************************************************************************/
#define  __SYLIXOS_STDIO
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if LW_CFG_CPU_FPU_EN > 0
#include "../sparcFpu.h"
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
static SPARC_FPU_OP     _G_fpuopVfp;
static ARCH_FPU_CTX     _G_fpuCtxInit;
/*********************************************************************************************************
  实现函数
*********************************************************************************************************/
extern VOID     sparcVfpEnable(VOID);
extern VOID     sparcVfpDisable(VOID);
extern BOOL     sparcVfpIsEnable(VOID);
extern VOID     sparcVfpSave(PVOID  pvFpuCtx);
extern VOID     sparcVfpRestore(PVOID  pvFpuCtx);
/*********************************************************************************************************
** 函数名称: sparcVfpCtxShow
** 功能描述: 显示 VFP 上下文
** 输　入  : iFd       输出文件描述符
**           pvFpuCtx  VFP 上下文
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  sparcVfpCtxShow (INT  iFd, PVOID  pvFpuCtx)
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
** 函数名称: sparcVfpEnableTask
** 功能描述: 系统发生 undef 异常时, 使能任务的 VFP
** 输　入  : ptcbCur    当前任务控制块
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  sparcVfpEnableTask (PLW_CLASS_TCB  ptcbCur)
{
    ARCH_REG_CTX  *pregctx;

    pregctx = &ptcbCur->TCB_archRegCtx;
    pregctx->REG_uiPsr |= PSR_EF;
}
/*********************************************************************************************************
** 函数名称: sparcVfpEnableException
** 功能描述: 使能 VFP 上下文异常
** 输　入  : pvFpuCtx  VFP 上下文
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  sparcVfpEnableException (PVOID pvFpuCtx)
{
    LW_FPU_CONTEXT  *pfpuCtx    = (LW_FPU_CONTEXT *)pvFpuCtx;
    ARCH_FPU_CTX    *pcpufpuCtx = &pfpuCtx->FPUCTX_fpuctxContext;

    pcpufpuCtx->FPUCTX_uiFpscr |= FSR_NVM | FSR_OFM | FSR_UFM | FSR_DZM;
}
/*********************************************************************************************************
** 函数名称: sparcVfpPrimaryInit
** 功能描述: 初始化并获取 VFP 控制器操作函数集
** 输　入  : pcMachineName 机器名
**           pcFpuName     浮点运算器名
** 输　出  : 操作函数集
** 全局变量:
** 调用模块:
*********************************************************************************************************/
PSPARC_FPU_OP  sparcVfpPrimaryInit (CPCHAR  pcMachineName, CPCHAR  pcFpuName)
{
    lib_bzero(&_G_fpuCtxInit, sizeof(ARCH_FPU_CTX));
    sparcVfpRestore(&_G_fpuCtxInit);

    _G_fpuopVfp.SFPU_pfuncSave            = sparcVfpSave;
    _G_fpuopVfp.SFPU_pfuncRestore         = sparcVfpRestore;
    _G_fpuopVfp.SFPU_pfuncEnable          = sparcVfpEnable;
    _G_fpuopVfp.SFPU_pfuncDisable         = sparcVfpDisable;
    _G_fpuopVfp.SFPU_pfuncIsEnable        = sparcVfpIsEnable;
    _G_fpuopVfp.SFPU_pfuncCtxShow         = sparcVfpCtxShow;
    _G_fpuopVfp.SFPU_pfuncEnableTask      = sparcVfpEnableTask;
    _G_fpuopVfp.SFPU_pfuncEnableException = sparcVfpEnableException;

    return  (&_G_fpuopVfp);
}
/*********************************************************************************************************
** 函数名称: sparcVfpSecondaryInit
** 功能描述: 初始化 VFP 控制器
** 输　入  : pcMachineName 机器名
**           pcFpuName     浮点运算器名
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  sparcVfpSecondaryInit (CPCHAR  pcMachineName, CPCHAR  pcFpuName)
{
    (VOID)pcMachineName;
    (VOID)pcFpuName;
}

#endif                                                                  /*  LW_CFG_CPU_FPU_EN > 0       */
/*********************************************************************************************************
  END
*********************************************************************************************************/

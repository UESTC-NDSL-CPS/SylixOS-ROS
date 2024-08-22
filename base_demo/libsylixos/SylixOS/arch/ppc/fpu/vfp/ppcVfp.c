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
** 文   件   名: ppcVfp.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 12 月 21 日
**
** 描        述: PowerPC 体系架构 FPU 支持.
*********************************************************************************************************/
#define  __SYLIXOS_STDIO
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if LW_CFG_CPU_FPU_EN > 0
#include "../ppcFpu.h"
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
static PPC_FPU_OP    _G_fpuopVfp;
static ARCH_FPU_CTX  _G_fpuCtxInit;
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
typedef struct {
    UINT32      FPSCR_uiFpscr;                                          /*  状态和控制寄存器            */
    UINT32      FPSCR_uiFpscrCopy;                                      /*  状态和控制寄存器的拷贝      */
} PPC_FPSCR_GETER, PPC_FPSCR_SETER;
/*********************************************************************************************************
  实现函数
*********************************************************************************************************/
extern VOID     ppcVfpEnable(VOID);
extern VOID     ppcVfpDisable(VOID);
extern BOOL     ppcVfpIsEnable(VOID);
extern UINT32   ppcVfpGetFpscrReg(PPC_FPSCR_GETER  *pgeter);
extern VOID     ppcVfpSetFpscrReg(PPC_FPSCR_SETER  *pseter);
extern VOID     ppcVfpSave(PVOID  pvFpuCtx);
extern VOID     ppcVfpRestore(PVOID  pvFpuCtx);
/*********************************************************************************************************
** 函数名称: ppcVfpGetFpscr
** 功能描述: 获得 FPSCR 寄存器的值
** 输　入  : NONE
** 输　出  : FPSCR 寄存器的值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
UINT32  ppcVfpFpscrGet (VOID)
{
    PPC_FPSCR_GETER  geter;

    return  (ppcVfpGetFpscrReg(&geter));
}
/*********************************************************************************************************
** 函数名称: ppcVfpFpscrSet
** 功能描述: 设置 FPSCR 寄存器的值
** 输　入  : uiFpscr       FPSCR 寄存器的值
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  ppcVfpFpscrSet (UINT32  uiFpscr)
{
    PPC_FPSCR_SETER  seter;

    seter.FPSCR_uiFpscr = uiFpscr;

    ppcVfpSetFpscrReg(&seter);
}
/*********************************************************************************************************
** 函数名称: ppcVfpCtxShow
** 功能描述: 显示 VFP 上下文
** 输　入  : iFd       输出文件描述符
**           pvFpuCtx  VFP 上下文
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
** 函数名称: ppcVfpEnableTask
** 功能描述: 系统发生 undef 异常时, 使能任务的 VFP
** 输　入  : ptcbCur    当前任务控制块
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  ppcVfpEnableTask (PLW_CLASS_TCB  ptcbCur)
{
    ARCH_REG_CTX  *pregctx;

    pregctx = &ptcbCur->TCB_archRegCtx;
    pregctx->REG_uiMsr |= ARCH_PPC_MSR_FP |
                          ARCH_PPC_MSR_FE0 | ARCH_PPC_MSR_FE1;          /*  Floating-point precise mode */
}
/*********************************************************************************************************
** 函数名称: ppcVfpEnableException
** 功能描述: 使能 VFP 上下文异常
** 输　入  : pvFpuCtx  VFP 上下文
** 输　出  : NONE
** 全局变量:
** 调用模块:
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
** 函数名称: ppcVfpPrimaryInit
** 功能描述: 初始化并获取 VFP 控制器操作函数集
** 输　入  : pcMachineName 机器名
**           pcFpuName     浮点运算器名
** 输　出  : 操作函数集
** 全局变量:
** 调用模块:
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
** 函数名称: ppcVfpSecondaryInit
** 功能描述: 初始化 VFP 控制器
** 输　入  : pcMachineName 机器名
**           pcFpuName     浮点运算器名
** 输　出  : NONE
** 全局变量:
** 调用模块:
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

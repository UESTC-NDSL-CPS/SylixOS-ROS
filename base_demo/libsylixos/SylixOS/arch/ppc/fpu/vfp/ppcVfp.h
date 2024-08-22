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
** 文   件   名: ppcVfp.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 12 月 21 日
**
** 描        述: PowerPC 体系架构 FPU 支持.
*********************************************************************************************************/

#ifndef __ARCH_PPCVFP_H
#define __ARCH_PPCVFP_H

UINT32        ppcVfpFpscrGet(VOID);
VOID          ppcVfpFpscrSet(UINT32  uiFpscr);

PPPC_FPU_OP   ppcVfpPrimaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);
VOID          ppcVfpSecondaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);

#endif                                                                  /*  __ARCH_PPCVFP_H             */
/*********************************************************************************************************
  END
*********************************************************************************************************/

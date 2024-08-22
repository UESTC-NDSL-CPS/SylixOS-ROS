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
** 文   件   名: ppcVfpSpe.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 05 月 02 日
**
** 描        述: PowerPC 体系架构 SPE 支持.
*********************************************************************************************************/

#ifndef __ARCH_PPCVFPSPE_H
#define __ARCH_PPCVFPSPE_H

UINT32       ppcVfpSpeFscrGet(VOID);
VOID         ppcVfpSpeFscrSet(UINT32  uiSpeFscr);

PPPC_FPU_OP  ppcVfpSpePrimaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);
VOID         ppcVfpSpeSecondaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);

#endif                                                                  /*  __ARCH_PPCVFPSPE_H          */
/*********************************************************************************************************
  END
*********************************************************************************************************/

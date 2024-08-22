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
** 文   件   名: x86FpuSse.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 08 月 05 日
**
** 描        述: x86 体系架构 FPU 支持.
*********************************************************************************************************/

#ifndef __ARCH_X86FPUSSE_H
#define __ARCH_X86FPUSSE_H

VOID          x86FpuSseFpsrGet(UINT16   *pusFpsr);
VOID          x86FpuSseMxcsrGet(UINT32  *puiMxcsr);
VOID          x86FpuSseMxcsrSet(UINT32  *puiMxcsr);

VOID          x86FpuSseExceptionClear(VOID);

PX86_FPU_OP   x86FpuSsePrimaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);
VOID          x86FpuSseSecondaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);

#endif                                                                  /*  __ARCH_X86FPUSSE_H          */
/*********************************************************************************************************
  END
*********************************************************************************************************/

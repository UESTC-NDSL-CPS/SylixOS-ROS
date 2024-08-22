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
** ��   ��   ��: x86FpuSse.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 08 �� 05 ��
**
** ��        ��: x86 ��ϵ�ܹ� FPU ֧��.
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

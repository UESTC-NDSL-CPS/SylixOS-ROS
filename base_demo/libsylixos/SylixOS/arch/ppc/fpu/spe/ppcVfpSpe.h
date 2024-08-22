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
** ��   ��   ��: ppcVfpSpe.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 05 �� 02 ��
**
** ��        ��: PowerPC ��ϵ�ܹ� SPE ֧��.
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

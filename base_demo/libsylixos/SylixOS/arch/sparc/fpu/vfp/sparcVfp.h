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
** ��   ��   ��: sparcVfp.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2017 �� 09 �� 29 ��
**
** ��        ��: SPARC ��ϵ�ܹ� FPU ֧��.
*********************************************************************************************************/

#ifndef __ARCH_SPARC_VFP_H
#define __ARCH_SPARC_VFP_H

VOID            sparcVfpGetFsr(UINT32  *puiFsr);
VOID            sparcVfpSetFsr(UINT32  *puiFsr);

PSPARC_FPU_OP   sparcVfpPrimaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);
VOID            sparcVfpSecondaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);

#endif                                                                  /*  __ARCH_SPARC_VFP_H          */
/*********************************************************************************************************
  END
*********************************************************************************************************/

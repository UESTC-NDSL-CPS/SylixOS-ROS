/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: _CpuHetrcc.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2023 �� 06 �� 07 ��
**
** ��        ��: CPU �칹�����ؿ⺯��.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
/*********************************************************************************************************
** ��������: _CpuHetrccCount
** ��������: ���ָ�������� ID �� CPU ����. (�����ں˱�����)
** �䡡��  : ulHccId       ������ ID
**           pcpuExcept    �ų������ CPU
** �䡡��  : CPU ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  _CpuHetrccCount (ULONG  ulHccId, PLW_CLASS_CPU  pcpuExcept)
{
    ULONG           i;
    ULONG           ulCount = 0;
    PLW_CLASS_CPU   pcpu;

    if (pcpuExcept) {
        LW_CPU_FOREACH_ACTIVE (i) {                                         /*  ������ active CPU           */
            pcpu = LW_CPU_GET(i);
            if (pcpu == pcpuExcept) {
                continue;
            }
            if (pcpu->CPU_ulHccId == ulHccId) {
                ulCount++;
            }
        }

    } else {
        ulCount = LW_HCCCPU_GET(ulHccId)->HCCCPU_uiActive;
    }

    return  (ulCount);
}
/*********************************************************************************************************
** ��������: _CpuHetrccValid
** ��������: �ж�ָ�������� ID ��ǰ�Ƿ���Ч. (�����ں˱�����)
** �䡡��  : ulHccId       ������ ID
**           bActive       �Ƿ����Ϊ����� CPU
**           pcpuExcept    �ų������ CPU
** �䡡��  : �Ƿ���Ч
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  _CpuHetrccValid (ULONG  ulHccId, BOOL  bActive, PLW_CLASS_CPU  pcpuExcept)
{
    ULONG           i;
    PLW_CLASS_CPU   pcpu;

    if (pcpuExcept) {
        LW_CPU_FOREACH (i) {
            pcpu = LW_CPU_GET(i);
            if ((pcpu == pcpuExcept) || (bActive && !LW_CPU_IS_ACTIVE(pcpu))) {
                continue;
            }
            if (pcpu->CPU_ulHccId == ulHccId) {
                return  (LW_TRUE);
            }
        }

    } else if (ulHccId < LW_CFG_MAX_PROCESSORS) {
        return  (LW_HCCCPU_GET(ulHccId)->HCCCPU_uiActive > 0);
    }

    return  (LW_FALSE);
}

#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
/*********************************************************************************************************
  END
*********************************************************************************************************/

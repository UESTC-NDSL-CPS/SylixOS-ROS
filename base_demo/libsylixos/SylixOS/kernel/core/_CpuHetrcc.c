/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: _CpuHetrcc.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2023 年 06 月 07 日
**
** 描        述: CPU 异构算力簇库函数.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
#if (LW_CFG_SMP_EN > 0) && (LW_CFG_CPU_ARCH_HETRC > 0)
/*********************************************************************************************************
** 函数名称: _CpuHetrccCount
** 功能描述: 获得指定算力簇 ID 的 CPU 个数. (进入内核被调用)
** 输　入  : ulHccId       算力簇 ID
**           pcpuExcept    排除计算的 CPU
** 输　出  : CPU 个数
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  _CpuHetrccCount (ULONG  ulHccId, PLW_CLASS_CPU  pcpuExcept)
{
    ULONG           i;
    ULONG           ulCount = 0;
    PLW_CLASS_CPU   pcpu;

    if (pcpuExcept) {
        LW_CPU_FOREACH_ACTIVE (i) {                                         /*  仅计算 active CPU           */
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
** 函数名称: _CpuHetrccValid
** 功能描述: 判断指定算力簇 ID 当前是否有效. (进入内核被调用)
** 输　入  : ulHccId       算力簇 ID
**           bActive       是否必须为激活的 CPU
**           pcpuExcept    排除计算的 CPU
** 输　出  : 是否有效
** 全局变量:
** 调用模块:
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

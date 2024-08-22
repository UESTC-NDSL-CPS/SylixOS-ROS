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
** 文   件   名: _PriorityInit.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2006 年 12 月 14 日
**
** 描        述: 这是系统优先级控制块初始化函数库。
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "../SylixOS/kernel/include/k_kernel.h"
/*********************************************************************************************************
** 函数名称: _PriorityInit
** 功能描述: 初始化优先级控制块.
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _PriorityInit (VOID)
{
    REGISTER ULONG    i;
    
#if LW_CFG_SMP_EN > 0
    REGISTER ULONG    j;
    
    for (i = 0; i < LW_CFG_MAX_PROCESSORS; i++) {
        for (j = 0; j < (LW_PRIO_LOWEST + 1); j++) {
            LW_CPU_RDY_PPCB(LW_CPU_GET(i), j)->PCB_ucPriority = (UINT8)j;
        }
    }

#if LW_CFG_CPU_ARCH_HETRC > 0
    for (i = 0; i < LW_CFG_MAX_PROCESSORS; i++) {
        for (j = 0; j < (LW_PRIO_LOWEST + 1); j++) {
            LW_HCC_RDY_PPCB(i, j)->PCB_ucPriority = (UINT8)j;
        }
    }
#endif                                                                  /*  LW_CFG_CPU_ARCH_HETRC > 0   */
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
    
    for (i = 0; i < (LW_PRIO_LOWEST + 1); i++) {
        LW_GLOBAL_RDY_PPCB(i)->PCB_ucPriority = (UINT8)i;
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

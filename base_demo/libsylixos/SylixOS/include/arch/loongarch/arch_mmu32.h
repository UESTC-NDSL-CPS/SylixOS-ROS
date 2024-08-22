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
** 文   件   名: arch_mmu32.h
**
** 创   建   人: Wang.Ziyang (王子阳)
**
** 文件创建日期: 2022 年 04 月 07 日
**
** 描        述: LoongArch32 内存管理相关.
*********************************************************************************************************/

#ifndef __LOONGARCH_ARCH_MMU32_H
#define __LOONGARCH_ARCH_MMU32_H

/*********************************************************************************************************
  L4 微内核虚拟机 MMU
*********************************************************************************************************/

#define LW_CFG_VMM_L4_HYPERVISOR_EN           0                         /*  是否使用 L4 虚拟机 MMU      */

/*********************************************************************************************************
  是否需要内核超过 3 级页表支持
*********************************************************************************************************/

#define LW_CFG_VMM_PAGE_4L_EN                 0                         /*  是否需要 4 级页表支持       */

/*********************************************************************************************************
  虚拟内存页表相关配置
*********************************************************************************************************/

#define LW_CFG_VMM_PAGE_SHIFT                 LW_CFG_LOONGARCH_PAGE_SHIFT
#define LW_CFG_VMM_PAGE_SIZE                  (__CONST64(1) << LW_CFG_VMM_PAGE_SHIFT)
#define LW_CFG_VMM_PAGE_MASK                  (~(LW_CFG_VMM_PAGE_SIZE - 1))

/*********************************************************************************************************
  内存分组数量
*********************************************************************************************************/
#ifdef __SYLIXOS_KERNEL

#define LW_CFG_VMM_ZONE_NUM                   8                         /*  物理分区数                  */
#define LW_CFG_VMM_VIR_NUM                    8                         /*  虚拟分区数                  */

/*********************************************************************************************************
  MMU 转换条目类型
*********************************************************************************************************/
#if !defined(__ASSEMBLY__) && !defined(ASSEMBLY)

typedef UINT32  LW_PGD_TRANSENTRY;                                      /*  页目录类型                  */
typedef UINT32  LW_PMD_TRANSENTRY;                                      /*  中间页目录类型              */
typedef UINT32  LW_PTE_TRANSENTRY;                                      /*  页表条目类型                */

#endif
#endif                                                                  /*  __SYLIXOS_KERNEL            */
#endif                                                                  /*  __LOONGARCH_ARCH_MMU32_H    */
/*********************************************************************************************************
  END
*********************************************************************************************************/

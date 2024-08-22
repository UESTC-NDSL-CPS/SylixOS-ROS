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
** 文   件   名: arch_def.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 12 月 26 日
**
** 描        述: PowerPC 相关定义.
*********************************************************************************************************/

#ifndef __PPC_ARCH_DEF_H
#define __PPC_ARCH_DEF_H

/*********************************************************************************************************
  stringify_in_c
*********************************************************************************************************/
#if defined(__SYLIXOS_KERNEL) || defined(__ASSEMBLY__) || defined(ASSEMBLY)

#if defined(__ASSEMBLY__) || defined(ASSEMBLY)
#define stringify_in_c(...)     __VA_ARGS__
#else
#define __stringify_in_c(...)   #__VA_ARGS__
#define stringify_in_c(...)     __stringify_in_c(__VA_ARGS__) " "
#endif                                                                  /*  __ASSEMBLY__ || ASSEMBLY    */

/*********************************************************************************************************
  BAT definitions
*********************************************************************************************************/
/*********************************************************************************************************
  General BAT defines for bit settings to compose BAT regs
  represent all the different block lengths
  The BL field is part of the Upper Bat Register
*********************************************************************************************************/

#define BAT_BL_128K                 0x00000000
#define BAT_BL_256K                 0x00000004
#define BAT_BL_512K                 0x0000000C
#define BAT_BL_1M                   0x0000001C
#define BAT_BL_2M                   0x0000003C
#define BAT_BL_4M                   0x0000007C
#define BAT_BL_8M                   0x000000FC
#define BAT_BL_16M                  0x000001FC
#define BAT_BL_32M                  0x000003FC
#define BAT_BL_64M                  0x000007FC
#define BAT_BL_128M                 0x00000FFC
#define BAT_BL_256M                 0x00001FFC

/*********************************************************************************************************
  Supervisor/user valid mode definitions - Upper BAT
*********************************************************************************************************/

#define BAT_VALID_SUPERVISOR        0x00000002
#define BAT_VALID_USER              0x00000001
#define BAT_INVALID                 0x00000000

/*********************************************************************************************************
  WIMG bit settings - Lower BAT
*********************************************************************************************************/

#define BAT_WRITE_THROUGH           0x00000040
#define BAT_CACHE_INHIBITED         0x00000020
#define BAT_COHERENT                0x00000010
#define BAT_GUARDED                 0x00000008

/*********************************************************************************************************
  Protection bits - Lower BAT
*********************************************************************************************************/

#define BAT_NO_ACCESS               0x00000000
#define BAT_READ_ONLY               0x00000001
#define BAT_READ_WRITE              0x00000002

/*********************************************************************************************************
  MSR definitions
*********************************************************************************************************/
/*********************************************************************************************************
  Upper Machine State Register (MSR) mask
*********************************************************************************************************/

#define ARCH_PPC_MSR_SF_U           0x8000          /*  Sixty-four bit mode (not                        */
                                                    /*  implemented for 32-bit machine)                 */
#define ARCH_PPC_MSR_POW_U          0x0004          /*  Power managemnet enable                         */
#define ARCH_PPC_MSR_ILE_U          0x0001          /*  Little endian mode                              */

/*********************************************************************************************************
  Lower Machine State Register (MSR) mask
*********************************************************************************************************/

#define ARCH_PPC_MSR_EE             0x8000          /*  External interrupt enable                       */
#define ARCH_PPC_MSR_PR             0x4000          /*  Privilege level                                 */
#define ARCH_PPC_MSR_FP             0x2000          /*  Floating-point available                        */
#define ARCH_PPC_MSR_ME             0x1000          /*  Machine check enable                            */
#define ARCH_PPC_MSR_FE0            0x0800          /*  Floating-point exception mode 0                 */
#define ARCH_PPC_MSR_SE             0x0400          /*  Single-step trace enable                        */
#define ARCH_PPC_MSR_BE             0x0200          /*  Branch trace enable                             */
#define ARCH_PPC_MSR_FE1            0x0100          /*  Floating-point exception mode 1                 */
#define ARCH_PPC_MSR_IP             0x0040          /*  Exception prefix                                */
#define ARCH_PPC_MSR_IR             0x0020          /*  Instruction address translation                 */
#define ARCH_PPC_MSR_DR             0x0010          /*  Data address translation                        */
#define ARCH_PPC_MSR_RI             0x0002          /*  Recoverable interrupt                           */
#define ARCH_PPC_MSR_LE             0x0001          /*  Little-endian mode                              */

#define ARCH_PPC_MSR_POWERUP        0x0040          /*  State of MSR at powerup                         */

/*********************************************************************************************************
  MSR bit definitions common to all PPC arch.
*********************************************************************************************************/

#define ARCH_PPC_MSR_BIT_EE         16              /*  MSR Ext. Intr. Enable bit - EE                  */
#define ARCH_PPC_MSR_BIT_PR         17              /*  MSR Privilege Level bit - PR                    */
#define ARCH_PPC_MSR_BIT_ME         19              /*  MSR Machine Check Enable bit - ME               */
#define ARCH_PPC_MSR_BIT_LE         31              /*  MSR Little Endian mode bit - LE                 */

/*********************************************************************************************************
  Macros to mask off particular bits of an MSR value
*********************************************************************************************************/

#define ARCH_PPC_INT_MASK(src, des) \
    RLWINM  des, src, 0, ARCH_PPC_MSR_BIT_EE + 1, ARCH_PPC_MSR_BIT_EE - 1

#define ARCH_PPC_POW_MASK(src, des) \
    RLWINM  des, src, 0, ARCH_PPC_MSR_BIT_POW + 1, ARCH_PPC_MSR_BIT_POW - 1

/*********************************************************************************************************
  Using bit #s here because names vary [DS,IS] vs [DR,IR]
*********************************************************************************************************/

#define ARCH_PPC_MMU_MASK(src, des) \
    RLWINM  des, src, 0, 28, 25

#define ARCH_PPC_RI_MASK(src, des) \
    RLWINM  des, src, 0, ARCH_PPC_MSR_BIT_RI + 1, ARCH_PPC_MSR_BIT_RI - 1

#define ARCH_PPC_SE_MASK(src, des) \
    RLWINM  des, src, 0, ARCH_PPC_MSR_BIT_SE + 1, ARCH_PPC_MSR_BIT_SE - 1

/*********************************************************************************************************
  SRR1 definitions
*********************************************************************************************************/

#define ARCH_PPC_SRR1_ISI_NOPT          0x40000000  /*  ISI: Not found in hash                          */
#define ARCH_PPC_SRR1_ISI_N_G_OR_CIP    0x10000000  /*  ISI: Access is no-exec or G or CI for a         */
                                                    /*  prefixed instruction                            */
#define ARCH_PPC_SRR1_ISI_PROT          0x08000000  /*  ISI: Other protection fault                     */
#define ARCH_PPC_SRR1_WAKEMASK          0x00380000  /*  reason for wakeup                               */
#define ARCH_PPC_SRR1_WAKEMASK_P8       0x003c0000  /*  reason for wakeup on POWER8 and 9               */
#define ARCH_PPC_SRR1_WAKEMCE_RESVD     0x003c0000  /*  Unused/reserved value used by MCE wakeup to     */
                                                    /*  indicate cause to idle wakeup handler           */
#define ARCH_PPC_SRR1_WAKESYSERR        0x00300000  /*  System error                                    */
#define ARCH_PPC_SRR1_WAKEEE            0x00200000  /*  External interrupt                              */
#define ARCH_PPC_SRR1_WAKEHVI           0x00240000  /*  Hypervisor Virtualization Interrupt (P9)        */
#define ARCH_PPC_SRR1_WAKEMT            0x00280000  /*  mtctrl                                          */
#define ARCH_PPC_SRR1_WAKEHMI           0x00280000  /*  Hypervisor maintenance                          */
#define ARCH_PPC_SRR1_WAKEDEC           0x00180000  /*  Decrementer interrupt                           */
#define ARCH_PPC_SRR1_WAKEDBELL         0x00140000  /*  Privileged doorbell on P8                       */
#define ARCH_PPC_SRR1_WAKETHERM         0x00100000  /*  Thermal management interrupt                    */
#define ARCH_PPC_SRR1_WAKERESET         0x00100000  /*  System reset                                    */
#define ARCH_PPC_SRR1_WAKEHDBELL        0x000c0000  /*  Hypervisor doorbell on P8                       */
#define ARCH_PPC_SRR1_WAKESTATE         0x00030000  /*  Powersave exit mask [46:47]                     */
#define ARCH_PPC_SRR1_WS_HVLOSS         0x00030000  /*  HV resources not maintained                     */
#define ARCH_PPC_SRR1_WS_GPRLOSS        0x00020000  /*  GPRs not maintained                             */
#define ARCH_PPC_SRR1_WS_NOLOSS         0x00010000  /*  All resources maintained                        */
#define ARCH_PPC_SRR1_PROGTM            0x00200000  /*  TM Bad Thing                                    */
#define ARCH_PPC_SRR1_PROGFPE           0x00100000  /*  Floating Point Enabled                          */
#define ARCH_PPC_SRR1_PROGILL           0x00080000  /*  Illegal instruction                             */
#define ARCH_PPC_SRR1_PROGPRIV          0x00040000  /*  Privileged instruction                          */
#define ARCH_PPC_SRR1_PROGTRAP          0x00020000  /*  Trap                                            */
#define ARCH_PPC_SRR1_PROGADDR          0x00010000  /*  SRR0 contains subsequent addr                   */

#define ARCH_PPC_SRR1_MCE_MCP           0x00080000  /*  Machine check signal caused interrupt           */
#define ARCH_PPC_SRR1_BOUNDARY          0x10000000  /*  Prefixed instruction crosses 64-byte boundary   */
#define ARCH_PPC_SRR1_PREFIXED          0x20000000  /*  Exception caused by prefixed instruction        */

/*********************************************************************************************************
  FPSCR definitions
*********************************************************************************************************/
/*********************************************************************************************************
  FPSCR bit definitions (valid for the PPC60X family)
*********************************************************************************************************/

#define ARCH_PPC_FPSCR_FX               0x80000000  /*  FP exception summary                            */
#define ARCH_PPC_FPSCR_FEX              0x40000000  /*  FP enabled exception summary                    */
#define ARCH_PPC_FPSCR_VX               0x20000000  /*  FP invalid exception summary                    */
#define ARCH_PPC_FPSCR_OX               0x10000000  /*  FP overflow exception                           */
#define ARCH_PPC_FPSCR_UX               0x08000000  /*  FP underflow exception                          */
#define ARCH_PPC_FPSCR_ZX               0x04000000  /*  FP divide by zero exception                     */
#define ARCH_PPC_FPSCR_XX               0x02000000  /*  FP inexact exception                            */
#define ARCH_PPC_FPSCR_VXSNAN           0x01000000  /*  FP invalid exception for SNAN                   */
#define ARCH_PPC_FPSCR_VXISI            0x00800000  /*  FP invalid exc. for INF-INF                     */
#define ARCH_PPC_FPSCR_VXIDI            0x00400000  /*  FP invalid exc. for INF/INF                     */
#define ARCH_PPC_FPSCR_VXZDZ            0x00200000  /*  FP invalid exc. for 0/0                         */
#define ARCH_PPC_FPSCR_VXIMZ            0x00100000  /*  FP invalid exc. for INF*0                       */
#define ARCH_PPC_FPSCR_VXVC             0x00080000  /*  FP invalid. exc. for invalid comp.              */
#define ARCH_PPC_FPSCR_FR               0x00040000  /*  FP fraction rounded                             */
#define ARCH_PPC_FPSCR_FI               0x00020000  /*  FP fraction inexact                             */
#define ARCH_PPC_FPSCR_FPRF             0x0001F000  /*  FP result flags                                 */
#define ARCH_PPC_FPSCR_VXSOFT           0x00000400  /*  FP invalid. exc. for soft. request              */
#define ARCH_PPC_FPSCR_VXSQRT           0x00000200  /*  FP invalid. exc. for sqrt                       */
#define ARCH_PPC_FPSCR_VXCVI            0x00000100  /*  FP invalid. exc. for int convert                */
#define ARCH_PPC_FPSCR_VE               0x00000080  /*  FP invalid exc. enable                          */
#define ARCH_PPC_FPSCR_OE               0x00000040  /*  FP overflow exc. enable                         */
#define ARCH_PPC_FPSCR_UE               0x00000020  /*  FP underflow exc. enable                        */
#define ARCH_PPC_FPSCR_ZE               0x00000010  /*  FP divide by zero exc. enable                   */
#define ARCH_PPC_FPSCR_XE               0x00000008  /*  FP inexact exc. enable                          */
#define ARCH_PPC_FPSCR_NI               0x00000004  /*  FP non-IEEE mode enable                         */
#define ARCH_PPC_FPSCR_RN(n)            (n)         /*  FP rounding control value                       */

#define ARCH_PPC_FPSCR_RN_MASK          0x00000003  /*  FP rounding control bits mask                   */
#define ARCH_PPC_FPSCR_EXC_MASK         0x1ff80700  /*  FP exception status bits mask                   */
#define ARCH_PPC_FPSCR_CTRL_MASK        0x000000ff  /*  FP exception control bits mask                  */

#define ARCH_PPC_FPSCR_INIT             (ARCH_PPC_FPSCR_OE | ARCH_PPC_FPSCR_UE | ARCH_PPC_FPSCR_ZE \
                                         | ARCH_PPC_FPSCR_RN(0))

/*********************************************************************************************************
  FPSCR definitions
*********************************************************************************************************/

#define ARCH_PPC_SPEFSCR_SOVH           0x80000000
#define ARCH_PPC_SPEFSCR_OVH            0x40000000
#define ARCH_PPC_SPEFSCR_FGH            0x20000000
#define ARCH_PPC_SPEFSCR_FXH            0x10000000
#define ARCH_PPC_SPEFSCR_FINVH          0x08000000
#define ARCH_PPC_SPEFSCR_FDBZH          0x04000000
#define ARCH_PPC_SPEFSCR_FUNFH          0x02000000
#define ARCH_PPC_SPEFSCR_FOVFH          0x01000000

#define ARCH_PPC_SPEFSCR_FINXS          0x00200000
#define ARCH_PPC_SPEFSCR_FINVS          0x00100000
#define ARCH_PPC_SPEFSCR_FDBZS          0x00080000
#define ARCH_PPC_SPEFSCR_FUNFS          0x00040000
#define ARCH_PPC_SPEFSCR_FOVFS          0x00020000
#define ARCH_PPC_SPEFSCR_MODE           0x00010000

#define ARCH_PPC_SPEFSCR_SOV            0x00008000
#define ARCH_PPC_SPEFSCR_OV             0x00004000
#define ARCH_PPC_SPEFSCR_FG             0x00002000
#define ARCH_PPC_SPEFSCR_FX             0x00001000
#define ARCH_PPC_SPEFSCR_FINV           0x00000800
#define ARCH_PPC_SPEFSCR_FDBZ           0x00000400
#define ARCH_PPC_SPEFSCR_FUNF           0x00000200
#define ARCH_PPC_SPEFSCR_FOVF           0x00000100

#define ARCH_PPC_SPEFSCR_FINXE          0x00000040
#define ARCH_PPC_SPEFSCR_FINVE          0x00000020
#define ARCH_PPC_SPEFSCR_FDBZE          0x00000010
#define ARCH_PPC_SPEFSCR_FUNFE          0x00000008
#define ARCH_PPC_SPEFSCR_FOVFE          0x00000004
#define ARCH_PPC_SPEFSCR_FRMC_RND_NR    0x00000000
#define ARCH_PPC_SPEFSCR_FRMC_RND_ZERO  0x00000001
#define ARCH_PPC_SPEFSCR_FRMC_RND_PINF  0x00000002
#define ARCH_PPC_SPEFSCR_FRMC_RND_NINF  0x00000003

/*********************************************************************************************************
  RFI OPCODE definitions
*********************************************************************************************************/

#define ARCH_PPC_RFI_OPCODE             0x4C000064

#endif                                                                  /*  __SYLIXOS_KERNEL            */
#endif                                                                  /*  __PPC_ARCH_DEF_H            */
/*********************************************************************************************************
  END
*********************************************************************************************************/

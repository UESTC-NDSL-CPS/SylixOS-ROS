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
** 文   件   名: arch_regs.h
**
** 创   建   人: Wang.Xuan (王翾)
**
** 文件创建日期: 2018 年 06 月 22 日
**
** 描        述: ARM64 寄存器相关.
*********************************************************************************************************/

#ifndef __ARM64_ARCH_REGS_H
#define __ARM64_ARCH_REGS_H

/*********************************************************************************************************
  定义
*********************************************************************************************************/

#define ARCH_GREG_NR            31                                      /*  通用寄存器数目              */

#define ARCH_REG_CTX_WORD_SIZE  36                                      /*  寄存器上下文字数            */
#define ARCH_STK_MIN_WORD_SIZE  256                                     /*  堆栈最小字数                */

#define ARCH_REG_SIZE           8                                       /*  寄存器大小                  */
#define ARCH_REG_CTX_SIZE       (ARCH_REG_CTX_WORD_SIZE * ARCH_REG_SIZE)/*  寄存器上下文大小            */

#define ARCH_STK_ALIGN_SIZE     16                                      /*  堆栈对齐要求                */

#define ARCH_JMP_BUF_WORD_SIZE  40                                      /*  跳转缓冲字数(向后兼容)      */

/*********************************************************************************************************
  寄存器在 ARCH_REG_CTX 中的偏移量
*********************************************************************************************************/

#define CTX_TYPE_OFFSET         (ARCH_REG_SIZE * 0)
#define XGREG_OFFSET(n)         (ARCH_REG_SIZE * (n + 1))
#define XSP_OFFSET              (ARCH_REG_SIZE * (ARCH_GREG_NR + 1))
#define XPC_OFFSET              (ARCH_REG_SIZE * (ARCH_GREG_NR + 2))
#define XPSTATE_OFFSET          (ARCH_REG_SIZE * (ARCH_GREG_NR + 3))
#define XTPIDR_EL0_OFFSET       (ARCH_REG_SIZE * (ARCH_GREG_NR + 4))
#define CPUID_OFFSET            (ARCH_REG_SIZE * 0)

/*********************************************************************************************************
  寄存器表
*********************************************************************************************************/

#if !defined(__ASSEMBLY__) && !defined(ASSEMBLY)

typedef UINT64          ARCH_REG_T;

typedef struct {
    ARCH_REG_T          REG_ulSmallCtx;                                 /*  小上下文                    */
    ARCH_REG_T          REG_ulReg[ARCH_GREG_NR];                        /*  31 个通用目的寄存器         */
    ARCH_REG_T          REG_ulSp;                                       /*  程序堆栈指针                */
    ARCH_REG_T          REG_ulPc;                                       /*  程序计数器寄存器            */
    ARCH_REG_T          REG_ulPstate;                                   /*  CPU 运行状态                */
                                                                        /*  [31 : 28]  NZCV             */
                                                                        /*  [ 9 :  6]  DAIF             */
    ARCH_REG_T          REG_ulTPIDR_EL0;                                /*  TLS 寄存器                  */
#define REG_ulFp        REG_ulReg[29]                                   /*  栈帧寄存器                  */
#define REG_ulLr        REG_ulReg[30]                                   /*  链接寄存器                  */
} ARCH_REG_CTX;

/*********************************************************************************************************
  EABI 标准调用回溯堆栈表
*********************************************************************************************************/

typedef struct {
    ARCH_REG_T          FP_ulFp;
    ARCH_REG_T          FP_ulLr;
} ARCH_FP_CTX;

/*********************************************************************************************************
  从上下文中获取信息
*********************************************************************************************************/

#define ARCH_REG_CTX_GET_PC(ctx)    ((PVOID)(ctx)->REG_ulPc)
#define ARCH_REG_CTX_GET_FRAME(ctx) ((PVOID)(ctx)->REG_ulFp)
#define ARCH_REG_CTX_GET_STACK(ctx) ((PVOID)(ctx)->REG_ulSp)

#endif                                                                  /*  !defined(__ASSEMBLY__)      */
#endif                                                                  /*  __ARM64_ARCH_REGS_H         */
/*********************************************************************************************************
  END
*********************************************************************************************************/

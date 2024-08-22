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
** 文   件   名: x64ContextAsm.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2017 年 06 月 06 日
**
** 描        述: x86-64 体系构架上下文切换.
*********************************************************************************************************/

#ifndef __ARCH_X64CONTEXTASM_H
#define __ARCH_X64CONTEXTASM_H

/*********************************************************************************************************
  头文件
*********************************************************************************************************/

#include "arch/x86/arch_regs.h"
#include "../x86Segment.h"

/*********************************************************************************************************

  高地址:  +-----------------+
           |       SS(8)     |
           +-----------------+
           |       RSP       |
           +-----------------+
           |     RFLAGS      |
           +-----------------+
           |       CS(8)     |
           +-----------------+
           |       RIP       |
           +-----------------+
           |      ERROR      |
           +-----------------+
           |       PAD       |
           +-----------------+
           |       RBP       |
           +-----------------+
           |       R15       |
           +-----------------+
           |       R14       |
           +-----------------+
           |       R13       |
           +-----------------+
           |       R12       |
           +-----------------+
           |       R11       |
           +-----------------+
           |       R10       |
           +-----------------+
           |       R9        |
           +-----------------+
           |       R8        |
           +-----------------+
           |       RDI       |
           +-----------------+
           |       RSI       |
           +-----------------+
           |       RDX       |
           +-----------------+
           |       RCX       |
           +-----------------+
           |       RBX       |
           +-----------------+
           |       RAX       |
  低地址:  +-----------------+

*********************************************************************************************************/

/*********************************************************************************************************
  保存任务寄存器(参数 %RAX: ARCH_REG_CTX 地址) 会破坏 %RCX
*********************************************************************************************************/

MACRO_DEF(SAVE_REGS)
    MOV     %RAX , XRAX(%RAX)
    MOV     %RBX , XRBX(%RAX)
    MOV     %RCX , XRCX(%RAX)
    MOV     %RDX , XRDX(%RAX)

    MOV     %RSI , XRSI(%RAX)
    MOV     %RDI , XRDI(%RAX)

    MOV     %R8  , XR8(%RAX)
    MOV     %R9  , XR9(%RAX)
    MOV     %R10 , XR10(%RAX)
    MOV     %R11 , XR11(%RAX)
    MOV     %R12 , XR12(%RAX)
    MOV     %R13 , XR13(%RAX)
    MOV     %R14 , XR14(%RAX)
    MOV     %R15 , XR15(%RAX)

    MOV     %RBP , XRBP(%RAX)

    MOV     %RSP , %RCX
    ADD     $8   , %RCX
    MOV     %RCX , XRSP(%RAX)

    MOV     0(%RSP) , %RCX
    MOV     %RCX , XRIP(%RAX)

    MOVW    %CS  , XCS(%RAX)
    MOVW    %SS  , XSS(%RAX)

    PUSHFQ
    POPQ    %RCX
    MOV     %RCX , XRFLAGS(%RAX)
    MACRO_END()

/*********************************************************************************************************
  恢复任务寄存器(参数 %RAX: ARCH_REG_CTX 地址)
*********************************************************************************************************/

MACRO_DEF(RESTORE_REGS)
    MOV     XRBX(%RAX) , %RBX
    MOV     XRCX(%RAX) , %RCX
    MOV     XRDX(%RAX) , %RDX

    MOV     XRSI(%RAX) , %RSI
    MOV     XRDI(%RAX) , %RDI

    MOV     XR8(%RAX)  , %R8
    MOV     XR9(%RAX)  , %R9
    MOV     XR10(%RAX) , %R10
    MOV     XR11(%RAX) , %R11
    MOV     XR12(%RAX) , %R12
    MOV     XR13(%RAX) , %R13
    MOV     XR14(%RAX) , %R14
    MOV     XR15(%RAX) , %R15

    MOV     XRBP(%RAX) , %RBP

    AND     $~15 , %RSP                                                 /*  RSP 向下 16 字节对齐        */

    PUSHQ   XSS(%RAX)
    PUSHQ   XRSP(%RAX)
    PUSHQ   XRFLAGS(%RAX)
    PUSHQ   XCS(%RAX)
    PUSHQ   XRIP(%RAX)

    MOV     XRAX(%RAX) , %RAX

    IRETQ                                                               /*  弹出 CS RIP RFLAGS SS RSP   */
    MACRO_END()

/*********************************************************************************************************
  异常/中断上下文保存
*********************************************************************************************************/

MACRO_DEF(INT_SAVE_REGS_HW_ERRNO)
    CLI
                                                                        /*  SS RSP RFLAGS CS RIP ERRNO  */
                                                                        /*  已经 PUSH                   */
    SUB     $(16 * ARCH_REG_SIZE) , %RSP

    MOV     %RAX , XRAX(%RSP)
    MOV     %RBX , XRBX(%RSP)
    MOV     %RCX , XRCX(%RSP)
    MOV     %RDX , XRDX(%RSP)

    MOV     %RSI , XRSI(%RSP)
    MOV     %RDI , XRDI(%RSP)

    MOV     %R8  , XR8(%RSP)
    MOV     %R9  , XR9(%RSP)
    MOV     %R10 , XR10(%RSP)
    MOV     %R11 , XR11(%RSP)
    MOV     %R12 , XR12(%RSP)
    MOV     %R13 , XR13(%RSP)
    MOV     %R14 , XR14(%RSP)
    MOV     %R15 , XR15(%RSP)

    MOV     %RBP , XRBP(%RSP)
    MACRO_END()

/*********************************************************************************************************
  异常/中断上下文保存
*********************************************************************************************************/

MACRO_DEF(INT_SAVE_REGS_FAKE_ERRNO)
    CLI

    PUSH    $0                                                          /*  PUSH FAKE ERROR CODE        */

    INT_SAVE_REGS_HW_ERRNO
    MACRO_END()

/*********************************************************************************************************
  恢复中断嵌套寄存器(参数 %RSP: ARCH_REG_CTX 地址)
*********************************************************************************************************/

MACRO_DEF(INT_NESTING_RESTORE_REGS)
    POPQ    %RAX
    POPQ    %RBX
    POPQ    %RCX
    POPQ    %RDX

    POPQ    %RSI
    POPQ    %RDI

    POPQ    %R8
    POPQ    %R9
    POPQ    %R10
    POPQ    %R11
    POPQ    %R12
    POPQ    %R13
    POPQ    %R14
    POPQ    %R15

    POPQ    %RBP

    ADD     $(2 * ARCH_REG_SIZE) , %RSP                                 /*  POP PAD ERROR               */

    IRETQ                                                               /*  弹出 CS RIP RFLAGS SS RSP   */
    MACRO_END()

#endif                                                                  /*  __ARCH_X64CONTEXTASM_H      */
/*********************************************************************************************************
  END
*********************************************************************************************************/

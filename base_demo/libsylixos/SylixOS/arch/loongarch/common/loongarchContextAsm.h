;/*********************************************************************************************************
;**
;**                                    中国软件开源组织
;**
;**                                   嵌入式实时操作系统
;**
;**                                       SylixOS(TM)
;**
;**                               Copyright  All Rights Reserved
;**
;**--------------文件信息--------------------------------------------------------------------------------
;**
;** 文   件   名: loongarchContextAsm.h
;**
;** 创   建   人: Wang.Ziyang (王子阳)
;**
;** 文件创建日期: 2021 年 11 月 04 日
;**
;** 描        述: LoongArch 体系架构上下文处理.
;*********************************************************************************************************/

#ifndef __ARCH_LOONGARCHCONTEXTASM_H
#define __ARCH_LOONGARCHCONTEXTASM_H

#include "arch/loongarch/arch_regs.h"

;/*********************************************************************************************************
;  保存 CSR 寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(SAVE_CSR)
    CSRRD       T1 , CSR_CRMD                                           ;/*  保存 CRMD 寄存器            */
    REG_S       T1 , T0 , XCRMD

    CSRRD       T1 , CSR_ESTAT                                          ;/*  保存 ESTAT 寄存器           */
    REG_S       T1 , T0 , XESTAT

    CSRRD       T1 , CSR_BADV                                           ;/*  保存 BADV 寄存器            */
    REG_S       T1 , T0 , XBADVADDR

    CSRRD       T1 , CSR_EUEN                                           ;/*  保存 EUEN 寄存器            */
    REG_S       T1 , T0 , XEUEN
    MACRO_END()

;/*********************************************************************************************************
;  恢复 CSR 寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_CSR)
    REG_L       T1 , T0 , XCRMD
    INT_SRLI    T2 , T1 , 6
    ANDI        T2 , T2 , 0x8
    ANDI        T1 , T1 , 0x7
    OR          T1 , T1 , T2
    CSRWR       T1 , CSR_PRMD                                           ;/*  恢复 CRMD 的 PLV IE WE      */

    REG_L       T1 , T0 , XEUEN
    CSRWR       T1 , CSR_EUEN                                           ;/*  恢复 EUEN 寄存器            */
    MACRO_END()

;/*********************************************************************************************************
;  保存小上下文寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(SAVE_SMALL_REG_CTX)
    ORI         T1 , ZERO , 1
    REG_S       T1 , T0 , CTX_TYPE_OFFSET                               ;/*  标记为小上下文              */

    ;/*
    ; * 保存 S0-S8($23-$31)
    ; */
    REG_S       $r23 , T0 , XGREG(23)
    REG_S       $r24 , T0 , XGREG(24)
    REG_S       $r25 , T0 , XGREG(25)
    REG_S       $r26 , T0 , XGREG(26)
    REG_S       $r27 , T0 , XGREG(27)
    REG_S       $r28 , T0 , XGREG(28)
    REG_S       $r29 , T0 , XGREG(29)
    REG_S       $r30 , T0 , XGREG(30)
    REG_S       $r31 , T0 , XGREG(31)
    ;/*
    ; * 保存 RA GP SP FP ($1-$3 $22)
    ; */
    REG_S       $r1  , T0 , XGREG(1)
    REG_S       $r2  , T0 , XGREG(2)
    REG_S       $r3  , T0 , XGREG(3)
    REG_S       $r22 , T0 , XGREG(22)

#if LW_CFG_CPU_FAST_TLS > 0
    REG_L       $r21 , T0 , XGREG(21)
#endif

    REG_S       RA , T0 , XPC                                           ;/*  RA 代替 PC 保存             */

    SAVE_CSR                                                            ;/*  保存 CSR 寄存器             */
    MACRO_END()

;/*********************************************************************************************************
;  恢复小上下文寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_SMALL_REG_CTX)
    REG_L       T1 , T0 , XPC
    CSRWR       T1 , CSR_ERA                                            ;/*  PC 恢复至 ERA 寄存器        */

    RESTORE_CSR                                                         ;/*  恢复 CSR 寄存器             */

    ;/*
    ; * 恢复 S0-S8($23-$31)
    ; */
    REG_L       $r23 , T0 , XGREG(23)
    REG_L       $r24 , T0 , XGREG(24)
    REG_L       $r25 , T0 , XGREG(25)
    REG_L       $r26 , T0 , XGREG(26)
    REG_L       $r27 , T0 , XGREG(27)
    REG_L       $r28 , T0 , XGREG(28)
    REG_L       $r29 , T0 , XGREG(29)
    REG_L       $r30 , T0 , XGREG(30)
    REG_L       $r31 , T0 , XGREG(31)
    ;/*
    ; * 恢复 RA GP SP A0 FP ($1-$4 $22)
    ; */
    REG_L       $r1  , T0 , XGREG(1)
    REG_L       $r2  , T0 , XGREG(2)
    REG_L       $r3  , T0 , XGREG(3)
    REG_L       $r4  , T0 , XGREG(4)
    REG_L       $r22 , T0 , XGREG(22)

#if LW_CFG_CPU_FAST_TLS > 0
    REG_L       $r21 , T0 , XGREG(21)
#endif

    ERTN                                                                ;/*  返回                        */
    MACRO_END()

;/*********************************************************************************************************
;  保存 $0 - $31 寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(SAVE_GREGS)
    ;/*
    ; * $0 固定为 0
    ; */
    REG_S       $r1  , T0 , XGREG(1)
    REG_S       $r2  , T0 , XGREG(2)
    REG_S       $r3  , T0 , XGREG(3)
    REG_S       $r4  , T0 , XGREG(4)
    REG_S       $r5  , T0 , XGREG(5)
    REG_S       $r6  , T0 , XGREG(6)
    REG_S       $r7  , T0 , XGREG(7)
    REG_S       $r8  , T0 , XGREG(8)
    REG_S       $r9  , T0 , XGREG(9)
    REG_S       $r10 , T0 , XGREG(10)
    REG_S       $r11 , T0 , XGREG(11)
    REG_S       $r12 , T0 , XGREG(12)
    REG_S       $r13 , T0 , XGREG(13)
    REG_S       $r14 , T0 , XGREG(14)
    REG_S       $r15 , T0 , XGREG(15)
    REG_S       $r16 , T0 , XGREG(16)
    REG_S       $r17 , T0 , XGREG(17)
    REG_S       $r18 , T0 , XGREG(18)
    REG_S       $r19 , T0 , XGREG(19)
    REG_S       $r20 , T0 , XGREG(20)
    REG_S       $r21 , T0 , XGREG(21)
    REG_S       $r22 , T0 , XGREG(22)
    REG_S       $r23 , T0 , XGREG(23)
    REG_S       $r24 , T0 , XGREG(24)
    REG_S       $r25 , T0 , XGREG(25)
    REG_S       $r26 , T0 , XGREG(26)
    REG_S       $r27 , T0 , XGREG(27)
    REG_S       $r28 , T0 , XGREG(28)
    REG_S       $r29 , T0 , XGREG(29)
    REG_S       $r30 , T0 , XGREG(30)
    REG_S       $r31 , T0 , XGREG(31)
    MACRO_END()

;/*********************************************************************************************************
;  恢复 $0 - $31 寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_GREGS)
    ;/*
    ; * $0 固定为 0
    ; */
    REG_L       $r1  , T0 , XGREG(1)
    REG_L       $r2  , T0 , XGREG(2)
    REG_L       $r3  , T0 , XGREG(3)
    REG_L       $r4  , T0 , XGREG(4)
    REG_L       $r5  , T0 , XGREG(5)
    REG_L       $r6  , T0 , XGREG(6)
    REG_L       $r7  , T0 , XGREG(7)
    REG_L       $r8  , T0 , XGREG(8)
    REG_L       $r9  , T0 , XGREG(9)
    REG_L       $r10 , T0 , XGREG(10)
    REG_L       $r11 , T0 , XGREG(11)
    ;/*
    ; * $12 是 T0(后面会恢复)
    ; */
    REG_L       $r13 , T0 , XGREG(13)
    REG_L       $r14 , T0 , XGREG(14)
    REG_L       $r15 , T0 , XGREG(15)
    REG_L       $r16 , T0 , XGREG(16)
    REG_L       $r17 , T0 , XGREG(17)
    REG_L       $r18 , T0 , XGREG(18)
    REG_L       $r19 , T0 , XGREG(19)
    REG_L       $r20 , T0 , XGREG(20)
    REG_L       $r21 , T0 , XGREG(21)
    REG_L       $r22 , T0 , XGREG(22)
    REG_L       $r23 , T0 , XGREG(23)
    REG_L       $r24 , T0 , XGREG(24)
    REG_L       $r25 , T0 , XGREG(25)
    REG_L       $r26 , T0 , XGREG(26)
    REG_L       $r27 , T0 , XGREG(27)
    REG_L       $r28 , T0 , XGREG(28)
    REG_L       $r29 , T0 , XGREG(29)
    REG_L       $r30 , T0 , XGREG(30)
    REG_L       $r31 , T0 , XGREG(31)
    MACRO_END()

;/*********************************************************************************************************
;  保存大上下文寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(SAVE_BIG_REG_CTX)
    SAVE_GREGS                                                          ;/*  保存 $0 - $31 寄存器        */

    REG_S       ZERO , T0 , CTX_TYPE_OFFSET                             ;/*  标记为大上下文              */

    REG_S       RA , T0 , XPC                                           ;/*  RA 代替 PC 保存             */

    SAVE_CSR                                                            ;/*  保存 CSR 寄存器             */
    MACRO_END()

;/*********************************************************************************************************
;  恢复大上下文寄存器(参数 T0: ARCH_REG_CTX 地址)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_BIG_REG_CTX)
    REG_L       T1 , T0 , XPC
    CSRWR       T1 , CSR_ERA                                            ;/*  PC 恢复至 ERA 寄存器        */

    RESTORE_CSR                                                         ;/*  恢复 CSR 寄存器             */

    RESTORE_GREGS                                                       ;/*  恢复 $0 - $31 寄存器        */

    REG_L       $r12 , T0 , XGREG(12)                                   ;/*  恢复 T0 寄存器              */

    ERTN                                                                ;/*  返回                        */
    MACRO_END()

#endif
;/*********************************************************************************************************
;  END
;*********************************************************************************************************/

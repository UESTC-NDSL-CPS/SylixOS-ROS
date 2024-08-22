;/*********************************************************************************************************
;**
;**                                    �й������Դ��֯
;**
;**                                   Ƕ��ʽʵʱ����ϵͳ
;**
;**                                       SylixOS(TM)
;**
;**                               Copyright  All Rights Reserved
;**
;**--------------�ļ���Ϣ--------------------------------------------------------------------------------
;**
;** ��   ��   ��: loongarchContextAsm.h
;**
;** ��   ��   ��: Wang.Ziyang (������)
;**
;** �ļ���������: 2021 �� 11 �� 04 ��
;**
;** ��        ��: LoongArch ��ϵ�ܹ������Ĵ���.
;*********************************************************************************************************/

#ifndef __ARCH_LOONGARCHCONTEXTASM_H
#define __ARCH_LOONGARCHCONTEXTASM_H

#include "arch/loongarch/arch_regs.h"

;/*********************************************************************************************************
;  ���� CSR �Ĵ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(SAVE_CSR)
    CSRRD       T1 , CSR_CRMD                                           ;/*  ���� CRMD �Ĵ���            */
    REG_S       T1 , T0 , XCRMD

    CSRRD       T1 , CSR_ESTAT                                          ;/*  ���� ESTAT �Ĵ���           */
    REG_S       T1 , T0 , XESTAT

    CSRRD       T1 , CSR_BADV                                           ;/*  ���� BADV �Ĵ���            */
    REG_S       T1 , T0 , XBADVADDR

    CSRRD       T1 , CSR_EUEN                                           ;/*  ���� EUEN �Ĵ���            */
    REG_S       T1 , T0 , XEUEN
    MACRO_END()

;/*********************************************************************************************************
;  �ָ� CSR �Ĵ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_CSR)
    REG_L       T1 , T0 , XCRMD
    INT_SRLI    T2 , T1 , 6
    ANDI        T2 , T2 , 0x8
    ANDI        T1 , T1 , 0x7
    OR          T1 , T1 , T2
    CSRWR       T1 , CSR_PRMD                                           ;/*  �ָ� CRMD �� PLV IE WE      */

    REG_L       T1 , T0 , XEUEN
    CSRWR       T1 , CSR_EUEN                                           ;/*  �ָ� EUEN �Ĵ���            */
    MACRO_END()

;/*********************************************************************************************************
;  ����С�����ļĴ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(SAVE_SMALL_REG_CTX)
    ORI         T1 , ZERO , 1
    REG_S       T1 , T0 , CTX_TYPE_OFFSET                               ;/*  ���ΪС������              */

    ;/*
    ; * ���� S0-S8($23-$31)
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
    ; * ���� RA GP SP FP ($1-$3 $22)
    ; */
    REG_S       $r1  , T0 , XGREG(1)
    REG_S       $r2  , T0 , XGREG(2)
    REG_S       $r3  , T0 , XGREG(3)
    REG_S       $r22 , T0 , XGREG(22)

#if LW_CFG_CPU_FAST_TLS > 0
    REG_L       $r21 , T0 , XGREG(21)
#endif

    REG_S       RA , T0 , XPC                                           ;/*  RA ���� PC ����             */

    SAVE_CSR                                                            ;/*  ���� CSR �Ĵ���             */
    MACRO_END()

;/*********************************************************************************************************
;  �ָ�С�����ļĴ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_SMALL_REG_CTX)
    REG_L       T1 , T0 , XPC
    CSRWR       T1 , CSR_ERA                                            ;/*  PC �ָ��� ERA �Ĵ���        */

    RESTORE_CSR                                                         ;/*  �ָ� CSR �Ĵ���             */

    ;/*
    ; * �ָ� S0-S8($23-$31)
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
    ; * �ָ� RA GP SP A0 FP ($1-$4 $22)
    ; */
    REG_L       $r1  , T0 , XGREG(1)
    REG_L       $r2  , T0 , XGREG(2)
    REG_L       $r3  , T0 , XGREG(3)
    REG_L       $r4  , T0 , XGREG(4)
    REG_L       $r22 , T0 , XGREG(22)

#if LW_CFG_CPU_FAST_TLS > 0
    REG_L       $r21 , T0 , XGREG(21)
#endif

    ERTN                                                                ;/*  ����                        */
    MACRO_END()

;/*********************************************************************************************************
;  ���� $0 - $31 �Ĵ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(SAVE_GREGS)
    ;/*
    ; * $0 �̶�Ϊ 0
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
;  �ָ� $0 - $31 �Ĵ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_GREGS)
    ;/*
    ; * $0 �̶�Ϊ 0
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
    ; * $12 �� T0(�����ָ�)
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
;  ����������ļĴ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(SAVE_BIG_REG_CTX)
    SAVE_GREGS                                                          ;/*  ���� $0 - $31 �Ĵ���        */

    REG_S       ZERO , T0 , CTX_TYPE_OFFSET                             ;/*  ���Ϊ��������              */

    REG_S       RA , T0 , XPC                                           ;/*  RA ���� PC ����             */

    SAVE_CSR                                                            ;/*  ���� CSR �Ĵ���             */
    MACRO_END()

;/*********************************************************************************************************
;  �ָ��������ļĴ���(���� T0: ARCH_REG_CTX ��ַ)
;*********************************************************************************************************/

MACRO_DEF(RESTORE_BIG_REG_CTX)
    REG_L       T1 , T0 , XPC
    CSRWR       T1 , CSR_ERA                                            ;/*  PC �ָ��� ERA �Ĵ���        */

    RESTORE_CSR                                                         ;/*  �ָ� CSR �Ĵ���             */

    RESTORE_GREGS                                                       ;/*  �ָ� $0 - $31 �Ĵ���        */

    REG_L       $r12 , T0 , XGREG(12)                                   ;/*  �ָ� T0 �Ĵ���              */

    ERTN                                                                ;/*  ����                        */
    MACRO_END()

#endif
;/*********************************************************************************************************
;  END
;*********************************************************************************************************/

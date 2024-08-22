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
;** 文   件   名: x64ContextAsm.S
;**
;** 创   建   人: Jiao.JinXing (焦进星)
;**
;** 文件创建日期: 2017 年 06 月 06 日
;**
;** 描        述: x86-64 体系构架上下文切换.
;*********************************************************************************************************/

#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/assembler.h>
#include <config/kernel/kernel_cfg.h>
#include <config/loader/loader_cfg.h>
#include <config/mp/mp_cfg.h>
#include "x64ContextAsm.h"
#include "arch/x86/pentium/x86Pentium.h"

    FILE_BEGIN()

    IMPORT_LABEL(_SchedSwp)
#if LW_CFG_SMP_EN > 0
    IMPORT_LABEL(_SchedSafeStack)
#endif
#if LW_CFG_COROUTINE_EN > 0
    IMPORT_LABEL(_SchedCrSwp)
#endif

    EXPORT_LABEL(archTaskCtxStart)
    EXPORT_LABEL(archTaskCtxSwitch)
#if LW_CFG_COROUTINE_EN > 0
    EXPORT_LABEL(archCrtCtxSwitch)
#endif
    EXPORT_LABEL(archIntCtxLoad)
    EXPORT_LABEL(archSigCtxSave)
    EXPORT_LABEL(archSigCtxLoad)
#if LW_CFG_MODULELOADER_EN > 0
    EXPORT_LABEL(archTaskCtxTlsLoad)
#endif

;/*********************************************************************************************************
;  TLS 内存指针在 TCB 结构中的偏移量
;*********************************************************************************************************/

#define X86_64_TLS_OFFSET0   264
#define X86_64_TLS_OFFSET1   (X86_64_TLS_OFFSET0 + 4)

;/*********************************************************************************************************
;  x86-64 处理器当前线程上下文 TLS 加载
;*********************************************************************************************************/

#if LW_CFG_MODULELOADER_EN > 0
FUNC_DEF(archTaskCtxTlsLoad)
    MOVL    X86_64_TLS_OFFSET0(%RDI) , %EAX                             ;/*  Low-order 32 bits           */
    MOVL    X86_64_TLS_OFFSET1(%RDI) , %EDX                             ;/*  High-order 32 bits          */
    MOV     $X86_MSR_IA32_FS_BASE , %RCX                                ;/*  Specify FS_BASE to write    */
    WRMSR                                                               ;/*  Write %EDX:%EAX to the MSR  */
    MOV     %RDI, %RAX                                                  ;/*  RAX = 当前TCB的 REG_CTX 地址*/
    RET
    FUNC_END(archTaskCtxTlsLoad)
#endif

;/*********************************************************************************************************
;  当前 CPU 开始运行多任务执行
;  参数为当前 CPU 控制块(%RDI 寄存器)
;*********************************************************************************************************/

FUNC_DEF(archTaskCtxStart)
#if LW_CFG_MODULELOADER_EN > 0
    MOV     0(%RDI) , %RDI                                              ;/*  RDI = 当前TCB的 REG_CTX 地址*/

    MOVL    X86_64_TLS_OFFSET0(%RDI) , %EAX                             ;/*  Low-order 32 bits           */
    MOVL    X86_64_TLS_OFFSET1(%RDI) , %EDX                             ;/*  High-order 32 bits          */
    MOV     $X86_MSR_IA32_FS_BASE , %RCX                                ;/*  Specify FS_BASE to write    */
    WRMSR                                                               ;/*  Write %EDX:%EAX to the MSR  */
    MOV     %RDI, %RAX                                                  ;/*  RAX = 当前TCB的 REG_CTX 地址*/
#else
    MOV     0(%RDI) , %RAX                                              ;/*  RAX = 当前TCB的 REG_CTX 地址*/
#endif

    RESTORE_REGS                                                        ;/*  恢复所有寄存器              */
    FUNC_END(archTaskCtxStart)

;/*********************************************************************************************************
;  线程切换 
;  参数为当前 CPU 控制块(%RDI 寄存器)
;*********************************************************************************************************/

FUNC_DEF(archTaskCtxSwitch)
    MOV     0(%RDI) , %RAX                                              ;/*  RAX = 当前TCB的 REG_CTX 地址*/

    SAVE_REGS                                                           ;/*  保存所有寄存器              */

#if LW_CFG_SMP_EN > 0
    MOV     %RDI , %R12                                                 ;/*  参数 = 当前 CPU 控制块指针  */
    CALL    _SchedSafeStack                                             ;/*  _SchedSafeStack();          */
    MOV     %R12 , %RDI
    MOV     %RAX , %RSP
#endif

    MOV     %RDI , %R12                                                 ;/*  参数 = 当前 CPU 控制块指针  */
    CALL    _SchedSwp                                                   ;/*  _SchedSwp();                */

#if LW_CFG_MODULELOADER_EN > 0
    MOV     0(%R12) , %R12                                              ;/*  R12 = 当前TCB的 REG_CTX 地址*/

    MOVL    X86_64_TLS_OFFSET0(%R12) , %EAX                             ;/*  Low-order 32 bits           */
    MOVL    X86_64_TLS_OFFSET1(%R12) , %EDX                             ;/*  High-order 32 bits          */
    MOV     $X86_MSR_IA32_FS_BASE , %RCX                                ;/*  Specify FS_BASE to write    */
    WRMSR                                                               ;/*  Write %EDX:%EAX to the MSR  */
    MOV     %R12, %RAX
#else
    MOV     0(%R12) , %RAX                                              ;/*  RAX = 当前TCB的 REG_CTX 地址*/
#endif

    RESTORE_REGS                                                        ;/*  恢复所有寄存器              */
    FUNC_END(archTaskCtxSwitch)

;/*********************************************************************************************************
;  协程切换 
;  参数为当前 CPU 控制块(%RDI 寄存器)
;*********************************************************************************************************/

#if LW_CFG_COROUTINE_EN > 0
FUNC_DEF(archCrtCtxSwitch)
    MOV     16(%RDI) , %RAX                                             ;/*  RAX = 当前CCB的 REG_CTX 地址*/

    SAVE_REGS                                                           ;/*  保存所有寄存器              */

    MOV     %RDI , %R12                                                 ;/*  参数 = 当前 CPU 控制块指针  */
    CALL    _SchedCrSwp                                                 ;/*  _SchedCrSwp();              */
    MOV     %R12 , %RDI

    MOV     16(%RDI) , %RAX                                             ;/*  RAX = 当前CCB的 REG_CTX 地址*/

    RESTORE_REGS                                                        ;/*  恢复所有寄存器              */
    FUNC_END(archCrtCtxSwitch)
#endif

;/*********************************************************************************************************
;  中断返回时, 线程装载 
;  参数为当前 CPU 控制块(%RDI 寄存器)
;*********************************************************************************************************/

FUNC_DEF(archIntCtxLoad)
#if LW_CFG_MODULELOADER_EN > 0
    MOV     0(%RDI) , %RDI                                              ;/*  RDI = 当前TCB的 REG_CTX 地址*/

    MOVL    X86_64_TLS_OFFSET0(%RDI) , %EAX                             ;/*  Low-order 32 bits           */
    MOVL    X86_64_TLS_OFFSET1(%RDI) , %EDX                             ;/*  High-order 32 bits          */
    MOV     $X86_MSR_IA32_FS_BASE , %RCX                                ;/*  Specify FS_BASE to write    */
    WRMSR                                                               ;/*  Write %EDX:%EAX to the MSR  */
    MOV     %RDI, %RAX
#else
    MOV     0(%RDI) , %RAX                                              ;/*  RAX = 当前TCB的 REG_CTX 地址*/
#endif

    RESTORE_REGS                                                        ;/*  恢复所有寄存器              */
    FUNC_END(archIntCtxLoad)

;/*********************************************************************************************************
;  信号上下文保存
;  参数为 ARCH_REG_CTX 指针(%RDI 寄存器)
;*********************************************************************************************************/

FUNC_DEF(archSigCtxSave)
    MOV     %RDI , %RAX                                                 ;/*  RAX = ARCH_REG_CTX 指针     */

    SAVE_REGS

    MOVL    $0 , %EAX                                                   ;/*  return 0                    */
    RET
    FUNC_END(archSigCtxSave)

;/*********************************************************************************************************
;  信号上下文返回
;  参数为 ARCH_REG_CTX 指针(%RDI 寄存器)
;*********************************************************************************************************/

FUNC_DEF(archSigCtxLoad)
    MOV     %RDI , %RAX                                                 ;/*  RAX = ARCH_REG_CTX 指针     */

    RESTORE_REGS                                                        ;/*  恢复所有寄存器              */
    FUNC_END(archSigCtxLoad)

    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/

#*********************************************************************************************************
#
#                                    中国软件开源组织
#
#                                   嵌入式实时操作系统
#
#                                SylixOS(TM)  LW : long wing
#
#                               Copyright All Rights Reserved
#
#--------------文件信息--------------------------------------------------------------------------------
#
# 文   件   名: platforms.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2024 年 04 月 17 日
#
# 描        述: 平台编译参数
#*********************************************************************************************************
ARM_926H           := arm-sylixos-eabi-          arm926ej-s      reserve                auto
ARM_926S           := arm-sylixos-eabi-          arm926ej-s      reserve                disable
ARM_920T           := arm-sylixos-eabi-          arm920t         reserve                disable
ARM_A5             := arm-sylixos-eabi-          cortex-a5       reserve                auto
ARM_A5_SOFT        := arm-sylixos-eabi-          cortex-a5       reserve                disable
ARM_A7             := arm-sylixos-eabi-          cortex-a7       reserve                auto
ARM_A7_SOFT        := arm-sylixos-eabi-          cortex-a7       reserve                disable
ARM_A8             := arm-sylixos-eabi-          cortex-a8       reserve                auto
ARM_A8_SOFT        := arm-sylixos-eabi-          cortex-a8       reserve                disable
ARM_A9             := arm-sylixos-eabi-          cortex-a9       reserve                auto
ARM_A9_SOFT        := arm-sylixos-eabi-          cortex-a9       reserve                disable
ARM_A15            := arm-sylixos-eabi-          cortex-a15      reserve                auto
ARM_A15_SOFT       := arm-sylixos-eabi-          cortex-a15      reserve                disable
ARM_V7A            := arm-sylixos-eabi-          generic-armv7-a reserve                auto
ARM_V7A_SOFT       := arm-sylixos-eabi-          generic-armv7-a reserve                disable

ARM64_A53          := aarch64-sylixos-elf-       cortex-a53      reserve                default
ARM64_A55          := aarch64-sylixos-elf-       cortex-a55      reserve                default
ARM64_A57          := aarch64-sylixos-elf-       cortex-a57      reserve                default
ARM64_A72          := aarch64-sylixos-elf-       cortex-a72      reserve                default
ARM64_GENERIC      := aarch64-sylixos-elf-       generic         reserve                default

MIPS32             := mips-sylixos-elf-          mips32          reserve                hard-float
MIPS32_SOFT        := mips-sylixos-elf-          mips32          reserve                soft-float
MIPS32_R2          := mips-sylixos-elf-          mips32r2        reserve                hard-float
MIPS32_R2_SOFT     := mips-sylixos-elf-          mips32r2        reserve                soft-float

MIPS64_R2          := mips64-sylixos-elf-        mips64r2        reserve                hard-float
MIPS64_R2_SOFT     := mips64-sylixos-elf-        mips64r2        reserve                soft-float
MIPS64_LS3A        := mips64-sylixos-elf-        mips64r2        -mtune=loongson3a      hard-float
MIPS64_LS3A_SOFT   := mips64-sylixos-elf-        mips64r2        -mtune=loongson3a      soft-float

x86_PENTIUM        := i386-sylixos-elf-          pentium         reserve                hard-float
x86_PENTIUM_SOFT   := i386-sylixos-elf-          pentium         reserve                soft-float

X86_64             := x86_64-sylixos-elf-        generic         reserve                hard-float

PPC_750            := ppc-sylixos-eabi-          750             reserve                hard-float
PPC_750_SOFT       := ppc-sylixos-eabi-          750             reserve                soft-float
PPC_464FP          := ppc-sylixos-eabi-          464fp           reserve                hard-float
PPC_464FP_SOFT     := ppc-sylixos-eabi-          464             reserve                soft-float
PPC_E500V1         := ppc-sylixos-eabi-          8540            reserve                hard-float -mspe -mfloat-gprs=single
PPC_E500V1_SOFT    := ppc-sylixos-eabi-          8540            reserve                soft-float
PPC_E500V2         := ppc-sylixos-eabi-          8548            reserve                hard-float -mspe -mfloat-gprs=double
PPC_E500V2_SOFT    := ppc-sylixos-eabi-          8548            reserve                soft-float
PPC_E500MC         := ppc-sylixos-eabi-          e500mc          reserve                hard-float
PPC_E500MC_SOFT    := ppc-sylixos-eabi-          e500mc          reserve                soft-float
PPC_E5500          := ppc-sylixos-eabi-          e5500           -m32                   hard-float
PPC_E5500_SOFT     := ppc-sylixos-eabi-          e5500           -m32                   soft-float
PPC_E6500          := ppc-sylixos-eabi-          e6500           -m32                   hard-float -maltivec
PPC_E6500_SOFT     := ppc-sylixos-eabi-          e6500           -m32                   soft-float

SPARC_LEON3        := sparc-sylixos-elf-         leon3           reserve                hard-float
SPARC_LEON3_SOFT   := sparc-sylixos-elf-         leon3           reserve                soft-float
SPARC_V8           := sparc-sylixos-elf-         v8              reserve                hard-float
SPARC_V8_SOFT      := sparc-sylixos-elf-         v8              reserve                soft-float

RISCV_GC32         := riscv-sylixos-elf-         rv32imafc       reserve                ilp32f
RISCV_GC32_SOFT    := riscv-sylixos-elf-         rv32imac        reserve                ilp32
RISCV_GC64         := riscv-sylixos-elf-         rv64imafdc      reserve                lp64d
RISCV_GC64_SOFT    := riscv-sylixos-elf-         rv64imac        reserve                lp64

LOONGARCH64        := loongarch64-sylixos-elf-   loongarch64     reserve                double-float
LOONGARCH64_SOFT   := loongarch64-sylixos-elf-   loongarch64     reserve                soft-float

CSKY_CK807         := csky-sylixos-elfabiv2-     ck807           reserve                float-abi=softfp
CSKY_CK807_SOFT    := csky-sylixos-elfabiv2-     ck807           reserve                float-abi=soft
CSKY_CK810         := csky-sylixos-elfabiv2-     ck810           reserve                float-abi=softfp
CSKY_CK810_SOFT    := csky-sylixos-elfabiv2-     ck810           reserve                float-abi=soft
CSKY_CK860         := csky-sylixos-elfabiv2-     ck860           reserve                float-abi=softfp
CSKY_CK860_SOFT    := csky-sylixos-elfabiv2-     ck860           reserve                float-abi=soft
#*********************************************************************************************************
# End
#*********************************************************************************************************

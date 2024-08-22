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
# 文   件   名: multi-platform.mk
#
# 创   建   人: Jiao.JinXing(焦进星)
#
# 文件创建日期: 2022 年 07 月 25 日
#
# 描        述: 多平台构建 makefile 模板
#*********************************************************************************************************

#*********************************************************************************************************
# Include config.mk
#*********************************************************************************************************
CONFIG_MK_EXIST = $(shell if [ -f ../config.mk ]; then echo exist; else echo notexist; fi;)
ifeq ($(CONFIG_MK_EXIST), exist)
include ../config.mk
else
CONFIG_MK_EXIST = $(shell if [ -f config.mk ]; then echo exist; else echo notexist; fi;)
ifeq ($(CONFIG_MK_EXIST), exist)
include config.mk
else
CONFIG_MK_EXIST =
endif
endif

#*********************************************************************************************************
# Include sylixos base config.mk
#*********************************************************************************************************
EMPTY =
SPACE = $(EMPTY) $(EMPTY)

SYLIXOS_BASE_PATH_BAK    := $(SYLIXOS_BASE_PATH)
TOOLCHAIN_PREFIX_BAK     := $(TOOLCHAIN_PREFIX)
DEBUG_LEVEL_BAK          := $(DEBUG_LEVEL)
CPU_TYPE_BAK             := $(CPU_TYPE)
FPU_TYPE_BAK             := $(FPU_TYPE)
FLOAT_ABI_BAK            := $(FLOAT_ABI)
MULTI_PLATFORM_BUILD_BAK := $(MULTI_PLATFORM_BUILD)
PLATFORMS_BAK            := $(PLATFORMS)

SYLIXOS_BASE_CONFIGMK    = $(subst $(SPACE),\ ,$(SYLIXOS_BASE_PATH))/config.mk
SYLIXOS_BASE_PLATFORMSMK = $(subst $(SPACE),\ ,$(SYLIXOS_BASE_PATH))/platforms.mk

include $(SYLIXOS_BASE_PLATFORMSMK)
include $(SYLIXOS_BASE_CONFIGMK)

SYLIXOS_BASE_PATH := $(SYLIXOS_BASE_PATH_BAK)
DEBUG_LEVEL       := $(DEBUG_LEVEL_BAK)

ifneq ($(TOOLCHAIN_PREFIX_BAK),)
TOOLCHAIN_PREFIX := $(TOOLCHAIN_PREFIX_BAK)
endif

ifneq ($(CPU_TYPE_BAK),)
CPU_TYPE := $(CPU_TYPE_BAK)
endif

ifneq ($(FPU_TYPE_BAK),)
FPU_TYPE := $(FPU_TYPE_BAK)
endif

ifneq ($(FLOAT_ABI_BAK),)
FLOAT_ABI := $(FLOAT_ABI_BAK)
endif

ifneq ($(MULTI_PLATFORM_BUILD_BAK),)
MULTI_PLATFORM_BUILD := $(MULTI_PLATFORM_BUILD_BAK)
endif

ifneq ($(PLATFORMS_BAK),)
PLATFORMS := $(PLATFORMS_BAK)
endif

#*********************************************************************************************************
# Multi-platform SDK
#*********************************************************************************************************
ifeq ($(MULTI_PLATFORM_BUILD), yes)

#*********************************************************************************************************
# all
#*********************************************************************************************************
.PHONY: all
all:
	@$(foreach platform,$(PLATFORMS),make all PLATFORM_NAME=$(platform) SYLIXOS_BASE_PATH="$(SYLIXOS_BASE_PATH)" TOOLCHAIN_PREFIX=$(word 1, $($(platform))) CPU_TYPE="$(filter-out reserve,$(wordlist 2, 3, $($(platform))))" FPU_TYPE="$(wordlist 4, 6, $($(platform)))";)

#*********************************************************************************************************
# clean
#*********************************************************************************************************
.PHONY: clean
clean:
	@$(foreach platform,$(PLATFORMS),make clean PLATFORM_NAME=$(platform) SYLIXOS_BASE_PATH="$(SYLIXOS_BASE_PATH)" TOOLCHAIN_PREFIX=$(word 1, $($(platform))) CPU_TYPE="$(filter-out reserve,$(wordlist 2, 3, $($(platform))))" FPU_TYPE="$(wordlist 4, 6, $($(platform)))";)

#*********************************************************************************************************
# Single-platform SDK
#*********************************************************************************************************
else

#*********************************************************************************************************
# all
#*********************************************************************************************************
.PHONY: all
all:
	make all -j

#*********************************************************************************************************
# clean
#*********************************************************************************************************
.PHONY: clean
clean:
	make clean -j

endif
#*********************************************************************************************************
# End
#*********************************************************************************************************

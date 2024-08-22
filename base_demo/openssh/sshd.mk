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
# 文   件   名: sshd.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2023 年 03 月 13 日
#
# 描        述: 本文件由 RealEvo-IDE 生成，用于配置 Makefile 功能，请勿手动修改
#*********************************************************************************************************

#*********************************************************************************************************
# Clear setting
#*********************************************************************************************************
include $(CLEAR_VARS_MK)

#*********************************************************************************************************
# Target
#*********************************************************************************************************
LOCAL_TARGET_NAME := sshd

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/openssh-9.2p1/audit-bsm.c \
src/openssh-9.2p1/audit-linux.c \
src/openssh-9.2p1/audit.c \
src/openssh-9.2p1/auth-bsdauth.c \
src/openssh-9.2p1/auth-krb5.c \
src/openssh-9.2p1/auth-options.c \
src/openssh-9.2p1/auth-pam.c \
src/openssh-9.2p1/auth-passwd.c \
src/openssh-9.2p1/auth-rhosts.c \
src/openssh-9.2p1/auth-shadow.c \
src/openssh-9.2p1/auth-sia.c \
src/openssh-9.2p1/auth.c \
src/openssh-9.2p1/auth2-chall.c \
src/openssh-9.2p1/auth2-gss.c \
src/openssh-9.2p1/auth2-hostbased.c \
src/openssh-9.2p1/auth2-kbdint.c \
src/openssh-9.2p1/auth2-none.c \
src/openssh-9.2p1/auth2-passwd.c \
src/openssh-9.2p1/auth2-pubkey.c \
src/openssh-9.2p1/auth2-pubkeyfile.c \
src/openssh-9.2p1/auth2.c \
src/openssh-9.2p1/groupaccess.c \
src/openssh-9.2p1/gss-serv-krb5.c \
src/openssh-9.2p1/gss-serv.c \
src/openssh-9.2p1/loginrec.c \
src/openssh-9.2p1/monitor.c \
src/openssh-9.2p1/monitor_wrap.c \
src/openssh-9.2p1/platform.c \
src/openssh-9.2p1/sandbox-capsicum.c \
src/openssh-9.2p1/sandbox-darwin.c \
src/openssh-9.2p1/sandbox-null.c \
src/openssh-9.2p1/sandbox-pledge.c \
src/openssh-9.2p1/sandbox-rlimit.c \
src/openssh-9.2p1/sandbox-seccomp-filter.c \
src/openssh-9.2p1/sandbox-solaris.c \
src/openssh-9.2p1/sandbox-systrace.c \
src/openssh-9.2p1/servconf.c \
src/openssh-9.2p1/serverloop.c \
src/openssh-9.2p1/session.c \
src/openssh-9.2p1/sftp-common.c \
src/openssh-9.2p1/sftp-server.c \
src/openssh-9.2p1/srclimit.c \
src/openssh-9.2p1/sshd.c \
src/openssh-9.2p1/sshlogin.c \
src/openssh-9.2p1/sshpty.c \
src/openssh-9.2p1/uidswap.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/openssh-9.2p1" \
-I"./src/openssh-9.2p1/openbsd-compat" \
-I"$(SYLIXOS_BASE_PATH)/zlib/zlib" \
-I"$(SYLIXOS_BASE_PATH)/libcextern/libcextern/include" \
-I"$(SYLIXOS_BASE_PATH)/openssl/openssl/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL :=  \
-DHAVE_CONFIG_H

#*********************************************************************************************************
# Compiler flags
#*********************************************************************************************************
LOCAL_CFLAGS := 
LOCAL_CXXFLAGS := 
LOCAL_LINKFLAGS := 

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB :=  \
-lopenbsd-compat \
-lssh \
-lz \
-lcrypto \
-lssl \
-lcextern

LOCAL_DEPEND_LIB_PATH :=  \
-L"$(OUTDIR)" \
-L"$(SYLIXOS_BASE_PATH)/zlib/$(OUTDIR)" \
-L"$(SYLIXOS_BASE_PATH)/libcextern/$(OUTDIR)" \
-L"$(SYLIXOS_BASE_PATH)/openssl/$(OUTDIR)"

#*********************************************************************************************************
# C++ config
#*********************************************************************************************************
LOCAL_USE_CXX        := no
LOCAL_USE_CXX_EXCEPT := no

#*********************************************************************************************************
# Linker specific
#*********************************************************************************************************
LOCAL_NO_UNDEF_SYM := no

#*********************************************************************************************************
# Code coverage config
#*********************************************************************************************************
LOCAL_USE_GCOV := no

#*********************************************************************************************************
# OpenMP config
#*********************************************************************************************************
LOCAL_USE_OMP := no

#*********************************************************************************************************
# Use short command for link and ar
#*********************************************************************************************************
LOCAL_USE_SHORT_CMD := no

#*********************************************************************************************************
# User link command
#*********************************************************************************************************
LOCAL_PRE_LINK_CMD := 
LOCAL_POST_LINK_CMD := 
LOCAL_PRE_STRIP_CMD := 
LOCAL_POST_STRIP_CMD := 

#*********************************************************************************************************
# Depend target
#*********************************************************************************************************
LOCAL_DEPEND_TARGET := \
$(OUTDIR)/libopenbsd-compat.so \
$(OUTDIR)/libssh.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

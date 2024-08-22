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
# 文   件   名: libssh.mk
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
LOCAL_TARGET_NAME := libssh.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/openssh-9.2p1/addr.c \
src/openssh-9.2p1/addrmatch.c \
src/openssh-9.2p1/atomicio.c \
src/openssh-9.2p1/authfd.c \
src/openssh-9.2p1/authfile.c \
src/openssh-9.2p1/bitmap.c \
src/openssh-9.2p1/canohost.c \
src/openssh-9.2p1/chacha.c \
src/openssh-9.2p1/channels.c \
src/openssh-9.2p1/cipher-aes.c \
src/openssh-9.2p1/cipher-aesctr.c \
src/openssh-9.2p1/cipher-chachapoly-libcrypto.c \
src/openssh-9.2p1/cipher-chachapoly.c \
src/openssh-9.2p1/cipher.c \
src/openssh-9.2p1/cleanup.c \
src/openssh-9.2p1/compat.c \
src/openssh-9.2p1/dh.c \
src/openssh-9.2p1/digest-libc.c \
src/openssh-9.2p1/digest-openssl.c \
src/openssh-9.2p1/dispatch.c \
src/openssh-9.2p1/dns.c \
src/openssh-9.2p1/ed25519.c \
src/openssh-9.2p1/entropy.c \
src/openssh-9.2p1/fatal.c \
src/openssh-9.2p1/gss-genr.c \
src/openssh-9.2p1/hash.c \
src/openssh-9.2p1/hmac.c \
src/openssh-9.2p1/hostfile.c \
src/openssh-9.2p1/kex.c \
src/openssh-9.2p1/kexc25519.c \
src/openssh-9.2p1/kexdh.c \
src/openssh-9.2p1/kexecdh.c \
src/openssh-9.2p1/kexgen.c \
src/openssh-9.2p1/kexgex.c \
src/openssh-9.2p1/kexgexc.c \
src/openssh-9.2p1/kexgexs.c \
src/openssh-9.2p1/kexsntrup761x25519.c \
src/openssh-9.2p1/krl.c \
src/openssh-9.2p1/log.c \
src/openssh-9.2p1/mac.c \
src/openssh-9.2p1/match.c \
src/openssh-9.2p1/misc.c \
src/openssh-9.2p1/moduli.c \
src/openssh-9.2p1/monitor_fdpass.c \
src/openssh-9.2p1/msg.c \
src/openssh-9.2p1/nchan.c \
src/openssh-9.2p1/packet.c \
src/openssh-9.2p1/platform-misc.c \
src/openssh-9.2p1/platform-pledge.c \
src/openssh-9.2p1/platform-tracing.c \
src/openssh-9.2p1/poly1305.c \
src/openssh-9.2p1/progressmeter.c \
src/openssh-9.2p1/readpass.c \
src/openssh-9.2p1/rijndael.c \
src/openssh-9.2p1/sftp-realpath.c \
src/openssh-9.2p1/smult_curve25519_ref.c \
src/openssh-9.2p1/sntrup761.c \
src/openssh-9.2p1/ssh-dss.c \
src/openssh-9.2p1/ssh-ecdsa-sk.c \
src/openssh-9.2p1/ssh-ecdsa.c \
src/openssh-9.2p1/ssh-ed25519-sk.c \
src/openssh-9.2p1/ssh-ed25519.c \
src/openssh-9.2p1/ssh-pkcs11.c \
src/openssh-9.2p1/ssh-rsa.c \
src/openssh-9.2p1/ssh-xmss.c \
src/openssh-9.2p1/sshbuf-getput-basic.c \
src/openssh-9.2p1/sshbuf-getput-crypto.c \
src/openssh-9.2p1/sshbuf-io.c \
src/openssh-9.2p1/sshbuf-misc.c \
src/openssh-9.2p1/sshbuf.c \
src/openssh-9.2p1/ssherr.c \
src/openssh-9.2p1/sshkey-xmss.c \
src/openssh-9.2p1/sshkey.c \
src/openssh-9.2p1/ssh_api.c \
src/openssh-9.2p1/ttymodes.c \
src/openssh-9.2p1/umac.c \
src/openssh-9.2p1/umac128.c \
src/openssh-9.2p1/utf8.c \
src/openssh-9.2p1/xmalloc.c \
src/openssh-9.2p1/xmss_commons.c \
src/openssh-9.2p1/xmss_fast.c \
src/openssh-9.2p1/xmss_hash.c \
src/openssh-9.2p1/xmss_hash_address.c \
src/openssh-9.2p1/xmss_wots.c \
src/openssh-9.2p1/regress/misc/fuzz-harness/ssh-sk-null.cc 

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
-lz \
-lcrypto \
-lssl \
-lcextern

LOCAL_DEPEND_LIB_PATH :=  \
-L"$(SYLIXOS_BASE_PATH)/zlib/$(OUTDIR)" \
-L"$(SYLIXOS_BASE_PATH)/libcextern/$(OUTDIR)" \
-L"$(SYLIXOS_BASE_PATH)/openssl/$(OUTDIR)"

#*********************************************************************************************************
# C++ config
#*********************************************************************************************************
LOCAL_USE_CXX        := no
LOCAL_USE_CXX_EXCEPT := no

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
LOCAL_DEPEND_TARGET :=  

include $(LIBRARY_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

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
# 文   件   名: openbsd-compat.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2023 年 03 月 02 日
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
LOCAL_TARGET_NAME := libopenbsd-compat.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/openssh-9.2p1/openbsd-compat/arc4random.c \
src/openssh-9.2p1/openbsd-compat/arc4random_uniform.c \
src/openssh-9.2p1/openbsd-compat/base64.c \
src/openssh-9.2p1/openbsd-compat/basename.c \
src/openssh-9.2p1/openbsd-compat/bcrypt_pbkdf.c \
src/openssh-9.2p1/openbsd-compat/bindresvport.c \
src/openssh-9.2p1/openbsd-compat/blowfish.c \
src/openssh-9.2p1/openbsd-compat/bsd-asprintf.c \
src/openssh-9.2p1/openbsd-compat/bsd-closefrom.c \
src/openssh-9.2p1/openbsd-compat/bsd-cygwin_util.c \
src/openssh-9.2p1/openbsd-compat/bsd-err.c \
src/openssh-9.2p1/openbsd-compat/bsd-flock.c \
src/openssh-9.2p1/openbsd-compat/bsd-getentropy.c \
src/openssh-9.2p1/openbsd-compat/bsd-getline.c \
src/openssh-9.2p1/openbsd-compat/bsd-getpagesize.c \
src/openssh-9.2p1/openbsd-compat/bsd-getpeereid.c \
src/openssh-9.2p1/openbsd-compat/bsd-malloc.c \
src/openssh-9.2p1/openbsd-compat/bsd-misc.c \
src/openssh-9.2p1/openbsd-compat/bsd-nextstep.c \
src/openssh-9.2p1/openbsd-compat/bsd-openpty.c \
src/openssh-9.2p1/openbsd-compat/bsd-poll.c \
src/openssh-9.2p1/openbsd-compat/bsd-pselect.c \
src/openssh-9.2p1/openbsd-compat/bsd-setres_id.c \
src/openssh-9.2p1/openbsd-compat/bsd-signal.c \
src/openssh-9.2p1/openbsd-compat/bsd-snprintf.c \
src/openssh-9.2p1/openbsd-compat/bsd-statvfs.c \
src/openssh-9.2p1/openbsd-compat/bsd-timegm.c \
src/openssh-9.2p1/openbsd-compat/bsd-waitpid.c \
src/openssh-9.2p1/openbsd-compat/daemon.c \
src/openssh-9.2p1/openbsd-compat/dirname.c \
src/openssh-9.2p1/openbsd-compat/explicit_bzero.c \
src/openssh-9.2p1/openbsd-compat/fake-rfc2553.c \
src/openssh-9.2p1/openbsd-compat/fmt_scaled.c \
src/openssh-9.2p1/openbsd-compat/fnmatch.c \
src/openssh-9.2p1/openbsd-compat/freezero.c \
src/openssh-9.2p1/openbsd-compat/getcwd.c \
src/openssh-9.2p1/openbsd-compat/getgrouplist.c \
src/openssh-9.2p1/openbsd-compat/getopt_long.c \
src/openssh-9.2p1/openbsd-compat/getrrsetbyname-ldns.c \
src/openssh-9.2p1/openbsd-compat/getrrsetbyname.c \
src/openssh-9.2p1/openbsd-compat/glob.c \
src/openssh-9.2p1/openbsd-compat/inet_aton.c \
src/openssh-9.2p1/openbsd-compat/inet_ntoa.c \
src/openssh-9.2p1/openbsd-compat/inet_ntop.c \
src/openssh-9.2p1/openbsd-compat/kludge-fd_set.c \
src/openssh-9.2p1/openbsd-compat/libressl-api-compat.c \
src/openssh-9.2p1/openbsd-compat/md5.c \
src/openssh-9.2p1/openbsd-compat/memmem.c \
src/openssh-9.2p1/openbsd-compat/mktemp.c \
src/openssh-9.2p1/openbsd-compat/openssl-compat.c \
src/openssh-9.2p1/openbsd-compat/port-aix.c \
src/openssh-9.2p1/openbsd-compat/port-irix.c \
src/openssh-9.2p1/openbsd-compat/port-linux.c \
src/openssh-9.2p1/openbsd-compat/port-net.c \
src/openssh-9.2p1/openbsd-compat/port-prngd.c \
src/openssh-9.2p1/openbsd-compat/port-solaris.c \
src/openssh-9.2p1/openbsd-compat/port-uw.c \
src/openssh-9.2p1/openbsd-compat/pwcache.c \
src/openssh-9.2p1/openbsd-compat/readpassphrase.c \
src/openssh-9.2p1/openbsd-compat/reallocarray.c \
src/openssh-9.2p1/openbsd-compat/recallocarray.c \
src/openssh-9.2p1/openbsd-compat/rresvport.c \
src/openssh-9.2p1/openbsd-compat/setenv.c \
src/openssh-9.2p1/openbsd-compat/setproctitle.c \
src/openssh-9.2p1/openbsd-compat/sha1.c \
src/openssh-9.2p1/openbsd-compat/sha2.c \
src/openssh-9.2p1/openbsd-compat/sigact.c \
src/openssh-9.2p1/openbsd-compat/strcasestr.c \
src/openssh-9.2p1/openbsd-compat/strlcat.c \
src/openssh-9.2p1/openbsd-compat/strlcpy.c \
src/openssh-9.2p1/openbsd-compat/strmode.c \
src/openssh-9.2p1/openbsd-compat/strndup.c \
src/openssh-9.2p1/openbsd-compat/strnlen.c \
src/openssh-9.2p1/openbsd-compat/strptime.c \
src/openssh-9.2p1/openbsd-compat/strsep.c \
src/openssh-9.2p1/openbsd-compat/strtoll.c \
src/openssh-9.2p1/openbsd-compat/strtonum.c \
src/openssh-9.2p1/openbsd-compat/strtoul.c \
src/openssh-9.2p1/openbsd-compat/strtoull.c \
src/openssh-9.2p1/openbsd-compat/timingsafe_bcmp.c \
src/openssh-9.2p1/openbsd-compat/vis.c \
src/openssh-9.2p1/openbsd-compat/xcrypt.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/openssh-9.2p1/openbsd-compat" \
-I"./src/openssh-9.2p1" \
-I"$(SYLIXOS_BASE_PATH)/libcextern/libcextern/include" \
-I"$(SYLIXOS_BASE_PATH)/openssl/openssl/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := \
-D_FORTIFY_SOURCE=2 \
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
-lcrypto \
-lssl \
-lcextern

LOCAL_DEPEND_LIB_PATH :=  \
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

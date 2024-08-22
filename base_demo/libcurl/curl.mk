#*********************************************************************************************************
#
#                                    �й������Դ��֯
#
#                                   Ƕ��ʽʵʱ����ϵͳ
#
#                                SylixOS(TM)  LW : long wing
#
#                               Copyright All Rights Reserved
#
#--------------�ļ���Ϣ--------------------------------------------------------------------------------
#
# ��   ��   ��: curl.mk
#
# ��   ��   ��: RealEvo-IDE
#
# �ļ���������: 2022 �� 02 �� 24 ��
#
# ��        ��: ���ļ��� RealEvo-IDE ���ɣ��������� Makefile ���ܣ������ֶ��޸�
#*********************************************************************************************************

#*********************************************************************************************************
# Clear setting
#*********************************************************************************************************
include $(CLEAR_VARS_MK)

#*********************************************************************************************************
# Target
#*********************************************************************************************************
LOCAL_TARGET_NAME := curl

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
CURL_VERSION = 7.82.0

LOCAL_SRCS := \
curl-$(CURL_VERSION)/src/slist_wc.c \
curl-$(CURL_VERSION)/src/tool_binmode.c \
curl-$(CURL_VERSION)/src/tool_bname.c \
curl-$(CURL_VERSION)/src/tool_cb_dbg.c \
curl-$(CURL_VERSION)/src/tool_cb_hdr.c \
curl-$(CURL_VERSION)/src/tool_cb_prg.c \
curl-$(CURL_VERSION)/src/tool_cb_rea.c \
curl-$(CURL_VERSION)/src/tool_cb_see.c \
curl-$(CURL_VERSION)/src/tool_cb_wrt.c \
curl-$(CURL_VERSION)/src/tool_cfgable.c \
curl-$(CURL_VERSION)/src/tool_dirhie.c \
curl-$(CURL_VERSION)/src/tool_doswin.c \
curl-$(CURL_VERSION)/src/tool_easysrc.c \
curl-$(CURL_VERSION)/src/tool_filetime.c \
curl-$(CURL_VERSION)/src/tool_findfile.c \
curl-$(CURL_VERSION)/src/tool_formparse.c \
curl-$(CURL_VERSION)/src/tool_getparam.c \
curl-$(CURL_VERSION)/src/tool_getpass.c \
curl-$(CURL_VERSION)/src/tool_help.c \
curl-$(CURL_VERSION)/src/tool_helpers.c \
curl-$(CURL_VERSION)/src/tool_hugehelp.c \
curl-$(CURL_VERSION)/src/tool_libinfo.c \
curl-$(CURL_VERSION)/src/tool_listhelp.c \
curl-$(CURL_VERSION)/src/tool_main.c \
curl-$(CURL_VERSION)/src/tool_msgs.c \
curl-$(CURL_VERSION)/src/tool_operate.c \
curl-$(CURL_VERSION)/src/tool_operhlp.c \
curl-$(CURL_VERSION)/src/tool_panykey.c \
curl-$(CURL_VERSION)/src/tool_paramhlp.c \
curl-$(CURL_VERSION)/src/tool_parsecfg.c \
curl-$(CURL_VERSION)/src/tool_progress.c \
curl-$(CURL_VERSION)/src/tool_setopt.c \
curl-$(CURL_VERSION)/src/tool_sleep.c \
curl-$(CURL_VERSION)/src/tool_strdup.c \
curl-$(CURL_VERSION)/src/tool_urlglob.c \
curl-$(CURL_VERSION)/src/tool_util.c \
curl-$(CURL_VERSION)/src/tool_vms.c \
curl-$(CURL_VERSION)/src/tool_writeout.c \
curl-$(CURL_VERSION)/src/tool_writeout_json.c \
curl-$(CURL_VERSION)/src/tool_xattr.c \
curl-$(CURL_VERSION)/lib/strtoofft.c \
curl-$(CURL_VERSION)/lib/nonblock.c \
curl-$(CURL_VERSION)/lib/warnless.c \
curl-$(CURL_VERSION)/lib/curl_ctype.c \
curl-$(CURL_VERSION)/lib/curl_multibyte.c \
curl-$(CURL_VERSION)/lib/version_win32.c \
curl-$(CURL_VERSION)/lib/dynbuf.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH := -I"curl-$(CURL_VERSION)/include/"
LOCAL_INC_PATH += -I"curl-$(CURL_VERSION)/lib"
LOCAL_INC_PATH += -I"curl-$(CURL_VERSION)/src"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := -DHAVE_CONFIG_H
LOCAL_DSYMBOL += -DOPENSSL_SUPPRESS_DEPRECATED

#*********************************************************************************************************
# Compiler flags
#*********************************************************************************************************
LOCAL_CFLAGS   := 
LOCAL_CXXFLAGS := 
LOCAL_LINKFLAGS :=

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB      := -lcurl
LOCAL_DEPEND_LIB_PATH := \
-L"$(SYLIXOS_BASE_PATH)/libcurl/$(OUTDIR)" 

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
LOCAL_PRE_LINK_CMD   := 
LOCAL_POST_LINK_CMD  := 
LOCAL_PRE_STRIP_CMD  := 
LOCAL_POST_STRIP_CMD := 

#*********************************************************************************************************
# Depend target
#*********************************************************************************************************
LOCAL_DEPEND_TARGET := $(OUTDIR)/libcurl.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

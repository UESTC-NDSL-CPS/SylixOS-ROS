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
# 文   件   名: librmw.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2024 年 05 月 12 日
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
LOCAL_TARGET_NAME := librmw.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/rmw-6.1.1/rmw/src/allocators.c \
src/rmw-6.1.1/rmw/src/convert_rcutils_ret_to_rmw_ret.c \
src/rmw-6.1.1/rmw/src/event.c \
src/rmw-6.1.1/rmw/src/init.c \
src/rmw-6.1.1/rmw/src/init_options.c \
src/rmw-6.1.1/rmw/src/message_sequence.c \
src/rmw-6.1.1/rmw/src/names_and_types.c \
src/rmw-6.1.1/rmw/src/network_flow_endpoint.c \
src/rmw-6.1.1/rmw/src/network_flow_endpoint_array.c \
src/rmw-6.1.1/rmw/src/publisher_options.c \
src/rmw-6.1.1/rmw/src/qos_string_conversions.c \
src/rmw-6.1.1/rmw/src/sanity_checks.c \
src/rmw-6.1.1/rmw/src/security_options.c \
src/rmw-6.1.1/rmw/src/subscription_content_filter_options.c \
src/rmw-6.1.1/rmw/src/subscription_options.c \
src/rmw-6.1.1/rmw/src/time.c \
src/rmw-6.1.1/rmw/src/topic_endpoint_info.c \
src/rmw-6.1.1/rmw/src/topic_endpoint_info_array.c \
src/rmw-6.1.1/rmw/src/types.c \
src/rmw-6.1.1/rmw/src/validate_full_topic_name.c \
src/rmw-6.1.1/rmw/src/validate_namespace.c \
src/rmw-6.1.1/rmw/src/validate_node_name.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/rmw-6.1.1/rmw/include" \
-I"$(WORKSPACE_rcutils)/src/rcutils-5.1.5/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := 

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
-lrcutils \
-lrosidl_runtime_c
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rcutils)/Release/strip" \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_rclcpp)/Release/strip"

#*********************************************************************************************************
# C++ config
#*********************************************************************************************************
LOCAL_USE_CXX := yes
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
LOCAL_DEPEND_TARGET :=  \
$(WORKSPACE_rcutils)/Release/strip/librcutils.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_runtime_c.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

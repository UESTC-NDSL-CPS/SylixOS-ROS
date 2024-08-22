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
# 文   件   名: librmw_dds_common__rosidl_typesupport_introspection_cpp.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2024 年 07 月 22 日
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
LOCAL_TARGET_NAME := librmw_dds_common__rosidl_typesupport_introspection_cpp.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/rmw_dds_common/rosidl_typesupport_introspection_cpp/rmw_dds_common/msg/detail/gid__type_support.cpp \
src/rmw_dds_common/rosidl_typesupport_introspection_cpp/rmw_dds_common/msg/detail/node_entities_info__type_support.cpp \
src/rmw_dds_common/rosidl_typesupport_introspection_cpp/rmw_dds_common/msg/detail/participant_entities_info__type_support.cpp

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/rmw_dds_common/rosidl_typesupport_introspection_cpp" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl_typesupport/rosidl_typesupport_cpp/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_cpp/include" \
-I"./src/rmw_dds_common/rosidl_generator_cpp" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_introspection_cpp/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_introspection_c/include"

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
-lrosidl_runtime_c \
-lrosidl_typesupport_cpp \
-lrosidl_typesupport_introspection_cpp \
-lrosidl_typesupport_introspection_c
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip"

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
$(WORKSPACE_rosidl)/Release/strip/librosidl_runtime_c.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_cpp.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_introspection_cpp.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_introspection_c.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

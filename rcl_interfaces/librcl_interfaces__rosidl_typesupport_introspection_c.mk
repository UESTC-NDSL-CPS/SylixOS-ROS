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
# 文   件   名: librcl_interfaces__rosidl_typesupport_introspection_c.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2024 年 05 月 23 日
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
LOCAL_TARGET_NAME := librcl_interfaces__rosidl_typesupport_introspection_c.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/floating_point_range__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/integer_range__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/list_parameters_result__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/log__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/parameter_descriptor__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/parameter_event_descriptors__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/parameter_event__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/parameter_type__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/parameter_value__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/parameter__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/msg/detail/set_parameters_result__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/srv/detail/describe_parameters__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/srv/detail/get_parameters__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/srv/detail/get_parameter_types__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/srv/detail/list_parameters__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/srv/detail/set_parameters_atomically__type_support.c \
src/rcl_interfaces/rosidl_typesupport_introspection_c/rcl_interfaces/srv/detail/set_parameters__type_support.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/rcl_interfaces/rosidl_typesupport_introspection_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_introspection_c/include" \
-I"./src/rcl_interfaces/rosidl_generator_c" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_c" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_typesupport_introspection_c"

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
-lrosidl_typesupport_introspection_c \
-lrcl_interfaces__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_typesupport_introspection_c
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_rcl_interfaces)/Release/strip" \
-L"$(WORKSPACE_builtin_interfaces)/Release/strip"

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
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_introspection_c.so \
$(WORKSPACE_rcl_interfaces)/Release/strip/librcl_interfaces__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_typesupport_introspection_c.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

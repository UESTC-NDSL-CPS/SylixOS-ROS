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
# 文   件   名: libstd_msgs__rosidl_typesupport_fastrtps_c.mk
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
LOCAL_TARGET_NAME := libstd_msgs__rosidl_typesupport_fastrtps_c.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/bool__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/byte_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/byte__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/char__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/color_rgba__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/empty__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/float32_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/float32__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/float64_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/float64__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/header__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int16_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int16__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int32_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int32__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int64_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int64__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int8_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/int8__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/multi_array_dimension__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/multi_array_layout__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/string__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int16_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int16__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int32_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int32__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int64_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int64__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int8_multi_array__type_support_c.cpp \
src/std_msgs/rosidl_typesupport_fastrtps_c/std_msgs/msg/detail/u_int8__type_support_c.cpp

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/std_msgs/rosidl_typesupport_fastrtps_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl_typesupport_fastrtps/rosidl_typesupport_fastrtps_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl_typesupport_fastrtps/rosidl_typesupport_fastrtps_cpp/include" \
-I"$(WORKSPACE_libfastrtps-master)/src/Fast-CDR/include" \
-I"$(WORKSPACE_libfastrtps-master)/src/Fast-CDR/build/include" \
-I"./src/std_msgs/rosidl_generator_c" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_c"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := 

#*********************************************************************************************************
# Compiler flags
#*********************************************************************************************************
LOCAL_CFLAGS := 
LOCAL_CXXFLAGS := -fexceptions
LOCAL_LINKFLAGS := 

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB :=  \
-lrosidl_runtime_c \
-lrosidl_typesupport_fastrtps_c \
-lrosidl_typesupport_fastrtps_cpp \
-lfastcdr \
-lstd_msgs__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_generator_c
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_libfastrtps-master)/Release/strip" \
-L"$(WORKSPACE_std_msgs)/Release/strip" \
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
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_fastrtps_c.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_fastrtps_cpp.so \
$(WORKSPACE_libfastrtps-master)/Release/strip/libfastcdr.so \
$(WORKSPACE_std_msgs)/Release/strip/libstd_msgs__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_generator_c.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

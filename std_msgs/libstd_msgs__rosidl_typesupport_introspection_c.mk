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
# ��   ��   ��: libstd_msgs__rosidl_typesupport_introspection_c.mk
#
# ��   ��   ��: RealEvo-IDE
#
# �ļ���������: 2024 �� 05 �� 23 ��
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
LOCAL_TARGET_NAME := libstd_msgs__rosidl_typesupport_introspection_c.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/bool__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/byte_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/byte__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/char__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/color_rgba__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/empty__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/float32_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/float32__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/float64_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/float64__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/header__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int16_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int16__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int32_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int32__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int64_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int64__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int8_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/int8__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/multi_array_dimension__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/multi_array_layout__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/string__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int16_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int16__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int32_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int32__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int64_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int64__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int8_multi_array__type_support.c \
src/std_msgs/rosidl_typesupport_introspection_c/std_msgs/msg/detail/u_int8__type_support.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/std_msgs/rosidl_typesupport_introspection_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_introspection_c/include" \
-I"./src/std_msgs/rosidl_generator_c" \
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
-lstd_msgs__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_typesupport_introspection_c
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip" \
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
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_introspection_c.so \
$(WORKSPACE_std_msgs)/Release/strip/libstd_msgs__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_typesupport_introspection_c.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

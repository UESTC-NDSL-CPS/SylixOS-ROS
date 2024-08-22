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
# ��   ��   ��: librcl_interfaces__rosidl_generator_c.mk
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
LOCAL_TARGET_NAME := librcl_interfaces__rosidl_generator_c.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/floating_point_range__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/integer_range__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/list_parameters_result__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/log__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/parameter_descriptor__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/parameter_event_descriptors__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/parameter_event__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/parameter_type__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/parameter_value__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/parameter__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/msg/detail/set_parameters_result__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/srv/detail/describe_parameters__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/srv/detail/get_parameters__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/srv/detail/get_parameter_types__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/srv/detail/list_parameters__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/srv/detail/set_parameters_atomically__functions.c \
src/rcl_interfaces/rosidl_generator_c/rcl_interfaces/srv/detail/set_parameters__functions.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/rcl_interfaces/rosidl_generator_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"$(WORKSPACE_rcutils)/src/rcutils-5.1.5/include" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_c"

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
-lrcutils \
-lbuiltin_interfaces__rosidl_generator_c
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_rcutils)/Release/strip" \
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
$(WORKSPACE_rcutils)/Release/strip/librcutils.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_generator_c.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

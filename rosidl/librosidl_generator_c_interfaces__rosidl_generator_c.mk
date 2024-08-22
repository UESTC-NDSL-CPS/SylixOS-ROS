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
# ��   ��   ��: librosidl_generator_c_interfaces__rosidl_generator_c.mk
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
LOCAL_TARGET_NAME := librosidl_generator_c_interfaces__rosidl_generator_c.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/arrays__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/basic_types__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/bounded_plain_sequences__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/bounded_sequences__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/constants__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/defaults__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/empty__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/multi_nested__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/nested__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/strings__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/unbounded_sequences__functions.c \
src/rosidl/rosidl_generator_c/rosidl_generator_c/rosidl_generator_c/msg/detail/w_strings__functions.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/rosidl/rosidl_generator_c/rosidl_generator_c" \
-I"./src/rosidl/rosidl_runtime_c/include" \
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
-lrcutils
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rcutils)/Release/strip"

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
$(WORKSPACE_rcutils)/Release/strip/librcutils.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

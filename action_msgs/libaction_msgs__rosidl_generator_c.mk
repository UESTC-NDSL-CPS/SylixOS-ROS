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
# ��   ��   ��: libaction_msgs__rosidl_generator_c.mk
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
LOCAL_TARGET_NAME := libaction_msgs__rosidl_generator_c.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/action_msgs/rosidl_generator_c/action_msgs/msg/detail/goal_info__functions.c \
src/action_msgs/rosidl_generator_c/action_msgs/msg/detail/goal_status_array__functions.c \
src/action_msgs/rosidl_generator_c/action_msgs/msg/detail/goal_status__functions.c \
src/action_msgs/rosidl_generator_c/action_msgs/srv/detail/cancel_goal__functions.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/action_msgs/rosidl_generator_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_unique_identifier_msgs)/src/unique_identifier_msgs/rosidl_generator_c" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_c" \
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
-lrosidl_runtime_c \
-lunique_identifier_msgs__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_generator_c \
-lrcutils
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_unique_identifier_msgs)/Release/strip" \
-L"$(WORKSPACE_builtin_interfaces)/Release/strip" \
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
$(WORKSPACE_rosidl)/Release/strip/librosidl_runtime_c.so \
$(WORKSPACE_unique_identifier_msgs)/Release/strip/libunique_identifier_msgs__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_generator_c.so \
$(WORKSPACE_rcutils)/Release/strip/librcutils.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

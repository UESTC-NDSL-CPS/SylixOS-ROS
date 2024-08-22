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
# ��   ��   ��: librclcpp-action.mk
#
# ��   ��   ��: RealEvo-IDE
#
# �ļ���������: 2024 �� 05 �� 14 ��
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
LOCAL_TARGET_NAME := librclcpp-action.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/rclcpp-16.0.8/rclcpp_action/src/client.cpp \
src/rclcpp-16.0.8/rclcpp_action/src/qos.cpp \
src/rclcpp-16.0.8/rclcpp_action/src/server.cpp \
src/rclcpp-16.0.8/rclcpp_action/src/server_goal_handle.cpp \
src/rclcpp-16.0.8/rclcpp_action/src/types.cpp

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/rclcpp-16.0.8/rclcpp_action/include" \
-I"$(WORKSPACE_RCL)/src/rcl-5.3.7/rcl_action/include" \
-I"$(WORKSPACE_RCL)/src/rcl-5.3.7/rcl/include" \
-I"$(WORKSPACE_rcutils)/src/rcutils-5.1.5/include" \
-I"$(WORKSPACE_rmw)/src/rmw-6.1.1/rmw/include" \
-I"./src/rclcpp-16.0.8/rclcpp/include" \
-I"$(WORKSPACE_rcpputils)/src/rcpputils-2.4.2/include" \
-I"$(WORKSPACE_action_msgs)/src/action_msgs/rosidl_generator_c" \
-I"$(WORKSPACE_unique_identifier_msgs)/src/unique_identifier_msgs/rosidl_generator_c" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"$(WORKSPACE_action_msgs)/src/action_msgs/rosidl_generator_cpp" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_cpp" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_cpp/include" \
-I"$(WORKSPACE_unique_identifier_msgs)/src/unique_identifier_msgs/rosidl_generator_cpp" \
-I"$(WORKSPACE_libstatistics_collector)/src/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := 

#*********************************************************************************************************
# Compiler flags
#*********************************************************************************************************
LOCAL_CFLAGS := 
LOCAL_CXXFLAGS := -fpermissive -fexceptions -frtti -std=c++17 -Wno-register
LOCAL_LINKFLAGS := 

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB :=  \
-lrcl-action \
-lrcutils \
-lrmw \
-lrclcpp \
-lrcpputils \
-laction_msgs__rosidl_generator_c \
-lunique_identifier_msgs__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_generator_c \
-lrosidl_runtime_c \
-llibstatistics_collector
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_RCL)/Release/strip" \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_rcutils)/Release/strip" \
-L"$(WORKSPACE_rmw)/Release/strip" \
-L"$(WORKSPACE_rclcpp)/Release/strip" \
-L"$(WORKSPACE_rcpputils)/Release/strip" \
-L"$(WORKSPACE_rcl_interface)/Release/strip" \
-L"$(WORKSPACE_rosidl)/Release" \
-L"$(WORKSPACE_action_msgs)/Release/strip" \
-L"$(WORKSPACE_unique_identifier_msgs)/Release/strip" \
-L"$(WORKSPACE_builtin_interfaces)/Release/strip" \
-L"$(WORKSPACE_libstatistics_collector)/Release/strip"

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
$(WORKSPACE_RCL)/Release/strip/librcl-action.so \
$(WORKSPACE_rcutils)/Release/strip/librcutils.so \
$(WORKSPACE_rmw)/Release/strip/librmw.so \
$(WORKSPACE_rclcpp)/Release/strip/librclcpp.so \
$(WORKSPACE_rcpputils)/Release/strip/librcpputils.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_generator_c.so \
$(WORKSPACE_unique_identifier_msgs)/Release/strip/libunique_identifier_msgs__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_generator_c.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_runtime_c.so \
$(WORKSPACE_libstatistics_collector)/Release/strip/liblibstatistics_collector.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

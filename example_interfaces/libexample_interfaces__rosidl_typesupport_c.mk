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
# ��   ��   ��: libexample_interfaces__rosidl_typesupport_c.mk
#
# ��   ��   ��: RealEvo-IDE
#
# �ļ���������: 2024 �� 07 �� 31 ��
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
LOCAL_TARGET_NAME := libexample_interfaces__rosidl_typesupport_c.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/action/fibonacci__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/bool__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/byte_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/byte__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/char__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/empty__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/float32_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/float32__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/float64_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/float64__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int16_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int16__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int32_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int32__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int64_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int64__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int8_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/int8__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/multi_array_dimension__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/multi_array_layout__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/string__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int16_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int16__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int32_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int32__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int64_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int64__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int8_multi_array__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/u_int8__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/msg/w_string__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/srv/add_two_ints__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/srv/set_bool__type_support.cpp \
src/example_interfaces/rosidl_typesupport_c/example_interfaces/srv/trigger__type_support.cpp

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/example_interfaces/rosidl_typesupport_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"./src/example_interfaces/rosidl_generator_c" \
-I"$(WORKSPACE_rosidl)/src/rosidl_typesupport/rosidl_typesupport_c/include" \
-I"$(WORKSPACE_unique_identifier_msgs)/src/unique_identifier_msgs/rosidl_generator_c" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_c" \
-I"$(WORKSPACE_action_msgs)/src/action_msgs/rosidl_generator_c"

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
-lexample_interfaces__rosidl_generator_c \
-lrosidl_typesupport_c \
-lunique_identifier_msgs__rosidl_generator_c \
-lbuiltin_interfaces__rosidl_generator_c \
-laction_msgs__rosidl_generator_c \
-laction_msgs__rosidl_typesupport_c \
-laction_msgs__rosidl_typesupport_cpp \
-laction_msgs__rosidl_typesupport_fastrtps_c \
-laction_msgs__rosidl_typesupport_fastrtps_cpp \
-laction_msgs__rosidl_typesupport_introspection_c \
-laction_msgs__rosidl_typesupport_introspection_cpp
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_example_interfaces)/Release/strip" \
-L"$(WORKSPACE_unique_identifier_msgs)/Release/strip" \
-L"$(WORKSPACE_builtin_interfaces)/Release/strip" \
-L"$(WORKSPACE_action_msgs)/Release/strip"

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
$(WORKSPACE_example_interfaces)/Release/strip/libexample_interfaces__rosidl_generator_c.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_c.so \
$(WORKSPACE_unique_identifier_msgs)/Release/strip/libunique_identifier_msgs__rosidl_generator_c.so \
$(WORKSPACE_builtin_interfaces)/Release/strip/libbuiltin_interfaces__rosidl_generator_c.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_generator_c.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_typesupport_c.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_typesupport_cpp.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_typesupport_fastrtps_c.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_typesupport_fastrtps_cpp.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_typesupport_introspection_c.so \
$(WORKSPACE_action_msgs)/Release/strip/libaction_msgs__rosidl_typesupport_introspection_cpp.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

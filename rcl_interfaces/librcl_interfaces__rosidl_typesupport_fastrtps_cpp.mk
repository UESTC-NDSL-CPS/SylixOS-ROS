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
# ��   ��   ��: librcl_interfaces__rosidl_typesupport_fastrtps_cpp.mk
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
LOCAL_TARGET_NAME := librcl_interfaces__rosidl_typesupport_fastrtps_cpp.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/floating_point_range__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/integer_range__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/list_parameters_result__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/log__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/parameter_descriptor__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/parameter_event_descriptors__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/parameter_event__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/parameter_type__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/parameter_value__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/parameter__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/msg/detail/dds_fastrtps/set_parameters_result__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/srv/detail/dds_fastrtps/describe_parameters__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/srv/detail/dds_fastrtps/get_parameters__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/srv/detail/dds_fastrtps/get_parameter_types__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/srv/detail/dds_fastrtps/list_parameters__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/srv/detail/dds_fastrtps/set_parameters_atomically__type_support.cpp \
src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp/rcl_interfaces/srv/detail/dds_fastrtps/set_parameters__type_support.cpp

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/rcl_interfaces/rosidl_typesupport_fastrtps_cpp" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_c/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_typesupport_interface/include" \
-I"./src/rcl_interfaces/rosidl_generator_cpp" \
-I"$(WORKSPACE_rosidl)/src/rosidl/rosidl_runtime_cpp/include" \
-I"$(WORKSPACE_libfastrtps-master)/src/Fast-CDR/include" \
-I"$(WORKSPACE_libfastrtps-master)/src/Fast-CDR/build/include" \
-I"$(WORKSPACE_rmw)/src/rmw-6.1.1/rmw/include" \
-I"$(WORKSPACE_rosidl)/src/rosidl_typesupport_fastrtps/rosidl_typesupport_fastrtps_cpp/include" \
-I"$(WORKSPACE_rcutils)/src/rcutils-5.1.5/include" \
-I"$(WORKSPACE_builtin_interfaces)/src/builtin_interfaces/rosidl_generator_cpp"

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
-lfastcdr \
-lrmw \
-lrosidl_typesupport_fastrtps_cpp \
-lrcutils
LOCAL_DEPEND_LIB_PATH :=  \
-L"$(WORKSPACE_rosidl)/Release/strip" \
-L"$(WORKSPACE_libfastrtps-master)/Release/strip" \
-L"$(WORKSPACE_rmw)/Release/strip" \
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
$(WORKSPACE_libfastrtps-master)/Release/strip/libfastcdr.so \
$(WORKSPACE_rmw)/Release/strip/librmw.so \
$(WORKSPACE_rosidl)/Release/strip/librosidl_typesupport_fastrtps_cpp.so \
$(WORKSPACE_rcutils)/Release/strip/librcutils.so

include $(APPLICATION_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************

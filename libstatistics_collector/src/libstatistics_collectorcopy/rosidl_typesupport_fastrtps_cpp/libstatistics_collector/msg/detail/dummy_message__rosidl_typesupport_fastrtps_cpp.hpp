// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__rosidl_typesupport_fastrtps_cpp.hpp.em
// with input from libstatistics_collector:msg/DummyMessage.idl
// generated code does not contain a copyright notice

#ifndef LIBSTATISTICS_COLLECTOR__MSG__DETAIL__DUMMY_MESSAGE__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
#define LIBSTATISTICS_COLLECTOR__MSG__DETAIL__DUMMY_MESSAGE__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_

#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "libstatistics_collector/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"
#include "libstatistics_collector/msg/detail/dummy_message__struct.hpp"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

#include "fastcdr/Cdr.h"

namespace libstatistics_collector
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_libstatistics_collector
cdr_serialize(
  const libstatistics_collector::msg::DummyMessage & ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_libstatistics_collector
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  libstatistics_collector::msg::DummyMessage & ros_message);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_libstatistics_collector
get_serialized_size(
  const libstatistics_collector::msg::DummyMessage & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_libstatistics_collector
max_serialized_size_DummyMessage(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace libstatistics_collector

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_libstatistics_collector
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, libstatistics_collector, msg, DummyMessage)();

#ifdef __cplusplus
}
#endif

#endif  // LIBSTATISTICS_COLLECTOR__MSG__DETAIL__DUMMY_MESSAGE__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_

// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from libstatistics_collector:msg/DummyMessage.idl
// generated code does not contain a copyright notice

#ifndef LIBSTATISTICS_COLLECTOR__MSG__DETAIL__DUMMY_MESSAGE__STRUCT_HPP_
#define LIBSTATISTICS_COLLECTOR__MSG__DETAIL__DUMMY_MESSAGE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__libstatistics_collector__msg__DummyMessage __attribute__((deprecated))
#else
# define DEPRECATED__libstatistics_collector__msg__DummyMessage __declspec(deprecated)
#endif

namespace libstatistics_collector
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct DummyMessage_
{
  using Type = DummyMessage_<ContainerAllocator>;

  explicit DummyMessage_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init)
  {
    (void)_init;
  }

  explicit DummyMessage_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    libstatistics_collector::msg::DummyMessage_<ContainerAllocator> *;
  using ConstRawPtr =
    const libstatistics_collector::msg::DummyMessage_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      libstatistics_collector::msg::DummyMessage_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      libstatistics_collector::msg::DummyMessage_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__libstatistics_collector__msg__DummyMessage
    std::shared_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__libstatistics_collector__msg__DummyMessage
    std::shared_ptr<libstatistics_collector::msg::DummyMessage_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DummyMessage_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    return true;
  }
  bool operator!=(const DummyMessage_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DummyMessage_

// alias to use template instance with default allocator
using DummyMessage =
  libstatistics_collector::msg::DummyMessage_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace libstatistics_collector

#endif  // LIBSTATISTICS_COLLECTOR__MSG__DETAIL__DUMMY_MESSAGE__STRUCT_HPP_

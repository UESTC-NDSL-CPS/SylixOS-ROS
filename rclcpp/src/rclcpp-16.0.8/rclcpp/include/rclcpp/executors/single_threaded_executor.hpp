// Copyright 2014 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCLCPP__EXECUTORS__SINGLE_THREADED_EXECUTOR_HPP_
#define RCLCPP__EXECUTORS__SINGLE_THREADED_EXECUTOR_HPP_

#include <rmw/rmw.h>

#include <cassert>
#include <cstdlib>
#include <memory>
#include <vector>

#include "rclcpp/executor.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/memory_strategies.hpp"
#include "rclcpp/node.hpp"
#include "rclcpp/utilities.hpp"
#include "rclcpp/rate.hpp"
#include "rclcpp/visibility_control.hpp"

namespace rclcpp
{
namespace executors
{

/// Single-threaded executor implementation.
/**
 * This is the default executor created by rclcpp::spin.
 */
class SingleThreadedExecutor : public rclcpp::Executor
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(SingleThreadedExecutor)

  /// Default constructor. See the default constructor for Executor.
  RCLCPP_PUBLIC
  explicit SingleThreadedExecutor(
    const rclcpp::ExecutorOptions & options = rclcpp::ExecutorOptions());

  /// Default destructor.
  RCLCPP_PUBLIC
  virtual ~SingleThreadedExecutor();

  /// Single-threaded implementation of spin.
  /**
   * This function will block until work comes in, execute it, and then repeat
   * the process until canceled.
   * It may be interrupt by a call to rclcpp::Executor::cancel() or by ctrl-c
   * if the associated context is configured to shutdown on SIGINT.
   * \throws std::runtime_error when spin() called while already spinning
   */
  RCLCPP_PUBLIC
  void
  spin() override;

  RCLCPP_PUBLIC
  void
  PiCAS_spin();

  RCLCPP_PUBLIC
  void 
  UsePiCAS()
  {
    this->use_PiCAS = true;
  }

  RCLCPP_PUBLIC
  void
  PiCAS_set_timer_priority(
    rclcpp::TimerBase::SharedPtr timer,
    int priority
  );

  RCLCPP_PUBLIC
  void
  PiCAS_set_subscription_priority(
    rclcpp::SubscriptionBase::SharedPtr subscription,
    int priority
  );

  RCLCPP_PUBLIC
  void
  PiCAS_set_client_priority(
    rclcpp::ClientBase::SharedPtr client,
    int priority
  );

  RCLCPP_PUBLIC
  void
  PiCAS_set_service_priority(
    rclcpp::ServiceBase::SharedPtr service,
    int priority
  );

  class ChainInfo
  {
    public:
      SingleThreadedExecutor& executor;
      ChainInfo(SingleThreadedExecutor& executorClass) : executor(executorClass) {};
      ~ChainInfo();

      void setChainInfo(int priority, rclcpp::TimerBase::SharedPtr timer) {
        this->priority = priority;
        this->timer = timer;
      }

      void addChainCallback(rclcpp::AnyExecutable callback) {
        this->callbacks.push_back(callback);
      }

      int counter = 0;
      int priority = 0;
      rclcpp::TimerBase::SharedPtr timer;
      std::vector<rclcpp::AnyExecutable> callbacks;
  };

private:
  bool use_PiCAS = false;
  RCLCPP_DISABLE_COPY(SingleThreadedExecutor)
};

}  // namespace executors
}  // namespace rclcpp

#endif  // RCLCPP__EXECUTORS__SINGLE_THREADED_EXECUTOR_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "rclcpp/qos.hpp"

using namespace std::chrono_literals;
// 3.定义节点类；
class MinimalPublisher : public rclcpp::Node
{
public:
  MinimalPublisher() : Node("minimal_publisher"), count_(0) {
    // 3-1.创建发布方；
//      auto qos_profile_ = rclcpp::QoS(rclcpp::KeepLast(10));//输出队列长度
//     qos_profile_.best_effort();//listen不用best_effort，收不到消息
      auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(20))
              .transient_local()
              .reliable();
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic", qos_profile);
    // 3-2.创建定时器；
    timer_ = this->create_wall_timer(
        500ms, std::bind(&MinimalPublisher::timer_callback, this));
  }

private:
  void timer_callback() {
    // 3-3.组织消息并发布。
    auto message = std_msgs::msg::String();
    message.data = "Hello, world! " + std::to_string(count_++);
    RCLCPP_INFO(this->get_logger(), "send message: '%s'",message.data.c_str());
    publisher_->publish(message);
  }
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  size_t count_;
};

int main(int argc, char *argv[]) {
  // 2.初始化 ROS2 客户端；
  rclcpp::init(argc, argv);
  // 4.调用 spin 函数，并传入节点对象指针。
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  // 5.释放资源；
  rclcpp::shutdown();
  return 0;
}

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "rclcpp/qos.hpp"

using std::placeholders::_1;
// 3.定义节点类；
class MinimalSubscriber : public rclcpp::Node {
public:
  MinimalSubscriber() : Node("minimal_subscriber") {
    // 3-1.创建订阅方；
//      auto qos_profile_ = rclcpp::QoS(rclcpp::KeepLast(10));//输出队列长度
//           qos_profile_.best_effort();//listen不用best_effort，收不到消息

      auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10))
              .transient_local()
              .reliable();
      subscription_ = this->create_subscription<std_msgs::msg::String>(
              "topic", qos_profile, std::bind(&MinimalSubscriber::topic_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "start listening");
  }

private:
  // 3-2.处理订阅到的消息；
  void topic_callback(const std_msgs::msg::String &msg) const {
    RCLCPP_INFO(this->get_logger(), "subscribe:'%s'", msg.data.c_str());
  }
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};
int main(int argc, char *argv[]) {
  // 2.初始化 ROS2 客户端；
  rclcpp::init(argc, argv);
  // 4.调用 spin 函数，并传入节点对象指针。
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  // 5.释放资源；
  rclcpp::shutdown();
  return 0;
}

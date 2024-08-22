#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "rclcpp/qos.hpp"

using std::placeholders::_1;
// 3.����ڵ��ࣻ
class MinimalSubscriber : public rclcpp::Node {
public:
  MinimalSubscriber() : Node("minimal_subscriber") {
    // 3-1.�������ķ���
//      auto qos_profile_ = rclcpp::QoS(rclcpp::KeepLast(10));//������г���
//           qos_profile_.best_effort();//listen����best_effort���ղ�����Ϣ

      auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10))
              .transient_local()
              .reliable();
      subscription_ = this->create_subscription<std_msgs::msg::String>(
              "topic", qos_profile, std::bind(&MinimalSubscriber::topic_callback, this, _1));

    RCLCPP_INFO(this->get_logger(), "start listening");
  }

private:
  // 3-2.�����ĵ�����Ϣ��
  void topic_callback(const std_msgs::msg::String &msg) const {
    RCLCPP_INFO(this->get_logger(), "subscribe:'%s'", msg.data.c_str());
  }
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};
int main(int argc, char *argv[]) {
  // 2.��ʼ�� ROS2 �ͻ��ˣ�
  rclcpp::init(argc, argv);
  // 4.���� spin ������������ڵ����ָ�롣
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  // 5.�ͷ���Դ��
  rclcpp::shutdown();
  return 0;
}

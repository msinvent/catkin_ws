#include <ros/ros.h>
#include <std_msgs/Int32.h>

int main(int argc, char **argv) {

  ros::init(argc, argv, "topic_publisher"); // telling name of the rosnode
  ros::NodeHandle nh; // Declares it as a node

  ros::Publisher pub = nh.advertise<std_msgs::Int32>("counter", 1000);
  ros::Rate loop_rate(2);

  std_msgs::Int32 count;
  count.data = 0;

  while (ros::ok()) {
    pub.publish(count);
    ros::spinOnce();
    loop_rate.sleep();
    ++count.data;
  }

  return 0;
}

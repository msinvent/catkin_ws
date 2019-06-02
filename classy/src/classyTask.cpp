#include <ros/ros.h>
#include <std_msgs/Int32.h>
#include "classy_lib.hpp"

int main(int argc, char **argv) {
  
  ros::init(argc, argv, "classy_node");
  ros::NodeHandle nh;

  testNameSpace::testClass t1(0);
  
  
  ros::Publisher pub = nh.advertise<std_msgs::Int32>("counter", 1000);
  ros::Rate loop_rate(2);

  std_msgs::Int32 count;
  count.data = 0;

  while (ros::ok()) {
    pub.publish(count);
    ros::spinOnce();
    loop_rate.sleep();
    t1.tempVariable++;
    count.data = t1.tempVariable;
  }

  return 0;
}

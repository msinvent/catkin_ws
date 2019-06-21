#include "topics_quiz.hpp"
#include <cstddef>
#include <cstdint>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <mutex>
#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>

int main(int argc, char **argv) {

  ros::init(argc, argv, "topic_publisher");

  planner::topics_quiz_class quiz_variable(0.5, 9.9);

  ros::Rate loop_rate(2);

  geometry_msgs::Twist command;
  //   command.linear.x = 0.1;
  //   command.angular.z = 0.1;

  while (ros::ok()) {
    // mutexed code
    {
      std::unique_lock<std::mutex> lck(quiz_variable.__mtx);
      command.linear.x = quiz_variable.getVel();
      command.angular.z = quiz_variable.getRot();
    }
    quiz_variable.__pub_cmd_vel.publish(command);
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}

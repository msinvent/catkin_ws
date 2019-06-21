#ifndef topics_quiz
#define topics_quiz

#include <cmath>
#include <geometry_msgs/Twist.h>
#include <mutex>
#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <vector>

namespace planner {
class topics_quiz_class {
  double internal_val;
  std::vector<double> __obstacles;
  double __laserResolution_deg;
  std::vector<double> __cosineCoeff;
  std::vector<double> __sineCoeff;
  static constexpr double __degToPi = 3.1416 / 180.0;
  double __xplus;
  double __yplus;

public:
  ros::NodeHandle __nh;
  ros::Publisher __pub_cmd_vel;
  ros::Subscriber __sub_kobuki_laser_scan;
  std::mutex __mtx; // mutex for critical section, to write __xplus and __yplus

public:
  topics_quiz_class(double val, double laserResolution_deg)
      : internal_val(val), __laserResolution_deg(laserResolution_deg) {
    __pub_cmd_vel = __nh.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

    __sub_kobuki_laser_scan =
        __nh.subscribe("kobuki/laser/scan", 1000,
                       &topics_quiz_class::laserScannerCallback, this);

    __obstacles.reserve(static_cast<uint16_t>(180 / __laserResolution_deg));

    std::size_t coeffSize = static_cast<std::size_t>(180 / laserResolution_deg);
    for (auto i = 0u; i < coeffSize; ++i) {
      __cosineCoeff.push_back(
          std::cos(laserResolution_deg * (i + 1) * __degToPi));
      __sineCoeff.push_back(
          std::sin(laserResolution_deg * (i + 1) * __degToPi));
    }
  }

  ~topics_quiz_class() {}

  void laserScannerCallback(const sensor_msgs::LaserScan::ConstPtr &msg) {
    __obstacles.clear();
    //   static uint32_t counter = 0;
    // ROS_INFO("%d,%f,%f", msg->ranges.size(), msg->angle_min, msg->angle_max);
    // x<<4 -> x*4

    // msg->ranges.size() is 720
    double minVal = 1000.0;
    for (size_t i = 0, j = 0; i < msg->ranges.size(); ++i) {
      minVal = minVal < msg->ranges[i] ? minVal : msg->ranges[i];
      if (i % static_cast<uint16_t>(4 * __laserResolution_deg) == 0 && i != 0) {
        __obstacles.push_back(minVal);
        minVal = 1000.0;
      }
    }

    // for (const auto it : __obstacles) {
    //   std::cout << it << " ";
    // }
    // std::cout << "\n";

    // for (auto it : __cosineCoeff) {
    //   std::cout << it << ",";
    // }
    // std::cout << "\n";

    // for (auto it : __sineCoeff) {
    //   std::cout << it << ",";
    // }
    // std::cout << "\n";

    // std::cout << __cosineCoeff.size() << "\t" << __sineCoeff.size() << "\t"
    //           << __obstacles.size() << "\n";
    effectiveObstacleForceFieldUpdate();
  }

  void effectiveObstacleForceFieldUpdate() {
    double xplus = 0.0;
    double yplus = 0.0;
    for (auto i = 0u; i < __cosineCoeff.size(); ++i) {
      if (__obstacles[i] <= 30.0) { // consider obsacle
        xplus -= (30.0 - __obstacles[i]) * __cosineCoeff[i];
        yplus -= (30.0 - __obstacles[i]) * __sineCoeff[i];
      } else { // consider no obsticle
        xplus += 10.0 * __cosineCoeff[i];
        yplus += 10.0 * __sineCoeff[i];
      }
    }

    double magnitude = std::sqrt(xplus * xplus + yplus * yplus);
    // xplus and yplus are coordinates in body frame

    xplus /= magnitude;
    yplus /= magnitude;

    {
      std::unique_lock<std::mutex> lck(__mtx);
      __xplus = xplus;
      __yplus = yplus;
      std::cout << __xplus << "\t" << __yplus << "\t" << magnitude << "\n";
    }
  }
  double getVel() { return __yplus; }
  double getRot() { return -__xplus; }
}; // namespace planner
}; // namespace planner

#endif

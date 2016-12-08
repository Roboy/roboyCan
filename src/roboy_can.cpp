#include "ros/ros.h"

#include <chrono>
#include <cstdint>
#include <thread>

#include "canopen_error.h"
#include "logger.h"
#include "master.h"

int main(int argc, char **argv) {
  ros::init(argc, argv, "Roboy_CAN");
  ros::NodeHandle n;
  ROS_INFO("Hello_workd");
}

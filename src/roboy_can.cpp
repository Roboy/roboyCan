#include "roboy_can/roboy_can.hpp"

canRoboy::canRoboy() {
  ros::NodeHandle nnh(roboyCanNH_, "Roboy_Can_Interface");
}
void canRoboy::initialise(std::string busname, std::string baudrate) {
  busname_ = busname;
  baudrate_ = baudrate;
  if (!master_.start(busname_, baudrate_)) {
    std::cout << "Could not start CAN master" << std::endl;
  }
  std::cout << "Configuring Nodes" << std::endl;
  configureNodes();
}

/**
 * [configureNodes description]
 * @method configureNodes
 * @param  filename       [description]
 */
void canRoboy::configureNodes(void) {
  std::vector<std::string> joint_names;
  joint_names = getJointNames();
  findNodes(joint_names);
  master_.core.nmt.reset_all_nodes();
  for (std::string joint : joint_names) {
    std::cout << "Motor: " << joint << std::endl;
    setupMotor(joint);
  }
}

/**
 * [moveMotor description]
 * @method moveMotor
 * @param  node_id   [description]
 * @param  setpoint  [description]
 */
void canRoboy::moveMotor(std::string jointName, int setpoint) {
  unsigned int canID = getCanAddress_cached(jointName);
  deviceVector_.at(canID).execute("enable_operation");
  deviceVector_.at(canID).execute("set_target_position", (int32_t)setpoint);
}

/**
 * [stopMotors description]
 * @method stopMotors
 * @param  motorMap   [description]
 */
void canRoboy::stopMotors(std::map<unsigned int, int> motorMap) {
  for (auto const &motors : motorMap) {
    deviceVector_.at(motors.first).execute("disable_operation");
  }
}

/**
 * [moveMultipleMotors description]
 * @method moveMultipleMotors
 * @param  motorMap           [description]
 */
void canRoboy::moveMultipleMotors(std::map<std::string, int> motorMap) {
  for (auto const &motors : motorMap) {
    moveMotor(motors.first, motors.second);
  }
}

std::vector<unsigned int> canRoboy::getMotorIDs(void) { return motor_ids_; }

/**
 * [getJointNames gets all joint names from the hardware interface part of the
 * parameter]
 * @method getJointNames
 * @param void
 * @return std::vector<std::string>  [vector of all joint names]
 */
std::vector<std::string> canRoboy::getJointNames(void) {
  std::vector<std::string> joint_names;
  std::size_t error = 0;
  error += !rosparam_shortcuts::get("hardware_interface", roboyCanNH_, "joints",
                                    joint_names);
  rosparam_shortcuts::shutdownIfError("hardware_interface", error);

  for (std::string joint : joint_names) {
    jointCanMap_[joint] = getCanAddress(joint);
  }
  return joint_names;
}

/**
 * [canRoboy::getCanAddress_cached description]
 * @method canRoboy::getCanAddress_cached
 * @param  jointName                      [description]
 * @return                                [description]
 */
unsigned int canRoboy::getCanAddress_cached(std::string jointName) {
  return jointCanMap_[jointName];
}

/**
 * [getCanAddress description]
 * @method getCanAddress
 * @param  std::string   [description]
 * @return unsigned int  [description]
 */
unsigned int canRoboy::getCanAddress(std::string jointName) {
  int canAddress = 0;
  std::ostringstream stringStream;
  stringStream << "MaxonConfigs/" << jointName << "/CANAddress";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          canAddress);
  return canAddress;
}
/**
 * [findNodes finds all motors on the can bus for you]
 * @param nodesToLookFor [Array of node_ids of the motors you want to control]
 */
void canRoboy::findNodes(std::vector<std::string> jointNames) {
  bool found_devices = false;
  while (!found_devices) {
    // Get all devices on the CAN bus and store them by node ID in the
    // deviceVector_
    for (size_t i = 0; i < master_.num_devices(); ++i) {
      kaco::Device &device = master_.get_device(i);
      deviceVector_.emplace(device.get_node_id(), master_.get_device(i));
    }
    found_devices = true;
    // Check with all nodes we want to control, to find if we are missing one
    for (auto const joint : jointNames) {
      if (deviceVector_.find(getCanAddress(joint)) != deviceVector_.end()) {
      } else {
        found_devices = false;
        std::cout << "canRoboy: Missing node with node_id: " << joint
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  }
}

/** [readPosition reads the position of all motors ]
 *
 */
std::vector<double> canRoboy::readPosition(void) {
  std::vector<std::string> jointNames = getJointNames();
  std::vector<double> temp;
  for (std::string motor : jointNames) {
    temp.push_back((double)deviceVector_.at(getCanAddress(motor))
                       .get_entry("Position Actual Value"));
  }
  return temp;
}

/**
 * [setupMotor Sets up the parameters for the motor]
 * @method setupMotor
 * @param  node_id    [node id of motor to be controlled]
 */
void canRoboy::setupMotor(std::string JointName) {
  unsigned int JointCanAddress = 0;
  JointCanAddress = getCanAddress(JointName);

  deviceVector_.at(JointCanAddress).start();
  std::cout << "RoboyCAN Started node." << std::endl;
  deviceVector_.at(JointCanAddress)
      .load_dictionary_from_eds("/home/rapha/Development/Roboy/can/kacanopen/"
                                "eds_library/MaxonMotor/epos2_347717.eds");
  std::cout << "RoboyCAN - : Loaded Dictionary" << std::endl;

  deviceVector_.at(JointCanAddress).set_entry("CAN Bitrate", (uint16_t)0);
  std::cout << "RoboyCAN - : Set bitrate to 0" << std::endl;

  int temp32 = 0;
  std::ostringstream stringStream;
  stringStream << "MaxonConfigs/" << JointName
               << "/Sensor Configuration/Pulse Number Incremental Encoder 1";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          temp32);
  deviceVector_.at(JointCanAddress)
      .set_entry("Sensor Configuration/Pulse Number Incremental Encoder 1",
                 (uint32_t)temp32);
  std::cout << "RoboyCAN -Set Encoder ticks per rotation" << std::endl;

  // deviceVector_.at(JointCanAddress).set_entry(
  //     "Sensor Configuration/Position Sensor Type",
  //     (uint16_t)motorConfig_["MaxonConfigs"][JointCanAddress]["Sensor
  //     Configuration"]
  //                         ["Position Sensor Type"]);
  // std::cout << "RoboyCAN - : Set Sensor type" << std::endl;

  // deviceVector_.at(JointCanAddress).set_entry("Miscellaneous
  // Configuration",
  //                                     (uint16_t)0x08);
  // std::cout << "RoboyCAN - : Set velocity measurement to high" <<
  // std::endl;

  deviceVector_.at(JointCanAddress)
      .set_entry("modes_of_operation",
                 deviceVector_.at(JointCanAddress)
                     .get_constant("profile_position_mode"));
  std::cout << "RoboyCAN - : Set Profile position mode" << std::endl;

  stringStream.str(std::string());
  stringStream << "MaxonConfigs/" << JointName << "/Max Following Error";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          temp32);
  deviceVector_.at(JointCanAddress)
      .set_entry("Max Following Error", (uint32_t)temp32);

  stringStream.str(std::string());
  stringStream << "MaxonConfigs/" << JointName << "/Profile Velocity";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          temp32);
  deviceVector_.at(JointCanAddress)
      .set_entry("Profile Velocity", (uint32_t)temp32);
  std::cout << "RoboyCAN - : Set max following error" << std::endl;

  stringStream.str(std::string());
  stringStream << "MaxonConfigs/" << JointName << "/Max Profile Velocity";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          temp32);
  deviceVector_.at(JointCanAddress)
      .set_entry("Max Profile Velocity", (uint32_t)temp32);
  std::cout << "RoboyCAN - : Set max Profile Velocity" << std::endl;

  stringStream.str(std::string());
  stringStream << "MaxonConfigs/" << JointName << "/Profile Acceleration";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          temp32);
  deviceVector_.at(JointCanAddress)
      .set_entry("Profile Acceleration", (uint32_t)temp32);
  std::cout << "RoboyCAN - : Set max profile acceleration" << std::endl;

  stringStream.str(std::string());
  stringStream << "MaxonConfigs/" << JointName << "/Profile Deceleration";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          temp32);
  deviceVector_.at(JointCanAddress)
      .set_entry("Profile Deceleration", (uint32_t)temp32);
  std::cout << "RoboyCAN - : Set profile deceleration" << std::endl;

  stringStream.str(std::string());
  stringStream << "MaxonConfigs/" << JointName << "/Motion Profile Type";
  rosparam_shortcuts::get("hardware_interface", roboyCanNH_, stringStream.str(),
                          temp32);
  deviceVector_.at(JointCanAddress)
      .set_entry("Motion Profile Type", (uint16_t)temp32);
  std::cout << "RoboyCAN - : Set motion profile type" << std::endl;

  deviceVector_.at(JointCanAddress).execute("enable_operation");
}

// int main(int argc, char **argv) {
//   canRoboy wums;
//   wums.configureNodes("/home/rapha/Development/Roboy/can/roboy_can/roboy.yaml");
//   std::map<unsigned int, int> data_out;
//   std::map<unsigned int, int> data_in;
//   std::cout << "sleep 3" << std::endl;
//   for (unsigned int id : wums.getMotorIDs()) {
//     data_in[id] = 50000;
//   }
//   wums.moveMultipleMotors(data_in);
//   std::this_thread::sleep_for(std::chrono::seconds(1));
//
//   wums.stopMotors(data_in);
//
//   std::this_thread::sleep_for(std::chrono::seconds(3));
//
//   data_out = wums.getPosition(wums.getMotorIDs());
//   std::cout << data_out[34] << std::endl;
//   wums.stopMotors(data_in);
// }

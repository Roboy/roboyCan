#include "roboy_can/roboy_can.hpp"
#define MAXONMOTORTICK_IN_RAD 4318.828535741

auto canRoboy::connect(masterMap kacanMasters, RoboyConfig &&roboyConfigs)
    -> variant<canRoboy, failedCanRoboy> {

  for (auto &master : roboyConfigs.getNetworks()) {
    try {
      if (!kacanMasters.at(master).get().start(
              "slcan0",
              "1M")) { // todo: take can info from
        // configuration different can
        // networks
        return failedCanRoboy{std::make_pair(
            std::move(roboyConfigs), RoboyCanStatus::CONNECTION_FAILED)};
      }
    } catch (std::out_of_range &e) {
      std::cout << "RoboyCan Error - Missmatch between masterMap and map of "
                   "masters in yaml file.: "
                << e.what() << std::endl;
      return failedCanRoboy{std::make_pair(std::move(roboyConfigs),
                                           RoboyCanStatus::CONNECTION_FAILED)};
    }
  }
  // if (configureNodes(masters, roboyConfigs.configs)) {
  //   return canRoboy(masters, std::move(roboyConfigs.configs));
  // } else {
  //   return failedCanRoboy{std::make_pair(std::move(roboyConfigs),
  //                                        RoboyCanStatus::CONNECTION_FAILED)};
  // }
}

canRoboy::canRoboy(masterMap canMasters, MotorConfigs &&motorConfigs) {
  std::cout << "Configuring Nodes" << std::endl;
  master_ = canMasters;
}
// canRoboy::initialise(std::string busname, std::string baudrate)
//     ->variant<kaco::Master, RoboyCanStatus> {
//   if () {
//     return master_;
//   } else {
//     std::cout << "Could not start CAN master" << std::endl;
//     return RoboyCanStatus::OTHER_ERROR;
//   }
// }

// // todo: remove this
// bool canRoboy::set_controllers(roboy_can::setEPOSControllers::Request &req,
//                                roboy_can::setEPOSControllers::Response &res)
//                                {
//   std::vector<std::string> joint_names;
//   joint_names = getJointNames();
//   for (size_t i = 0; i < req.request.size(); i++) {
//     switch (req.request.at(i)) {
//     case 1:
//       enable_profile_position_mode(joint_names.at(i));
//       break;
//     case -3:
//       enable_current_mode(joint_names.at(i));
//       break;
//
//     default:
//       ROS_INFO("The control mode: %d is unknown for motor %s",
//                req.request.at(i), joint_names.at(i).c_str());
//     }
//   }
// }

/**
 * [configureNodes description]
 * @method configureNodes
 * @param  filename       [description]
 */
// bool canRoboy::configureNodes(masterMap masters, MotorConfigs motorConfigs) {
//
//   for (auto &muscle : motorConfigs) {
//   }
//   // master_->core.nmt.reset_all_nodes();
//   for (std::string joint : joint_names) {
//     std::cout << "Motor: " << joint << std::endl;
// #define PROFILEPOSIITONMODE 1
//     controlMode_[getCanAddress(joint)] = PROFILEPOSIITONMODE;
//     setupMotor(joint);
//     PDO_setup(joint);
//   }
//   return true;
//   // todo:  return non true if something goes wrong...
// }

/**
 * [canRoboy::enable_current_mode description]
 * @method canRoboy::enable_current_mode
 */
// void canRoboy::enable_current_mode(std::string jointName) {
//   unsigned int canID = getCanAddress(jointName);
//
//   std::cout << "Activate current mode " << std::endl;
// #define CURRENTMODE -3
//   controlMode_[canID] = CURRENTMODE;
//   deviceVector_.at(canID).set_entry(
//       "modes_of_operation",
//       deviceVector_.at(canID).get_constant("current_mode"));
// }

/**
 * [canRoboy::enable_profile_position_mode Enables the profile position mode -
 * only used if it was changed beforehand away from it]
 * @method canRoboy::enable_profile_position_mode
 */
// void canRoboy::enable_profile_position_mode(std::string jointName) {
//   unsigned int canID = getCanAddress(jointName);
//
//   std::cout << "Activate profile_position_mode mode " << std::endl;
//
//   deviceVector_.at(canID).set_entry(
//       "modes_of_operation",
//       deviceVector_.at(canID).get_constant("profile_position_mode"));
// }
/**
 * [moveMotor description]
 * @method moveMotor
 * @param  node_id   [description]
 * @param  setpoint  [description]
 */
// void canRoboy::moveMotor(std::string jointName, int setpoint) {
//   unsigned int canID = getCanAddress_cached(jointName);
//
//   switch (controlMode_[canID]) {
//   case -3:
//     deviceVector_.at(canID).set_entry("Current Mode Setting Value",
//                                       (int16_t)70);
//     break;
//   case 1:
//     deviceVector_.at(canID).execute(
//         "set_target_position", (int32_t)(setpoint * MAXONMOTORTICK_IN_RAD));
//     break;
//   }
// }

/**
 * [stopMotors description]
 * @method stopMotors
 * @param  motorMap   [description]
 */
// void canRoboy::stopMotors(std::map<unsigned int, int> motorMap) {
//   for (auto const &motors : motorMap) {
//     deviceVector_.at(motors.first).execute("disable_operation");
//   }
// }

/**
 * [moveMultipleMotors description]
 * @method moveMultipleMotors
 * @param  motorMap           [description]
 */
// void canRoboy::moveMultipleMotors(std::map<std::string, int> motorMap) {
//   for (auto const &motors : motorMap) {
//     moveMotor(motors.first, motors.second);
//   }
// }
//
// std::vector<unsigned int> canRoboy::getMotorIDs(void) { return motor_ids_; }

/**
 * [getJointNames gets all joint names from the hardware interface part of the
 * parameter]
 * @method getJointNames
 * @param void
 * @return std::vector<std::string>  [vector of all joint names]
 */
// std::vector<std::string> canRoboy::getJointNames(void) {
//   std::vector<std::string> joint_names;
//   std::size_t error = 0;
//   error += !rosparam_shortcuts::get("hardware_interface", roboyCanNH_,
//                                     "hardware_interface/joints",
//                                     joint_names);
//   rosparam_shortcuts::shutdownIfError("hardware_interface", error);
//
//   for (std::string joint : joint_names) {
//     jointCanMap_[joint] = getCanAddress(joint);
//   }
//   return joint_names;
// }

/**
 * [canRoboy::getCanAddress_cached description]
 * @method canRoboy::getCanAddress_cached
 * @param  jointName                      [description]
 * @return                                [description]
 */
// unsigned int canRoboy::getCanAddress_cached(std::string jointName) {
//   return jointCanMap_[jointName];
// }

/**
 * [getCanAddress description]
 * @method getCanAddress
 * @param  std::string   [description]
 * @return unsigned int  [description]
 */
// unsigned int canRoboy::getCanAddress(std::string jointName) {
//   int canAddress = 0;
//   std::ostringstream stringStream;
//   stringStream << "hardware_interface/MaxonConfigs/" << jointName
//                << "/CANAddress";
//   rosparam_shortcuts::get("hardware_interface", roboyCanNH_,
//   stringStream.str(),
//                           canAddress);
//   return canAddress;
// }
/**
 * [findNodes finds all motors on the can bus for you]
 * @param nodesToLookFor [Array of node_ids of the motors you want to control]
 */
// void canRoboy::findNodes(std::vector<std::string> jointNames) {
//   bool found_devices = false;
//   while (!found_devices) {
//     // Get all devices on the CAN bus and store them by node ID in the
//     // deviceVector_
//     for (size_t i = 0; i < master_->num_devices(); ++i) {
//       kaco::Device &device = master_->get_device(i);
//       deviceVector_.emplace(device.get_node_id(), master_->get_device(i));
//     }
//     found_devices = true;
//     // Check with all nodes we want to control, to find if we are missing one
//     for (auto const joint : jointNames) {
//       if (deviceVector_.find(getCanAddress(joint)) != deviceVector_.end()) {
//       } else {
//         found_devices = false;
//         std::cout << "canRoboy: Missing node with node_id: " << joint
//                   << std::endl;
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//       }
//     }
//   }
// }

/** [readPosition reads the position of all motors ]
 *
 */
// std::vector<double> canRoboy::readPosition(void) {
//   std::vector<std::string> jointNames = getJointNames();
//   std::vector<double> temp;
//   int32_t data_temp;
//   for (std::string motor : jointNames) {
//     data_temp = deviceVector_.at(getCanAddress(motor))
//                     .get_entry("Position Actual Value");
//     temp.push_back((double)data_temp);
//     std::cout << "Motor current: "
//               << deviceVector_.at(getCanAddress(motor))
//                      .get_entry("Current Demand Value")
//               << std::endl;
//     std::cout << "Modes of operation: "
//               << deviceVector_.at(getCanAddress(motor))
//                      .get_entry("Modes of Operation Display")
//               << std::endl;
//   }
//   return temp;
// }

/**
 * [setupMotor Sets up the parameters for the motor]
 * @method setupMotor
 * @param  node_id    [node id of motor to be controlled]
 */

// void canRoboy::PDO_setup(std::string JointName) {
//   /**
//    * Setup PDO MOSI Mapping:
//    * - This only works when the driver is pre-operational.
//    * - Disable the PDO mapping to allow modifiing the mapped objects
//    * - Map the first object to the Target_Position
//    * - Define the number of objects that are mapped together to this one PDO
//    *    The maximum size of a PDO is 64 bit make sure you stay under it.
//    * - Start the node and enable the device
//    * - Add the Kacan 'transmit' (MOSI) PDO1 mapping which is by standard
//    * configured for the control word...
//    * - Add the Kacan 'transmit' (MOSI) PDO2 mapping for the master
//    *
//    */
//   unsigned int JointCanAddress = 0;
//   JointCanAddress = getCanAddress(JointName);
//
//   master_->core.nmt.send_nmt_message(JointCanAddress,
//                                      kaco::NMT::Command::enter_preoperational);
//
//   std::cout << "COB-ID of PDO2"
//             << deviceVector_.at(JointCanAddress)
//                    .get_entry("Receive PDO 2 Parameter/COB-ID used by RxPDO
//                    2")
//             << std::endl;
//
//   deviceVector_.at(JointCanAddress)
//       .set_entry("Receive PDO 2 Mapping/Number of Mapped Application Objects
//       "
//                  "in RxPDO 2",
//                  (uint8_t)0);
//   std::cout << "PDO mapping number of objects to 0" << std::endl;
//   // map to the Target position object
//   deviceVector_.at(JointCanAddress)
//       .set_entry("Receive PDO 2 Mapping/1st Mapped Object in RxPDO 2",
//                  (uint32_t)0x607A0020);
//   std::cout << "PDO mapping number of objects to 0" << std::endl;
//
//   deviceVector_.at(JointCanAddress)
//       .set_entry("Receive PDO 2 Mapping/Number of Mapped Application Objects
//       "
//                  "in RxPDO 2",
//                  (uint8_t)1);
//   master_->core.nmt.send_nmt_message(JointCanAddress,
//                                      kaco::NMT::Command::start_node);
//   deviceVector_.at(JointCanAddress).execute("enable_operation");
//
//   deviceVector_.at(JointCanAddress)
//       .add_transmit_pdo_mapping(0x00000200 + JointCanAddress,
//                                 {{"controlword", 0}});
//   deviceVector_.at(JointCanAddress)
//       .add_transmit_pdo_mapping(0x00000300 + JointCanAddress,
//                                 {{"Target Position", 0}});
//   deviceVector_.at(JointCanAddress).execute("enable_operation");
// }

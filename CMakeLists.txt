cmake_minimum_required(VERSION 2.8.3)
project(roboy_can)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../roboy_can/cmake)
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third_party")

SET(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  kacanopen
  std_msgs
  std_srvs
  message_generation
)
# Boost
find_package ( Boost REQUIRED COMPONENTS thread )

find_package ( Mapbox REQUIRED )
find_package ( yaml-cpp REQUIRED )



catkin_package(
  INCLUDE_DIRS include
  LIBRARIES libRoboyCanMotor
  CATKIN_DEPENDS kacanopen std_msgs std_srvs
  DEPENDS Mapbox Boost YAML_CPP
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${Mapbox_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIRS}
)

## Declare a C++ library
 # add_library(libRoboyCanMotor
 #   src/Motors.cpp
 # )
 #
 # target_link_libraries(libRoboyCanMotor ${Boost_LIBRARIES})
 #
 # add_dependencies(libRoboyCanMotor ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS} )
 #
 # target_link_libraries(libRoboyCanMotor
 #   ${catkin_LIBRARIES}
 # )


#  set_target_properties(libRoboyParser PROPERTIES LINKER_LANGUAGE CXX)


#############
## Install ##
#############

install(TARGETS
        #libRoboyCanMotor
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        )


## Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.hpp"
  PATTERN ".svn" EXCLUDE
)

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

set(TEST_SOURCE_FILES
    test/Parsers.cpp
)
catkin_add_gtest(unit_tests ${TEST_SOURCE_FILES})
target_link_libraries(unit_tests ${YAML_CPP_LIBRARIES})
configure_file(test/data/roboy.yaml ${CMAKE_CURRENT_BINARY_DIR}/roboy.yaml COPYONLY)

from ros:kinetic

RUN apt-get update && apt-get install vim wget -y
RUN apt-get install -y ros-kinetic-rosparam-shortcuts ros-kinetic-ros-control ros-kinetic-ros-controllers ros-kinetic-control-msgs ros-kinetic-urdf ros-kinetic-control-toolbox libgflags-dev
RUN wget http://www.peak-system.com/fileadmin/media/linux/files/peak-linux-driver-8.2.tar.gz
RUN tar xzf peak-linux-driver-8.2.tar.gz
RUN cd peak-linux-driver-8.2

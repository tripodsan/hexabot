cmake_minimum_required(VERSION 3.13)
project(apod)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        apod main.cpp bus/SPIDevice.cpp PS2X_lib.cpp SSCDriver.cpp ServoOffsets.cpp
        HexaPod.cpp HexLeg.cpp GaitSequencer.cpp InputController.cpp)
target_link_libraries(apod robotcontrol)

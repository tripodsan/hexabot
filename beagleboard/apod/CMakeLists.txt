cmake_minimum_required(VERSION 3.13)
project(apod)

set(CMAKE_CXX_STANDARD 17)

add_executable(apod main.cpp bus/SPIDevice.cpp PS2X_lib.cpp SSCDriver.cpp)
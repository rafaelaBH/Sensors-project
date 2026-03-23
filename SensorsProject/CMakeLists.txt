cmake_minimum_required(VERSION 3.29)
project(SensorsProject)

set(CMAKE_CXX_STANDARD 20)

add_executable(SensorsProject main.cpp
        Sensor.cpp
        Controller.h
        Controller.cpp
        SensorsData.h
        ControllerTest.h
        ControllerTest.cpp)

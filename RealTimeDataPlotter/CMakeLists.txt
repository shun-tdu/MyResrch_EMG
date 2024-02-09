cmake_minimum_required(VERSION 3.27)
project(RealTimeDataPlotter)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        RealTimeDataPlotter
        main.cpp
        crlserial.hpp
        udp_connection.h
)




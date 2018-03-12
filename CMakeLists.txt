cmake_minimum_required(VERSION 3.9)
project(naama)

set(CMAKE_CXX_STANDARD 98)

add_executable(naama
        BalancedTreeK.cpp
        BalancedTreeK.h
        Key.h
        main.cpp
        Node.cpp
        Node.h
        ParameterK.h
        Value.h)

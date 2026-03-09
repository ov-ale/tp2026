cmake_minimum_required(VERSION 3.10)
project(T4_Project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(T4_Target
        main.cpp
        rectangle.cpp
        circle.cpp
        composite-shape.cpp
)
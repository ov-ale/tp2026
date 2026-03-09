cmake_minimum_required(VERSION 3.10)
project(GeometricShapes)

set(CMAKE_CXX_STANDARD 17)

add_executable(GeometricShapes 
    main.cpp
    rectangle.cpp
    ellipse.cpp
    rhombus.cpp
    composite_shape.cpp
)

cmake_minimum_required(VERSION 3.10)

# Project name and version
project(main VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add the executable target and specify the location of main.cpp
add_executable(main src/main.cpp)

# Link OpenCV libraries to the target
target_link_libraries(main ${OpenCV_LIBS})

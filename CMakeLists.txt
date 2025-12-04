cmake_minimum_required(VERSION 3.20)

project(tinychainling VERSION 0.1 LANGUAGES CXX)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable and source files
add_executable(tinychainling
    src/main.cpp
)

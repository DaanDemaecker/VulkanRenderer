# Set cmake version
cmake_minimum_required(VERSION 3.11)

# Project name
project(VulkanRenderer)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0079 NEW)

add_subdirectory(VulkanRenderer2D)
add_subdirectory(VulkanRenderer3D)
add_subdirectory(3rdParty)
add_subdirectory(Resources)
cmake_minimum_required(VERSION 3.20)

project(harbor
  VERSION 0.1.0
  LANGUAGES NONE
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

add_subdirectory(examples/riscv/minimal)

cmake_minimum_required(VERSION 3.12)

project(qlogcollector)

add_subdirectory(src)

option(BUILD_TEST "enable test projects" OFF)

if (BUILD_TEST)
  message("config qlogcollector tests!")
  add_subdirectory(test/consoleapp)
  add_subdirectory(test/widgetapp)
endif ()
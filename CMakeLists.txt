cmake_minimum_required(VERSION 3.12)

project(qlogcollector)

add_subdirectory(src)

option(LOG_COLLECTOR_BUILD_TEST "enable qlogcollector test projects" OFF)

if (LOG_COLLECTOR_BUILD_TEST)
  message("config qlogcollector tests!")
  add_subdirectory(test/consoleapp)
  add_subdirectory(test/widgetapp)
endif ()
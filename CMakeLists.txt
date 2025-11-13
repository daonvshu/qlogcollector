cmake_minimum_required(VERSION 3.12)

project(qlogcollector VERSION 1.0.7)

option(QLOGCOLLECTOR_BUILD_STATIC "Build static libraries" ON)

add_subdirectory(src/qlogcollector/server)

option(LOG_COLLECTOR_BUILD_TEST "enable qlogcollector test projects" OFF)

if (LOG_COLLECTOR_BUILD_TEST)
  message("config qlogcollector tests!")
  add_subdirectory(test/consoleapp)
  add_subdirectory(test/widgetapp)
endif ()

include(CMakePackageConfigHelpers)

add_custom_target(QLogCollector ALL)
add_dependencies(QLogCollector
    Server
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QLogCollectorConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/QLogCollectorConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QLogCollectorConfig.cmake"
    INSTALL_DESTINATION lib/cmake/QLogCollector
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/qlogcollector/comm/
    DESTINATION include/qlogcollector/comm
    FILES_MATCHING PATTERN "*.h"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QLogCollectorConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QLogCollectorConfigVersion.cmake"
    DESTINATION lib/cmake/QLogCollector
)

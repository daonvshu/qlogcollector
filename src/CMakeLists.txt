cmake_minimum_required(VERSION 3.12)

project(qlogcollectorserver VERSION 1.0.5)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

if (WIN32)
    if (MSVC)
        add_definitions(-wd4267)
    endif()
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

set(CMAKE_DEBUG_POSTFIX "d")

add_library(${PROJECT_NAME}
    qlogcollector.cpp
    cache.cpp
    colorformatter.cpp
    console.cpp
    consolestyle.cpp
    styledstring.cpp
    message.cpp
)

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME} PUBLIC QT_MESSAGELOGCONTEXT)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT QLogCollectorServerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/QLogCollectorServerConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/QLogCollectorServerConfig.cmake"
    INSTALL_DESTINATION lib/cmake/QLogCollectorServer
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QLogCollectorServerConfig.cmake"
    DESTINATION lib/cmake/QLogCollectorServer
)

install(EXPORT QLogCollectorServerTargets
    FILE QLogCollectorServerTargets.cmake
    DESTINATION lib/cmake/QLogCollectorServer
)
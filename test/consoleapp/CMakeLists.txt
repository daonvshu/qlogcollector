cmake_minimum_required(VERSION 3.12)

project(server-console-test VERSION 1.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(${PROJECT_NAME}
               main.cpp
               ../printtest.h
               ../printtest.cpp
               )

target_link_libraries(${PROJECT_NAME}
                      Qt${QT_VERSION_MAJOR}::Core
                      qlogcollector::server)
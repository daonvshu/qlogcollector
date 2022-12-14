cmake_minimum_required(VERSION 3.12)

project(server-widget-test VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
             Core Gui Widgets)

add_executable(${PROJECT_NAME} WIN32
               main.cpp
               logtest.h
               logtest.cpp
               logtest.ui
               ../printtest.h
               ../printtest.cpp
               )

target_link_libraries(${PROJECT_NAME}
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets
                      qlogcollector::server)
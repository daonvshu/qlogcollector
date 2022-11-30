cmake_minimum_required(VERSION 3.12)

project(qlogcollectorclient VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
             Core Gui Widgets)

set(framelessWindow ../3rdparty/Qt-Nice-Frameless-Window)
include_directories(${framelessWindow})

list(APPEND WIDGETS_SRC
     widgets/titlebar.h
     widgets/titlebar.cpp
     widgets/titlebar.ui
     )

add_executable(${PROJECT_NAME}
               main.cpp
               qlogcollector.h
               qlogcollector.cpp
               qlogcollector.qrc

               ${WIDGETS_SRC}

               ${framelessWindow}/framelesswindow.h
               ${framelessWindow}/framelesswindow.cpp
               )

target_link_libraries(${PROJECT_NAME}
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets
                      qtwidgetex::lib)
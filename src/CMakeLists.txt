cmake_minimum_required(VERSION 3.12)

project(qlogcollectorserver VERSION 1.0.0 LANGUAGES CXX C)

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

add_compile_definitions(ROOT_PROJECT_PATH="${CMAKE_SOURCE_DIR}")

add_library(${PROJECT_NAME}
    qlogcollector.cpp
    cache.cpp
    colorformatter.cpp
    console.cpp
    consolestyle.cpp
    styledstring.cpp
    message.cpp
)

add_library(qlogcollector::server ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
                      Qt${QT_VERSION_MAJOR}::Core
                      )

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_compile_definitions(${PROJECT_NAME} PUBLIC QT_MESSAGELOGCONTEXT)
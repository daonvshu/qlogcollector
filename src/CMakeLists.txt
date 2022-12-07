cmake_minimum_required(VERSION 3.12)

project(qlogcollectorserver VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

set(JSON_UTIL_PATH ../3rdparty/qjsonutil/include)
include_directories(${JSON_UTIL_PATH})

set(UNISHOX2_PATH ../3rdparty/unishox2)
include_directories(${UNISHOX2_PATH})

include(${UNISHOX2_PATH}/cmake/compilerFlags.cmake REQUIRED)

if (MSVC)
    add_definitions(-wd4267)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

add_library(${PROJECT_NAME}
            qlogcollector.h
            qlogcollector.cpp
            cache.h
            cache.cpp
            colorformatter.h
            colorformatter.cpp
            console.h
            console.cpp
            sender.h
            sender.cpp
            styledstring.h
            styledstring.cpp

            ${UNISHOX2_PATH}/unishox2.h
            ${UNISHOX2_PATH}/unishox2.c
               )

add_library(qlogcollector::server ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Network
                      )

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PUBLIC QT_MESSAGELOGCONTEXT)
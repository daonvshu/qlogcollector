cmake_minimum_required(VERSION 3.12)

project(Server)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include_directories(${CMAKE_SOURCE_DIR}/src)

if (WIN32)
    if (MSVC)
        add_definitions(-wd4267)
    endif()
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

set(CMAKE_DEBUG_POSTFIX "d")

if (QLOGCOLLECTOR_BUILD_STATIC)
    set(_build_type STATIC)
else ()
    set(_build_type SHARED)
endif ()

add_library(${PROJECT_NAME} ${_build_type}
    logcollector.h
    logcollector.cpp
    outputtarget.h
    outputtarget.cpp
    outputstyleconfig.h
    outputstyleconfig.cpp
    messagehandler.h
    messagehandler.cpp
    patternformatter.h
    patternformatter.cpp

    colors/colorattr.h
    colors/colorformatter.h
    colors/colorformatter.cpp
    colors/styledstring.h
    colors/styledstring.cpp

    outputs/stdoutputtarget.h
    outputs/stdoutputtarget.cpp
    outputs/win32consoleapptarget.h
    outputs/win32consoleapptarget.cpp
    outputs/win32debugconsoletarget.h
    outputs/win32debugconsoletarget.cpp
    outputs/fileoutputtarget.h
    outputs/fileoutputtarget.cpp
    outputs/signaloutputtarget.h
    outputs/signaloutputtarget.cpp
    outputs/networkoutputtarget.h
    outputs/networkoutputtarget.cpp

    utils/textutils.h
    utils/textutils.cpp
)

if (QLOGCOLLECTOR_BUILD_STATIC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC QLOGCOLLECTOR_STATIC)
else ()
    target_compile_definitions(${PROJECT_NAME} PRIVATE QLOGCOLLECTOR_LIBRARY)
endif ()

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME} PUBLIC QT_MESSAGELOGCONTEXT)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT ServerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/qlogcollector/server
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/ServerConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ServerConfig.cmake"
    INSTALL_DESTINATION lib/cmake/Server
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ServerConfig.cmake"
    DESTINATION lib/cmake/Server
)

install(EXPORT ServerTargets
    FILE ServerTargets.cmake
    NAMESPACE QLogCollector::
    DESTINATION lib/cmake/Server
)
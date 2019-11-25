cmake_minimum_required(VERSION 3.0.0)

# Project
set(PROJECTNAME "SpeedTest")
project(${PROJECTNAME} C CXX)

# Используем C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# qt
find_package(Qt4 REQUIRED)
set (QT_USE_QTMAIN TRUE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include (${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

#qwt
include (FindPkgConfig)
pkg_check_modules (QWT qwt)

add_executable(${PROJECTNAME}
        main.cpp
        widget.cpp
        qaxiszoomsvc.cpp
        qwheelzoomsvc.cpp
        qwtchartzoom.cpp
        kalmanfiltersimple1d.cpp
        widget.h
        qaxiszoomsvc.h
        qwheelzoomsvc.h
        qwtchartzoom.h
        kalmanfiltersimple1d.hpp
        widget.ui
)

target_include_directories (${PROJECTNAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QWT_INCLUDE_DIRS}
)

target_link_libraries(${PROJECTNAME}
    ${QT_LIBRARIES}
    ${QWT_LIBRARIES}
)


target_link_libraries(${PROJECT_NAME}
    ${QT_LIBRARIES}
    zmq
)

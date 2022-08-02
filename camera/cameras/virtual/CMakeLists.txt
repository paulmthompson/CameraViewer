cmake_minimum_required(VERSION 3.21)
project(virtual
        VERSION 0.0.1
        DESCRIPTION "Virtual Camera for CameraViewer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(virtual SHARED virtual_camera.h
                        virtual_camera.cpp
                    ../../API/camera.hpp)

target_link_libraries(virtual PRIVATE ffmpeg_wrapper::ffmpeg_wrapper)

target_include_directories(virtual
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    PRIVATE
    ../../API/)

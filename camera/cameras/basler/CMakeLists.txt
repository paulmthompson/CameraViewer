cmake_minimum_required(VERSION 3.21)
project(basler
        VERSION 0.0.1
        DESCRIPTION "Basler Camera for CameraViewer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT PYLON_VERSION)
    set(PYLON_VERSION 5)
endif()

add_library(basler SHARED basler_camera.h
                        basler_camera.cpp
                    ../../API/camera.hpp)
IF (UNIX)
    if (PYLON_VERSION EQUAL 5)
        find_library(PYLON_BASE_LIBRARY pylonbase HINTS ${BASLER_PATH})
        find_library(PYLON_GENAPI_LIBRARY GenApi_gcc_v3_1_Basler_pylon HINTS ${BASLER_PATH})
        find_library(PYLON_GCBASE_LIBRARY GCBase_gcc_v3_1_Basler_pylon HINTS ${BASLER_PATH})
        find_library(PYLON_UTILITY_LIBRARY pylonutility HINTS ${BASLER_PATH})
    endif()
ELSEIF (WIN32)
    if (PYLON_VERSION EQUAL 6)
        find_library(PYLON_BASE_LIBRARY PylonBase_v6_1 HINTS ${BASLER_PATH} REQUIRED)
        find_library(PYLON_GENAPI_LIBRARY GenApi_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH} REQUIRED)
        find_library(PYLON_GCBASE_LIBRARY GCBase_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH} REQUIRED)
        find_library(PYLON_UTILITY_LIBRARY PylonUtility_v6_1 HINTS ${BASLER_PATH} REQUIRED)
    endif()
endif()

target_link_libraries(basler PRIVATE ffmpeg_wrapper::ffmpeg_wrapper)

target_link_libraries(basler PUBLIC
     ${PYLON_BASE_LIBRARY}
     ${PYLON_GENAPI_LIBRARY}
     ${PYLON_GCBASE_LIBRARY}
     ${PYLON_UTILITY_LIBRARY}
)

target_include_directories(basler
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    ${BASLER_INCLUDE_PATH}
    PRIVATE
    ../../API/)

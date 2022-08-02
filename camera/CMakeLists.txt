cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(BASLER_PATH "C:\\Program\ Files\\Basler\\pylon\ 6\\Development\\lib\\x64")
set(BASLER_PATH "/opt/pylon5/lib64")

set(BASLER_INCLUDE_PATH "/opt/pylon5/include")
#set(BASLER_INCLUDE_PATH "C:\\Program\ Files\\Basler\\pylon\ 6\\Development\\include")

set(FFMPEG_WRAPPER_PATH "/home/wanglab/Documents/ffmpeg_wrapper/build/test")
#set(FFMPEG_WRAPPER_PATH "C:\\Users\\wanglab\\Documents\\ffmpeg_wrapper\\build\\test")


add_library(camera SHARED API/camera.hpp
                        basler_camera.cpp
                        include/basler_camera.h
                        include/virtual_camera.h
                        virtual_camera.cpp)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(ffmpeg_wrapper CONFIG REQUIRED PATHS ${FFMPEG_WRAPPER_PATH})

find_library(PYLON_BASE_LIBRARY pylonbase HINTS ${BASLER_PATH})
find_library(PYLON_GENAPI_LIBRARY GenApi_gcc_v3_1_Basler_pylon HINTS ${BASLER_PATH})
find_library(PYLON_GCBASE_LIBRARY GCBase_gcc_v3_1_Basler_pylon HINTS ${BASLER_PATH})
find_library(PYLON_UTILITY_LIBRARY pylonutility HINTS ${BASLER_PATH})

#find_library(PYLON_BASE_LIBRARY PylonBase_v6_1 HINTS ${BASLER_PATH})
#find_library(PYLON_GENAPI_LIBRARY GenApi_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH})
#find_library(PYLON_GCBASE_LIBRARY GCBase_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH})
#find_library(PYLON_UTILITY_LIBRARY PylonUtility_v6_1 HINTS ${BASLER_PATH})

target_include_directories(camera PUBLIC
                                    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
                                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                                     ${BASLER_INCLUDE_PATH}
)

target_link_libraries(camera PRIVATE
     ${PYLON_BASE_LIBRARY}
     ${PYLON_GENAPI_LIBRARY}
     ${PYLON_GCBASE_LIBRARY}
     ${PYLON_UTILITY_LIBRARY}
)

target_link_libraries(camera PUBLIC ffmpeg_wrapper::ffmpeg_wrapper)
target_link_libraries(camera PRIVATE nlohmann_json::nlohmann_json)

include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

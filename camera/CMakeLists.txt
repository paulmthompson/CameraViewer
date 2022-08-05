cmake_minimum_required(VERSION 3.21)



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#set_source_files_properties(C_Wrapper/CameraCWrapper.c PROPERTIES LANGUAGE C)

add_library(camera SHARED 
    API/camera.hpp 
    CameraManager/cameramanager.hpp
    C_Wrapper/CameraCWrapper.cpp
    C_Wrapper/CameraCWrapper.h)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(ffmpeg_wrapper CONFIG REQUIRED)

target_include_directories(camera PUBLIC
                                    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/API>"
                                    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/CameraManager>"
                                    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/C_Wrapper>"
                                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                                     )

target_link_libraries(camera PUBLIC ffmpeg_wrapper::ffmpeg_wrapper)
target_link_libraries(camera PRIVATE nlohmann_json::nlohmann_json)

add_subdirectory(cameras/virtual)
add_subdirectory(cameras/basler)

target_link_libraries(camera PUBLIC virtual)
target_link_libraries(camera PUBLIC basler)

include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

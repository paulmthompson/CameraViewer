cmake_minimum_required(VERSION 3.21)

include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG 892f50819a1a4f6852d62a77271f140783c9b87d
)

add_subdirectory(CameraManager)

#find_package(ffmpeg_wrapper CONFIG REQUIRED) #This is needed for camera interface library
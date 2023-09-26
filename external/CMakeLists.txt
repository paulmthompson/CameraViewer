cmake_minimum_required(VERSION 3.21)

include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG 18e33e1dcabef63abf61961ac9666cdd68bb6862
)

add_subdirectory(CameraManager)

#find_package(ffmpeg_wrapper CONFIG REQUIRED) #This is needed for camera interface library
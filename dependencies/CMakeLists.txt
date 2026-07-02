

include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG e1d21c33dc53f5b0b298135611cd3710c5402df7
)

add_subdirectory(CameraManager)




include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG a728f331b98db9667586b36eb59c0fd2fb80754b
)

add_subdirectory(CameraManager)




include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG fd00445a0fd2ddbbe6fbade52584893de66d1353
)

add_subdirectory(CameraManager)


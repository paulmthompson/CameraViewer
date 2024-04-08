

include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG a2a29cd03399a20c16c27cb194846d90087a10fd
)

add_subdirectory(CameraManager)


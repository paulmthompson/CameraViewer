

include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG ff48ad176004334a3dc719c7e3f296d4581baa4e
)

add_subdirectory(CameraManager)




include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG 86f8faab3021c0fd464c0c83ff60c5976eff762a
)

add_subdirectory(CameraManager)




include(FetchContent)

FetchContent_Declare(
  CameraManager
  GIT_REPOSITORY https://github.com/paulmthompson/CameraManager.git
  GIT_TAG 99bde54afeb600701a9180a723f1138f56c91632
)

add_subdirectory(CameraManager)


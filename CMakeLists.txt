cmake_minimum_required(VERSION 3.5)

project(CameraViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("include")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS MultimediaWidgets)

set(FFMPEG_PATH "C:\\Users\\wanglab\\Downloads\\ffmpeg-5.0-full_build-shared\\lib")
set(BASLER_PATH "C:\\Program\ Files\\Basler\\pylon\ 6\\Development\\lib\\x64")

find_library(AVCODEC_LIBRARY avcodec HINTS ${FFMPEG_PATH})
find_library(AVFORMAT_LIBRARY avformat HINTS ${FFMPEG_PATH})
find_library(AVUTIL_LIBRARY avutil HINTS ${FFMPEG_PATH})
find_library(AVFILTER_LIBRARY avfilter HINTS ${FFMPEG_PATH})
find_library(SWSCALE_LIBRARY swscale HINTS ${FFMPEG_PATH})
find_library(SWRESAMPLE_LIBRARY swresample HINTS ${FFMPEG_PATH})

find_library(PYLON_BASE_LIBRARY PylonBase_v6_1 HINTS ${BASLER_PATH})
find_library(PYLON_GENAPI_LIBRARY GenApi_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH})
find_library(PYLON_GCBASE_LIBRARY GCBase_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH})
find_library(PYLON_UTILITY_LIBRARY PylonUtility_v6_1 HINTS ${BASLER_PATH})

include_directories("C:\\Users\\wanglab\\Documents\\ffmpeg_wrapper\\headers")
include_directories("C:\\Program\ Files\\Basler\\pylon\ 6\\Development\\include")
include_directories("C:\\Users\\wanglab\\Downloads\\ffmpeg-5.0-full_build-shared\\include")

find_library(FFMPEG_WRAPPER_LIBRARY ffmpeg_wrapper HINTS "C:\\Users\\wanglab\\Documents\\ffmpeg_wrapper\\build")

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        include/mainwindow.h
        src/mainwindow.ui
        cameraviewer.qss
        cameraviewer.qrc
        include/camera.h
        src/camera.cpp
        src/virtual_camera.cpp
        include/virtual_camera.h
        include/basler_camera.h
        src/basler_camera.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CameraViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CameraViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CameraViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CameraViewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CameraViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(CameraViewer PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(CameraViewer PRIVATE Qt${QT_VERSION_MAJOR}::MultimediaWidgets)

target_link_libraries(CameraViewer PRIVATE
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${AVFILTER_LIBRARY}
    ${SWSCALE_LIBRARY}
    ${SWRESAMPLE_LIBRARY}
    ${FFMPEG_WRAPPER_LIBRARY}
    ${PYLON_BASE_LIBRARY}
    ${PYLON_GENAPI_LIBRARY}
    ${PYLON_GCBASE_LIBRARY}
    ${PYLON_UTILITY_LIBRARY}
)

set_target_properties(CameraViewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CameraViewer)
endif()

cmake_minimum_required(VERSION 3.21)

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

#set(BASLER_PATH "C:\\Program\ Files\\Basler\\pylon\ 6\\Development\\lib\\x64")
set(BASLER_PATH "/opt/pylon5/lib64")

#find_library(PYLON_BASE_LIBRARY PylonBase_v6_1 HINTS ${BASLER_PATH})
#find_library(PYLON_GENAPI_LIBRARY GenApi_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH})
#find_library(PYLON_GCBASE_LIBRARY GCBase_MD_VC141_v3_1_Basler_pylon HINTS ${BASLER_PATH})
#find_library(PYLON_UTILITY_LIBRARY PylonUtility_v6_1 HINTS ${BASLER_PATH})

find_library(PYLON_BASE_LIBRARY pylonbase HINTS ${BASLER_PATH})
find_library(PYLON_GENAPI_LIBRARY GenApi_gcc_v3_1_Basler_pylon HINTS ${BASLER_PATH})
find_library(PYLON_GCBASE_LIBRARY GCBase_gcc_v3_1_Basler_pylon HINTS ${BASLER_PATH})
find_library(PYLON_UTILITY_LIBRARY pylonutility HINTS ${BASLER_PATH})


include_directories("/opt/pylon5/include")
find_package(ffmpeg_wrapper CONFIG REQUIRED PATHS "/home/wanglab/Documents/ffmpeg_wrapper/build/test")

find_package(nlohmann_json CONFIG REQUIRED)


#include_directories("C:\\Program\ Files\\Basler\\pylon\ 6\\Development\\include")
#find_package(ffmpeg_wrapper CONFIG REQUIRED PATHS "C:\\Users\\wanglab\\Documents\\ffmpeg_wrapper\\build\\test")

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


if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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
    ${PYLON_BASE_LIBRARY}
    ${PYLON_GENAPI_LIBRARY}
    ${PYLON_GCBASE_LIBRARY}
    ${PYLON_UTILITY_LIBRARY}
)

target_link_libraries(CameraViewer PRIVATE ffmpeg_wrapper::ffmpeg_wrapper)

target_link_libraries(CameraViewer PRIVATE nlohmann_json::nlohmann_json)

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

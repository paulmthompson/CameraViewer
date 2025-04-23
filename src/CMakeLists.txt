
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow/mainwindow.cpp
        mainwindow/mainwindow.h
        mainwindow/mainwindow.ui
        cameraviewer.qss
        cameraviewer.qrc
        color_scheme.hpp
)

qt_add_executable(CameraViewer
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
)

target_include_directories(CameraViewer PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mainwindow>"
        "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(CameraViewer PRIVATE Qt6::Widgets)

target_link_libraries(CameraViewer PRIVATE CameraManager)

set_target_properties(CameraViewer PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(CameraViewer)


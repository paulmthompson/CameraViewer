
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS MultimediaWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow/mainwindow.cpp
        mainwindow/mainwindow.h
        mainwindow/mainwindow.ui
        cameraviewer.qss
        cameraviewer.qrc
        color_scheme.hpp
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

target_include_directories(CameraViewer PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mainwindow>"
        "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(CameraViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(CameraViewer PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(CameraViewer PRIVATE Qt${QT_VERSION_MAJOR}::MultimediaWidgets)

target_link_libraries(CameraViewer PRIVATE CameraManager)

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

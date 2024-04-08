#=============================
# Preamble
#=============================
cmake_minimum_required(VERSION 3.25)
project(CameraViewer 
        VERSION 0.1 
        LANGUAGES CXX)
include(CTest)

#=============================
# Project Wide Setup
#=============================

if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_MANIFEST_MODE ON)

#=============================
# Externally Provided Content
#=============================

add_subdirectory(dependencies)

#=============================
# Main Build Targets
#=============================

add_subdirectory(src)
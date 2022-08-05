# CameraViewer
QT viewer for viewing and saving from an arbitrary number of Basler Cameras
  
  
### Compiling

I compile this using QT-creator with Qt6, and I have working versions on both windows and linux (Ubuntu). All libraries can be installed with vcpkg (https://vcpkg.io/en/index.html). The requred vcpkg packages are the following:

* ffmpeg[nvcodec]
* nlohmann_json
  
It also uses my library to wrap ffmpeg (https://github.com/paulmthompson/ffmpeg_wrapper).  Currently, this should be build and installed, and in cmake, the variable "ffmpeg_wrapper_DIR" must be set to the ffmpeg_wrapper install cmake directory

#### Windows

#### Linux


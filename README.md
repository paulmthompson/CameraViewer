# CameraViewer
QT viewer for viewing and saving from an arbitrary number of Basler Cameras
  
  
### Compiling

I compile this using QT-creator with Qt6, and I have working versions on both windows and linux (Ubuntu). All libraries can be installed with vcpkg (https://vcpkg.io/en/index.html). The requred vcpkg packages are the following:

* ffmpeg[nvcodec]
* nlohmann_json
  
It also uses my library to wrap ffmpeg (https://github.com/paulmthompson/ffmpeg_wrapper).  Built and install ffmpeg_wrapper whereever you like, and then specify the cmake variable "ffmpeg_wrapper_DIR" for the CameraViewer project to the ffmpeg_wrapper install directory.

Pylon libraries can be downloaded from the basler website here:  
https://www.baslerweb.com/en/downloads/software-downloads/  
You must specify the cmake variable BASLER_PATH which points to the required libraries, and BASLER_INCLUDE_PATH.

#### Windows

#### Linux

The ffmpeg installation needs to be dynamic libraries instead of static, but if you are installing with vcpkg, it will install static by default. You must make a custom triplet to specify the dynamic installtion. Read about this here: 
  
https://vcpkg.readthedocs.io/en/latest/examples/overlay-triplets-linux-dynamic/


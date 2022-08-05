### Camera Shared Library

The camera shared library uses the CameraManager class to control an arbitrary number of cameras to acquire data. Each camera must use the camera interface given in API/camera.hpp. Currently Basler cameras are supposed, and a virtual camera class can be used for testing which displays random noise data.
  
Data is saved using a ffmpeg wrapper. https://github.com/paulmthompson/ffmpeg_wrapper Currently the only codec supported is NVENC with nvidia graphics cards. 


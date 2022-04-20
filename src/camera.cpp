
#include "camera.h"

/*
Change camera resolution, bits, fps
*/

Camera::Camera() {
   ve = std::make_unique<VideoEncoder>();
   this->attached = false;
   this->save_file_path = "./";
   this->save_file_name = "test.mp4";
   totalFramesAcquired = 0;
   totalFramesSaved = 0;
   this->saveData = false;
   this->acquisitionActive = false;
   this->triggered = false;
}


void Camera::changeSize(int width, int height) {
    this->w = width;
    this->h = height;
}

void Camera::setRecord(bool record_state) {
    this->saveData = record_state;

    if (record_state) {
        this->ve->openFile();
    } else {
        this->ve->closeFile();
    }
}

/*
Change camera output path and configuration
*/

void Camera::setConfig(std::string path, std::string name) {
    this->config_file_path = path;
    this->config_file_name = name;
}

void Camera::setSave(std::string path, std::string name) {
    this->save_file_path = path;
    this->save_file_name = name;
}

/*


*/


//Create a video encoder object for the specified save path
//Don't start writing the video yet
void Camera::initializeVideoEncoder() {

    std::string full_path = this->save_file_path + this->save_file_name;
    ve->setSavePath(full_path);

    this->ve->createContext(this->w,this->h,25);
    this->ve->set_pixel_format(GRAY8);
}
/*
If we receive a stop signal from the GUI


*/
void Camera::stopVideoEncoder() {
    this->saveData = false;
    this->ve->closeFile();
}

/*
Camera properties
*/

void Camera::changeExposureTime(float exposure) {
    this->exposure_time = exposure;
}

void Camera::changeGain(float new_gain) {
    this->gain = new_gain;
}


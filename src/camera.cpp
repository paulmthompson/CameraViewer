
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
   this->saveData = false;
}

void Camera::changeSize(int width, int height) {
    this->w = width;
    this->h = height;
}

void Camera::assignID(int id) {
    this->id = id;
}

void Camera::setRecord(bool record_state) {
    this->saveData = record_state;
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

void Camera::startAcquisition() {
    this->acquisitionActive = true;
    this->ve->openFile();
}

void Camera::stopAcquisition() {
    this->acquisitionActive = false;
    this->ve->closeFile();
}


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


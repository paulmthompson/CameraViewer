
#include "camera.h"

/*
Change camera resolution, bits, fps
*/

Camera::Camera() {
   ve = std::make_unique<VideoEncoder>();
}

void Camera::changeSize(int width, int height) {
    this->w = width;
    this->h = height;
}

int Camera::getHeight() const {
    return this->h;
}

int Camera::getWidth() const {
    return this->w;
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
    this->ve->openFile("test.mp4");
    //Camera.StartGrabbing();
}

void Camera::stopAcquisition() {
    this->acquisitionActive = false;
    this->ve->closeFile();
    //Camera.StopGrabbing();
}

void Camera::initializeVideoEncoder() {
    this->ve->createContext(this->w,this->h,25);
    this->ve->set_pixel_format(GRAY8);
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

std::string Camera::getSerial() const {
    return this->serial_num;
}

std::string Camera::getModel() const {
    return this->model;
}

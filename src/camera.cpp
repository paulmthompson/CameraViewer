
#include "camera.h"

Camera::Camera() {

}

/*
Change camera resolution, bits, fps
*/

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
    //Camera.StartGrabbing();
}

void Camera::stopAcquisition() {
    this->acquisitionActive = false;
    //Camera.StopGrabbing();
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

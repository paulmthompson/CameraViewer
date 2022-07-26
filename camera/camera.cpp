
#include "include/camera.h"

/*
Change camera resolution, bits, fps
*/

Camera::Camera() {
   id = 0;
   ve = std::make_unique<ffmpeg_wrapper::VideoEncoder>();
   this->attached = false;
   this->save_file_path = "./";
   this->save_file_name = "test.mp4";
   totalFramesAcquired = 0;
   totalFramesSaved = 0;
   this->saveData = false;
   this->acquisitionActive = false;
   this->triggered = false;

   exposure_time = 0.005f;
   gain = 100.0f;
   w = 640;
   h = 480;
   bit_depth = 8;

   img = std::vector<uint8_t>(w * h);
}


void Camera::changeSize(int width, int height) {
    this->w = width;
    this->h = height;
    this->img.resize(this->w * this->h);
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

int Camera::get_data() {
    return this->doGetData();
}

int Camera::get_data(std::vector<uint8_t>& input_data) {
    int framesCollected = doGetData();

    input_data = this->img;

    return framesCollected;
}

/*
Returns true if camera is successfully connected
Returns false if camera is not able to be connected, or was already connected
*/
bool Camera::connectCamera() {
    if (!this->attached) {
        if (this->doConnectCamera()) {
            return true;
        } else {
            std::cout << "Camera could not be connected" << std::endl;
            return false;
        }
    } else {
        std::cout << "Camera is already connected" << std::endl;
        return false;
    }
}

/*


*/


//Create a video encoder object for the specified save path
//Don't start writing the video yet
void Camera::initializeVideoEncoder() {

    std::string full_path = this->save_file_path + this->save_file_name;
    ve->setSavePath(full_path);

    this->ve->createContext(this->w,this->h,25);
    this->ve->set_pixel_format(ffmpeg_wrapper::GRAY8);
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
    doChangeExposure(exposure );
}

void Camera::changeGain(float new_gain) {
    this->gain = new_gain;
    doChangeGain(new_gain);
}


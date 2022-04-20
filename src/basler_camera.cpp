
#include "basler_camera.h"

#include <memory>

BaslerCamera::BaslerCamera() {

}

void BaslerCamera::startAcquisition() {
    this->acquisitionActive = true;
}

void BaslerCamera::stopAcquisition() {
    this->acquisitionActive = false;
}

void BaslerCamera::startTrigger() {
    this->triggered = true;
}
void BaslerCamera::stopTrigger() {
    this->triggered = false;
}

void BaslerCamera::connectCamera() {

}

int BaslerCamera::get_data(std::vector<uint8_t>& data_out) {

    int frames_acquired = 0;


    return frames_acquired;
}

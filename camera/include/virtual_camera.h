#ifndef VIRTUAL_CAMERA_H
#define VIRTUAL_CAMERA_H

#include "API/camera.hpp"

#include <memory>
#include <random>


class VirtualCamera : public Camera {
public:
    VirtualCamera();

    std::unique_ptr<Camera> copy_class() override {
        return std::unique_ptr<Camera>(std::make_unique<VirtualCamera>());
    }

    void startAcquisition() override {this->acquisitionActive = true;}
    void stopAcquisition() override {this->acquisitionActive = false;}
    void startTrigger() override {this->triggered = true;}
    void stopTrigger() override {this->triggered = false;}

private:
    int fps; // Use this for stress testing.

    std::vector<std::vector<uint8_t>> random_nums;
    int random_index;

    int doGetData() override;
    bool doConnectCamera() override;
};

#endif // VIRTUAL_CAMERA_H

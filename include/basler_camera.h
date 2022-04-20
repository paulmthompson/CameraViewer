#ifndef BASLER_CAMERA_H
#define BASLER_CAMERA_H


#include "camera.h"

#include <memory>
#include <random>


class BaslerCamera : public Camera {
public:
    BaslerCamera();

    std::unique_ptr<Camera> copy_class() override {
        return std::unique_ptr<Camera>(std::make_unique<BaslerCamera>());
    }

    void startAcquisition() override;
    void stopAcquisition() override;
    void startTrigger() override;
    void stopTrigger() override;

    void connectCamera() override;
    int get_data(std::vector<uint8_t>& data_out) override;

private:

};

#endif // BASLER_CAMERA_H

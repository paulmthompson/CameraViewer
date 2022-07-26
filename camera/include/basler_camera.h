#ifndef BASLER_CAMERA_H
#define BASLER_CAMERA_H


#include "camera.h"

#include <memory>
#include <string>
#include <vector>

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

class BaslerCamera : public Camera {
public:
    BaslerCamera();
    ~BaslerCamera();

    std::unique_ptr<Camera> copy_class() override {
        return std::unique_ptr<Camera>(std::make_unique<BaslerCamera>());
    }

    std::vector<std::string> scan();

    void startAcquisition() override;
    void stopAcquisition() override;
    void startTrigger() override;
    void stopTrigger() override;
    bool doChangeGain(float new_gain) override;
    bool doChangeExposure(float new_exposure) override;

private:
    Pylon::CBaslerUsbInstantCamera camera;
    std::string configFileName;
    int doGetData() override;
    bool doConnectCamera() override;

    void set_trigger(Basler_UsbCameraParams::TriggerSourceEnums trigger_line);
};

#endif // BASLER_CAMERA_H

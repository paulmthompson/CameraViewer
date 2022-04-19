#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vector>

#include "videoencoder.h"
//#include <pylon/PylonIncludes.h>
//#include <pylon/usb/BaslerUsbInstantCamera.h>

#include <memory>

class Camera {
public:

    Camera();

    void setConfig(std::string path, std::string name);
    void setSave(std::string path, std::string name);

    void initializeVideoEncoder();

    virtual void connectCamera() {

    }

    void changeSize(int width, int height);
    void changeExposureTime(float exposure);
    void changeGain(float new_gain);

    void startAcquisition();
    void stopAcquisition();

    virtual std::unique_ptr<Camera> copy_class() {
        return std::unique_ptr<Camera>(std::make_unique<Camera>());
    }

    virtual void get_data(std::vector<uint8_t>& data_out) {

    }

    void assignID(int id);

    int getHeight() const;
    int getWidth() const;
    std::string getSerial() const;
    std::string getModel() const;
    bool getAttached() const;
    long getTotalFrames() const;

protected:
    int id;
    std::string serial_num;
    std::string model;

    int h;
    int w;
    int bit_depth;

    std::string save_file_name;
    std::string save_file_path;

    std::string config_file_name;
    std::string config_file_path;

    bool attached;
    bool acquisitionActive;
    bool saveData;

    float gain;
    float exposure_time;

    long totalFramesAcquired;

    std::unique_ptr<VideoEncoder> ve;
    // Pylon::CBaslerUsbInstantCamera camera // This should be in a specialized class.

};



#endif // CAMERA_H

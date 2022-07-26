#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vector>

#include <ffmpeg_wrapper/videoencoder.h>

#include <memory>

class Camera {
public:

    Camera();

    void setConfig(std::string path, std::string name);
    void setSave(std::string path, std::string name);

    void initializeVideoEncoder();
    void stopVideoEncoder();

    bool connectCamera();

    void changeSize(int width, int height);
    void changeExposureTime(float exposure);
    void changeGain(float new_gain);

    virtual void startAcquisition() {}
    virtual void stopAcquisition() {}
    virtual void startTrigger() {}
    virtual void stopTrigger() {}
    void setRecord(bool record_state);

    virtual std::unique_ptr<Camera> copy_class() {
        return std::unique_ptr<Camera>(std::make_unique<Camera>());
    }

    int get_data();
    int get_data(std::vector<uint8_t>& data_out); //This should return the number of frames acquired.

    void assignID(int id) {this->id = id;}
    void assignSerial(std::string serial) {this->serial_num = serial;}

    int getHeight() const {return h;}
    int getWidth() const {return w;}
    std::string getSerial() const {return serial_num;}
    std::string getModel() const {return model;}
    bool getAttached() const {return attached;}
    long getTotalFrames() const {return totalFramesAcquired;}
    long getTotalFramesSaved() const {return totalFramesSaved;}
    bool getAquisitionState() const {return acquisitionActive;}
    bool getTriggered() const {return triggered;}
    int getID() const {return id;}

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

    bool attached; //Specifies if the camera is connected and initialized.

    //The camera has received a signal to begin acquiring frames. In this state, it may depend on an internally generated software signal,
    //or it may be waiting for externally provided triggers
    //The get_data loop will therefore only be initiated if the camera is in the active acquisition state
    // If a camera is successflly connected, in a free run mode, it would be best to keep this true
    bool acquisitionActive;

    //After possibly acquiring frames, if the camera has state saveData, all of the frames will be saved using the
    //video encoder object.
    bool saveData;

    bool triggered;

    float gain;
    float exposure_time;

    long totalFramesAcquired;
    long totalFramesSaved;

    std::vector<uint8_t> img;

    std::unique_ptr<ffmpeg_wrapper::VideoEncoder> ve;

    virtual int doGetData() {return 0;}
    virtual bool doConnectCamera() {return false;}
    virtual bool doChangeGain(float new_gain) {return 0;}
    virtual bool doChangeExposure(float new_exposure) {return 0;}
};



#endif // CAMERA_H

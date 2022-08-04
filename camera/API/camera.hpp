
#include <ffmpeg_wrapper/videoencoder.h>

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#pragma once

struct ImageProperties {
    ImageProperties() : height(0),width(0),bit_depth(0) {}
    ImageProperties(int h, int w, int bd) :
        height(h), width(w), bit_depth(bd) {}
    int height;
    int width;
    int bit_depth;
};

class Camera {
public:

    Camera() {
        id = 0;
        ve = std::make_unique<ffmpeg_wrapper::VideoEncoder>();
        this->attached = false;
        this->save_file = "./test.mp4";
        totalFramesAcquired = 0;
        totalFramesSaved = 0;
        this->saveData = false;
        this->acquisitionActive = false;
        this->triggered = false;

        exposure_time = 0.005f;
        gain = 100.0f;

        img_prop = ImageProperties(480,640,8);

        img = std::vector<uint8_t>(480 * 640);
    };

    void setConfig(std::filesystem::path path) {
        this->config_file = path;
    };
    void setSave(std::filesystem::path path) {

        if (path.extension().compare(".mp4") != 0 ) {
            path.replace_extension(".mp4");
        }

        // Each camera needs to have a unique save file name
        // Append camera ID to filename for those greater than 0
        if (this->id > 0) {
            std::filesystem::path extension = path.extension();
            std::filesystem::path filename = path.filename().replace_extension().string();

            path.replace_filename(filename.string() + std::to_string(this->id));
            path.replace_extension(extension);
        }

        this->save_file = path;
        this->initializeVideoEncoder();
    };

    void initializeVideoEncoder() {
        ve->setSavePath(save_file.string());

        this->ve->createContext(this->img_prop.width,this->img_prop.height,25);
        this->ve->set_pixel_format(ffmpeg_wrapper::VideoEncoder::INPUT_PIXEL_FORMAT::GRAY8);
    };
    void stopVideoEncoder() {
        this->saveData = false;
        this->ve->closeFile();
    };

    /*
    Returns true if camera is successfully connected
    Returns false if camera is not able to be connected, or was already connected
    */
    bool connectCamera() {
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
    };

    void changeSize(int width, int height) {
        img_prop.width = width;
        img_prop.height = height;
        this->img.resize(width * height);
    };
    void changeExposureTime(float exposure) {
        this->exposure_time = exposure;
        doChangeExposure(exposure );
    };
    void changeGain(float new_gain) {
        this->gain = new_gain;
        doChangeGain(new_gain);
    };

    virtual void startAcquisition() {}
    virtual void stopAcquisition() {}
    virtual void startTrigger() {}
    virtual void stopTrigger() {}
    void setRecord(bool record_state) {
        this->saveData = record_state;

        if (record_state) {
            this->ve->openFile();
        } else {
            this->ve->closeFile();
        }
    };

    virtual std::unique_ptr<Camera> copy_class() {
        return std::unique_ptr<Camera>(std::make_unique<Camera>());
    }

    int get_data() {
        return this->doGetData();
    };
    int get_data(std::vector<uint8_t>& input_data) {
        int framesCollected = doGetData();

        input_data = this->img;

        return framesCollected;
    };

    void get_image(std::vector<uint8_t>& input_data) {
        input_data = this->img;
    }

    void assignID(int id) {this->id = id;}
    void assignSerial(std::string serial) {this->serial_num = serial;}

    ImageProperties getImageProp() const {return img_prop;}
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

    std::filesystem::path save_file;

    std::filesystem::path config_file;

    ImageProperties img_prop;

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

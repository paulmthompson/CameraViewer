
//#include <ffmpeg_wrapper/videoencoder.h>

#include <nlohmann/json.hpp>

#include "../API/camera.hpp"

#include "../cameras/virtual/virtual_camera.h"
#include "../cameras/basler/basler_camera.h"

#include <string>
#include <vector>
#include <memory>
#include <chrono>

using json = nlohmann::json;

#pragma once

class CameraManager {
public:
    CameraManager()
    {
        cams = std::vector<std::unique_ptr<Camera>>();
        save_file_path = "./test.mp4";
        record_countdown = 0;
    }
    bool getAttached(int cam_num) {return cams[cam_num]->getAttached();}
    bool connectCamera(int cam_num) {

        if (!this->getAttached(cam_num)) {
            cams[cam_num]->setSave(this->save_file_path);
            if (cams[cam_num]->connectCamera()) {
                startAcquisition(cam_num);
                return true;
             } else {
                std::cout << "The requested camera could not be connected" << std::endl;
                return false;
             }
        } else {
            std::cout << "The requested camera is already attached" << std::endl;
            return false;
        }
        return false;
    }
    void startAcquisition(int cam_num) {cams[cam_num]->startAcquisition();}
    std::string getModel(int cam_num) {return cams[cam_num].get()->getModel();}
    std::string getSerial(int cam_num) {return cams[cam_num].get()->getSerial();}

    void setRecord(bool recordState) {
        for (auto& cam : this->cams) {
            if (cam->getAttached()) {
                cam->setRecord(recordState);
            }
        }
    }
    void startTrigger() {
        for (auto& cam : this->cams) {
            if (cam->getAttached() && cam->getAquisitionState()) {
                cam->startTrigger();
            }
        }
    }

    void stopTrigger() {
        for (auto& cam : this->cams) {
            if (cam->getAttached() && cam->getAquisitionState()) {
                cam->stopTrigger();
            }
        }
    }

    bool areCamerasConnected() {

        bool output = false;
        for (auto& cam : this->cams) {
            output |= cam->getAttached();
        }

        return output;
    }

    void changeFileNames(std::filesystem::path p) {
        this->save_file_path = p;
        for (auto& cam : this->cams) {
            cam->setSave(this->save_file_path);
        }
    }

    size_t numberOfCameras() const {return cams.size();}

    int getCanvasSize(int cam_num) const {
        auto img_prop = cams[cam_num]->getImageProp();
        return img_prop.height * img_prop.width;
    }

    int getCanvasHeight(int cam_num) const {
        auto img_prop = cams[cam_num]->getImageProp();
        return img_prop.height;
    }

    int getCanvasWidth(int cam_num) const {
        auto img_prop = cams[cam_num]->getImageProp();
        return img_prop.width;
    }

    int acquisitionLoop() {

        //auto start = std::chrono::high_resolution_clock::now();
        int num_frames_acquired = 0;
        if (this->areCamerasConnected()) {

            //Cameras in the "active" state will return frames if they have them.
            for (auto& cam : this->cams) {
                if (cam->getAttached() && cam->getAquisitionState()) {
                    num_frames_acquired += cam->get_data();
                }
             }
             // If the cameras are no longer triggered and we were saving, or we were told to stop saving (but still have a trigger), we should close the file
             if (record_countdown > 0) {
                if (record_countdown == 1) {
                    this->setRecord(false);
                 }
                record_countdown--;
             }
        }
        return num_frames_acquired;
    }
    int getTotalFramesSaved(int cam_num) {return cams[cam_num]->getTotalFramesSaved();}
    int getTotalFrames(int cam_num) {return cams[cam_num]->getTotalFrames();}
    void setRecordCountdown() {this->record_countdown = 5;}

    void getImage(std::vector<uint8_t>& img,int cam_num) {
        cams[cam_num]->get_image(img);
    }

    void addVirtualCamera() {
        VirtualCamera v;
        cams.push_back(std::unique_ptr<Camera>(v.copy_class()));

        cams[cams.size()-1]->assignID(cams.size()-1);
    }
    void scanForCameras() {

        auto b = BaslerCamera();

        auto connected_camera_strings = b.scan();

        for (auto& serial_num : connected_camera_strings) { //This should return a pair of the model name and serial number I think so that both can be put in the table
            cams.push_back(std::unique_ptr<Camera>(b.copy_class()));
            cams[cams.size()-1]->assignID(cams.size()-1);
            cams[cams.size()-1]->assignSerial(serial_num);
        }
    }
private:
    std::vector<std::unique_ptr<Camera>> cams;
    std::filesystem::path save_file_path;
    int record_countdown;
};

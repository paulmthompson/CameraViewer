
#include <nlohmann/json.hpp>

#include "../API/camera.hpp"

#include "../cameras/virtual/virtual_camera.h"
#include "../cameras/basler/basler_camera.h"

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>

using json = nlohmann::json;

#pragma once

#if defined _WIN32 || defined __CYGWIN__
    #define DLLOPT __declspec(dllexport)
#else
    #define DLLOPT __attribute__((visibility("default")))
#endif

class DLLOPT CameraManager {
public:
    CameraManager()
    {
        cams = std::vector<std::unique_ptr<Camera>>();
        save_file_path = "./test.mp4";

        record_countdown = 0;
        record_countdown_state = false;
        std::vector<uint8_t> data{0};
    }
    std::vector<uint8_t> data;

    CameraManager(const CameraManager&) =delete;
    CameraManager& operator=(const CameraManager&) =delete;

    bool getAttached(int cam_num) {return cams[cam_num]->getAttached();}
    bool connectCamera(int cam_num) {

        if (!this->getAttached(cam_num)) {
            cams[cam_num]->setSave(this->save_file_path);
            if (cams[cam_num]->connectCamera()) {
                startAcquisition(cam_num);
                this->acquire_cams.push_back(cam_num);
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
        //If we are starting to record, we should change recording state to true

        //Alternatively, if we are setting recordings to be off
        //we should check if we are in the record_countdown_state which
        //is when our acquisition loop will run for several extra iterations
        //to make sure that we don't miss any frames
        if (record_countdown_state == true || recordState) {
            for (auto& cam : this->cams) {
                if (cam->getAttached()) {
                    cam->setRecord(recordState);
                }
            }
        } else {
            this->record_countdown_state = true;
            this->record_countdown = 5;
        }
    }
    void trigger(bool trigger) {
        for (auto& cam : this->cams) { // This should only trigger attached cameras
            if (cam->getAttached() && cam->getAquisitionState()) {
                if (trigger) {
                    cam->startTrigger();
                } else {
                    cam->stopTrigger();
                }
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
             if (record_countdown_state) {
                if (record_countdown == 1) {
                    this->setRecord(false);
                    this->record_countdown_state = false;
                 }
                record_countdown--;
             }
        }
        return num_frames_acquired;
    }
    int getTotalFramesSaved(int cam_num) {return cams[cam_num]->getTotalFramesSaved();}
    int getTotalFrames(int cam_num) {return cams[cam_num]->getTotalFrames();}

    void getImage(std::vector<uint8_t>& img,int cam_num) {
        cams[cam_num]->get_image(img);
    }
    void getImage(int cam_num) {
        cams[cam_num]->get_image(this->data);
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
    void loadConfigurationFile(std::filesystem::path& config_path) {

        std::ifstream f(config_path.string());
        json data = json::parse(f);
        f.close();

        for (const auto& entry :data["cameras"]) {
            std::cout << "Loading first camera named " << entry["name"] << std::endl;
            std::string camera_type = entry["type"];
            if (camera_type.compare("virtual") == 0) {
                std::cout << "Loading virtual camera" << std::endl;

                this->addVirtualCamera();

            } else if (camera_type.compare("basler") == 0) {
                std::cout << "loading basler camera" << std::endl;

                if (cams.size() == 0) {
                    this->scanForCameras();
                }

                for (auto& cam : cams) {
                   std::string serial_num = cam->getSerial();
                   if (serial_num.compare(entry["serial-number"]) == 0) {
                       std::cout << "found matched serial number " << std::endl;

                       cam->setConfig(entry["config-filepath"]);

                       if (this->connectCamera(cam->getID())) {
                           std::cout << "Camera connected" << std::endl;
                       }
                       break;
                   }
                }

            } else {
                std::cout << "Unknown camera type " << camera_type << std::endl;
            }
        }

    }
    std::vector<int> getAcquireCams() {return this->acquire_cams;}
private:
    std::vector<std::unique_ptr<Camera>> cams; // These are all of the cameras that are connected to the computer and detected
    std::vector<int> acquire_cams; // This array lists the indexes of cameras where we actually want to collect data from above
    std::filesystem::path save_file_path;
    int record_countdown;
    bool record_countdown_state;
};

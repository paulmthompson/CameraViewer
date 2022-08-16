#include "CameraCWrapper.h"
#include "../CameraManager/cameramanager.hpp"
#include <filesystem>
#include <string>
#include <vector>

extern "C" {

    CameraManager* newCameraManager() {
        return new CameraManager();
    }

    void CameraManager_LoadConfigurationFile(CameraManager* cam, const char *config_file) {
        std::filesystem::path path = std::string(config_file);
        cam->loadConfigurationFile(path);
    }

    void CameraManager_SetRecord(CameraManager* cam, bool record_state) {
        cam->setRecord(record_state);
    }

    void CameraManager_ChangeFileNames(CameraManager* cam, const char *save_file_path) {
        cam->changeFileNames(save_file_path);
    }

    int CameraManager_AcquisitionLoop(CameraManager* cam) {
        return cam->acquisitionLoop();
    }

    void CameraManager_GetImage(CameraManager* cam, uint8_t* data, int cam_num) {
        cam->getImage(cam_num);
        data = cam->data.data();
    }
    int CameraManager_GetActiveCameras(CameraManager* cam, int* active_cams) {
        active_cams = cam->getAcquireCams().data();
        return cam->getAcquireCams().size();
    }



}

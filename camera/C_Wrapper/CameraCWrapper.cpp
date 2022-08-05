#include "CameraCWrapper.h"
#include "../CameraManager/cameramanager.hpp"


extern "C" {

    CameraManager* newCameraManager() {
        return new CameraManager();
    }



}

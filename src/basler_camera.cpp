
#include "basler_camera.h"

#include <memory>
#include <iostream>

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

BaslerCamera::BaslerCamera() {

}

void BaslerCamera::startAcquisition() {
    this->acquisitionActive = true;
    camera.StartGrabbing();
}

void BaslerCamera::stopAcquisition() {
    this->acquisitionActive = false;
    camera.StopGrabbing();
}

void BaslerCamera::startTrigger() {
    this->triggered = true;
}
void BaslerCamera::stopTrigger() {
    this->triggered = false;
}

void BaslerCamera::connectCamera() {

    // Get the transport layer factory.
    Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();

    // Get all attached devices and exit application if no device is found.
    Pylon::DeviceInfoList_t devices;

    if ( tlFactory.EnumerateDevices(devices) == 0 )
    {
        throw RUNTIME_EXCEPTION( "Not enough cameras present.");
    }

    for (int i = 0; i < devices.size(); i++) {

        if (devices[i].GetSerialNumber() == this->serial_num.c_str()) {
            std::cout << "Matched serial number for " << devices[i].GetSerialNumber() << std::endl;

            camera.Attach( tlFactory.CreateDevice( devices[ i ]));

            if (camera.IsPylonDeviceAttached())
            {
                std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << std::endl;
                attached = true;

                camera.MaxNumBuffer = 50;
                camera.Open(); // Need to access parameters

                //Load values from configuration file
                Pylon::CFeaturePersistence::Load(configFileName.c_str(), &camera.GetNodeMap(), true);
            } else {
                std::cout << "Camera was not able to be initialized. Is one connected?" << std::endl;
            }
        } else {
            std::cout << "Not matched serial number for " << devices[i].GetSerialNumber() << std::endl;
        }
    }

    this->attached = true;
}

int BaslerCamera::get_data(std::vector<uint8_t>& data_out) {

    int frames_acquired = 0;

    Pylon::CGrabResultPtr ptrGrabResult;

    if (camera.RetrieveResult(0, ptrGrabResult, Pylon::TimeoutHandling_Return)) {

        memcpy(&data_out.data()[0],ptrGrabResult->GetBuffer(),this->h*this->w);

        if (this->saveData) {
            ve->writeFrameGray8(data_out);
            this->totalFramesSaved++;
        }
        this->totalFramesAcquired++;
        frames_acquired++;
    }

    return frames_acquired;
}

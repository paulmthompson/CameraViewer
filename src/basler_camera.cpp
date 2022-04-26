
#include "basler_camera.h"

#include <memory>
#include <iostream>

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

BaslerCamera::BaslerCamera() {
    Pylon::PylonInitialize();
    configFileName = "";
}

BaslerCamera::~BaslerCamera() {
    Pylon::PylonTerminate();
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
    camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Software);
    this->triggered = true;
}

void BaslerCamera::stopTrigger() {
    camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line3);
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

                //camera.RegisterConfiguration( new Pylon::CSoftwareTriggerConfiguration, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);
                //camera.RegisterConfiguration(new Pylon::CAcquireContinuousConfiguration, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

                camera.Open(); // Need to access parameters

                //Load values from configuration file
                if (!configFileName.empty()) {
                    Pylon::CFeaturePersistence::Load(configFileName.c_str(), &camera.GetNodeMap(), true);
                }

                //Here we should update all of the parameters for the camera
                this->gain = camera.Gain.GetValue();
                this->w = camera.Width.GetValue();
                this->h = camera.Height.GetValue();
                this->exposure_time = camera.ExposureTime.GetValue();
                std::string pix_fmt = camera.PixelFormat.ToString().c_str();
                if (pix_fmt == "Mono8") {
                    this->bit_depth = 1;
                }
                //Configure the

                // Select the Frame Start trigger
                camera.TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);

                // Enable triggered image acquisition for the Frame Start trigger
                camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);
                // Set the trigger source for the Frame Start trigger to Software
                //camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Software);
                camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line3);
                // Generate a software trigger signal
                //
                //Pylon::CSoftwareTriggerConfiguration

            } else {
                std::cout << "Camera was not able to be initialized. Is one connected?" << std::endl;
            }
        } else {
            std::cout << "Not matched serial number for " << devices[i].GetSerialNumber() << std::endl;
        }
    }

    this->attached = true;
}

int BaslerCamera::doGetData() {

    int frames_acquired = 0;

    if (this->triggered) {
        camera.TriggerSoftware.Execute();
    }

    Pylon::CGrabResultPtr ptrGrabResult;

    while (camera.RetrieveResult(0, ptrGrabResult, Pylon::TimeoutHandling_Return)) {

        memcpy(&this->img.data()[0],ptrGrabResult->GetBuffer(),this->h*this->w);

        if (this->saveData) {
            ve->writeFrameGray8(this->img);
            this->totalFramesSaved++;
        }
        this->totalFramesAcquired++;
        frames_acquired++;
    }

    return frames_acquired;
}

std::vector<std::string> BaslerCamera::scan() {

    std::vector<std::string> output = {};

    Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();

    Pylon::DeviceInfoList_t devices;
    if ( tlFactory.EnumerateDevices(devices) == 0 )
    {
        return output;
    } else {
        for (auto& device : devices) {
            output.push_back(device.GetSerialNumber().c_str());
        }
        return output;
    }
}

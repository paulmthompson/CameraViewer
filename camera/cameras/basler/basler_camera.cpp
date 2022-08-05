
#include "basler_camera.h"

#include <memory>
#include <iostream>
#include <filesystem>

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

BaslerCamera::BaslerCamera() {
    Pylon::PylonInitialize();
    config_file = "default.pfs";
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
    set_trigger(Basler_UsbCameraParams::TriggerSource_Software);
    this->triggered = true;
}

void BaslerCamera::stopTrigger() {
    set_trigger(Basler_UsbCameraParams::TriggerSource_Line3);
    this->triggered = false;
}

bool BaslerCamera::doConnectCamera() {

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

                camera.MaxNumBuffer = 50;

                //camera.RegisterConfiguration( new Pylon::CSoftwareTriggerConfiguration, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);
                //camera.RegisterConfiguration(new Pylon::CAcquireContinuousConfiguration, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

                camera.Open(); // Need to access parameters

                //Load values from configuration file
                if (!config_file.empty()) {
                    if (std::filesystem::exists(this->config_file)) {
                        Pylon::CFeaturePersistence::Load(config_file.string().c_str(), &camera.GetNodeMap(), true);
                        std::cout << "Configuration file " << this->config_file << " loaded" << std::endl;
                    } else {
                        std::cout << "Could not find configuration file: " << this->config_file << std::endl;
                    }
                }

                //Here we should update all of the parameters for the camera
                this->gain = camera.Gain.GetValue();
                this->img_prop.width = camera.Width.GetValue();
                this->img_prop.height = camera.Height.GetValue();
                this->exposure_time = camera.ExposureTime.GetValue();
                std::string pix_fmt = camera.PixelFormat.ToString().c_str();
                if (pix_fmt == "Mono8") {
                    this->img_prop.bit_depth = 1;
                }

                set_trigger(Basler_UsbCameraParams::TriggerSource_Line3);

            } else {
                std::cout << "Camera was not able to be initialized. Is one connected?" << std::endl;
                return false;
            }
        } else {
            std::cout << "Not matched serial number for " << devices[i].GetSerialNumber() << std::endl;
            return false;
        }
    }

    this->attached = true;
    return true;
}

void BaslerCamera::set_trigger(Basler_UsbCameraParams::TriggerSourceEnums trigger_line) {

    //camera.AcquisitionMode.SetValue(Basler_UsbCameraParams::AcquisitionMode_SingleFrame);

    camera.TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);

    camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);

    camera.TriggerSource.SetValue(trigger_line);

    camera.TriggerActivation.SetValue(Basler_UsbCameraParams::TriggerActivation_RisingEdge);
}

int BaslerCamera::doGetData() {

    int frames_acquired = 0;

    if (this->triggered) {
        camera.TriggerSoftware.Execute();
    }

    Pylon::CGrabResultPtr ptrGrabResult;

    while (camera.RetrieveResult(0, ptrGrabResult, Pylon::TimeoutHandling_Return)) {

        memcpy(&this->img.data()[0],ptrGrabResult->GetBuffer(),this->img_prop.height*this->img_prop.width);

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

bool BaslerCamera::doChangeGain(float new_gain) {

    camera.Gain.SetValue(new_gain);

    return true;
}

bool BaslerCamera::doChangeExposure(float new_exposure) {

    camera.ExposureTime.SetValue(new_exposure);

    return true;
}

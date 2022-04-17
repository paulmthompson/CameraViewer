#ifndef VIRTUAL_CAMERA_H
#define VIRTUAL_CAMERA_H

#include "camera.h"

#include <memory>

class VirtualCamera : public Camera {
public:
    VirtualCamera();

    std::unique_ptr<Camera> copy_class() override {
        return std::unique_ptr<Camera>(std::make_unique<VirtualCamera>());
    }

private:
};

std::unique_ptr<VirtualCamera> createVirtualCamera();
#endif // VIRTUAL_CAMERA_H

#ifndef VIRTUAL_CAMERA_H
#define VIRTUAL_CAMERA_H

#include "camera.h"

#include <memory>
#include <random>


class VirtualCamera : public Camera {
public:
    VirtualCamera();

    std::unique_ptr<Camera> copy_class() override {
        return std::unique_ptr<Camera>(std::make_unique<VirtualCamera>());
    }

    void connectCamera() override;
    int get_data(std::vector<uint8_t>& data_out) override;

private:
    int fps; // Use this for stress testing.

    std::vector<std::vector<uint8_t>> random_nums;
    int random_index;
};

std::unique_ptr<VirtualCamera> createVirtualCamera();
#endif // VIRTUAL_CAMERA_H


#include "virtual_camera.h"

#include <memory>

VirtualCamera::VirtualCamera() {
    serial_num = "xxx-xxx-xxx";
    model = "Virtual";

    h = 480;
    w = 640;

}

std::unique_ptr<VirtualCamera> createVirtualCamera() {

    return std::make_unique<VirtualCamera>(VirtualCamera());
}

void VirtualCamera::get_data(std::vector<uint8_t>& data_out) {

    for (int i = 0; i < data_out.size(); i++) {
        data_out[i] = rand() % 255;
    }

    ve->writeFrameGray8(data_out);
}


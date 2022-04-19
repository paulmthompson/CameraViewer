
#include "virtual_camera.h"

#include <memory>
#include <random>
#include <algorithm>

VirtualCamera::VirtualCamera() {
    serial_num = "xxx-xxx-xxx";
    model = "Virtual";

    h = 480;
    w = 640;
    fps = 500;
    random_index = 0;

    std::random_device random_device;
    std::mt19937 random_engine{random_device()};
    std::uniform_int_distribution distribution_0_255 = std::uniform_int_distribution<uint16_t>(0,255);

    random_nums = std::vector<std::vector<uint8_t>>(50);

    auto gen = [&distribution_0_255, &random_engine](){
                       return distribution_0_255(random_engine);
                   };
    for (int i = 0; i< random_nums.size(); i ++) {
        random_nums[i] = std::vector<uint8_t>(h*w);
        std::generate(begin(random_nums[i]),end(random_nums[i]),gen);
    }
}

std::unique_ptr<VirtualCamera> createVirtualCamera() {

    return std::make_unique<VirtualCamera>(VirtualCamera());
}

void VirtualCamera::connectCamera() {
    this->attached = true;
}

void VirtualCamera::get_data(std::vector<uint8_t>& data_out) {

    // 1000 ms / 40 ms (loop speed) = 25 fps normally. We should loop in multiples of this



    for (int j = 0; j < this->fps / 25; j++) {

        memcpy(&data_out.data()[0],&random_nums[this->random_index++].data()[0],this->h*this->w);

        ve->writeFrameGray8(data_out);

        if (random_index >= random_nums.size()) {
            random_index = 0;
        }
    }
}


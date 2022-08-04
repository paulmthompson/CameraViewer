
#include "virtual_camera.h"

#include <memory>
#include <random>
#include <algorithm>

VirtualCamera::VirtualCamera() {
    serial_num = "xxx-xxx-" + std::to_string(std::rand() % 999);
    model = "Virtual";

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
        random_nums[i] = std::vector<uint8_t>(img_prop.height*img_prop.width);
        std::generate(begin(random_nums[i]),end(random_nums[i]),gen);
    }
}

bool VirtualCamera::doConnectCamera() {
    this->attached = true;
    return true;
}

int VirtualCamera::doGetData() {

    // 1000 ms / 40 ms (loop speed) = 25 fps normally. We should loop in multiples of this

    int frames_acquired = 0;

    if (triggered) {

    for (int j = 0; j < this->fps / 25; j++) {

        memcpy(&this->img.data()[0],&random_nums[this->random_index++].data()[0],img_prop.height*img_prop.width);

        if (this->saveData) {
            ve->writeFrameGray8(this->img);
            this->totalFramesSaved++;
        }

        if (random_index >= random_nums.size()) {
            random_index = 0;
        }
        this->totalFramesAcquired++;
        frames_acquired++;
    }

    }
    return frames_acquired;
}


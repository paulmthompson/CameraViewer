
#include "virtual_camera.h"

#include <memory>

VirtualCamera::VirtualCamera() {
    serial_num = "xxx-xxx-xxx";
    model = "Virtual";

}

std::unique_ptr<VirtualCamera> createVirtualCamera() {

    return std::make_unique<VirtualCamera>(VirtualCamera());
}

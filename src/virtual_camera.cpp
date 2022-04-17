
#include "virtual_camera.h"

#include <memory>

VirtualCamera::VirtualCamera() {

}

std::unique_ptr<VirtualCamera> createVirtualCamera() {

    return std::make_unique<VirtualCamera>(VirtualCamera());
}

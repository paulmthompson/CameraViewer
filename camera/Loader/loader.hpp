
//#include <ffmpeg_wrapper/videoencoder.h>

#include <nlohmann/json.hpp>

#include "API/camera.hpp"

#include <string>
#include <vector>
#include <memory>

using json = nlohmann::json;

#pragma once

class CameraLoader {

private:
    std::vector<std::unique_ptr<Camera>> cams;
};

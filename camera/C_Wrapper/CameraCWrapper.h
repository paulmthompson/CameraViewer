#ifndef __CAMERACWRAPPER_H
#define __CAMERACWRAPPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CameraManager CameraManager;

#if defined _WIN32 || defined __CYGWIN__
    #define DLLOPT __declspec(dllexport)
#else
    #define DLLOPT __attribute__((visibility("default")))
#endif

DLLOPT CameraManager* newCameraManager();


#ifdef __cplusplus
}
#endif
#endif

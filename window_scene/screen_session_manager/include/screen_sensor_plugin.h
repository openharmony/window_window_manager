/*
* Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
*/

/*
 * part plugin interface
 *
 * @since 2024-10-25
 */

 #ifndef PLUGIN_H
 #define PLUGIN_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
#if (defined(__aarch64__) || defined(__x86_64__))
const std::string PLUGIN_SO_PATH = "/system/lib64/platformsdk/libmotion_agent.z.so";
#else
const std::string PLUGIN_SO_PATH = "/system/lib/platformsdk/libmotion_agent.z.so";
#endif

typedef struct MotionSensorEvent {
    int32_t type = -1;
    int32_t status = -1;
    int32_t dataLen = -1;
    int32_t *data = nullptr;
} MotionSensorEvent;

using OnMotionChangedPtr = void (*)(const MotionSensorEvent&);
using MotionSubscriberCallbackPtr =  bool (*)(int32_t, OnMotionChangedPtr);
using MotionUnsubscriberCallbackPtr = bool (*)(int32_t, OnMotionChangedPtr);

bool LoadMotionSensor(void);
void UnloadMotionSensor(void);
bool SubscribeCallback(int32_t motionType, OnMotionChangedPtr callback);
bool UnsubscribeCallback(int32_t motionType, OnMotionChangedPtr callback);
}
}
#endif /* PLUGIN_H */
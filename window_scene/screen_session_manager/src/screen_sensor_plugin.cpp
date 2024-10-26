/*
* Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
*/

/*
 * part plugin interface
 *
 * @since 2024-10-25
 */
#include "screen_sensor_plugin.h"

namespace OHOS {
namespace Rosen {

static void *g_handle = nullptr;

bool LoadMotionSensor(void)
{
    if (g_handle == nullptr) {
        g_handle = dlopen(PLUGIN_SO_PATH.c_str(), RTLD_LAZY);
        if (!g_handle) {
            TLOGE(WmsLogTag::DMS, "g_handle is nullptr, so path: %{public}s", PLUGIN_SO_PATH.c_str());
            return false;
        }
    }
    return true;
}

void UnloadMotionSensor(void)
{
    if (g_handle != nullptr) {
        dlclose(g_handle);
        g_handle = nullptr;
    }
}

__attribute__((no_sanitize("cfi"))) bool SubscribeCallback(int32_t motionType, OnMotionChangedPtr callback)
{
    if (callback == nullptr) {
        TLOGE(WmsLogTag::DMS, "callback is nullptr");
        return false;
    }
    if (g_handle == nullptr) {
        TLOGE(WmsLogTag::DMS, "g_handle is nullptr");
        return false;
    }
    MotionSubscriberCallbackPtr func = (MotionSubscriberCallbackPtr)(dlsym(g_handle, "MotionSubscriberCallbackPtr"));
    const char* dlsym_error = dlerror();
    if (func == nullptr) {
        TLOGE(WmsLogTag::DMS, "MotionSubscriberCallbackPtr is nullptr");
        if  (dlsym_error) {
            TLOGW(WmsLogTag::DMS, "dlsym error: %{public}s", dlsym_error);
        }
        return false;
    }
    return func(motionType, callback);
}

__attribute__((no_sanitize("cfi"))) bool UnsubscribeCallback(int32_t motionType, OnMotionChangedPtr callback)
{
    if (callback == nullptr) {
        TLOGE(WmsLogTag::DMS, "callback is nullptr");
        return false;
    }
    if (g_handle == nullptr) {
        TLOGE(WmsLogTag::DMS, "g_handle is nullptr");
        return false;
    }
    MotionUnsubscriberCallbackPtr func =
        (MotionUnsubscriberCallbackPtr)(dlsym(g_handle, "MotionUnsubscriberCallbackPtr"));
    const char* dlsym_error = dlerror();
    if (func == nullptr) {
        TLOGE(WmsLogTag::DMS, "MotionUnsubscriberCallbackPtr is nullptr");
        if  (dlsym_error) {
            TLOGW(WmsLogTag::DMS, "dlsym error: %{public}s", dlsym_error);
        }
        return false;
    }
    return func(motionType, callback);
}
}
}
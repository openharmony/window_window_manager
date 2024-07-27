/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_SETTING_HELPER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_SETTING_HELPER_H

#include <cstdint>
#include <string>

#include "setting_observer.h"

namespace OHOS {
namespace Rosen {
class ScreenSettingHelper {
public:
    static void RegisterSettingDpiObserver(SettingObserver::UpdateFunc func);
    static void UnregisterSettingDpiObserver();
    static bool GetSettingDpi(uint32_t& dpi, const std::string& key = SETTING_DPI_KEY);
    static bool GetSettingValue(uint32_t& value, const std::string& key);
    static void RegisterSettingCastObserver(SettingObserver::UpdateFunc func);
    static void UnregisterSettingCastObserver();
    static bool GetSettingCast(bool& enable, const std::string& key = SETTING_CAST_KEY);
    static void RegisterSettingRotationObserver(SettingObserver::UpdateFunc func);
    static void UnregisterSettingRotationObserver();
    static void SetSettingRotation(int32_t rotation);
    static void SetSettingRotationScreenId(int32_t screenId);
    static bool GetSettingRotation(int32_t& rotation, const std::string& key = SETTING_ROTATION_KEY);
    static bool GetSettingRotationScreenID(int32_t& screenId, const std::string& key = SETTING_ROTATION_SCREEN_ID_KEY);

private:
    static const constexpr char* SETTING_DPI_KEY {"user_set_dpi_value"};
    static const constexpr char* SETTING_CAST_KEY {"huaweicast.data.privacy_projection_state"};
    static const constexpr char* SETTING_ROTATION_KEY {"screen_rotation_value"};
    static const constexpr char* SETTING_ROTATION_SCREEN_ID_KEY {"screen_rotation_screen_id_value"};
    static sptr<SettingObserver> dpiObserver_;
    static sptr<SettingObserver> castObserver_;
    static sptr<SettingObserver> rotationObserver_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SETTING_HELPER_H

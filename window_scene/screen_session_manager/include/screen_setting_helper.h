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

#include <charconv>
#include <cstdint>
#include <string>
#include <sstream>
#include <cinttypes>
#include <set>

#include "screen_edid_parse.h"
#include "setting_observer.h"
#include "dm_common.h"

namespace OHOS {
namespace Rosen {
class ScreenSettingHelper {
public:
    static void RegisterSettingDpiObserver(SettingObserver::UpdateFunc func);
    static void UnregisterSettingDpiObserver();
    static bool GetSettingDpi(uint32_t& dpi, const std::string& key = SETTING_DPI_KEY);
    static bool SetSettingDefaultDpi(uint32_t& dpi, const std::string& key);
    static bool GetSettingValue(uint32_t& value, const std::string& key);
    static bool SetSettingValue(const std::string& key, uint32_t value);
    static bool GetSettingValue(const std::string& key, std::string& value);
    static bool SetSettingValue(const std::string& key, const std::string& value);
    static void RegisterSettingCastObserver(SettingObserver::UpdateFunc func);
    static void UnregisterSettingCastObserver();
    static bool GetSettingCast(bool& enable, const std::string& key = SETTING_CAST_KEY);
    static void RegisterSettingRotationObserver(SettingObserver::UpdateFunc func);
    static void UnregisterSettingRotationObserver();
    static void SetSettingRotation(int32_t rotation);
    static void SetSettingRotationScreenId(int32_t screenId);
    static bool GetSettingRotation(int32_t& rotation, const std::string& key = SETTING_ROTATION_KEY);
    static bool GetSettingRotationScreenID(int32_t& screenId, const std::string& key = SETTING_ROTATION_SCREEN_ID_KEY);
    static std::string RemoveInvalidChar(const std::string& input);
    static bool SplitString(std::vector<std::string>& splitValues, const std::string& input, char delimiter = ',');
    static bool IsNumber(const std::string& str);
    static std::map<std::string, MultiScreenInfo> GetMultiScreenInfo(const std::string& key = SETTING_SCREEN_MODE_KEY);
    static bool GetScreenMode(MultiScreenInfo& info, const std::string& inputString);
    static bool UpdateScreenMode(MultiScreenInfo& info, uint32_t mode, bool isExternal);
    static bool GetScreenRelativePosition(MultiScreenInfo& info, const std::string& inputString);
    static ScreenShape GetScreenShape(ScreenId screenId);
    static void RegisterSettingWireCastObserver(SettingObserver::UpdateFunc func);
    static void UnregisterSettingWireCastObserver();
    static void RegisterSettingExtendScreenDpiObserver(SettingObserver::UpdateFunc func);
    static void UnRegisterSettingExtendScreenDpiObserver();
    static bool GetSettingExtendScreenDpi(float& coef, const std::string& key = SETTING_EXTEND_DPI_KEY);
    static bool ConvertStrToUint64(const std::string& str, uint64_t& num);
    static bool ConvertStrToInt32(const std::string& str, int32_t& num);

private:
    static const constexpr char* SETTING_DPI_KEY {"user_set_dpi_value"};
    static const constexpr char* SETTING_CAST_KEY {"huaweicast.data.privacy_projection_state"};
    static const constexpr char* SETTING_ROTATION_KEY {"screen_rotation_value"};
    static const constexpr char* SETTING_ROTATION_SCREEN_ID_KEY {"screen_rotation_screen_id_value"};
    static const constexpr char* SETTING_SCREEN_MODE_KEY {"user_set_screen_mode_edid"};
    static const constexpr char* SETTING_EXTEND_DPI_KEY {"user_set_dpi_extend"};
    static const constexpr uint32_t BASE_TEN = 10;
    static sptr<SettingObserver> dpiObserver_;
    static sptr<SettingObserver> castObserver_;
    static sptr<SettingObserver> rotationObserver_;
    static sptr<SettingObserver> wireCastObserver_;
    static sptr<SettingObserver> extendScreenDpiObserver_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SETTING_HELPER_H

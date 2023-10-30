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
    static void RegisterSettingDpiObserver(PowerMgr::SettingObserver::UpdateFunc func);
    static void UnregisterSettingDpiObserver();
    static bool GetSettingDpi(uint32_t& dpi, const std::string& key = SETTING_DPI_KEY);

private:
    static const constexpr char* SETTING_DPI_KEY {"user_set_dpi_value"};
    static sptr<PowerMgr::SettingObserver> dpiObserver_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SETTING_HELPER_H
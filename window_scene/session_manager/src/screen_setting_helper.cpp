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
#include "screen_setting_helper.h"

#include "window_manager_hilog.h"
#include "setting_provider.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSettingHelper" };
}

sptr<PowerMgr::SettingObserver> ScreenSettingHelper::dpiObserver_;

void ScreenSettingHelper::RegisterSettingDpiObserver(PowerMgr::SettingObserver::UpdateFunc func)
{
    if (dpiObserver_) {
        WLOGFD("setting dpi observer is already registered");
        return;
    }
    PowerMgr::SettingProvider& provider = PowerMgr::SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    dpiObserver_ = provider.CreateObserver(SETTING_DPI_KEY, func);
    ErrCode ret = provider.RegisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        WLOGFW("register setting dpi observer failed, ret=%{public}d", ret);
        dpiObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingDpiObserver()
{
    if (dpiObserver_ == nullptr) {
        WLOGFD("dpiObserver_ is nullptr, no need to unregister");
        return;
    }
    PowerMgr::SettingProvider& provider = PowerMgr::SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.UnregisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        WLOGFW("unregister setting dpi observer failed, ret=%{public}d", ret);
    }
    dpiObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingDpi(uint32_t& dpi, const std::string& key)
{
#ifdef POWER_MANAGER_ENABLE
    PowerMgr::SettingProvider& provider = PowerMgr::SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    int32_t value;
    ErrCode ret = provider.GetIntValue(key, value);
    if (ret != ERR_OK) {
        WLOGFW("get setting dpi failed, ret=%{public}d", ret);
        return false;
    }
    dpi = static_cast<uint32_t>(value);
    return true;
#else
    WLOGFW("Can not find the sub system of PowerMgr");
    return false;
#endif
}
} // namespace Rosen
} // namespace OHOS
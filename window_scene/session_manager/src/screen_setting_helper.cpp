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
sptr<SettingObserver> ScreenSettingHelper::dpiObserver_;
sptr<SettingObserver> ScreenSettingHelper::castObserver_;
sptr<SettingObserver> ScreenSettingHelper::rotationObserver_;

void ScreenSettingHelper::RegisterSettingDpiObserver(SettingObserver::UpdateFunc func)
{
    if (dpiObserver_) {
        TLOGD(WmsLogTag::DMS, "setting dpi observer is already registered");
        return;
    }
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    dpiObserver_ = provider.CreateObserver(SETTING_DPI_KEY, func);
    ErrCode ret = provider.RegisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "register setting dpi observer failed, ret=%{public}d", ret);
        dpiObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingDpiObserver()
{
    if (dpiObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "dpiObserver_ is nullptr, no need to unregister");
        return;
    }
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.UnregisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "unregister setting dpi observer failed, ret=%{public}d", ret);
    }
    dpiObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingDpi(uint32_t& dpi, const std::string& key)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    int32_t value;
    ErrCode ret = provider.GetIntValue(key, value);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "get setting dpi failed, ret=%{public}d", ret);
        return false;
    }
    dpi = static_cast<uint32_t>(value);
    return true;
}

bool ScreenSettingHelper::GetSettingValue(uint32_t& value, const std::string& key)
{
    SettingProvider& settingData = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    int32_t getValue;
    ErrCode ret = settingData.GetIntValue(key, getValue);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "get setting value failed, ret=%{public}d", ret);
        return false;
    }
    value = static_cast<uint32_t>(getValue);
    return true;
}

void ScreenSettingHelper::RegisterSettingCastObserver(SettingObserver::UpdateFunc func)
{
    if (castObserver_) {
        TLOGD(WmsLogTag::DMS, "setting cast observer is already registered");
        return;
    }
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    castObserver_ = castProvider.CreateObserver(SETTING_CAST_KEY, func);
    ErrCode ret = castProvider.RegisterObserver(castObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "register setting cast observer failed, ret=%{public}d", ret);
        castObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingCastObserver()
{
    if (castObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "castObserver_ is nullptr, no need to unregister");
        return;
    }
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = castProvider.UnregisterObserver(castObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "unregister setting cast observer failed, ret=%{public}d", ret);
    }
    castObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingCast(bool& enable, const std::string& key)
{
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = castProvider.GetBoolValue(key, enable);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "get setting dpi failed, ret=%{public}d", ret);
        return false;
    }
    return true;
}

void ScreenSettingHelper::RegisterSettingRotationObserver(SettingObserver::UpdateFunc func)
{
    if (rotationObserver_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "setting rotation observer is already registered");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    rotationObserver_ = settingProvider.CreateObserver(SETTING_ROTATION_KEY, func);
    ErrCode ret = settingProvider.RegisterObserver(rotationObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "register setting rotation observer failed, ret:%{public}d", ret);
        rotationObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingRotationObserver()
{
    if (rotationObserver_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "rotationObserver_ is nullptr, no need to unregister");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.UnregisterObserver(rotationObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "unregister setting rotation observer failed, ret:%{public}d", ret);
    }
    rotationObserver_ = nullptr;
}

void ScreenSettingHelper::SetSettingRotation(int32_t rotation)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.PutIntValue(SETTING_ROTATION_KEY, rotation, true);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "set setting rotation failed, ret:%{public}d", ret);
        return;
    }
    TLOGE(WmsLogTag::DMS, "set setting rotation succeed, ret:%{public}d", ret);
}

bool ScreenSettingHelper::GetSettingRotation(int32_t& rotation, const std::string& key)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetIntValue(key, rotation);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "get setting rotation failed, ret:%{public}d", ret);
        return false;
    }
    TLOGE(WmsLogTag::DMS, "current rotation:%{public}d", rotation);
    return true;
}

void ScreenSettingHelper::SetSettingRotationScreenId(int32_t screenId)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.PutIntValue(SETTING_ROTATION_SCREEN_ID_KEY, screenId, false);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "set setting rotation screen id failed, ret:%{public}d", ret);
        return;
    }
    TLOGE(WmsLogTag::DMS, "set setting rotation screen id succeed, ret:%{public}d", ret);
}

bool ScreenSettingHelper::GetSettingRotationScreenId(int32_t& screenId, const std::string& key)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetIntValue(key, screenId);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "get setting rotation screen id failed, ret:%{public}d", ret);
        return false;
    }
    TLOGE(WmsLogTag::DMS, "current rotation screen id:%{public}d", screenId);
    return true;
}
} // namespace Rosen
} // namespace OHOS

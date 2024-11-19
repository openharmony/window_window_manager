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
constexpr int32_t PARAM_NUM_TEN = 10;
constexpr int32_t EXPECT_SCREEN_MODE_SIZE = 2;
constexpr int32_t EXPECT_RELATIVE_POSITION_SIZE = 3;
constexpr int32_t EXPECT_RESOLUTION_SIZE = 3;
constexpr int32_t RESOLVED_DATA_INDEX_ZERO = 0;
constexpr int32_t RESOLVED_DATA_INDEX_ONE = 1;
constexpr int32_t RESOLVED_DATA_INDEX_TWO = 2;

void ScreenSettingHelper::RegisterSettingDpiObserver(SettingObserver::UpdateFunc func)
{
    if (dpiObserver_) {
        TLOGD(WmsLogTag::DMS, "setting dpi observer is registered");
        return;
    }
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    dpiObserver_ = provider.CreateObserver(SETTING_DPI_KEY, func);
    ErrCode ret = provider.RegisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        dpiObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingDpiObserver()
{
    if (dpiObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "dpiObserver_ is nullptr");
        return;
    }
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.UnregisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
    }
    dpiObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingDpi(uint32_t& dpi, const std::string& key)
{
    return GetSettingValue(dpi, key);
}

bool ScreenSettingHelper::GetSettingValue(uint32_t& value, const std::string& key)
{
    SettingProvider& settingData = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    int32_t getValue;
    ErrCode ret = settingData.GetIntValue(key, getValue);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    value = static_cast<uint32_t>(getValue);
    return true;
}

bool ScreenSettingHelper::SetSettingDefaultDpi(uint32_t& dpi, const std::string& key)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.PutIntValue(key, dpi, false);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "put int value failed, ret=%{public}d", ret);
        return false;
    }
    return true;
}

void ScreenSettingHelper::RegisterSettingCastObserver(SettingObserver::UpdateFunc func)
{
    if (castObserver_) {
        TLOGD(WmsLogTag::DMS, "setting cast observer is registered");
        return;
    }
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    castObserver_ = castProvider.CreateObserver(SETTING_CAST_KEY, func);
    ErrCode ret = castProvider.RegisterObserver(castObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        castObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingCastObserver()
{
    if (castObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "castObserver_ is nullptr");
        return;
    }
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = castProvider.UnregisterObserver(castObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
    }
    castObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingCast(bool& enable, const std::string& key)
{
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = castProvider.GetBoolValue(key, enable);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    return true;
}

void ScreenSettingHelper::RegisterSettingRotationObserver(SettingObserver::UpdateFunc func)
{
    if (rotationObserver_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "setting rotation observer is registered");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    rotationObserver_ = settingProvider.CreateObserver(SETTING_ROTATION_KEY, func);
    ErrCode ret = settingProvider.RegisterObserver(rotationObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        rotationObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingRotationObserver()
{
    if (rotationObserver_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "rotationObserver_ is nullptr");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.UnregisterObserver(rotationObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
    }
    rotationObserver_ = nullptr;
}

void ScreenSettingHelper::SetSettingRotation(int32_t rotation)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.PutIntValue(SETTING_ROTATION_KEY, rotation, true);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return;
    }
    TLOGE(WmsLogTag::DMS, "succeed, ret:%{public}d", ret);
}

bool ScreenSettingHelper::GetSettingRotation(int32_t& rotation, const std::string& key)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetIntValue(key, rotation);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
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
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return;
    }
    TLOGE(WmsLogTag::DMS, "ssucceed, ret:%{public}d", ret);
}

bool ScreenSettingHelper::GetSettingRotationScreenID(int32_t& screenId, const std::string& key)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetIntValue(key, screenId);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return false;
    }
    TLOGE(WmsLogTag::DMS, "current rotation screen id:%{public}d", screenId);
    return true;
}

std::string ScreenSettingHelper::RemoveInvalidChar(const std::string& input)
{
    std::string resultString = "";
    for (char character : input) {
        if (std::isdigit(character) || character == ' ' || character == ',' || character == '.') {
            resultString += character;
        }
    }
    TLOGI(WmsLogTag::DMS, "process done, resultString: %{public}s", resultString.c_str());
    return resultString;
}

bool ScreenSettingHelper::SplitString(std::vector<std::string>& splitValues, const std::string& input,
    char delimiter)
{
    TLOGI(WmsLogTag::DMS, "input string: %{public}s", input.c_str());
    if (input.size() == 0) {
        TLOGE(WmsLogTag::DMS, "noting in input string");
        return false;
    }
    std::stringstream stream(input);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        splitValues.push_back(token);
    }
    if (splitValues.size() == 0) {
        TLOGE(WmsLogTag::DMS, "resolving split values failed");
        return false;
    }
    return true;
}

int32_t ScreenSettingHelper::GetDataFromString(std::vector<uint64_t>& datas, const std::string& inputString)
{
    TLOGI(WmsLogTag::DMS, "begin to resolve string, value: %{public}s", inputString.c_str());
    int32_t strLength = inputString.size();
    int32_t beginIdx = 0;
    for (int32_t currentIdx = 0; currentIdx < strLength; currentIdx++) {
        if (inputString[currentIdx] != ' ') {
            continue;
        }
        std::string dataStr = inputString.substr(beginIdx, currentIdx - beginIdx);
        if (dataStr.size() > 0) {
            uint64_t data = static_cast<uint64_t>(strtoll(dataStr.c_str(), nullptr, PARAM_NUM_TEN));
            datas.push_back(data);
            TLOGI(WmsLogTag::DMS, "resolving data success, data: %{public}d", static_cast<uint32_t>(data));
        }
        beginIdx = currentIdx + 1;
    }
    std::string dataStr = inputString.substr(beginIdx, strLength - beginIdx);
    if (dataStr.size() > 0) {
        uint64_t data = static_cast<uint64_t>(strtoll(dataStr.c_str(), nullptr, PARAM_NUM_TEN));
        datas.push_back(data);
        TLOGI(WmsLogTag::DMS, "resolving data success, data: %{public}d", static_cast<uint32_t>(data));
    }
    return datas.size();
}

bool ScreenSettingHelper::GetSettingRecoveryResolutionString(std::vector<std::string>& resolutionString,
    const std::string& key)
{
    std::string value;
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetStringValue(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "get setting recovery resolution failed, ret=%{public}d", ret);
        return false;
    }
    std::string validString = RemoveInvalidChar(value);
    bool ret1 = SplitString(resolutionString, validString);
    if (!ret1) {
        TLOGE(WmsLogTag::DMS, "resolving resolution string failed");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingRecoveryResolutionMap
    (std::map<uint64_t, std::pair<int32_t, int32_t>>& resolution)
{
    std::vector<std::string> resolutionStrings;
    bool ret = GetSettingRecoveryResolutionString(resolutionStrings);
    if (!ret) {
        TLOGE(WmsLogTag::DMS, "get resolution string failed");
        return false;
    }
    for (auto& resolutionString : resolutionStrings) {
        std::vector<uint64_t> resolutionData;
        int32_t dataSize = GetDataFromString(resolutionData, resolutionString);
        if (dataSize != EXPECT_RESOLUTION_SIZE) {
            TLOGE(WmsLogTag::DMS, "get data failed");
            continue;
        }
        uint64_t screenId = resolutionData[RESOLVED_DATA_INDEX_ZERO];
        int32_t width = static_cast<int32_t>(resolutionData[RESOLVED_DATA_INDEX_ONE]);
        int32_t height = static_cast<int32_t>(resolutionData[RESOLVED_DATA_INDEX_TWO]);
        TLOGI(WmsLogTag::DMS, "get data success, screenId: %{public}d, width: %{public}d, height: %{public}d",
            static_cast<uint32_t>(screenId), width, height);
        resolution[screenId] = std::make_pair(width, height);
    }
    if (resolution.empty()) {
        TLOGE(WmsLogTag::DMS, "no resolution found");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingScreenModeString(std::vector<std::string>& screenModeStrings,
    const std::string& key)
{
    std::string value;
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetStringValue(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "get setting screen mode failed, ret=%{public}d", ret);
        return false;
    }
    std::string validString = RemoveInvalidChar(value);
    bool ret1 = SplitString(screenModeStrings, validString);
    if (!ret1) {
        TLOGE(WmsLogTag::DMS, "resolving screen mode failed");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingScreenModeMap(std::map<uint64_t, uint32_t>& screenMode)
{
    std::vector<std::string> screenModeStrings;
    bool ret = GetSettingScreenModeString(screenModeStrings);
    if (!ret) {
        TLOGE(WmsLogTag::DMS, "get last screen mode string failed");
        return false;
    }
    for (auto& screenModeString : screenModeStrings) {
        std::vector<uint64_t> screenModeData;
        int32_t dataSize = GetDataFromString(screenModeData, screenModeString);
        if (dataSize != EXPECT_SCREEN_MODE_SIZE) {
            TLOGE(WmsLogTag::DMS, "get data failed");
            continue;
        }
        uint64_t screenId = screenModeData[RESOLVED_DATA_INDEX_ZERO];
        uint32_t mode = static_cast<int32_t>(screenModeData[RESOLVED_DATA_INDEX_ONE]);
        TLOGI(WmsLogTag::DMS, "get data success, screenId: %{public}d, mode: %{public}d",
            static_cast<uint32_t>(screenId), mode);
        screenMode[screenId] = mode;
    }
    if (screenMode.empty()) {
        TLOGE(WmsLogTag::DMS, "no last screen mode found");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingRelativePositionString(std::vector<std::string>& relativePositionStrings,
    const std::string& key)
{
    std::string value;
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetStringValue(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "get setting relative position failed, ret=%{public}d", ret);
        return false;
    }
    std::string validString = RemoveInvalidChar(value);
    bool ret1 = SplitString(relativePositionStrings, validString);
    if (!ret1) {
        TLOGE(WmsLogTag::DMS, "resolving relative position failed");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingRelativePositionMap
    (std::map<uint64_t, std::pair<uint32_t, uint32_t>>& relativePosition)
{
    std::vector<std::string> relativePositionStrings;
    bool ret = GetSettingRelativePositionString(relativePositionStrings);
    if (!ret) {
        TLOGE(WmsLogTag::DMS, "get relative position string failed");
        return false;
    }
    for (auto& relativePositionString : relativePositionStrings) {
        std::vector<uint64_t> relativePositionData;
        int32_t dataSize = GetDataFromString(relativePositionData, relativePositionString);
        if (dataSize != EXPECT_RELATIVE_POSITION_SIZE) {
            TLOGE(WmsLogTag::DMS, "get data failed");
            continue;
        }
        uint64_t screenId = relativePositionData[RESOLVED_DATA_INDEX_ZERO];
        uint32_t startX = static_cast<uint32_t>(relativePositionData[RESOLVED_DATA_INDEX_ONE]);
        uint32_t startY = static_cast<uint32_t>(relativePositionData[RESOLVED_DATA_INDEX_TWO]);
        TLOGI(WmsLogTag::DMS, "get data success, screenId: %{public}d, startX: %{public}d, startY: %{public}d",
            static_cast<uint32_t>(screenId), startX, startY);
        relativePosition[screenId] = std::make_pair(startX, startY);
    }
    if (relativePosition.empty()) {
        TLOGE(WmsLogTag::DMS, "no relative position found");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

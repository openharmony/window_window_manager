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

#include <cerrno>
#include <parameters.h>

#include "screen_session_manager/include/screen_session_manager.h"
#include "setting_provider.h"
#include "system_ability_definition.h"
#include "window_manager_hilog.h"
#include "fold_screen_state_internel.h"
#include "fold_screen_controller/super_fold_state_manager.h"

namespace OHOS {
namespace Rosen {
sptr<SettingObserver> ScreenSettingHelper::dpiObserver_;
sptr<SettingObserver> ScreenSettingHelper::castObserver_;
sptr<SettingObserver> ScreenSettingHelper::rotationObserver_;
sptr<SettingObserver> ScreenSettingHelper::wireCastObserver_;
sptr<SettingObserver> ScreenSettingHelper::extendScreenDpiObserver_;
constexpr int32_t PARAM_NUM_TEN = 10;
constexpr uint32_t EXPECT_SCREEN_MODE_SIZE = 2;
constexpr uint32_t EXPECT_RELATIVE_POSITION_SIZE = 3;
constexpr uint32_t VALID_MULTI_SCREEN_INFO_SIZE = 4;
constexpr uint32_t INDEX_SCREEN_INFO = 0;
constexpr uint32_t INDEX_SCREEN_MODE = 1;
constexpr uint32_t INDEX_FIRST_RELATIVE_POSITION = 2;
constexpr uint32_t INDEX_SECOND_RELATIVE_POSITION = 3;
constexpr uint32_t DATA_INDEX_ZERO = 0;
constexpr uint32_t DATA_INDEX_ONE = 1;
constexpr uint32_t DATA_INDEX_TWO = 2;
constexpr uint32_t SCREEN_MAIN_IN_DATA = 0;
constexpr uint32_t SCREEN_MIRROR_IN_DATA = 1;
constexpr uint32_t SCREEN_EXTEND_IN_DATA = 2;
constexpr ScreenId RS_ID_INTERNAL = 0;
constexpr uint32_t EXTEND_SCREEN_DPI_LEVEL_NEG_ONE = -1;
constexpr uint32_t EXTEND_SCREEN_DPI_LEVEL_ZERO = 0;
constexpr uint32_t EXTEND_SCREEN_DPI_LEVEL_ONE = 1;
constexpr float EXTEND_SCREEN_DPI_NEG_ONE_PARAMETER = 0.7f;
constexpr float EXTEND_SCREEN_DPI_ZERO_PARAMETER = 0.85f;
constexpr float EXTEND_SCREEN_DPI_ONE_PARAMETER = 1.00f;
const std::string SCREEN_SHAPE = system::GetParameter("const.window.screen_shape", "0:0");
constexpr int32_t INDEX_EXTEND_SCREEN_DPI_POSITION = -1;

void ScreenSettingHelper::RegisterSettingDpiObserver(SettingObserver::UpdateFunc func)
{
    if (dpiObserver_) {
        TLOGD(WmsLogTag::DMS, "setting dpi observer is registered");
        return;
    }
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    dpiObserver_ = provider.CreateObserver(SETTING_DPI_KEY, func);
    if (dpiObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
    }
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

bool ScreenSettingHelper::GetSettingValue(const std::string& key, std::string& value)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    std::string getValue = "";
    ErrCode ret = provider.GetStringValue(key, getValue);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    value = getValue;
    return true;
}

bool ScreenSettingHelper::SetSettingValue(const std::string& key, uint32_t value)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.PutIntValue(key, value, false);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return false;
    }
    return true;
}

bool ScreenSettingHelper::SetSettingValue(const std::string& key, const std::string& value)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.PutStringValue(key, value, false);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return false;
    }
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
    if (castObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
    }
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
    if (rotationObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
    }
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
    TLOGE(WmsLogTag::DMS, "succeed, ret:%{public}d", ret);
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
    TLOGW(WmsLogTag::DMS, "input string: %{public}s", input.c_str());
    std::string resultString = "";
    for (char character : input) {
        if (std::isdigit(character) || character == ' ' || character == ',' ||
            character == '.' || character == ';' || character == '_') {
            resultString += character;
        }
    }
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
    std::string token = "";
    while (std::getline(stream, token, delimiter)) {
        splitValues.push_back(token);
    }
    if (splitValues.size() == 0) {
        TLOGE(WmsLogTag::DMS, "failed, noting split");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::IsNumber(const std::string& str)
{
    int32_t length = static_cast<int32_t>(str.size());
    int32_t start = 0;
    while (start < length && isspace(str[start])) {
        start++;
    }
    bool hasDot = false;
    bool hasDigit = false;
    if (start < length && str[start] == '-') {
        start++;
    }
    for (int32_t i = start; i < length; ++i) {
        if (str[i] == '.') {
            if (hasDot) {
                return false;
            }
            hasDot = true;
        } else if (isdigit(str[i])) {
            hasDigit = true;
        } else {
            return false;
        }
    }
    return hasDigit;
}

bool ScreenSettingHelper::ConvertStrToUint64(const std::string& str, uint64_t& num)
{
    char *endFlag;
    errno = 0;
    uint64_t tmp = static_cast<uint64_t>(std::strtoull(str.c_str(), &endFlag, BASE_TEN));
    if (errno == ERANGE) {
        return false;
    } else if (endFlag == nullptr || *endFlag != '\0') {
        return false;
    }
    num = tmp;
    return true;
}

std::map<std::string, MultiScreenInfo> ScreenSettingHelper::GetMultiScreenInfo(const std::string& key)
{
    std::map<std::string, MultiScreenInfo> multiScreenInfoMap = {};
    std::string value = "";
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetStringValueMultiUser(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return multiScreenInfoMap;
    }
    std::string validString = RemoveInvalidChar(value);
    std::vector<std::string> restoredScreen = {};
    bool split = SplitString(restoredScreen, validString, ',');
    if (!split) {
        TLOGE(WmsLogTag::DMS, "split screen failed");
        return multiScreenInfoMap;
    }
    for (auto infoString : restoredScreen) {
        std::vector<std::string> infoVector = {};
        split = SplitString(infoVector, infoString, ';');
        if (!split || infoVector.size() != VALID_MULTI_SCREEN_INFO_SIZE) {
            TLOGE(WmsLogTag::DMS, "split info failed");
            continue;
        }
        MultiScreenInfo info = {};
        if (!GetScreenMode(info, infoVector[INDEX_SCREEN_MODE])) {
            continue;
        }
        if (!GetScreenRelativePosition(info, infoVector[INDEX_FIRST_RELATIVE_POSITION])) {
            continue;
        }
        if (!GetScreenRelativePosition(info, infoVector[INDEX_SECOND_RELATIVE_POSITION])) {
            continue;
        }
        if (info.mainScreenOption.screenId_ == info.secondaryScreenOption.screenId_) {
            TLOGE(WmsLogTag::DMS, "invalid screen of relative position!");
            continue;
        }
        multiScreenInfoMap[infoVector[INDEX_SCREEN_INFO]] = info;
    }
    return multiScreenInfoMap;
}

bool ScreenSettingHelper::GetScreenMode(MultiScreenInfo& info, const std::string& inputString)
{
    std::vector<std::string> screenMode = {};
    bool split = SplitString(screenMode, inputString, ' ');
    uint32_t dataSize = screenMode.size();
    if (!split || dataSize != EXPECT_SCREEN_MODE_SIZE) {
        TLOGE(WmsLogTag::DMS, "split failed, data size: %{public}d", dataSize);
        return false;
    }

    uint32_t mode = 0;
    if (!IsNumber(screenMode[DATA_INDEX_ZERO])) {
        TLOGE(WmsLogTag::DMS, "not number");
        return false;
    } else {
        mode = static_cast<uint32_t>(strtoll(screenMode[DATA_INDEX_ZERO].c_str(), nullptr, PARAM_NUM_TEN));
        TLOGW(WmsLogTag::DMS, "internal screen mode: %{public}d", mode);
        if (!UpdateScreenMode(info, mode, false)) {
            return false;
        }
    }
    if (!IsNumber(screenMode[DATA_INDEX_ONE])) {
        TLOGE(WmsLogTag::DMS, "not number");
        return false;
    } else {
        mode = static_cast<uint32_t>(strtoll(screenMode[DATA_INDEX_ONE].c_str(), nullptr, PARAM_NUM_TEN));
#ifdef FOLD_ABILITY_ENABLE
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice() &&
        SuperFoldStateManager::GetInstance().GetCurrentStatus() != SuperFoldStatus::EXPANDED) {
        mode = SCREEN_MIRROR_IN_DATA;
    }
#endif
        TLOGW(WmsLogTag::DMS, "external screen mode: %{public}d", mode);
        if (!UpdateScreenMode(info, mode, true)) {
            return false;
        }
    }
    return true;
}

bool ScreenSettingHelper::UpdateScreenMode(MultiScreenInfo& info, uint32_t mode, bool isExternal)
{
    if (mode == SCREEN_MAIN_IN_DATA) {
        info.isExtendMain = isExternal;
    } else if (mode == SCREEN_MIRROR_IN_DATA) {
        info.multiScreenMode = MultiScreenMode::SCREEN_MIRROR;
    } else if (mode == SCREEN_EXTEND_IN_DATA) {
        info.multiScreenMode = MultiScreenMode::SCREEN_EXTEND;
    } else {
        TLOGE(WmsLogTag::DMS, "invalid mode!");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetScreenRelativePosition(MultiScreenInfo& info, const std::string& inputString)
{
    std::vector<std::string> relativePosition = {};
    bool split = SplitString(relativePosition, inputString, ' ');
    uint32_t dataSize = relativePosition.size();
    if (!split || dataSize != EXPECT_RELATIVE_POSITION_SIZE) {
        TLOGE(WmsLogTag::DMS, "split failed, data size: %{public}d", dataSize);
        return false;
    }

    ScreenId screenId = SCREEN_ID_INVALID;
    uint32_t startX = 0;
    uint32_t startY = 0;
    if (!IsNumber(relativePosition[DATA_INDEX_ZERO])) {
        TLOGE(WmsLogTag::DMS, "not number");
        return false;
    } else {
        screenId = static_cast<ScreenId>(strtoll(relativePosition[DATA_INDEX_ZERO].c_str(), nullptr, PARAM_NUM_TEN));
    }
    if (!IsNumber(relativePosition[DATA_INDEX_ONE])) {
        TLOGE(WmsLogTag::DMS, "not number");
        return false;
    } else {
        startX = static_cast<uint32_t>(strtoll(relativePosition[DATA_INDEX_ONE].c_str(), nullptr, PARAM_NUM_TEN));
    }
    if (!IsNumber(relativePosition[DATA_INDEX_TWO])) {
        TLOGE(WmsLogTag::DMS, "not number");
        return false;
    } else {
        startY = static_cast<uint32_t>(strtoll(relativePosition[DATA_INDEX_TWO].c_str(), nullptr, PARAM_NUM_TEN));
    }
    TLOGW(WmsLogTag::DMS, "screenId: %{public}" PRIu64 ", startX: %{public}d, startY: %{public}d",
        screenId, startX, startY);

    if ((RS_ID_INTERNAL == screenId && !info.isExtendMain) || (RS_ID_INTERNAL != screenId && info.isExtendMain)) {
        TLOGI(WmsLogTag::DMS, "find main screen option");
        info.mainScreenOption.screenId_ = screenId;
        info.mainScreenOption.startX_ = startX;
        info.mainScreenOption.startY_ = startY;
    } else {
        TLOGI(WmsLogTag::DMS, "find secondary screen option");
        info.secondaryScreenOption.screenId_ = screenId;
        info.secondaryScreenOption.startX_ = startX;
        info.secondaryScreenOption.startY_ = startY;
    }
    return true;
}

ScreenShape ScreenSettingHelper::GetScreenShape(ScreenId screenId)
{
    std::istringstream iss(SCREEN_SHAPE);
    std::string id;
    std::string shape;
    while (std::getline(iss, id, ':')) {
        std::getline(iss, shape, ';');
        if (!IsNumber(id)) {
            TLOGI(WmsLogTag::DMS, "id is invalid");
            continue;
        }
        if (screenId == static_cast<ScreenId>(std::stoi(id))) {
            if (!IsNumber(shape)) {
                TLOGE(WmsLogTag::DMS, "shape is invalid");
                return ScreenShape::RECTANGLE;
            }
            return static_cast<ScreenShape>(std::stoi(shape));
        }
    }
    TLOGI(WmsLogTag::DMS, "Can not find screen shape info. ccm:%{public}s", SCREEN_SHAPE.c_str());
    return ScreenShape::RECTANGLE;
}

void ScreenSettingHelper::RegisterSettingWireCastObserver(SettingObserver::UpdateFunc func)
{
    if (wireCastObserver_) {
        TLOGD(WmsLogTag::DMS, "setting wire cast observer is registered");
        return;
    }
    SettingProvider& wireCastProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    wireCastObserver_ = wireCastProvider.CreateObserver(SETTING_CAST_KEY, func);
    if (wireCastObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
        return;
    }
    ErrCode ret = wireCastProvider.RegisterObserver(wireCastObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        wireCastObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingWireCastObserver()
{
    if (wireCastObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "wireCastObserver_ is nullptr");
        return;
    }
    SettingProvider& wireCastProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = wireCastProvider.UnregisterObserver(wireCastObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
    }
    wireCastObserver_ = nullptr;
}

void ScreenSettingHelper::RegisterSettingExtendScreenDpiObserver(SettingObserver::UpdateFunc func)
{
    if (extendScreenDpiObserver_ != nullptr) {
        TLOGD(WmsLogTag::DMS, "setting extend dpi observer is registered");
        return;
    }
    SettingProvider& extendScreenProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    extendScreenDpiObserver_ = extendScreenProvider.CreateObserver(SETTING_EXTEND_DPI_KEY, func);
    if (extendScreenDpiObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
        return;
    }
    ErrCode ret = extendScreenProvider.RegisterObserver(extendScreenDpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        extendScreenDpiObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnRegisterSettingExtendScreenDpiObserver()
{
    if (extendScreenDpiObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "extendScreenDpiObserver_ is nullptr");
        return;
    }
    SettingProvider& extendScreenProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = extendScreenProvider.UnregisterObserver(extendScreenDpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
    }
    extendScreenDpiObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingExtendScreenDpi(float& coef, const std::string& key)
{
    SettingProvider& extendScreenProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    int32_t value = INDEX_EXTEND_SCREEN_DPI_POSITION;
    ErrCode ret = extendScreenProvider.GetIntValue(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    TLOGI(WmsLogTag::DMS, "get setting extend dpi is %{public}d", value);
    switch (value) {
        case EXTEND_SCREEN_DPI_LEVEL_NEG_ONE:
            coef = EXTEND_SCREEN_DPI_NEG_ONE_PARAMETER;
            break;
        case EXTEND_SCREEN_DPI_LEVEL_ZERO:
            coef = EXTEND_SCREEN_DPI_ZERO_PARAMETER;
            break;
        case EXTEND_SCREEN_DPI_LEVEL_ONE:
            coef = EXTEND_SCREEN_DPI_ONE_PARAMETER;
            break;
        default:
            coef = EXTEND_SCREEN_DPI_ONE_PARAMETER;
            break;
    }
    return true;
}

bool ScreenSettingHelper::ConvertStrToInt32(const std::string& str, int32_t& num)
{
    auto res = std::from_chars(str.data(), str.data() + str.size(), num);
    if (res.ec != std::errc()) {
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

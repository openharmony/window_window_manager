/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef MOCK_RESOURCE_MANAGER_H
#define MOCK_RESOURCE_MANAGER_H

#include <gmock/gmock.h>
#include "session_manager/include/scene_session_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
ResourceManager::~ResourceManager() {};
class ResourceManagerMocker : public Resource::ResourceManager {
public:
    ResourceManagerMocker() {};
    ~ResourceManagerMocker() {};

    MOCK_METHOD2(GetColorById, RState(uint32_t id, uint32_t& outValue));
    MOCK_METHOD3(GetMediaById, RState(uint32_t id, std::string& outValue, uint32_t density));
    MOCK_METHOD2(AddResource, bool(const char* path, const uint32_t& selectedTypes));
    MOCK_METHOD1(GetResConfig, void(ResConfig& resconfig));
    MOCK_METHOD2(GetStringById, RState(uint32_t id, std::string& outValue));
    MOCK_METHOD2(GetStringByName, RState(const char* name, std::string& outValue));
    MOCK_METHOD2(GetStringArrayById, RState(uint32_t id, std::vector<std::string>& outValue));
    MOCK_METHOD2(GetStringArrayByName, RState(const char* name, std::vector<std::string>& outValue));
    MOCK_METHOD2(GetPatternById, RState(uint32_t id, std::map<std::string, std::string>& outValue));
    MOCK_METHOD2(GetPatternByName, RState(const char* name, std::map<std::string, std::string>& outValue));
    MOCK_METHOD3(GetPluralStringById, RState(uint32_t id, int quantity, std::string& outValue));
    MOCK_METHOD3(GetPluralStringByName, RState(const char* name, int quantity, std::string& outValue));
    MOCK_METHOD2(GetThemeById, RState(uint32_t id, std::map<std::string, std::string>& outValue));
    MOCK_METHOD2(GetThemeByName, RState(const char* name, std::map<std::string, std::string>& outValue));
    MOCK_METHOD2(UpdateResConfig, RState(ResConfig& resConfig, bool isUpdateTheme));
    RState GetStringFormatById(std::string& outValue, uint32_t id, ...) { return RState::ERROR; };
    RState GetStringFormatByName(std::string& outValue, const char* name, ...) { return RState::ERROR; };
    RState GetPluralStringByIdFormat(std::string& outValue, uint32_t id, int quantity, ...) { return RState::ERROR; };
    RState GetPluralStringByNameFormat(std::string& outValue, const char* name, int quantity, ...)
    {
        return RState::ERROR;
    };
    RState GetBooleanById(uint32_t id, bool& outValue) { return RState::ERROR; };
    RState GetBooleanByName(const char* name, bool& outValue) { return RState::ERROR; };
    RState GetIntegerById(uint32_t id, int& outValue) { return RState::ERROR; };
    RState GetIntegerByName(const char* name, int& outValue) { return RState::ERROR; };
    RState GetFloatById(uint32_t id, float& outValue) { return RState::ERROR; };
    RState GetFloatById(uint32_t id, float& outValue, std::string& unit) { return RState::ERROR; };
    RState GetFloatByName(const char* name, float& outValue) { return RState::ERROR; };
    RState GetFloatByName(const char* name, float& outValue, std::string& unit) { return RState::ERROR; };
    RState GetIntArrayById(uint32_t id, std::vector<int>& outValue) { return RState::ERROR; };
    RState GetIntArrayByName(const char* name, std::vector<int>& outValue) { return RState::ERROR; };
    RState GetColorByName(const char* name, uint32_t& outValue) { return RState::ERROR; };
    RState GetProfileById(uint32_t id, std::string& outValue) { return RState::ERROR; };
    RState GetProfileByName(const char* name, std::string& outValue) { return RState::ERROR; };
    RState GetMediaByName(const char* name, std::string& outValue, uint32_t density) { return RState::ERROR; };
    RState GetRawFilePathByName(const std::string& name, std::string& outValue) { return RState::ERROR; };
    RState GetRawFileDescriptor(const std::string& name, RawFileDescriptor& descriptor) { return RState::ERROR; };
    RState CloseRawFileDescriptor(const std::string& name) { return RState::ERROR; };
    RState GetMediaDataById(uint32_t id, size_t& len, std::unique_ptr<uint8_t[]>& outValue, uint32_t density)
    {
        return RState::ERROR;
    };
    RState GetMediaDataByName(const char* name, size_t& len, std::unique_ptr<uint8_t[]>& outValue, uint32_t density)
    {
        return RState::ERROR;
    };
    RState GetMediaBase64DataById(uint32_t id, std::string& outValue, uint32_t density) { return RState::ERROR; };
    RState GetMediaBase64DataByName(const char* name, std::string& outValue, uint32_t density)
    {
        return RState::ERROR;
    };
    RState GetProfileDataById(uint32_t id, size_t& len, std::unique_ptr<uint8_t[]>& outValue) { return RState::ERROR; };
    RState GetProfileDataByName(const char* name, size_t& len, std::unique_ptr<uint8_t[]>& outValue)
    {
        return RState::ERROR;
    };
    RState GetRawFileFromHap(const std::string& rawFileName, size_t& len, std::unique_ptr<uint8_t[]>& outValue)
    {
        return RState::ERROR;
    };
    RState GetRawFileDescriptorFromHap(const std::string& rawFileName, RawFileDescriptor& descriptor)
    {
        return RState::ERROR;
    };
    RState IsLoadHap(std::string& hapPath) { return RState::ERROR; };
    RState GetRawFileList(const std::string& rawDirPath, std::vector<std::string>& rawfileList)
    {
        return RState::ERROR;
    };
    RState GetDrawableInfoById(uint32_t id, std::string& type, size_t& len,
        std::unique_ptr<uint8_t[]>& outValue, uint32_t density)
    {
        return RState::ERROR;
    };
    RState GetDrawableInfoByName(const char* name, std::string& type, size_t& len,
        std::unique_ptr<uint8_t[]>& outValue, uint32_t density)
    {
        return RState::ERROR;
    };
    bool AddResource(const std::string& path, const std::vector<std::string>& overlayPaths)
    {
        return true;
    };
    bool RemoveResource(const std::string& path, const std::vector<std::string>& overlayPaths)
    {
        return true;
    };
    RState GetStringFormatById(uint32_t id, std::string& outValue,
        std::vector<std::tuple<NapiValueType, std::string>>& jsParams)
    {
        return RState::ERROR;
    };
    RState GetStringFormatByName(const char* name, std::string& outValue,
        std::vector<std::tuple<NapiValueType, std::string>>& jsParams)
    {
        return RState::ERROR;
    };
    uint32_t GetResourceLimitKeys() { return 0; };
    bool AddAppOverlay(const std::string& path) { return true; };
    bool RemoveAppOverlay(const std::string& path) { return true; };
    RState GetRawFdNdkFromHap(const std::string& rawFileName, RawFileDescriptor& descriptor)
    {
        return RState::ERROR;
    };
    RState GetResId(const std::string& resTypeName, uint32_t& resId) { return RState::ERROR; };
    void GetLocales(std::vector<std::string>& outValue, bool includeSystem = false) {};
    RState GetDrawableInfoById(uint32_t id, std::tuple<std::string, size_t, std::string>& drawableInfo,
        std::unique_ptr<uint8_t[]>& outValue, uint32_t iconType, uint32_t density)
    {
        return RState::ERROR;
    };
    RState GetDrawableInfoByName(const char* name, std::tuple<std::string, size_t, std::string>& drawableInfo,
        std::unique_ptr<uint8_t[]>& outValue, uint32_t iconType, uint32_t density)
    {
        return RState::ERROR;
    };
    RState GetSymbolById(uint32_t id, uint32_t& outValue) { return RState::ERROR; };
    RState GetSymbolByName(const char* name, uint32_t& outValue) { return RState::ERROR; };
    RState GetThemeIcons(uint32_t resId, std::pair<std::unique_ptr<uint8_t[]>, size_t>& foregroundInfo,
        std::pair<std::unique_ptr<uint8_t[]>, size_t>& backgroundInfo, uint32_t density,
        const std::string& abilityName)
    {
        return RState::ERROR;
    };
    std::string GetThemeMask() { return ""; };
    bool HasIconInTheme(const std::string& bundleName) { return true; };
    RState GetOtherIconsInfo(const std::string & iconName, std::unique_ptr<uint8_t[]>& outValue,
        size_t& len, bool isGlobalMask)
    {
        return RState::ERROR;
    };
    RState IsRawDirFromHap(const std::string& pathName, bool& outValue)
    {
        return RState::ERROR;
    };
    std::shared_ptr<ResourceManager> GetOverrideResourceManager(std::shared_ptr<ResConfig> overrideResConfig)
    {
        return std::shared_ptr<ResourceManager>();
    };
    RState UpdateOverrideResConfig(ResConfig& resConfig) { return RState::ERROR; };
    void GetOverrideResConfig(ResConfig& resConfig) {};
    RState GetDynamicIcon(const std::string& resName, std::pair<std::unique_ptr<uint8_t[]>, size_t>& iconInfo,
        uint32_t density)
    {
        return RState::ERROR;
    };
    RState GetStringFormatById(std::string& outValue, uint32_t id, va_list args) { return RState::ERROR; };
    RState GetStringFormatByName(std::string& outValue, const char* name, va_list args) { return RState::ERROR; };
    RState GetFormatPluralStringById(std::string& outValue, uint32_t id, int quantity,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>>& jsParams)
    {
        return RState::ERROR;
    };
    RState GetFormatPluralStringByName(std::string& outValue, const char* name, int quantity,
        std::vector<std::tuple<ResourceManager::NapiValueType, std::string>>& jsParams)
    {
        return RState::ERROR;
    };
    bool AddPatchResource(const char* path, const char* patchPath) { return true; };
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif

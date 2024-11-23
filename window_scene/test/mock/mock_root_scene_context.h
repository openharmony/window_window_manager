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

#ifndef MOCK_ROOT_SCENE_CONTEXT_H
#define MOCK_ROOT_SCENE_CONTEXT_H

#include <gmock/gmock.h>
#include "session_manager/include/scene_session_manager.h"

namespace OHOS {
namespace AbilityRuntime {
const size_t Context::CONTEXT_TYPE_ID(std::hash<const char*> {} ("MockContext"));
class RootSceneContextMocker : public AbilityRuntime::Context {
public:
    RootSceneContextMocker() {};
    ~RootSceneContextMocker() {};

    MOCK_CONST_METHOD0(GetResourceManager, std::shared_ptr<Global::Resource::ResourceManager>());
    MOCK_CONST_METHOD0(GetBundleName, std::string());
    MOCK_METHOD1(CreateBundleContext, std::shared_ptr<Context>(const std::string& bundleName));
    MOCK_CONST_METHOD0(GetApplicationInfo, std::shared_ptr<AppExecFwk::ApplicationInfo>());
    MOCK_CONST_METHOD0(GetBundleCodePath, std::string());
    MOCK_CONST_METHOD0(GetHapModuleInfo, std::shared_ptr<AppExecFwk::HapModuleInfo>());
    MOCK_METHOD0(GetBundleCodeDir, std::string());
    MOCK_METHOD0(GetCacheDir, std::string());
    MOCK_METHOD0(GetTempDir, std::string());
    MOCK_METHOD0(GetFilesDir, std::string());
    MOCK_METHOD0(GetResourceDir, std::string());
    MOCK_METHOD0(GetDatabaseDir, std::string());
    MOCK_METHOD0(GetPreferencesDir, std::string());
    MOCK_METHOD0(IsUpdatingConfigurations, bool());
    MOCK_METHOD0(PrintDrawnCompleted, bool());
    MOCK_METHOD3(GetSystemDatabaseDir, int32_t(const std::string& groupId, bool checkExist,
        std::string& databaseDir));
    MOCK_METHOD3(GetSystemPreferencesDir, int32_t(const std::string& groupId, bool checkExist,
        std::string& preferencesDir));
    MOCK_METHOD1(GetGroupDir, std::string(std::string groupId));
    MOCK_METHOD0(GetDistributedFilesDir, std::string());
    MOCK_METHOD0(GetCloudFileDir, std::string());
    MOCK_METHOD0(GetToken, sptr<IRemoteObject>());
    MOCK_METHOD1(SetToken, void(const sptr<IRemoteObject>& token));
    MOCK_METHOD1(SwitchArea, void(int mode));
    MOCK_METHOD1(CreateModuleContext, std::shared_ptr<Context>(const std::string& moduleName));
    MOCK_METHOD2(CreateModuleContext, std::shared_ptr<Context>(const std::string& bundleName,
        const std::string& moduleName));
    MOCK_METHOD2(CreateModuleResourceManager, std::shared_ptr<Global::Resource::ResourceManager>
        (const std::string& bundleName, const std::string& moduleName));
    MOCK_METHOD3(CreateSystemHspModuleResourceManager, int32_t(const std::string& bundleName,
        const std::string& moduleName, std::shared_ptr<Global::Resource::ResourceManager>& ResourceManager));
    MOCK_METHOD0(GetArea, int());
    MOCK_CONST_METHOD0(GetConfiguration, std::shared_ptr<AppExecFwk::Configuration>());
    MOCK_CONST_METHOD0(GetBaseDir, std::string());
    MOCK_CONST_METHOD0(GetDeviceType, Global::Resource::DeviceType());
    MOCK_METHOD1(CreateAreaModeContext, std::shared_ptr<Context>(int areaMode));
};
} // namespace AbilityRuntime
} // namespace OHOS
#endif

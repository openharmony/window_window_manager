/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ATOMICSERVICE_BASIC_ENGINE_PLUGIN
#define ATOMICSERVICE_BASIC_ENGINE_PLUGIN

#include <dlfcn.h>
#include <mutex>
#include <string>

#include "parcel.h"

namespace OHOS::Rosen {
#ifdef WINDOW_ATOMIC_SERVICE_ATTRIBUTION_ENABLE
class AtomicserviceIconInfo : public virtual Parcelable {
public:
    AtomicserviceIconInfo();
    virtual ~AtomicserviceIconInfo() = default;

    // The following functions are used to get data
    std::string GetBundleName() const;
    std::string GetModuleName() const;
    std::string GetAbilityName() const;
    std::string GetCircleIcon() const;
    std::string GetEyelashRingIcon() const;
    std::string GetAppName() const;
    std::vector<std::string> GetDeviceTypes() const;
    int32_t GetResizable() const;
    std::vector<std::string> GetSupportWindowMode() const;

    // The following functions are used to set data
    void SetBundleName(const std::string& val);
    void SetModuleName(const std::string& val);
    void SetAbilityName(const std::string& val);
    void SetCircleIcon(const std::string& val);
    void SetEyelashRingIcon(const std::string& val);
    void SetAppName(const std::string& val);
    void SetDeviceTypes(const std::vector<std::string>& val);
    void SetResizable(int32_t val);
    void SetSupportWindowMode(const std::vector<std::string>& val);

    // The following functions are used for serialization and deserialization
    bool Marshalling(Parcel& parcel) const override;
    static AtomicserviceIconInfo* Unmarshalling(Parcel& parcel);

private:
    // Member variable
    std::string bundleName_;
    std::string moduleName_;
    std::string abilityName_;
    std::string circleIcon_;
    std::string eyelashRingIcon_;
    std::string appName_;
    std::vector<std::string> deviceTypes_;
    int32_t resizable_;
    std::vector<std::string> supportWindowMode_;
};

class AtomicServiceBasicEnginePlugin {
public:
    static AtomicServiceBasicEnginePlugin& GetInstance();
    int32_t ReleaseData();
    AtomicserviceIconInfo* GetParamsFromAtomicServiceBasicEngine(const std::string& bundleName);

private:
    AtomicServiceBasicEnginePlugin();
    ~AtomicServiceBasicEnginePlugin();

    typedef int32_t GetAtomicserviceIconInfoPlugin(
        std::string bundleName, AtomicserviceIconInfo** atomicserviceIconInfo);
    typedef void FreeData(AtomicserviceIconInfo* data);
    
    std::mutex mutex_;
    void* atomicServiceBasicEngine_ = nullptr;
    AtomicserviceIconInfo* atomicserviceIconInfo_ = nullptr;
    GetAtomicserviceIconInfoPlugin* getAtomicserviceIconInfoPlugin_ = nullptr;
    FreeData* freeData_ = nullptr;
}; // AtomicServiceBasicEnginePlugin
#endif // WINDOW_ATOMIC_SERVICE_ATTRIBUTION_ENABLE
} // namespace OHOS::Rosen
#endif // ATOMICSERVICE_BASIC_ENGINE_PLUGIN

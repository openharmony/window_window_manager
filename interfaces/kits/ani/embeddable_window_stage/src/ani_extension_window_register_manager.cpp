/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "ani.h"
#include "ani_extension_window_listener.h"
#include "ani_extension_window_register_manager.h"
#include "singleton_container.h"
#include "window_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string WINDOW_SIZE_CHANGE_CB = "windowSizeChange";
const std::string AVOID_AREA_CHANGE_CB = "avoidAreaChange";
const std::string WINDOW_STAGE_EVENT_CB = "windowStageEvent";
const std::string WINDOW_EVENT_CB = "windowEvent";
}

AniExtensionWindowRegisterManager::AniExtensionWindowRegisterManager()
{
    // white register list for window
    listenerCodeMap_[CaseType::CASE_WINDOW] = {
        {WINDOW_SIZE_CHANGE_CB, ListenerType::WINDOW_SIZE_CHANGE_CB},
        {AVOID_AREA_CHANGE_CB, ListenerType::AVOID_AREA_CHANGE_CB},
        {WINDOW_EVENT_CB, ListenerType::WINDOW_EVENT_CB},
    };
    // white register list for window stage
    listenerCodeMap_[CaseType::CASE_STAGE] = {
        {WINDOW_STAGE_EVENT_CB, ListenerType::WINDOW_STAGE_EVENT_CB}
    };
}

AniExtensionWindowRegisterManager::~AniExtensionWindowRegisterManager()
{
}

WMError AniExtensionWindowRegisterManager::ProcessWindowChangeRegister(sptr<AniExtensionWindowListener> listener,
    sptr<Window> window, bool isRegister)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[NAPI]Window is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    sptr<IWindowChangeListener> thisListener(listener);
    if (isRegister) {
        return window->RegisterWindowChangeListener(thisListener);
    } else {
        return window->UnregisterWindowChangeListener(thisListener);
    }
}

WMError AniExtensionWindowRegisterManager::ProcessAvoidAreaChangeRegister(sptr<AniExtensionWindowListener> listener,
    sptr<Window> window, bool isRegister)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[NAPI]Window is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    sptr<IAvoidAreaChangedListener> thisListener(listener);
    if (isRegister) {
        return window->RegisterAvoidAreaChangeListener(thisListener);
    } else {
        return window->UnregisterAvoidAreaChangeListener(thisListener);
    }
}

WMError AniExtensionWindowRegisterManager::ProcessLifeCycleEventRegister(sptr<AniExtensionWindowListener> listener,
    sptr<Window> window, bool isRegister)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Window is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    sptr<IWindowLifeCycle> thisListener(listener);
    if (isRegister) {
        return window->RegisterLifeCycleListener(thisListener);
    } else {
        return window->UnregisterLifeCycleListener(thisListener);
    }
}

bool AniExtensionWindowRegisterManager::IsCallbackRegistered(ani_env* env, const std::string& type, ani_object fn)
{
    if (aniCbMap_.empty() || aniCbMap_.find(type) == aniCbMap_.end()) {
        TLOGI(WmsLogTag::WMS_UIEXT, "[ANI]Method %{public}s has not been registered", type.c_str());
        return false;
    }

    for (auto iter = aniCbMap_[type].begin(); iter != aniCbMap_[type].end(); ++iter) {
        sptr<AniExtensionWindowListener> oldListener= iter->second;
        bool registed = oldListener->IsSameRef(fn);
        if (registed) {
            TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

WMError AniExtensionWindowRegisterManager::RegisterListener(sptr<Window> window, const std::string& type,
    CaseType caseType, ani_env* env, ani_object fn, ani_object fnArg)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (IsCallbackRegistered(env, type, fn)) {
        return WMError::WM_DO_NOTHING;
    }
    if (listenerCodeMap_[caseType].count(type) == 0) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]Type %{public}s is not supported", type.c_str());
        return WMError::WM_DO_NOTHING;
    }
    ani_status ret {};
    ani_ref fnRef {};
    if ((ret = env->GlobalReference_Create(fn, &fnRef)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]create global ref fail");
        return WMError::WM_DO_NOTHING;
    };
    ani_ref fnArgRef {};
    if ((ret = env->GlobalReference_Create(fnArg, &fnArgRef)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]create global ref fail");
        return WMError::WM_DO_NOTHING;
    };
    sptr<AniExtensionWindowListener> extensionWindowListener =
        sptr<AniExtensionWindowListener>::MakeSptr(env, fnRef, fnArgRef);
    extensionWindowListener->SetMainEventHandler();
    WMError retCode = ProcessRegister(caseType, extensionWindowListener, window, type, true);
    if (retCode != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]Register type %{public}s failed", type.c_str());
        return retCode;
    }
    aniCbMap_[type][fnRef] = extensionWindowListener;
    TLOGI(WmsLogTag::WMS_UIEXT, "[ANI]Register type %{public}s success! callback map size: %{public}zu", type.c_str(),
          aniCbMap_[type].size());
    return WMError::WM_OK;
}

WMError AniExtensionWindowRegisterManager::UnregisterListener(sptr<Window> window, std::string type,
    CaseType caseType, ani_env* env, ani_object fn)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (aniCbMap_.empty() || aniCbMap_.find(type) == aniCbMap_.end()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[NAPI]Type %{public}s was not registered", type.c_str());
        return WMError::WM_DO_NOTHING;
    }
    if (listenerCodeMap_[caseType].count(type) == 0) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[NAPI]Type %{public}s is not supported", type.c_str());
        return WMError::WM_DO_NOTHING;
    }
    if (fn == nullptr) {
        for (auto it = aniCbMap_[type].begin(); it != aniCbMap_[type].end();) {
            WMError ret = ProcessRegister(caseType, it->second, window, type, false);
            if (ret != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]Unregister type %{public}s failed, no value", type.c_str());
                return ret;
            }
            aniCbMap_[type].erase(it++);
        }
    } else {
        bool findFlag = false;
        for (auto it = aniCbMap_[type].begin(); it != aniCbMap_[type].end(); ++it) {
            sptr<AniExtensionWindowListener> oldListener= it->second;
            bool isSame = oldListener->IsSameRef(fn);
            if (!isSame) {
                continue;
            }
            findFlag = true;
            WMError ret = ProcessRegister(caseType, it->second, window, type, false);
            if (ret != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]Unregister type %{public}s failed", type.c_str());
                return ret;
            }
            aniCbMap_[type].erase(it);
            break;
        }
        if (!findFlag) {
            TLOGE(WmsLogTag::WMS_UIEXT,
                "[NAPI]Unregister type %{public}s failed because not found callback!", type.c_str());
            return WMError::WM_DO_NOTHING;
        }
    }
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]Unregister type %{public}s success! callback map size: %{public}zu",
        type.c_str(), aniCbMap_[type].size());
    // erase type when there is no callback in one type
    if (aniCbMap_[type].empty()) {
        aniCbMap_.erase(type);
    }
    return WMError::WM_OK;
}

WMError AniExtensionWindowRegisterManager::ProcessRegister(CaseType caseType,
    const sptr<AniExtensionWindowListener>& listener, const sptr<Window>& window, const std::string& type,
    bool isRegister)
{
    WMError ret = WMError::WM_OK;
    if (caseType == CaseType::CASE_WINDOW) {
        switch (listenerCodeMap_[caseType][type]) {
            case ListenerType::WINDOW_SIZE_CHANGE_CB:
                ret = ProcessWindowChangeRegister(listener, window, isRegister);
                break;
            case ListenerType::AVOID_AREA_CHANGE_CB:
                ret = ProcessAvoidAreaChangeRegister(listener, window, isRegister);
                break;
            case ListenerType::WINDOW_EVENT_CB:
                ret = ProcessLifeCycleEventRegister(listener, window, isRegister);
                break;
            default:
                break;
        }
    } else if (caseType == CaseType::CASE_STAGE) {
        if (type == WINDOW_STAGE_EVENT_CB) {
            ret = ProcessLifeCycleEventRegister(listener, window, isRegister);
        }
    }
    return ret;
}

} // namespace Rosen
} // namespace OHOS
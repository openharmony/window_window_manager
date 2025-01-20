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

#ifndef WINDOW_REGISTER_MANAGER_H
#define WINDOW_REGISTER_MANAGER_H

#include <map>
#include <mutex>
#include <shared_mutex>
#include "window_listener.h"

namespace OHOS {
namespace Rosen {

struct RegListenerInfo {
    bool isRegister;
    int64_t parameter;

    RegListenerInfo() : isRegister(true), parameter(0) {}
    RegListenerInfo(bool reg, int64_t param) : isRegister(reg), parameter(param) {}
    RegListenerInfo(const RegListenerInfo& other) : isRegister(other.isRegister), parameter(other.parameter) {}

    RegListenerInfo& operator=(const RegListenerInfo& other)
    {
        if (this != &other) {
            isRegister = other.isRegister;
            parameter = other.parameter;
        }
        return *this;
    }
};

class CjWindowRegisterManager {
public:
    CjWindowRegisterManager();
    ~CjWindowRegisterManager() {}
    WmErrorCode RegisterListener(sptr<Window>, std::string type,
        CaseType caseType, int64_t callbackObject, int64_t parameter);
    WmErrorCode UnregisterListener(sptr<Window>, std::string type,
        CaseType caseType, int64_t callbackObject);
private:
    void InitWindowManagerListeners();
    void InitWindowListeners();
    void InitStageListeners();
    bool IsCallbackRegistered(std::string type, int64_t callbackObject);
    WmErrorCode ProcessWindowChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessSystemAvoidAreaChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessAvoidAreaChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessLifeCycleEventRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessOccupiedAreaChangeRegister(sptr<CjWindowListener> listener,
        sptr<Window> window, const RegListenerInfo& info);
    WmErrorCode ProcessSystemBarChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessTouchOutsideRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessScreenshotRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessDialogTargetTouchRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessDialogDeathRecipientRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessGestureNavigationEnabledChangeRegister(sptr<CjWindowListener> listener,
        sptr<Window> window, const RegListenerInfo& info);
    WmErrorCode ProcessWaterMarkFlagChangeRegister(sptr<CjWindowListener> listener,
        sptr<Window> window, const RegListenerInfo& info);
    WmErrorCode ProcessWindowVisibilityChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessWindowStatusChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessWindowTitleButtonRectChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessWindowRectChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessSubWindowCloseRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    WmErrorCode ProcessNoInteractionDetectedRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        const RegListenerInfo& info);
    using Func = std::function<WmErrorCode(sptr<CjWindowListener>, sptr<Window>, const RegListenerInfo&)>;
    std::map<std::string, std::map<int64_t, sptr<CjWindowListener>>> cjCbMap_;
    mutable std::shared_mutex mtx_;
    std::map<CaseType, std::map<std::string, Func>> listenerProcess_;
};
}
}
#endif

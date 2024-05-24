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
class CjWindowRegisterManager {
public:
    CjWindowRegisterManager();
    ~CjWindowRegisterManager() {}
    WmErrorCode RegisterListener(sptr<Window>, std::string type,
        CaseType caseType, int64_t callbackObject);
    WmErrorCode UnregisterListener(sptr<Window>, std::string type,
        CaseType caseType, int64_t callbackObject);
private:
    bool IsCallbackRegistered(std::string type, int64_t callbackObject);
    WmErrorCode ProcessWindowChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister);
    WmErrorCode ProcessSystemAvoidAreaChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        bool isRegister);
    WmErrorCode ProcessAvoidAreaChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister);
    WmErrorCode ProcessLifeCycleEventRegister(sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister);
    WmErrorCode ProcessOccupiedAreaChangeRegister(sptr<CjWindowListener> listener,
        sptr<Window> window, bool isRegister);
    WmErrorCode ProcessSystemBarChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister);
    WmErrorCode ProcessTouchOutsideRegister(sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister);
    WmErrorCode ProcessScreenshotRegister(sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister);
    WmErrorCode ProcessDialogTargetTouchRegister(sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister);
    WmErrorCode ProcessDialogDeathRecipientRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        bool isRegister);
    WmErrorCode ProcessGestureNavigationEnabledChangeRegister(sptr<CjWindowListener> listener,
        sptr<Window> window, bool isRegister);
    WmErrorCode ProcessWaterMarkFlagChangeRegister(sptr<CjWindowListener> listener,
        sptr<Window> window, bool isRegister);
    WmErrorCode ProcessWindowVisibilityChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        bool isRegister);
    WmErrorCode ProcessWindowStatusChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        bool isRegister);
    WmErrorCode ProcessWindowTitleButtonRectChangeRegister(sptr<CjWindowListener> listener, sptr<Window> window,
        bool isRegister);
    using Func = WmErrorCode(CjWindowRegisterManager::*)(sptr<CjWindowListener>,
        sptr<Window> window, bool);
    std::map<std::string, std::map<int64_t, sptr<CjWindowListener>>> cjCbMap_;
    mutable std::shared_mutex mtx_;
    std::map<CaseType, std::map<std::string, Func>> listenerProcess_;
};
}
}
#endif

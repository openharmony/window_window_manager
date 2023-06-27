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

#include "js_scene_utils.h"

#include <js_runtime_utils.h>

#include "interfaces/include/ws_common.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneUtils" };
}

bool ConvertSessionInfoFromJs(NativeEngine& engine, NativeObject* jsObject, SessionInfo& sessionInfo)
{
    NativeValue* jsBundleName = jsObject->GetProperty("bundleName");
    NativeValue* jsModuleName = jsObject->GetProperty("moduleName");
    NativeValue* jsAbilityName = jsObject->GetProperty("abilityName");
    NativeValue* jsIsSystem = jsObject->GetProperty("isSystem");
    NativeValue* jsPersistentId = jsObject->GetProperty("persistentId");
    NativeValue* jsCallState = jsObject->GetProperty("callState");
    NativeValue* jsSessionType = jsObject->GetProperty("sessionType");

    if (jsBundleName->TypeOf() != NATIVE_UNDEFINED) {
        std::string bundleName;
        if (!ConvertFromJsValue(engine, jsBundleName, bundleName)) {
            WLOGFE("[NAPI]Failed to convert parameter to bundleName");
            return false;
        }
        sessionInfo.bundleName_ = bundleName;
    }
    if (jsModuleName->TypeOf() != NATIVE_UNDEFINED) {
        std::string moduleName;
        if (!ConvertFromJsValue(engine, jsModuleName, moduleName)) {
            WLOGFE("[NAPI]Failed to convert parameter to moduleName");
            return false;
        }
        sessionInfo.moduleName_ = moduleName;
    }
    if (jsAbilityName->TypeOf() != NATIVE_UNDEFINED) {
        std::string abilityName;
        if (!ConvertFromJsValue(engine, jsAbilityName, abilityName)) {
            WLOGFE("[NAPI]Failed to convert parameter to abilityName");
            return false;
        }
        sessionInfo.abilityName_ = abilityName;
    }
    if (jsIsSystem->TypeOf() != NATIVE_UNDEFINED) {
        bool isSystem;
        if (!ConvertFromJsValue(engine, jsIsSystem, isSystem)) {
            WLOGFE("[NAPI]Failed to convert parameter to isSystem");
            return false;
        }
        sessionInfo.isSystem_ = isSystem;
    }
    if (jsPersistentId->TypeOf() != NATIVE_UNDEFINED) {
        int64_t persistentId;
        if (!ConvertFromJsValue(engine, jsPersistentId, persistentId)) {
            WLOGFE("[NAPI]Failed to convert parameter to persistentId");
            return false;
        }
        sessionInfo.persistentId_ = persistentId;
    }
    if (jsCallState->TypeOf() != NATIVE_UNDEFINED) {
        int32_t callState;
        if (!ConvertFromJsValue(engine, jsCallState, callState)) {
            WLOGFE("[NAPI]Failed to convert parameter to callState");
            return false;
        }
        sessionInfo.callState_ = callState;
    }
    if (jsSessionType->TypeOf() != NATIVE_UNDEFINED) {
        uint32_t windowType = 0;
        if (!ConvertFromJsValue(engine, jsSessionType, windowType)) {
            WLOGFE("[NAPI]Failed to convert parameter to windowType");
            return false;
        }
        if (JS_SESSION_TO_WINDOW_TYPE_MAP.count(static_cast<JsSessionType>(windowType)) != 0) {
            sessionInfo.windowType_ = static_cast<uint32_t>(
                JS_SESSION_TO_WINDOW_TYPE_MAP.at(static_cast<JsSessionType>(windowType)));
        }
    }
    return true;
}

NativeValue* CreateJsSessionInfo(NativeEngine& engine, const SessionInfo& sessionInfo)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to convert sessionInfo to jsObject");
        return nullptr;
    }
    object->SetProperty("bundleName", CreateJsValue(engine, sessionInfo.bundleName_));
    object->SetProperty("moduleName", CreateJsValue(engine, sessionInfo.moduleName_));
    object->SetProperty("abilityName", CreateJsValue(engine, sessionInfo.abilityName_));
    object->SetProperty("isSystem", CreateJsValue(engine, sessionInfo.isSystem_));
    object->SetProperty("persistentId", CreateJsValue(engine, static_cast<int64_t>(sessionInfo.persistentId_)));
    object->SetProperty("callerPersistentId", CreateJsValue(engine,
        static_cast<int64_t>(sessionInfo.callerPersistentId_)));
    object->SetProperty("callState", CreateJsValue(engine, static_cast<int32_t>(sessionInfo.callState_)));
    return objValue;
}

NativeValue* CreateJsSessionState(NativeEngine& engine)
{
    NativeValue *objValue = engine.CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("STATE_DISCONNECT", CreateJsValue(engine,
        static_cast<int32_t>(SessionState::STATE_DISCONNECT)));
    object->SetProperty("STATE_CONNECT", CreateJsValue(engine,
        static_cast<int32_t>(SessionState::STATE_CONNECT)));
    object->SetProperty("STATE_FOREGROUND", CreateJsValue(engine,
        static_cast<int32_t>(SessionState::STATE_FOREGROUND)));
    object->SetProperty("STATE_ACTIVE", CreateJsValue(engine,
        static_cast<int32_t>(SessionState::STATE_ACTIVE)));
    object->SetProperty("STATE_INACTIVE", CreateJsValue(engine,
        static_cast<int32_t>(SessionState::STATE_INACTIVE)));
    object->SetProperty("STATE_BACKGROUND", CreateJsValue(engine,
        static_cast<int32_t>(SessionState::STATE_BACKGROUND)));
    object->SetProperty("STATE_END", CreateJsValue(engine,
        static_cast<int32_t>(SessionState::STATE_END)));

    return objValue;
}

NativeValue* CreateJsSessionRect(NativeEngine& engine, const WSRect& rect)
{
    WLOGFD("CreateJsSessionRect.");
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WLOGFE("Failed to create object!");
        return engine.CreateUndefined();
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert object!");
        return engine.CreateUndefined();
    }

    object->SetProperty("posX_", CreateJsValue(engine, rect.posX_));
    object->SetProperty("posY_", CreateJsValue(engine, rect.posY_));
    object->SetProperty("width_", CreateJsValue(engine, rect.width_));
    object->SetProperty("height_", CreateJsValue(engine, rect.height_));
    return objValue;
}

NativeValue* SessionTypeInit(NativeEngine* engine)
{
    WLOGFD("SessionTypeInit");

    if (engine == nullptr) {
        WLOGFE("Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("TYPE_UNDEFINED", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_UNDEFINED)));
    object->SetProperty("TYPE_APP", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_APP)));
    object->SetProperty("TYPE_SUB_APP", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_SUB_APP)));
    object->SetProperty("TYPE_SYSTEM_ALERT", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_SYSTEM_ALERT)));
    object->SetProperty("TYPE_INPUT_METHOD", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_INPUT_METHOD)));
    object->SetProperty("TYPE_STATUS_BAR", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_STATUS_BAR)));
    object->SetProperty("TYPE_PANEL", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_PANEL)));
    object->SetProperty("TYPE_KEYGUARD", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_KEYGUARD)));
    object->SetProperty("TYPE_VOLUME_OVERLAY", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_VOLUME_OVERLAY)));
    object->SetProperty("TYPE_NAVIGATION_BAR", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_NAVIGATION_BAR)));
    object->SetProperty("TYPE_FLOAT", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_FLOAT)));
    object->SetProperty("TYPE_WALLPAPER", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_WALLPAPER)));
    object->SetProperty("TYPE_DESKTOP", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_DESKTOP)));
    object->SetProperty("TYPE_LAUNCHER_DOCK", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_LAUNCHER_DOCK)));
    object->SetProperty("TYPE_FLOAT_CAMERA", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_FLOAT_CAMERA)));
    object->SetProperty("TYPE_DIALOG", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_DIALOG)));
    object->SetProperty("TYPE_SCREENSHOT", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_SCREENSHOT)));
    object->SetProperty("TYPE_TOAST", CreateJsValue(*engine,
        static_cast<int32_t>(JsSessionType::TYPE_TOAST)));
    return objValue;
}
} // namespace OHOS::Rosen

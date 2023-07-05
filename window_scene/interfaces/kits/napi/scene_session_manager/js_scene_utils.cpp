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

#include <iomanip>
#include <js_runtime_utils.h>
#include "js_scene_utils.h"

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

static std::string GetHexColor(uint32_t color)
{
    const int32_t rgbaLength = 8;

    std::stringstream ioss;
    std::string temp;
    ioss << std::setiosflags(std::ios::uppercase) << std::hex << color;
    ioss >> temp;
    int count = rgbaLength - static_cast<int>(temp.length());
    std::string finalColor("#");
    std::string tmpColor(count, '0');
    tmpColor += temp;
    finalColor += tmpColor;

    return finalColor;
}

static NativeValue* CreateJsSystemBarPropertyObject(
    NativeEngine& engine, const WindowType type, const SystemBarProperty& property)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert SystemBarProperty to jsObject");
        return nullptr;
    }
    if (WINDOW_TO_JS_SESSION_TYPE_MAP.count(type) != 0) {
        object->SetProperty("type", CreateJsValue(engine, WINDOW_TO_JS_SESSION_TYPE_MAP.at(type)));
    } else {
        object->SetProperty("type", CreateJsValue(engine, type));
    }
    object->SetProperty("enable", CreateJsValue(engine, property.enable_));
    std::string bkgColor = GetHexColor(property.backgroundColor_);
    object->SetProperty("backgroundcolor", CreateJsValue(engine, bkgColor));
    std::string contentColor = GetHexColor(property.contentColor_);
    object->SetProperty("contentcolor", CreateJsValue(engine, contentColor));

    return objValue;
}

NativeValue* CreateJsSystemBarPropertyArrayObject(
    NativeEngine& engine, const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    NativeValue* objValue = engine.CreateArray(propertyMap.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(objValue);
    if (array == nullptr) {
        WLOGFE("Failed to convert SystemBarPropertyMap to jsArrayObject");
        return nullptr;
    }
    uint32_t index = 0;
    for (auto iter: propertyMap) {
        array->SetElement(index++, CreateJsSystemBarPropertyObject(engine, iter.first, iter.second));
    }
    return objValue;
}

static void SetTypeProperty(NativeObject *object, NativeEngine* engine, std::string name, JsSessionType type)
{
    object->SetProperty(name.c_str(), CreateJsValue(*engine, static_cast<int32_t>(type)));
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
    SetTypeProperty(object, engine, "TYPE_UNDEFINED", JsSessionType::TYPE_UNDEFINED);
    SetTypeProperty(object, engine, "TYPE_APP", JsSessionType::TYPE_APP);
    SetTypeProperty(object, engine, "TYPE_SUB_APP", JsSessionType::TYPE_SUB_APP);
    SetTypeProperty(object, engine, "TYPE_SYSTEM_ALERT", JsSessionType::TYPE_SYSTEM_ALERT);
    SetTypeProperty(object, engine, "TYPE_INPUT_METHOD", JsSessionType::TYPE_INPUT_METHOD);
    SetTypeProperty(object, engine, "TYPE_STATUS_BAR", JsSessionType::TYPE_STATUS_BAR);
    SetTypeProperty(object, engine, "TYPE_PANEL", JsSessionType::TYPE_PANEL);
    SetTypeProperty(object, engine, "TYPE_KEYGUARD", JsSessionType::TYPE_KEYGUARD);
    SetTypeProperty(object, engine, "TYPE_VOLUME_OVERLAY", JsSessionType::TYPE_VOLUME_OVERLAY);
    SetTypeProperty(object, engine, "TYPE_NAVIGATION_BAR", JsSessionType::TYPE_NAVIGATION_BAR);
    SetTypeProperty(object, engine, "TYPE_FLOAT", JsSessionType::TYPE_FLOAT);
    SetTypeProperty(object, engine, "TYPE_WALLPAPER", JsSessionType::TYPE_WALLPAPER);
    SetTypeProperty(object, engine, "TYPE_DESKTOP", JsSessionType::TYPE_DESKTOP);
    SetTypeProperty(object, engine, "TYPE_LAUNCHER_DOCK", JsSessionType::TYPE_LAUNCHER_DOCK);
    SetTypeProperty(object, engine, "TYPE_FLOAT_CAMERA", JsSessionType::TYPE_FLOAT_CAMERA);
    SetTypeProperty(object, engine, "TYPE_DIALOG", JsSessionType::TYPE_DIALOG);
    SetTypeProperty(object, engine, "TYPE_SCREENSHOT", JsSessionType::TYPE_SCREENSHOT);
    SetTypeProperty(object, engine, "TYPE_TOAST", JsSessionType::TYPE_TOAST);
    SetTypeProperty(object, engine, "TYPE_POINTER", JsSessionType::TYPE_POINTER);
    SetTypeProperty(object, engine, "TYPE_LAUNCHER_RECENT", JsSessionType::TYPE_LAUNCHER_RECENT);
    SetTypeProperty(object, engine, "TYPE_SCENE_BOARD", JsSessionType::TYPE_SCENE_BOARD);
    return objValue;
}
} // namespace OHOS::Rosen

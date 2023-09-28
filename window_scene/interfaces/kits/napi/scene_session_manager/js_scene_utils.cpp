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
#include "session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneUtils" };
constexpr int32_t NUMBER_2 = 2;
constexpr int32_t NUMBER_3 = 3;
constexpr int32_t US_PER_NS = 1000;

int32_t GetMMITouchType(int32_t aceType)
{
    switch (aceType) {
        case 0:
            return MMI::PointerEvent::POINTER_ACTION_DOWN;
        case 1:
            return MMI::PointerEvent::POINTER_ACTION_UP;
        case NUMBER_2:
            return MMI::PointerEvent::POINTER_ACTION_MOVE;
        case NUMBER_3:
            return MMI::PointerEvent::POINTER_ACTION_CANCEL;
        default:
            return MMI::PointerEvent::POINTER_ACTION_UNKNOWN;
    }
}

int32_t GetMMISourceType(int32_t aceType)
{
    switch (aceType) {
        case 1:
            return MMI::PointerEvent::SOURCE_TYPE_MOUSE;
        case NUMBER_2:
            return MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN;
        case NUMBER_3:
            return MMI::PointerEvent::SOURCE_TYPE_TOUCHPAD;
        default:
            return MMI::PointerEvent::SOURCE_TYPE_UNKNOWN;
    }
}
} // namespace

bool IsJsBundleNameUndefind(NativeEngine& engine, NativeValue* jsBundleName, SessionInfo& sessionInfo)
{
    if (jsBundleName->TypeOf() != NATIVE_UNDEFINED) {
        std::string bundleName;
        if (!ConvertFromJsValue(engine, jsBundleName, bundleName)) {
            WLOGFE("[NAPI]Failed to convert parameter to bundleName");
            return false;
        }
        sessionInfo.bundleName_ = bundleName;
    }
    return true;
}

bool IsJsModuleNameUndefind(NativeEngine& engine, NativeValue* jsModuleName, SessionInfo& sessionInfo)
{
    if (jsModuleName->TypeOf() != NATIVE_UNDEFINED) {
        std::string moduleName;
        if (!ConvertFromJsValue(engine, jsModuleName, moduleName)) {
            WLOGFE("[NAPI]Failed to convert parameter to moduleName");
            return false;
        }
        sessionInfo.moduleName_ = moduleName;
    }
    return true;
}

bool IsJsAbilityUndefind(NativeEngine& engine, NativeValue* jsAbilityName, SessionInfo& sessionInfo)
{
    if (jsAbilityName->TypeOf() != NATIVE_UNDEFINED) {
        std::string abilityName;
        if (!ConvertFromJsValue(engine, jsAbilityName, abilityName)) {
            WLOGFE("[NAPI]Failed to convert parameter to abilityName");
            return false;
        }
        sessionInfo.abilityName_ = abilityName;
    }
    return true;
}

bool IsJsAppIndexUndefind(NativeEngine& engine, NativeValue* jsAppIndex, SessionInfo& sessionInfo)
{
    if (jsAppIndex->TypeOf() != NATIVE_UNDEFINED) {
        int32_t appIndex;
        if (!ConvertFromJsValue(engine, jsAppIndex, appIndex)) {
            WLOGFE("[NAPI]Failed to convert parameter to appIndex");
            return false;
        }
        sessionInfo.appIndex_ = appIndex;
    }
    return true;
}

bool IsJsIsSystemUndefind(NativeEngine& engine, NativeValue* jsIsSystem, SessionInfo& sessionInfo)
{
    if (jsIsSystem->TypeOf() != NATIVE_UNDEFINED) {
        bool isSystem;
        if (!ConvertFromJsValue(engine, jsIsSystem, isSystem)) {
            WLOGFE("[NAPI]Failed to convert parameter to isSystem");
            return false;
        }
        sessionInfo.isSystem_ = isSystem;
    }
    return true;
}

bool IsJsPersistentIdUndefind(NativeEngine& engine, NativeValue* jsPersistentId, SessionInfo& sessionInfo)
{
    if (jsPersistentId->TypeOf() != NATIVE_UNDEFINED) {
        int32_t persistentId;
        if (!ConvertFromJsValue(engine, jsPersistentId, persistentId)) {
            WLOGFE("[NAPI]Failed to convert parameter to persistentId");
            return false;
        }
        sessionInfo.persistentId_ = persistentId;
    }
    return true;
}

bool IsJsCallStateUndefind(NativeEngine& engine, NativeValue* jsCallState, SessionInfo& sessionInfo)
{
    if (jsCallState->TypeOf() != NATIVE_UNDEFINED) {
        int32_t callState;
        if (!ConvertFromJsValue(engine, jsCallState, callState)) {
            WLOGFE("[NAPI]Failed to convert parameter to callState");
            return false;
        }
        sessionInfo.callState_ = static_cast<uint32_t>(callState);
    }
    return true;
}

bool IsJsSessionTypeUndefind(NativeEngine& engine, NativeValue* jsSessionType, SessionInfo& sessionInfo)
{
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

bool ConvertSessionInfoFromJs(NativeEngine& engine, NativeObject* jsObject, SessionInfo& sessionInfo)
{
    NativeValue* jsBundleName = jsObject->GetProperty("bundleName");
    NativeValue* jsModuleName = jsObject->GetProperty("moduleName");
    NativeValue* jsAbilityName = jsObject->GetProperty("abilityName");
    NativeValue* jsAppIndex = jsObject->GetProperty("appIndex");
    NativeValue* jsIsSystem = jsObject->GetProperty("isSystem");
    NativeValue* jsPersistentId = jsObject->GetProperty("persistentId");
    NativeValue* jsCallState = jsObject->GetProperty("callState");
    NativeValue* jsSessionType = jsObject->GetProperty("sessionType");

    if (!IsJsBundleNameUndefind(engine, jsBundleName, sessionInfo)) {
        return false;
    }
    if (!IsJsModuleNameUndefind(engine, jsModuleName, sessionInfo)) {
        return false;
    }
    if (!IsJsAbilityUndefind(engine, jsAbilityName, sessionInfo)) {
        return false;
    }
    if (!IsJsAppIndexUndefind(engine, jsAppIndex, sessionInfo)) {
        return false;
    }
    if (!IsJsIsSystemUndefind(engine, jsIsSystem, sessionInfo)) {
        return false;
    }
    if (!IsJsPersistentIdUndefind(engine, jsPersistentId, sessionInfo)) {
        return false;
    }
    if (!IsJsCallStateUndefind(engine, jsCallState, sessionInfo)) {
        return false;
    }
    if (!IsJsSessionTypeUndefind(engine, jsSessionType, sessionInfo)) {
        return false;
    }
    return true;
}

bool ConvertPointerItemFromJs(NativeEngine& engine, NativeObject* touchObject, MMI::PointerEvent& pointerEvent)
{
    auto displayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (!displayInfo) {
        WLOGFE("[NAPI]Default display info is null");
        return false;
    }
    auto vpr = displayInfo->GetVirtualPixelRatio();
    MMI::PointerEvent::PointerItem pointerItem;
    NativeValue* jsId = touchObject->GetProperty("id");
    NativeValue* jsTouchType = touchObject->GetProperty("type");
    NativeValue* jsWindowX = touchObject->GetProperty("windowX");
    NativeValue* jsWindowY = touchObject->GetProperty("windowY");
    NativeValue* jsDisplayX = touchObject->GetProperty("displayX");
    NativeValue* jsDisplayY = touchObject->GetProperty("displayY");
    int32_t id;
    if (!ConvertFromJsValue(engine, jsId, id)) {
        WLOGFE("[NAPI]Failed to convert parameter to id");
        return false;
    }
    pointerItem.SetPointerId(id);
    pointerEvent.SetPointerId(id);
    int32_t touchType;
    if (!ConvertFromJsValue(engine, jsTouchType, touchType)) {
        WLOGFE("[NAPI]Failed to convert parameter to touchType");
        return false;
    }
    pointerEvent.SetPointerAction(GetMMITouchType(touchType));
    double windowX;
    if (!ConvertFromJsValue(engine, jsWindowX, windowX)) {
        WLOGFE("[NAPI]Failed to convert parameter to windowX");
        return false;
    }
    pointerItem.SetWindowX(std::round(windowX * vpr));
    double windowY;
    if (!ConvertFromJsValue(engine, jsWindowY, windowY)) {
        WLOGFE("[NAPI]Failed to convert parameter to windowY");
        return false;
    }
    pointerItem.SetWindowY(std::round(windowY * vpr));
    double displayX;
    if (!ConvertFromJsValue(engine, jsDisplayX, displayX)) {
        WLOGFE("[NAPI]Failed to convert parameter to displayX");
        return false;
    }
    pointerItem.SetDisplayX(std::round(displayX * vpr));
    double displayY;
    if (!ConvertFromJsValue(engine, jsDisplayY, displayY)) {
        WLOGFE("[NAPI]Failed to convert parameter to displayY");
        return false;
    }
    pointerItem.SetDisplayY(std::round(displayY * vpr));
    pointerEvent.AddPointerItem(pointerItem);
    return true;
}

bool ConvertPointerEventFromJs(NativeEngine& engine, NativeObject* jsObject, MMI::PointerEvent& pointerEvent)
{
    NativeValue* jsSourceType = jsObject->GetProperty("source");
    NativeValue* jsTimestamp = jsObject->GetProperty("timestamp");
    NativeValue* jsChangedTouches = jsObject->GetProperty("changedTouches");
    int32_t sourceType;
    if (!ConvertFromJsValue(engine, jsSourceType, sourceType)) {
        WLOGFE("[NAPI]Failed to convert parameter to sourceType");
        return false;
    }
    pointerEvent.SetSourceType(GetMMISourceType(sourceType));
    double timestamp;
    if (!ConvertFromJsValue(engine, jsTimestamp, timestamp)) {
        WLOGFE("[NAPI]Failed to convert parameter to timestamp");
        return false;
    }
    pointerEvent.SetActionTime(std::round(timestamp / US_PER_NS));
    NativeArray* changedTouchesArray = ConvertNativeValueTo<NativeArray>(jsChangedTouches);
    if (changedTouchesArray == nullptr) {
        WLOGFE("[NAPI]Failed to convert parameter to touchesArray");
        return false;
    }
    // use changedTouches[0] only
    NativeObject* touchObject = ConvertNativeValueTo<NativeObject>(changedTouchesArray->GetElement(0));
    if (touchObject == nullptr) {
        WLOGFE("[NAPI]Failed to convert parameter to touchObject");
        return false;
    }
    if (!ConvertPointerItemFromJs(engine, touchObject, pointerEvent)) {
        return false;
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
    object->SetProperty("appIndex", CreateJsValue(engine, sessionInfo.appIndex_));
    object->SetProperty("isSystem", CreateJsValue(engine, sessionInfo.isSystem_));
    object->SetProperty("persistentId", CreateJsValue(engine, static_cast<int32_t>(sessionInfo.persistentId_)));
    object->SetProperty("callerPersistentId", CreateJsValue(engine,
        static_cast<int32_t>(sessionInfo.callerPersistentId_)));
    object->SetProperty("callState", CreateJsValue(engine, static_cast<int32_t>(sessionInfo.callState_)));
    object->SetProperty("windowMode", CreateJsValue(engine, static_cast<int32_t>(sessionInfo.windowMode)));
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

NativeValue* CreateJsSessionSizeChangeReason(NativeEngine& engine)
{
    NativeValue *objValue = engine.CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("UNDEFINED", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::UNDEFINED)));
    object->SetProperty("MAXIMIZE", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::MAXIMIZE)));
    object->SetProperty("RECOVER", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::RECOVER)));
    object->SetProperty("ROTATION", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::ROTATION)));
    object->SetProperty("DRAG", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::DRAG)));
    object->SetProperty("DRAG_START", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::DRAG_START)));
    object->SetProperty("DRAG_END", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::DRAG_END)));
    object->SetProperty("RESIZE", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::RESIZE)));
    object->SetProperty("MOVE", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::MOVE)));
    object->SetProperty("HIDE", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::HIDE)));
    object->SetProperty("TRANSFORM", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::TRANSFORM)));
    object->SetProperty("CUSTOM_ANIMATION_SHOW", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::CUSTOM_ANIMATION_SHOW)));
    object->SetProperty("FULL_TO_SPLIT", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::FULL_TO_SPLIT)));
    object->SetProperty("SPLIT_TO_FULL", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::SPLIT_TO_FULL)));
    object->SetProperty("END", CreateJsValue(engine,
        static_cast<int32_t>(SizeChangeReason::END)));

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
    if (propertyMap.empty()) {
        WLOGFE("Empty propertyMap");
        return nullptr;
    }
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

static void SetTypeProperty(NativeObject *object, NativeEngine* engine, const std::string& name, JsSessionType type)
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
    SetTypeProperty(object, engine, "TYPE_DRAGGING_EFFECT", JsSessionType::TYPE_DRAGGING_EFFECT);
    SetTypeProperty(object, engine, "TYPE_INPUT_METHOD_STATUS_BAR", JsSessionType::TYPE_INPUT_METHOD_STATUS_BAR);
    SetTypeProperty(object, engine, "TYPE_GLOBAL_SEARCH", JsSessionType::TYPE_GLOBAL_SEARCH);
    SetTypeProperty(object, engine, "TYPE_NEGATIVE_SCREEN", JsSessionType::TYPE_NEGATIVE_SCREEN);
    SetTypeProperty(object, engine, "TYPE_VOICE_INTERACTION", JsSessionType::TYPE_VOICE_INTERACTION);
    SetTypeProperty(object, engine, "TYPE_SYSTEM_TOAST", JsSessionType::TYPE_SYSTEM_TOAST);
    return objValue;
}
} // namespace OHOS::Rosen

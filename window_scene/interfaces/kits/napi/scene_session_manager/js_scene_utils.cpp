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

#include <iomanip>

#include <event_handler.h>
#include <js_runtime_utils.h>

#include "root_scene.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneUtils" };
constexpr int32_t NUMBER_2 = 2;
constexpr int32_t NUMBER_3 = 3;
constexpr int32_t US_PER_NS = 1000;
constexpr int32_t INVALID_VAL = -9999;

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

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_valuetype GetType(napi_env env, napi_value value)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, value, &res);
    return res;
}

bool IsJsBundleNameUndefind(napi_env env, napi_value jsBundleName, SessionInfo& sessionInfo)
{
    if (GetType(env, jsBundleName) != napi_undefined) {
        std::string bundleName;
        if (!ConvertFromJsValue(env, jsBundleName, bundleName)) {
            WLOGFE("[NAPI]Failed to convert parameter to bundleName");
            return false;
        }
        sessionInfo.bundleName_ = bundleName;
    }
    return true;
}

bool IsJsModuleNameUndefind(napi_env env, napi_value jsModuleName, SessionInfo& sessionInfo)
{
    if (GetType(env, jsModuleName) != napi_undefined) {
        std::string moduleName;
        if (!ConvertFromJsValue(env, jsModuleName, moduleName)) {
            WLOGFE("[NAPI]Failed to convert parameter to moduleName");
            return false;
        }
        sessionInfo.moduleName_ = moduleName;
    }
    return true;
}

bool IsJsAbilityUndefind(napi_env env, napi_value jsAbilityName, SessionInfo& sessionInfo)
{
    if (GetType(env, jsAbilityName) != napi_undefined) {
        std::string abilityName;
        if (!ConvertFromJsValue(env, jsAbilityName, abilityName)) {
            WLOGFE("[NAPI]Failed to convert parameter to abilityName");
            return false;
        }
        sessionInfo.abilityName_ = abilityName;
    }
    return true;
}

bool IsJsAppIndexUndefind(napi_env env, napi_value jsAppIndex, SessionInfo& sessionInfo)
{
    if (GetType(env, jsAppIndex) != napi_undefined) {
        int32_t appIndex;
        if (!ConvertFromJsValue(env, jsAppIndex, appIndex)) {
            WLOGFE("[NAPI]Failed to convert parameter to appIndex");
            return false;
        }
        sessionInfo.appIndex_ = appIndex;
    }
    return true;
}

bool IsJsIsSystemUndefind(napi_env env, napi_value jsIsSystem, SessionInfo& sessionInfo)
{
    if (GetType(env, jsIsSystem) != napi_undefined) {
        bool isSystem;
        if (!ConvertFromJsValue(env, jsIsSystem, isSystem)) {
            WLOGFE("[NAPI]Failed to convert parameter to isSystem");
            return false;
        }
        sessionInfo.isSystem_ = isSystem;
    }
    return true;
}

bool IsJsPersistentIdUndefind(napi_env env, napi_value jsPersistentId, SessionInfo& sessionInfo)
{
    if (GetType(env, jsPersistentId) != napi_undefined) {
        int32_t persistentId;
        if (!ConvertFromJsValue(env, jsPersistentId, persistentId)) {
            WLOGFE("[NAPI]Failed to convert parameter to persistentId");
            return false;
        }
        sessionInfo.persistentId_ = persistentId;
    }
    return true;
}

bool IsJsCallStateUndefind(napi_env env, napi_value jsCallState, SessionInfo& sessionInfo)
{
    if (GetType(env, jsCallState) != napi_undefined) {
        int32_t callState;
        if (!ConvertFromJsValue(env, jsCallState, callState)) {
            WLOGFE("[NAPI]Failed to convert parameter to callState");
            return false;
        }
        sessionInfo.callState_ = static_cast<uint32_t>(callState);
    }
    return true;
}

bool IsJsSessionTypeUndefind(napi_env env, napi_value jsSessionType, SessionInfo& sessionInfo)
{
    if (GetType(env, jsSessionType) != napi_undefined) {
        uint32_t windowType = 0;
        if (!ConvertFromJsValue(env, jsSessionType, windowType)) {
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

bool IsJsScreenIdUndefind(napi_env env, napi_value JsScreenId, SessionInfo& sessionInfo)
{
    if (GetType(env, JsScreenId) != napi_undefined) {
        int32_t screenId = 0;
        if (!ConvertFromJsValue(env, JsScreenId, screenId)) {
            WLOGFE("[NAPI]Failed to convert parameter to screenId");
            return false;
        }
        sessionInfo.screenId_ = static_cast<uint64_t>(screenId);
    }
    return true;
}

bool IsJsIsPersistentRecoverUndefined(napi_env env, napi_value jsIsPersistentRecover, SessionInfo& sessionInfo)
{
    if (GetType(env, jsIsPersistentRecover) != napi_undefined) {
        bool isPersistentRecover = false;
        if (!ConvertFromJsValue(env, jsIsPersistentRecover, isPersistentRecover)) {
            WLOGFE("[NAPI]Failed to convert parameter to isPersistentRecover");
            return false;
        }
        sessionInfo.isPersistentRecover_ = isPersistentRecover;
    }
    return true;
}

bool IsJsIsRotatableUndefined(napi_env env, napi_value jsIsRotatable, SessionInfo& sessionInfo)
{
    if (GetType(env, jsIsRotatable) != napi_undefined) {
        bool isRotable = false;
        if (!ConvertFromJsValue(env, jsIsRotatable, isRotable)) {
            WLOGFE("[NAPI]Failed to convert parameter to isRotable");
            return false;
        }
        sessionInfo.isRotable_ = isRotable;
    }
    return true;
}

bool IsJsIsSystemInputUndefined(napi_env env, napi_value jsIsSystemInput, SessionInfo& sessionInfo)
{
    if (GetType(env, jsIsSystemInput) != napi_undefined) {
        bool isSystemInput = false;
        if (!ConvertFromJsValue(env, jsIsSystemInput, isSystemInput)) {
            WLOGFE("[NAPI]Failed to convert parameter to isSystemInput");
            return false;
        }
        sessionInfo.isSystemInput_ = isSystemInput;
    }
    return true;
}

bool ConvertSessionInfoName(napi_env env, napi_value jsObject, SessionInfo& sessionInfo)
{
    napi_value jsBundleName = nullptr;
    napi_get_named_property(env, jsObject, "bundleName", &jsBundleName);
    napi_value jsModuleName = nullptr;
    napi_get_named_property(env, jsObject, "moduleName", &jsModuleName);
    napi_value jsAbilityName = nullptr;
    napi_get_named_property(env, jsObject, "abilityName", &jsAbilityName);
    napi_value jsAppIndex = nullptr;
    napi_get_named_property(env, jsObject, "appIndex", &jsAppIndex);
    napi_value jsIsSystem = nullptr;
    napi_get_named_property(env, jsObject, "isSystem", &jsIsSystem);
    if (!IsJsBundleNameUndefind(env, jsBundleName, sessionInfo)) {
        return false;
    }
    if (!IsJsModuleNameUndefind(env, jsModuleName, sessionInfo)) {
        return false;
    }
    if (!IsJsAbilityUndefind(env, jsAbilityName, sessionInfo)) {
        return false;
    }
    if (!IsJsAppIndexUndefind(env, jsAppIndex, sessionInfo)) {
        return false;
    }
    if (!IsJsIsSystemUndefind(env, jsIsSystem, sessionInfo)) {
        return false;
    }
    return true;
}

bool ConvertSessionInfoState(napi_env env, napi_value jsObject, SessionInfo& sessionInfo)
{
    napi_value jsPersistentId = nullptr;
    napi_get_named_property(env, jsObject, "persistentId", &jsPersistentId);
    napi_value jsCallState = nullptr;
    napi_get_named_property(env, jsObject, "callState", &jsCallState);
    napi_value jsSessionType = nullptr;
    napi_get_named_property(env, jsObject, "sessionType", &jsSessionType);
    napi_value jsScreenId = nullptr;
    napi_get_named_property(env, jsObject, "screenId", &jsScreenId);
    napi_value jsIsPersistentRecover = nullptr;
    napi_get_named_property(env, jsObject, "isPersistentRecover", &jsIsPersistentRecover);
    napi_value jsIsRotable = nullptr;
    napi_get_named_property(env, jsObject, "isRotable", &jsIsRotable);
    napi_value jsIsSystemInput = nullptr;
    napi_get_named_property(env, jsObject, "isSystemInput", &jsIsSystemInput);

    if (!IsJsPersistentIdUndefind(env, jsPersistentId, sessionInfo)) {
        return false;
    }
    if (!IsJsCallStateUndefind(env, jsCallState, sessionInfo)) {
        return false;
    }
    if (!IsJsSessionTypeUndefind(env, jsSessionType, sessionInfo)) {
        return false;
    }
    if (!IsJsScreenIdUndefind(env, jsScreenId, sessionInfo)) {
        return false;
    }
    if (!IsJsIsPersistentRecoverUndefined(env, jsIsPersistentRecover, sessionInfo)) {
        return false;
    }
    if (!IsJsIsRotatableUndefined(env, jsIsRotable, sessionInfo)) {
        return false;
    }
    if (!IsJsIsSystemInputUndefined(env, jsIsSystemInput, sessionInfo)) {
        return false;
    }
    return true;
}

bool ConvertSessionInfoFromJs(napi_env env, napi_value jsObject, SessionInfo& sessionInfo)
{
    if (!ConvertSessionInfoName(env, jsObject, sessionInfo)) {
        return false;
    }
    if (!ConvertSessionInfoState(env, jsObject, sessionInfo)) {
        return false;
    }
    return true;
}

bool IsJsObjNameUndefind(napi_env env, napi_value jsObjName, int32_t& objName)
{
    if (GetType(env, jsObjName) != napi_undefined) {
        if (!ConvertFromJsValue(env, jsObjName, objName)) {
            WLOGFE("[NAPI]Failed to convert parameter to objName");
            return false;
        }
    }
    return true;
}

bool ConvertRectInfoFromJs(napi_env env, napi_value jsObject, WSRect& rect)
{
    napi_value jsLeftName = nullptr;
    napi_get_named_property(env, jsObject, "left", &jsLeftName);
    napi_value jsTopName = nullptr;
    napi_get_named_property(env, jsObject, "top", &jsTopName);
    napi_value jsRightName = nullptr;
    napi_get_named_property(env, jsObject, "right", &jsRightName);
    napi_value jsBottomName = nullptr;
    napi_get_named_property(env, jsObject, "bottom", &jsBottomName);

    int32_t leftName = 0;
    if (!IsJsObjNameUndefind(env, jsLeftName, leftName)) {
        return false;
    }
    rect.posX_ = leftName;

    int32_t topName = 0;
    if (!IsJsObjNameUndefind(env, jsTopName, topName)) {
        return false;
    }
    rect.posY_ = topName;

    int32_t rightName = 0;
    if (!IsJsObjNameUndefind(env, jsRightName, rightName)) {
        return false;
    }
    rect.width_ = rightName - rect.posX_;

    int32_t bottomName = 0;
    if (!IsJsObjNameUndefind(env, jsBottomName, bottomName)) {
        return false;
    }
    rect.height_ = bottomName - rect.posY_;
    return true;
}

bool ConvertPointerItemFromJs(napi_env env, napi_value touchObject, MMI::PointerEvent& pointerEvent)
{
    auto vpr = RootScene::staticRootScene_->GetDisplayDensity();
    MMI::PointerEvent::PointerItem pointerItem;
    napi_value jsId = nullptr;
    napi_get_named_property(env, touchObject, "id", &jsId);
    napi_value jsTouchType = nullptr;
    napi_get_named_property(env, touchObject, "type", &jsTouchType);
    napi_value jsWindowX = nullptr;
    napi_get_named_property(env, touchObject, "windowX", &jsWindowX);
    napi_value jsWindowY = nullptr;
    napi_get_named_property(env, touchObject, "windowY", &jsWindowY);
    napi_value jsDisplayX = nullptr;
    napi_get_named_property(env, touchObject, "displayX", &jsDisplayX);
    napi_value jsDisplayY = nullptr;
    napi_get_named_property(env, touchObject, "displayY", &jsDisplayY);
    int32_t id;
    if (!ConvertFromJsValue(env, jsId, id)) {
        WLOGFE("[NAPI]Failed to convert parameter to id");
        return false;
    }
    pointerItem.SetPointerId(id);
    pointerEvent.SetPointerId(id);
    int32_t touchType;
    if (!ConvertFromJsValue(env, jsTouchType, touchType)) {
        WLOGFE("[NAPI]Failed to convert parameter to touchType");
        return false;
    }
    pointerEvent.SetPointerAction(GetMMITouchType(touchType));
    double windowX;
    if (!ConvertFromJsValue(env, jsWindowX, windowX)) {
        WLOGFE("[NAPI]Failed to convert parameter to windowX");
        return false;
    }
    pointerItem.SetWindowX(std::round(windowX * vpr));
    double windowY;
    if (!ConvertFromJsValue(env, jsWindowY, windowY)) {
        WLOGFE("[NAPI]Failed to convert parameter to windowY");
        return false;
    }
    pointerItem.SetWindowY(std::round(windowY * vpr));
    double displayX;
    if (!ConvertFromJsValue(env, jsDisplayX, displayX)) {
        WLOGFE("[NAPI]Failed to convert parameter to displayX");
        return false;
    }
    pointerItem.SetDisplayX(std::round(displayX * vpr));
    double displayY;
    if (!ConvertFromJsValue(env, jsDisplayY, displayY)) {
        WLOGFE("[NAPI]Failed to convert parameter to displayY");
        return false;
    }
    pointerItem.SetDisplayY(std::round(displayY * vpr));
    pointerEvent.AddPointerItem(pointerItem);
    return true;
}

bool ConvertPointerEventFromJs(napi_env env, napi_value jsObject, MMI::PointerEvent& pointerEvent)
{
    napi_value jsSourceType = nullptr;
    napi_get_named_property(env, jsObject, "source", &jsSourceType);
    napi_value jsTimestamp = nullptr;
    napi_get_named_property(env, jsObject, "timestamp", &jsTimestamp);
    napi_value jsChangedTouches = nullptr;
    napi_get_named_property(env, jsObject, "changedTouches", &jsChangedTouches);
    int32_t sourceType;
    if (!ConvertFromJsValue(env, jsSourceType, sourceType)) {
        WLOGFE("[NAPI]Failed to convert parameter to sourceType");
        return false;
    }
    pointerEvent.SetSourceType(GetMMISourceType(sourceType));
    double timestamp;
    if (!ConvertFromJsValue(env, jsTimestamp, timestamp)) {
        WLOGFE("[NAPI]Failed to convert parameter to timestamp");
        return false;
    }
    pointerEvent.SetActionTime(std::round(timestamp / US_PER_NS));
    if (jsChangedTouches == nullptr) {
        WLOGFE("[NAPI]Failed to convert parameter to touchesArray");
        return false;
    }
    // use changedTouches[0] only
    napi_value touchObject = nullptr;
    napi_get_element(env, jsChangedTouches, 0, &touchObject);
    if (touchObject == nullptr) {
        WLOGFE("[NAPI]Failed get to touchObject");
        return false;
    }
    if (!ConvertPointerItemFromJs(env, touchObject, pointerEvent)) {
        return false;
    }
    return true;
}

bool ConvertInt32ArrayFromJs(napi_env env, napi_value jsObject, std::vector<int32_t> &intList)
{
    bool isArray = false;
    napi_is_array(env, jsObject, &isArray);
    if (jsObject == nullptr || !isArray) {
        WLOGFE("[NAPI]Failed to convert to integer list");
        return false;
    }

    uint32_t length = 0;
    napi_get_array_length(env, jsObject, &length);
    for (uint32_t i = 0; i < length; i++) {
        int32_t persistentId;
        napi_value elementVal = nullptr;
        napi_get_element(env, jsObject, i, &elementVal);
        if (!ConvertFromJsValue(env, elementVal, persistentId)) {
            WLOGFE("[NAPI]Failed to convert to index %{public}u to integer", i);
            return false;
        }
        intList.push_back(persistentId);
    }

    return true;
}

JsSessionType GetApiType(WindowType type)
{
    auto iter = WINDOW_TO_JS_SESSION_TYPE_MAP.find(type);
    if (iter == WINDOW_TO_JS_SESSION_TYPE_MAP.end()) {
        WLOGFE("[NAPI]window type: %{public}u cannot map to api type!", type);
        return JsSessionType::TYPE_UNDEFINED;
    } else {
        return iter->second;
    }
}

napi_value CreateJsSessionInfo(napi_env env, const SessionInfo& sessionInfo)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Failed to get jsObject");
        return nullptr;
    }
    napi_set_named_property(env, objValue, "bundleName", CreateJsValue(env, sessionInfo.bundleName_));
    napi_set_named_property(env, objValue, "moduleName", CreateJsValue(env, sessionInfo.moduleName_));
    napi_set_named_property(env, objValue, "abilityName", CreateJsValue(env, sessionInfo.abilityName_));
    napi_set_named_property(env, objValue, "appIndex", CreateJsValue(env, sessionInfo.appIndex_));
    napi_set_named_property(env, objValue, "isSystem", CreateJsValue(env, sessionInfo.isSystem_));
    napi_set_named_property(env, objValue, "persistentId",
        CreateJsValue(env, static_cast<int32_t>(sessionInfo.persistentId_)));
    napi_set_named_property(env, objValue, "callerPersistentId",
        CreateJsValue(env, static_cast<int32_t>(sessionInfo.callerPersistentId_)));
    napi_set_named_property(env, objValue, "callState",
        CreateJsValue(env, static_cast<int32_t>(sessionInfo.callState_)));
    napi_set_named_property(env, objValue, "windowMode",
        CreateJsValue(env, static_cast<int32_t>(sessionInfo.windowMode)));
    napi_set_named_property(env, objValue, "screenId",
        CreateJsValue(env, static_cast<int32_t>(sessionInfo.screenId_)));
    napi_set_named_property(env, objValue, "sessionType",
        CreateJsValue(env, static_cast<uint32_t>(GetApiType(static_cast<WindowType>(sessionInfo.windowType_)))));
    napi_set_named_property(env, objValue, "sessionState",
        CreateJsValue(env, static_cast<int32_t>(sessionInfo.sessionState_)));
    auto requestOrientation =
        WINDOW_ORIENTATION_TO_JS_SESSION_MAP.at(static_cast<Orientation>(sessionInfo.requestOrientation_));
    napi_set_named_property(env, objValue, "requestOrientation",
        CreateJsValue(env, static_cast<uint32_t>(requestOrientation)));

    if (sessionInfo.want != nullptr) {
        napi_set_named_property(env, objValue, "windowTop",
            GetWindowRectIntValue(env,
            sessionInfo.want->GetIntParam(AAFwk::Want::PARAM_RESV_WINDOW_TOP, INVALID_VAL)));
        napi_set_named_property(env, objValue, "windowLeft",
            GetWindowRectIntValue(env,
            sessionInfo.want->GetIntParam(AAFwk::Want::PARAM_RESV_WINDOW_LEFT, INVALID_VAL)));
        napi_set_named_property(env, objValue, "windowWidth",
            GetWindowRectIntValue(env,
            sessionInfo.want->GetIntParam(AAFwk::Want::PARAM_RESV_WINDOW_WIDTH, INVALID_VAL)));
        napi_set_named_property(env, objValue, "windowHeight",
            GetWindowRectIntValue(env,
            sessionInfo.want->GetIntParam(AAFwk::Want::PARAM_RESV_WINDOW_HEIGHT, INVALID_VAL)));

        napi_set_named_property(env, objValue, "withAnimation",
            CreateJsValue(env, sessionInfo.want->GetBoolParam(AAFwk::Want::PARAM_RESV_WITH_ANIMATION, true)));
    }

    return objValue;
}

napi_value GetWindowRectIntValue(napi_env env, int val)
{
    if (val != INVALID_VAL) {
        return  CreateJsValue(env, val);
    } else {
        return NapiGetUndefined(env);
    }
}

napi_value CreateJsSessionState(napi_env env)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "STATE_DISCONNECT", CreateJsValue(env,
        static_cast<int32_t>(SessionState::STATE_DISCONNECT)));
    napi_set_named_property(env, objValue, "STATE_CONNECT", CreateJsValue(env,
        static_cast<int32_t>(SessionState::STATE_CONNECT)));
    napi_set_named_property(env, objValue, "STATE_FOREGROUND", CreateJsValue(env,
        static_cast<int32_t>(SessionState::STATE_FOREGROUND)));
    napi_set_named_property(env, objValue, "STATE_ACTIVE", CreateJsValue(env,
        static_cast<int32_t>(SessionState::STATE_ACTIVE)));
    napi_set_named_property(env, objValue, "STATE_INACTIVE", CreateJsValue(env,
        static_cast<int32_t>(SessionState::STATE_INACTIVE)));
    napi_set_named_property(env, objValue, "STATE_BACKGROUND", CreateJsValue(env,
        static_cast<int32_t>(SessionState::STATE_BACKGROUND)));
    napi_set_named_property(env, objValue, "STATE_END", CreateJsValue(env,
        static_cast<int32_t>(SessionState::STATE_END)));

    return objValue;
}

napi_value CreateJsSessionSizeChangeReason(napi_env env)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "UNDEFINED", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::UNDEFINED)));
    napi_set_named_property(env, objValue, "MAXIMIZE", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::MAXIMIZE)));
    napi_set_named_property(env, objValue, "RECOVER", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::RECOVER)));
    napi_set_named_property(env, objValue, "ROTATION", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::ROTATION)));
    napi_set_named_property(env, objValue, "DRAG", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::DRAG)));
    napi_set_named_property(env, objValue, "DRAG_START", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::DRAG_START)));
    napi_set_named_property(env, objValue, "DRAG_END", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::DRAG_END)));
    napi_set_named_property(env, objValue, "RESIZE", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::RESIZE)));
    napi_set_named_property(env, objValue, "MOVE", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::MOVE)));
    napi_set_named_property(env, objValue, "HIDE", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::HIDE)));
    napi_set_named_property(env, objValue, "TRANSFORM", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::TRANSFORM)));
    napi_set_named_property(env, objValue, "CUSTOM_ANIMATION_SHOW", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::CUSTOM_ANIMATION_SHOW)));
    napi_set_named_property(env, objValue, "FULL_TO_SPLIT", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::FULL_TO_SPLIT)));
    napi_set_named_property(env, objValue, "SPLIT_TO_FULL", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::SPLIT_TO_FULL)));
    napi_set_named_property(env, objValue, "END", CreateJsValue(env,
        static_cast<int32_t>(SizeChangeReason::END)));

    return objValue;
}

napi_value CreateJsSessionRect(napi_env env, const WSRect& rect)
{
    WLOGFD("CreateJsSessionRect.");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to create object!");
        return NapiGetUndefined(env);
    }

    napi_set_named_property(env, objValue, "posX_", CreateJsValue(env, rect.posX_));
    napi_set_named_property(env, objValue, "posY_", CreateJsValue(env, rect.posY_));
    napi_set_named_property(env, objValue, "width_", CreateJsValue(env, rect.width_));
    napi_set_named_property(env, objValue, "height_", CreateJsValue(env, rect.height_));
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

static napi_value CreateJsSystemBarPropertyObject(
    napi_env env, const WindowType type, const SystemBarProperty& property)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get jsObject");
        return nullptr;
    }
    if (WINDOW_TO_JS_SESSION_TYPE_MAP.count(type) != 0) {
        napi_set_named_property(env, objValue, "type", CreateJsValue(env, WINDOW_TO_JS_SESSION_TYPE_MAP.at(type)));
    } else {
        napi_set_named_property(env, objValue, "type", CreateJsValue(env, type));
    }
    napi_set_named_property(env, objValue, "enable", CreateJsValue(env, property.enable_));
    std::string bkgColor = GetHexColor(property.backgroundColor_);
    napi_set_named_property(env, objValue, "backgroundcolor", CreateJsValue(env, bkgColor));
    std::string contentColor = GetHexColor(property.contentColor_);
    napi_set_named_property(env, objValue, "contentcolor", CreateJsValue(env, contentColor));

    return objValue;
}

napi_value CreateJsSystemBarPropertyArrayObject(
    napi_env env, const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    if (propertyMap.empty()) {
        WLOGFE("Empty propertyMap");
        return nullptr;
    }
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, propertyMap.size(), &arrayValue);
    if (arrayValue == nullptr) {
        WLOGFE("Failed to get jsArrayObject");
        return nullptr;
    }
    uint32_t index = 0;
    for (auto iter: propertyMap) {
        napi_set_element(env, arrayValue, index++, CreateJsSystemBarPropertyObject(env, iter.first, iter.second));
    }
    return arrayValue;
}

static void SetTypeProperty(napi_value object, napi_env env, const std::string& name, JsSessionType type)
{
    napi_set_named_property(env, object, name.c_str(), CreateJsValue(env, static_cast<int32_t>(type)));
}

napi_value SessionTypeInit(napi_env env)
{
    WLOGFD("SessionTypeInit");

    if (env == nullptr) {
        WLOGFE("Env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }
    SetTypeProperty(objValue, env, "TYPE_UNDEFINED", JsSessionType::TYPE_UNDEFINED);
    SetTypeProperty(objValue, env, "TYPE_APP", JsSessionType::TYPE_APP);
    SetTypeProperty(objValue, env, "TYPE_SUB_APP", JsSessionType::TYPE_SUB_APP);
    SetTypeProperty(objValue, env, "TYPE_SYSTEM_ALERT", JsSessionType::TYPE_SYSTEM_ALERT);
    SetTypeProperty(objValue, env, "TYPE_INPUT_METHOD", JsSessionType::TYPE_INPUT_METHOD);
    SetTypeProperty(objValue, env, "TYPE_STATUS_BAR", JsSessionType::TYPE_STATUS_BAR);
    SetTypeProperty(objValue, env, "TYPE_PANEL", JsSessionType::TYPE_PANEL);
    SetTypeProperty(objValue, env, "TYPE_KEYGUARD", JsSessionType::TYPE_KEYGUARD);
    SetTypeProperty(objValue, env, "TYPE_VOLUME_OVERLAY", JsSessionType::TYPE_VOLUME_OVERLAY);
    SetTypeProperty(objValue, env, "TYPE_NAVIGATION_BAR", JsSessionType::TYPE_NAVIGATION_BAR);
    SetTypeProperty(objValue, env, "TYPE_FLOAT", JsSessionType::TYPE_FLOAT);
    SetTypeProperty(objValue, env, "TYPE_WALLPAPER", JsSessionType::TYPE_WALLPAPER);
    SetTypeProperty(objValue, env, "TYPE_DESKTOP", JsSessionType::TYPE_DESKTOP);
    SetTypeProperty(objValue, env, "TYPE_LAUNCHER_DOCK", JsSessionType::TYPE_LAUNCHER_DOCK);
    SetTypeProperty(objValue, env, "TYPE_FLOAT_CAMERA", JsSessionType::TYPE_FLOAT_CAMERA);
    SetTypeProperty(objValue, env, "TYPE_DIALOG", JsSessionType::TYPE_DIALOG);
    SetTypeProperty(objValue, env, "TYPE_SCREENSHOT", JsSessionType::TYPE_SCREENSHOT);
    SetTypeProperty(objValue, env, "TYPE_TOAST", JsSessionType::TYPE_TOAST);
    SetTypeProperty(objValue, env, "TYPE_POINTER", JsSessionType::TYPE_POINTER);
    SetTypeProperty(objValue, env, "TYPE_LAUNCHER_RECENT", JsSessionType::TYPE_LAUNCHER_RECENT);
    SetTypeProperty(objValue, env, "TYPE_SCENE_BOARD", JsSessionType::TYPE_SCENE_BOARD);
    SetTypeProperty(objValue, env, "TYPE_DRAGGING_EFFECT", JsSessionType::TYPE_DRAGGING_EFFECT);
    SetTypeProperty(objValue, env, "TYPE_INPUT_METHOD_STATUS_BAR", JsSessionType::TYPE_INPUT_METHOD_STATUS_BAR);
    SetTypeProperty(objValue, env, "TYPE_GLOBAL_SEARCH", JsSessionType::TYPE_GLOBAL_SEARCH);
    SetTypeProperty(objValue, env, "TYPE_NEGATIVE_SCREEN", JsSessionType::TYPE_NEGATIVE_SCREEN);
    SetTypeProperty(objValue, env, "TYPE_VOICE_INTERACTION", JsSessionType::TYPE_VOICE_INTERACTION);
    SetTypeProperty(objValue, env, "TYPE_SYSTEM_TOAST", JsSessionType::TYPE_SYSTEM_TOAST);
    SetTypeProperty(objValue, env, "TYPE_SYSTEM_FLOAT", JsSessionType::TYPE_SYSTEM_FLOAT);
    SetTypeProperty(objValue, env, "TYPE_PIP", JsSessionType::TYPE_PIP);
    SetTypeProperty(objValue, env, "TYPE_THEME_EDITOR", JsSessionType::TYPE_THEME_EDITOR);
    SetTypeProperty(objValue, env, "TYPE_NAVIGATION_INDICATOR", JsSessionType::TYPE_NAVIGATION_INDICATOR);
    return objValue;
}


struct AsyncInfo {
    napi_env env;
    napi_async_work work;
    std::function<void()> func;
};

void NapiAsyncWork(napi_env env, std::function<void()> task)
{
    napi_value resource = nullptr;
    AsyncInfo* info = new AsyncInfo();
    info->env = env;
    info->func = task;
    napi_create_string_utf8(env, "AsyncWork", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
    },
    [](napi_env env, napi_status status, void* data) {
        AsyncInfo* info = (AsyncInfo*)data;
        info->func();
        napi_delete_async_work(env, info->work);
    }, (void*)info, &info->work);
    napi_queue_async_work(env, info->work);
    delete info;
}

MainThreadScheduler::MainThreadScheduler(napi_env env)
    : env_(env)
{
    GetMainEventHandler();
}

inline void MainThreadScheduler::GetMainEventHandler()
{
    if (handler_ != nullptr) {
        return;
    }
    auto runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
    if (runner == nullptr) {
        return;
    }
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
}

void MainThreadScheduler::PostMainThreadTask(Task&& localTask, std::string traceInfo, int64_t delayTime)
{
    GetMainEventHandler();
    auto task = [env = env_, localTask, traceInfo] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SCBCb:%s", traceInfo.c_str());
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        localTask();
        napi_close_handle_scope(env, scope);
    };
    if (handler_ && handler_->GetEventRunner()->IsCurrentRunnerThread()) {
        return task();
    } else if (handler_ && !handler_->GetEventRunner()->IsCurrentRunnerThread()) {
        handler_->PostTask(std::move(task), "wms:" + traceInfo, delayTime,
            OHOS::AppExecFwk::EventQueue::Priority::IMMEDIATE);
    } else {
        NapiAsyncWork(env_, task);
    }
}
} // namespace OHOS::Rosen
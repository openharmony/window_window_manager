/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "js_screen.h"

#include <cinttypes>
#include "screen.h"
#include "screen_info.h"
#include "window_manager_hilog.h"
#include "xpower_event_js.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsScreen"};
}

static thread_local std::map<ScreenId, std::shared_ptr<NativeReference>> g_JsScreenMap;
std::recursive_mutex g_mutex;

JsScreen::JsScreen(const sptr<Screen>& screen) : screen_(screen)
{
}

JsScreen::~JsScreen()
{
    WLOGI("JsScreen::~JsScreen is called");
}

void JsScreen::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("JsScreen::Finalizer is called");
    auto jsScreen = std::unique_ptr<JsScreen>(static_cast<JsScreen*>(data));
    if (jsScreen == nullptr) {
        WLOGFE("jsScreen::Finalizer jsScreen is null");
        return;
    }
    sptr<Screen> screen = jsScreen->screen_;
    if (screen == nullptr) {
        WLOGFE("JsScreen::Finalizer screen is null");
        return;
    }
    ScreenId screenId = screen->GetId();
    WLOGI("JsScreen::Finalizer screenId : %{public}" PRIu64"", screenId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsScreenMap.find(screenId) != g_JsScreenMap.end()) {
        WLOGI("JsScreen::Finalizer screen is destroyed: %{public}" PRIu64"", screenId);
        g_JsScreenMap.erase(screenId);
    }
}

napi_value JsScreen::SetOrientation(napi_env env, napi_callback_info info)
{
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(env, info);
    return (me != nullptr) ? me->OnSetOrientation(env, info) : nullptr;
}

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

napi_value JsScreen::OnSetOrientation(napi_env env, napi_callback_info info)
{
    WLOGI("OnSetOrientation is called");
    bool paramValidFlag = true;
    Orientation orientation = Orientation::UNSPECIFIED;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("OnSetOrientation Params not match, info argc: %{public}zu", argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(env, argv[0], orientation)) {
            paramValidFlag = false;
            WLOGFE("Failed to convert parameter to orientation");
        }
    }
    if (!paramValidFlag) {
        WLOGE("OnSetOrientation paramValidFlag error");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    if (orientation < Orientation::BEGIN || orientation > Orientation::END) {
        WLOGE("Orientation param error! orientation value must from enum Orientation");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(screen_->SetOrientation(orientation));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGI("OnSetOrientation success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                                                  "JsScreen::OnSetOrientation failed."));
                WLOGFE("OnSetOrientation failed");
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr &&
        GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreen::OnSetOrientation",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}


napi_value JsScreen::SetScreenActiveMode(napi_env env, napi_callback_info info)
{
    WLOGI("SetScreenActiveMode is called");
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(env, info);
    if (me != nullptr) {
        HiviewDFX::ReportXPowerJsStackSysEvent(env, "EPS_LCD_FREQ");
    }
    return (me != nullptr) ? me->OnSetScreenActiveMode(env, info) : nullptr;
}

napi_value JsScreen::OnSetScreenActiveMode(napi_env env, napi_callback_info info)
{
    WLOGI("OnSetScreenActiveMode is called");
    bool paramValidFlag = true;
    uint32_t modeId = 0;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("OnSetScreenActiveMode Params not match %{public}zu", argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(env, argv[0], modeId)) {
            WLOGFE("Failed to convert parameter to modeId");
            paramValidFlag = false;
        }
    }
    if (!paramValidFlag) {
        WLOGFE("OnSetScreenActiveMode paramValidFlag error");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(screen_->SetScreenActiveMode(modeId));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGI("OnSetScreenActiveMode success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                                                "JsScreen::OnSetScreenActiveMode failed."));
                WLOGFE("OnSetScreenActiveMode failed");
            }
        };

    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr &&
        GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }

    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreen::OnSetScreenActiveMode",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsScreen::SetDensityDpi(napi_env env, napi_callback_info info)
{
    WLOGI("SetDensityDpi is called");
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(env, info);
    return (me != nullptr) ? me->OnSetDensityDpi(env, info) : nullptr;
}

napi_value JsScreen::OnSetDensityDpi(napi_env env, napi_callback_info info)
{
    WLOGI("OnSetDensityDpi is called");
    bool paramValidFlag = true;
    uint32_t densityDpi = 0;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("OnSetDensityDpi Params not match %{public}zu", argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(env, argv[0], densityDpi)) {
            WLOGFE("Failed to convert parameter to densityDpi");
            paramValidFlag = false;
        }
    }
    if (!paramValidFlag) {
        WLOGFE("OnSetDensityDpi paramValidFlag error");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(screen_->SetDensityDpi(densityDpi));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGI("OnSetDensityDpi success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                                                "JsScreen::OnSetDensityDpi failed."));
                WLOGFE("OnSetDensityDpi failed");
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr &&
        GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreen::OnSetDensityDpi",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

std::shared_ptr<NativeReference> FindJsDisplayObject(ScreenId screenId)
{
    WLOGI("[NAPI]Try to find screen %{public}" PRIu64" in g_JsScreenMap", screenId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsScreenMap.find(screenId) == g_JsScreenMap.end()) {
        WLOGI("[NAPI]Can not find screen %{public}" PRIu64" in g_JsScreenMap", screenId);
        return nullptr;
    }
    return g_JsScreenMap[screenId];
}

napi_value CreateJsScreenObject(napi_env env, sptr<Screen>& screen)
{
    WLOGI("JsScreen::CreateJsScreen is called");
    napi_value objValue = nullptr;
    std::shared_ptr<NativeReference> jsScreenObj = FindJsDisplayObject(screen->GetId());
    if (jsScreenObj != nullptr && jsScreenObj->GetNapiValue() != nullptr) {
        WLOGI("[NAPI]FindJsScreenObject %{public}" PRIu64"", screen->GetId());
        objValue = jsScreenObj->GetNapiValue();
    }
    if (objValue == nullptr) {
        napi_create_object(env, &objValue);
    }
    if (objValue == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return NapiGetUndefined(env);
    }
    std::unique_ptr<JsScreen> jsScreen = std::make_unique<JsScreen>(screen);
    napi_wrap(env, objValue, jsScreen.release(), JsScreen::Finalizer, nullptr, nullptr);
    auto info = screen->GetScreenInfo();
    if (info == nullptr) {
        WLOGFE("Failed to GetScreenInfo");
        return NapiGetUndefined(env);
    }
    ScreenId screenId = info->GetScreenId();
    napi_set_named_property(env, objValue, "id",
        CreateJsValue(env, screenId == SCREEN_ID_INVALID ? -1 : static_cast<int64_t>(screenId)));
    ScreenId parentId = info->GetParentId();
    napi_set_named_property(env, objValue, "parent",
        CreateJsValue(env, parentId == SCREEN_ID_INVALID ? -1 : static_cast<int64_t>(parentId)));
    napi_set_named_property(env, objValue, "orientation", CreateJsValue(env, info->GetOrientation()));
    napi_set_named_property(env, objValue, "sourceMode", CreateJsValue(env, info->GetSourceMode()));
    napi_set_named_property(env, objValue, "activeModeIndex", CreateJsValue(env, info->GetModeId()));
    napi_set_named_property(env, objValue, "supportedModeInfo", CreateJsScreenModeArrayObject(env, info->GetModes()));
    napi_set_named_property(env, objValue, "densityDpi", CreateJsValue(env,
        static_cast<uint32_t>(info->GetVirtualPixelRatio() * DOT_PER_INCH))); // Dpi = Density(VPR) * 160.
    if (jsScreenObj == nullptr || jsScreenObj->GetNapiValue() == nullptr) {
        std::shared_ptr<NativeReference> JsScreenRef;
        napi_ref result = nullptr;
        napi_create_reference(env, objValue, 1, &result);
        JsScreenRef.reset(reinterpret_cast<NativeReference*>(result));
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        g_JsScreenMap[screenId] = JsScreenRef;
        const char *moduleName = "JsScreen";
        BindNativeFunction(env, objValue, "setScreenActiveMode", moduleName, JsScreen::SetScreenActiveMode);
        BindNativeFunction(env, objValue, "setOrientation", moduleName, JsScreen::SetOrientation);
        BindNativeFunction(env, objValue, "setDensityDpi", moduleName, JsScreen::SetDensityDpi);
    }
    return objValue;
}

napi_value CreateJsScreenModeArrayObject(napi_env env, std::vector<sptr<SupportedScreenModes>> screenModes)
{
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, screenModes.size(), &arrayValue);
    size_t i = 0;
    for (const auto& mode : screenModes) {
        napi_set_element(env, arrayValue, i++, CreateJsScreenModeObject(env, mode));
    }
    return arrayValue;
}

napi_value CreateJsScreenModeObject(napi_env env, const sptr<SupportedScreenModes>& mode)
{
    WLOGI("JsScreen::CreateJsScreenMode is called");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return NapiGetUndefined(env);
    }
    uint32_t id = mode->id_;
    uint32_t width = mode->width_;
    uint32_t height = mode->height_;
    uint32_t refreshRate = mode->refreshRate_;
    napi_set_named_property(env, objValue, "id", CreateJsValue(env, id));
    napi_set_named_property(env, objValue, "width", CreateJsValue(env, width));
    napi_set_named_property(env, objValue, "height", CreateJsValue(env, height));
    napi_set_named_property(env, objValue, "refreshRate", CreateJsValue(env, refreshRate));
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS

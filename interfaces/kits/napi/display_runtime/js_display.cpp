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

#include "js_display.h"

#include <cinttypes>
#include <map>

#include "cutout_info.h"
#include "display.h"
#include "display_info.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsDisplay"};
    const std::map<DisplayState,      DisplayStateMode> NATIVE_TO_JS_DISPLAY_STATE_MAP {
        { DisplayState::UNKNOWN,      DisplayStateMode::STATE_UNKNOWN      },
        { DisplayState::OFF,          DisplayStateMode::STATE_OFF          },
        { DisplayState::ON,           DisplayStateMode::STATE_ON           },
        { DisplayState::DOZE,         DisplayStateMode::STATE_DOZE         },
        { DisplayState::DOZE_SUSPEND, DisplayStateMode::STATE_DOZE_SUSPEND },
        { DisplayState::VR,           DisplayStateMode::STATE_VR           },
        { DisplayState::ON_SUSPEND,   DisplayStateMode::STATE_ON_SUSPEND   },
    };
}

static thread_local std::map<DisplayId, std::shared_ptr<NativeReference>> g_JsDisplayMap;
std::recursive_mutex g_mutex;

JsDisplay::JsDisplay(const sptr<Display>& display) : display_(display)
{
}

JsDisplay::~JsDisplay()
{
    WLOGI("JsDisplay::~JsDisplay is called");
}

void JsDisplay::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("JsDisplay::Finalizer is called");
    auto jsDisplay = std::unique_ptr<JsDisplay>(static_cast<JsDisplay*>(data));
    if (jsDisplay == nullptr) {
        WLOGFE("JsDisplay::Finalizer jsDisplay is null");
        return;
    }
    sptr<Display> display = jsDisplay->display_;
    if (display == nullptr) {
        WLOGFE("JsDisplay::Finalizer display is null");
        return;
    }
    DisplayId displayId = display->GetId();
    WLOGI("JsDisplay::Finalizer displayId : %{public}" PRIu64"", displayId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsDisplayMap.find(displayId) != g_JsDisplayMap.end()) {
        WLOGI("JsDisplay::Finalizer Display is destroyed: %{public}" PRIu64"", displayId);
        g_JsDisplayMap.erase(displayId);
    }
}

napi_value JsDisplay::GetCutoutInfo(napi_env env, napi_callback_info info)
{
    WLOGI("GetCutoutInfo is called");
    JsDisplay* me = CheckParamsAndGetThis<JsDisplay>(env, info);
    return (me != nullptr) ? me->OnGetCutoutInfo(env, info) : nullptr;
}

napi_valuetype GetType(napi_env env, napi_value value)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, value, &res);
    return res;
}

napi_value JsDisplay::OnGetCutoutInfo(napi_env env, napi_callback_info info)
{
    WLOGI("OnGetCutoutInfo is called");
    NapiAsyncTask::CompleteCallback complete =
        [this](napi_env env, NapiAsyncTask& task, int32_t status) {
            sptr<CutoutInfo> cutoutInfo = display_->GetCutoutInfo();
            if (cutoutInfo != nullptr) {
                task.Resolve(env, CreateJsCutoutInfoObject(env, cutoutInfo));
                WLOGI("JsDisplay::OnGetCutoutInfo success");
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN), "JsDisplay::OnGetCutoutInfo failed."));
            }
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = nullptr;
    if (argc >= ARGC_ONE && argv[ARGC_ONE - 1] != nullptr &&
        GetType(env, argv[ARGC_ONE - 1]) == napi_function) {
        lastParam = argv[ARGC_ONE - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsDisplay::OnGetCutoutInfo",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

std::shared_ptr<NativeReference> FindJsDisplayObject(DisplayId displayId)
{
    WLOGI("[NAPI]Try to find display %{public}" PRIu64" in g_JsDisplayMap", displayId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsDisplayMap.find(displayId) == g_JsDisplayMap.end()) {
        WLOGI("[NAPI]Can not find display %{public}" PRIu64" in g_JsDisplayMap", displayId);
        return nullptr;
    }
    return g_JsDisplayMap[displayId];
}

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value CreateJsCutoutInfoObject(napi_env env, sptr<CutoutInfo> cutoutInfo)
{
    WLOGI("JsDisplay::CreateJsCutoutInfoObject is called");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return NapiGetUndefined(env);

    }
    if (cutoutInfo == nullptr) {
        WLOGFE("Get null cutout info");
        return NapiGetUndefined(env);
    }
    std::vector<DMRect> boundingRects = cutoutInfo->GetBoundingRects();
    WaterfallDisplayAreaRects waterfallDisplayAreaRects = cutoutInfo->GetWaterfallDisplayAreaRects();
    napi_set_named_property(env, objValue, "boundingRects", CreateJsBoundingRectsArrayObject(env, boundingRects));
    napi_set_named_property(env, objValue, "waterfallDisplayAreaRects",
        CreateJsWaterfallDisplayAreaRectsObject(env, waterfallDisplayAreaRects));
    return objValue;
}

napi_value CreateJsRectObject(napi_env env, DMRect rect)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    napi_set_named_property(env, objValue, "left", CreateJsValue(env, rect.posX_));
    napi_set_named_property(env, objValue, "top", CreateJsValue(env, rect.posY_));
    napi_set_named_property(env, objValue, "width", CreateJsValue(env, rect.width_));
    napi_set_named_property(env, objValue, "height", CreateJsValue(env, rect.height_));
    return objValue;
}

napi_value CreateJsWaterfallDisplayAreaRectsObject(napi_env env,
    WaterfallDisplayAreaRects waterfallDisplayAreaRects)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    napi_set_named_property(env, objValue, "left", CreateJsRectObject(env, waterfallDisplayAreaRects.left));
    napi_set_named_property(env, objValue, "top", CreateJsRectObject(env, waterfallDisplayAreaRects.top));
    napi_set_named_property(env, objValue, "right", CreateJsRectObject(env, waterfallDisplayAreaRects.right));
    napi_set_named_property(env, objValue, "bottom", CreateJsRectObject(env, waterfallDisplayAreaRects.bottom));
    return objValue;
}

napi_value CreateJsBoundingRectsArrayObject(napi_env env, std::vector<DMRect> boundingRects)
{
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, boundingRects.size(), &arrayValue);
    size_t i = 0;
    for (const auto& rect : boundingRects) {
        napi_set_element(env, arrayValue, i++, CreateJsRectObject(env, rect));
    }
    return arrayValue;
}

void NapiSetNamedProperty(napi_env env, napi_value objValue, sptr<DisplayInfo> info) {
    napi_set_named_property(env, objValue, "id", CreateJsValue(env, static_cast<uint32_t>(info->GetDisplayId())));
    napi_set_named_property(env, objValue, "name", CreateJsValue(env, info->GetName()));
    napi_set_named_property(env, objValue, "alive", CreateJsValue(env, info->GetAliveStatus()));
    if (NATIVE_TO_JS_DISPLAY_STATE_MAP.count(info->GetDisplayState()) != 0) {
        napi_set_named_property(env, objValue, "state",
            CreateJsValue(env, NATIVE_TO_JS_DISPLAY_STATE_MAP.at(info->GetDisplayState())));
    } else {
        napi_set_named_property(env, objValue, "state", CreateJsValue(env, DisplayStateMode::STATE_UNKNOWN));
    }
    napi_set_named_property(env, objValue, "refreshRate", CreateJsValue(env, info->GetRefreshRate()));
    napi_set_named_property(env, objValue, "rotation", CreateJsValue(env, info->GetRotation()));
    napi_set_named_property(env, objValue, "width", CreateJsValue(env, info->GetWidth()));
    napi_set_named_property(env, objValue, "height", CreateJsValue(env, info->GetHeight()));
    napi_set_named_property(env, objValue, "densityDPI",
        CreateJsValue(env, info->GetVirtualPixelRatio() * DOT_PER_INCH));
    napi_set_named_property(env, objValue, "orientation", CreateJsValue(env, info->GetDisplayOrientation()));
    napi_set_named_property(env, objValue, "densityPixels", CreateJsValue(env, info->GetVirtualPixelRatio()));
    napi_set_named_property(env, objValue, "scaledDensity", CreateJsValue(env, info->GetVirtualPixelRatio()));
    napi_set_named_property(env, objValue, "xDPI", CreateJsValue(env, info->GetXDpi()));
    napi_set_named_property(env, objValue, "yDPI", CreateJsValue(env, info->GetYDpi()));
}

napi_value CreateJsDisplayObject(napi_env env, sptr<Display>& display)
{
    WLOGI("CreateJsDisplay is called");
    napi_value objValue = nullptr;
    std::shared_ptr<NativeReference> jsDisplayObj = FindJsDisplayObject(display->GetId());
    if (jsDisplayObj != nullptr && jsDisplayObj->GetNapiValue() != nullptr) {
        WLOGI("[NAPI]FindJsDisplayObject %{public}" PRIu64"", display->GetId());
        objValue = jsDisplayObj->GetNapiValue();
    }
    if (objValue == nullptr) {
        napi_create_object(env, &objValue);
    }
    if (objValue == nullptr) {
        WLOGFE("Failed to get jsObject");
        return NapiGetUndefined(env);
    }
    auto info = display->GetDisplayInfo();
    if (info == nullptr) {
        WLOGFE("Failed to GetDisplayInfo");
        return NapiGetUndefined(env);
    }

    NapiSetNamedProperty(env, objValue, info);

    if (jsDisplayObj == nullptr || jsDisplayObj->GetNapiValue() == nullptr) {
        std::unique_ptr<JsDisplay> jsDisplay = std::make_unique<JsDisplay>(display);
        napi_wrap(env, objValue, jsDisplay.release(), JsDisplay::Finalizer, nullptr, nullptr);
        BindNativeFunction(env, objValue, "getCutoutInfo", "JsDisplay", JsDisplay::GetCutoutInfo);
        std::shared_ptr<NativeReference> jsDisplayRef;
        napi_ref result = nullptr;
        napi_create_reference(env, objValue, 1, &result);
        jsDisplayRef.reset(reinterpret_cast<NativeReference*>(result));
        DisplayId displayId = display->GetId();
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        g_JsDisplayMap[displayId] = jsDisplayRef;
    }
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS

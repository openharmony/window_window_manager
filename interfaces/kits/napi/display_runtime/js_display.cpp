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
#include <set>

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

using GraphicCM_ColorSpaceType = enum {
    GRAPHIC_CM_COLORSPACE_NONE,

    GRAPHIC_CM_BT601_EBU_FULL      = 2 | (1 << 8) | (2 << 16) | (1 << 21),  // COLORPRIMARIES_BT601_P | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_P << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_BT601_SMPLE_C_FULL  = 3 | (1 << 8) | (3 << 16) | (1 << 21),  // COLORPRIMARIES_BT601_N | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_N << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_BT709_FULL          = 1 | (1 << 8) | (1 << 16) | (1 << 21),  // COLORPRIMARIES_BT709   | (TRANSFUNC_BT709 << 8) | (MATRIX_BT709   << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_BT2020_HLG_FULL     = 4 | (5 << 8) | (4 << 16) | (1 << 21),  // COLORPRIMARIES_BT2020  | (TRANSFUNC_HLG   << 8) | (MATRIX_BT2020  << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_BT2020_PQ_FULL      = 4 | (4 << 8) | (4 << 16) | (1 << 21),  // COLORPRIMARIES_BT2020  | (TRANSFUNC_PQ    << 8) | (MATRIX_BT2020  << 16) | (RANGE_FULL << 21)

    GRAPHIC_CM_BT601_EBU_LIMIT     = 2 | (1 << 8) | (2 << 16) | (2 << 21),  // COLORPRIMARIES_BT601_P | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_P << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_BT601_SMPLE_C_LIMIT = 3 | (1 << 8) | (3 << 16) | (2 << 21),  // COLORPRIMARIES_BT601_N | (TRANSFUNC_BT709 << 8) | (MATRIX_BT601_N << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_BT709_LIMIT         = 1 | (1 << 8) | (1 << 16) | (2 << 21),  // COLORPRIMARIES_BT709   | (TRANSFUNC_BT709 << 8) | (MATRIX_BT709   << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_BT2020_HLG_LIMIT    = 4 | (5 << 8) | (4 << 16) | (2 << 21),  // COLORPRIMARIES_BT2020  | (TRANSFUNC_HLG   << 8) | (MATRIX_BT2020  << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_BT2020_PQ_LIMIT     = 4 | (4 << 8) | (4 << 16) | (2 << 21),  // COLORPRIMARIES_BT2020  | (TRANSFUNC_PQ    << 8) | (MATRIX_BT2020  << 16) | (RANGE_LIMITED << 21)

    GRAPHIC_CM_SRGB_FULL           = 1 | (2 << 8) | (3 << 16) | (1 << 21),  // COLORPRIMARIES_SRGB     | (TRANSFUNC_SRGB     << 8) | (MATRIX_BT601_N  << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_P3_FULL             = 6 | (2 << 8) | (3 << 16) | (1 << 21),  // COLORPRIMARIES_P3_D65   | (TRANSFUNC_SRGB     << 8) | (MATRIX_P3       << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_P3_HLG_FULL         = 6 | (5 << 8) | (3 << 16) | (1 << 21),  // COLORPRIMARIES_P3_D65   | (TRANSFUNC_HLG      << 8) | (MATRIX_P3       << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_P3_PQ_FULL          = 6 | (4 << 8) | (3 << 16) | (1 << 21),  // COLORPRIMARIES_P3_D65   | (TRANSFUNC_PQ       << 8) | (MATRIX_P3       << 16) | (RANGE_FULL << 21)
    GRAPHIC_CM_ADOBERGB_FULL       = 23 | (6 << 8) | (0 << 16) | (1 << 21), // COLORPRIMARIES_ADOBERGB | (TRANSFUNC_ADOBERGB << 8) | (MATRIX_ADOBERGB << 16) | (RANGE_FULL << 21)

    GRAPHIC_CM_SRGB_LIMIT          = 1 | (2 << 8) | (3 << 16) | (2 << 21),  // COLORPRIMARIES_SRGB     | (TRANSFUNC_SRGB     << 8) | (MATRIX_BT601_N  << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_P3_LIMIT            = 6 | (2 << 8) | (3 << 16) | (2 << 21),  // COLORPRIMARIES_P3_D65   | (TRANSFUNC_SRGB     << 8) | (MATRIX_P3       << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_P3_HLG_LIMIT        = 6 | (5 << 8) | (3 << 16) | (2 << 21),  // COLORPRIMARIES_P3_D65   | (TRANSFUNC_HLG      << 8) | (MATRIX_P3       << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_P3_PQ_LIMIT         = 6 | (4 << 8) | (3 << 16) | (2 << 21),  // COLORPRIMARIES_P3_D65   | (TRANSFUNC_PQ       << 8) | (MATRIX_P3       << 16) | (RANGE_LIMITED << 21)
    GRAPHIC_CM_ADOBERGB_LIMIT      = 23 | (6 << 8) | (0 << 16) | (2 << 21), // COLORPRIMARIES_ADOBERGB | (TRANSFUNC_ADOBERGB << 8) | (MATRIX_ADOBERGB << 16) | (RANGE_LIMITED << 21)

    GRAPHIC_CM_LINEAR_SRGB         = 1 | (3 << 8),                          // COLORPRIMARIES_SRGB   | (TRANSFUNC_LINEAR << 8)
    GRAPHIC_CM_LINEAR_BT709        = 1 | (3 << 8),                          // equal to GRAPHIC_CM_LINEAR_SRGB
    GRAPHIC_CM_LINEAR_P3           = 6 | (3 << 8),                          // COLORPRIMARIES_P3_D65 | (TRANSFUNC_LINEAR << 8)
    GRAPHIC_CM_LINEAR_BT2020       = 4 | (3 << 8),                          // COLORPRIMARIES_BT2020 | (TRANSFUNC_LINEAR << 8)

    GRAPHIC_CM_DISPLAY_SRGB        = 1 | (2 << 8) | (3 << 16) | (1 << 21),  // equal to GRAPHIC_CM_SRGB_FULL
    GRAPHIC_CM_DISPLAY_P3_SRGB     = 6 | (2 << 8) | (3 << 16) | (1 << 21),  // equal to GRAPHIC_CM_P3_FULL
    GRAPHIC_CM_DISPLAY_P3_HLG      = 6 | (5 << 8) | (3 << 16) | (1 << 21),  // equal to GRAPHIC_CM_P3_HLG_FULL
    GRAPHIC_CM_DISPLAY_P3_PQ       = 6 | (4 << 8) | (3 << 16) | (1 << 21),  // equal to GRAPHIC_CM_P3_PQ_FULL
    GRAPHIC_CM_DISPLAY_BT2020_SRGB = 4 | (2 << 8) | (4 << 16) | (1 << 21),  // COLORPRIMARIES_BT2020   | (TRANSFUNC_SRGB << 8)     | (MATRIX_BT2020 << 16)   | (RANGE_FULL << 21)
    GRAPHIC_CM_DISPLAY_BT2020_HLG  = 4 | (5 << 8) | (4 << 16) | (1 << 21),  // equal to GRAPHIC_CM_BT2020_HLG_FULL
    GRAPHIC_CM_DISPLAY_BT2020_PQ   = 4 | (4 << 8) | (4 << 16) | (1 << 21)   // equal to GRAPHIC_CM_BT2020_PQ_FULL
};

typedef enum : uint32_t {
    NOT_SUPPORT_HDR = 0,
    VIDEO_HLG,
    VIDEO_HDR10,
    VIDEO_HDR_VIVID,
    IMAGE_HDR_VIVID_DUAL,
    IMAGE_HDR_VIVID_SINGLE,
    IMAGE_HDR_ISO_DUAL,
    IMAGE_HDR_ISO_SINGLE,
} ScreenHDRFormat;

const std::map<GraphicCM_ColorSpaceType, ColorSpace> NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP {
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE,        ColorSpace::UNKNOWN },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_FULL,          ColorSpace::ADOBE_RGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_LIMIT,         ColorSpace::ADOBE_RGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_FULL,        ColorSpace::BT2020_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_LIMIT,       ColorSpace::BT2020_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_BT2020_HLG,     ColorSpace::BT2020_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_FULL,         ColorSpace::BT2020_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_LIMIT,        ColorSpace::BT2020_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_BT2020_PQ,      ColorSpace::BT2020_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL,         ColorSpace::BT601_EBU },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_LIMIT,        ColorSpace::BT601_EBU },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPLE_C_FULL,     ColorSpace::BT601_SMPTE_C },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPLE_C_LIMIT,    ColorSpace::BT601_SMPTE_C },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_FULL,             ColorSpace::BT709 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_LIMIT,            ColorSpace::BT709 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_FULL,            ColorSpace::P3_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_LIMIT,           ColorSpace::P3_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_HLG,         ColorSpace::P3_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_FULL,             ColorSpace::P3_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_LIMIT,            ColorSpace::P3_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_PQ,          ColorSpace::P3_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL,                ColorSpace::DISPLAY_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_LIMIT,               ColorSpace::DISPLAY_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_SRGB,        ColorSpace::DISPLAY_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL,              ColorSpace::SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_LIMIT,             ColorSpace::SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_SRGB,           ColorSpace::SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_SRGB,            ColorSpace::LINEAR_SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_BT709,           ColorSpace::LINEAR_SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_P3,              ColorSpace::LINEAR_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_BT2020,          ColorSpace::LINEAR_BT2020 },
};

const std::map<ScreenHDRFormat, HDRFormat> NATIVE_TO_JS_HDR_FORMAT_TYPE_MAP {
    { ScreenHDRFormat::NOT_SUPPORT_HDR,             HDRFormat::NONE },
    { ScreenHDRFormat::VIDEO_HLG,                   HDRFormat::VIDEO_HLG },
    { ScreenHDRFormat::VIDEO_HDR10,                 HDRFormat::VIDEO_HDR10 },
    { ScreenHDRFormat::VIDEO_HDR_VIVID,             HDRFormat::VIDEO_HDR_VIVID },
    { ScreenHDRFormat::IMAGE_HDR_VIVID_DUAL,        HDRFormat::IMAGE_HDR_VIVID_DUAL },
    { ScreenHDRFormat::IMAGE_HDR_VIVID_SINGLE,      HDRFormat::IMAGE_HDR_VIVID_SINGLE },
    { ScreenHDRFormat::IMAGE_HDR_ISO_DUAL,          HDRFormat::IMAGE_HDR_ISO_DUAL },
    { ScreenHDRFormat::IMAGE_HDR_ISO_SINGLE,        HDRFormat::IMAGE_HDR_ISO_SINGLE },
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

napi_value JsDisplay::HasImmersiveWindow(napi_env env, napi_callback_info info)
{
    WLOGI("HasImmersiveWindow is called");
    JsDisplay* me = CheckParamsAndGetThis<JsDisplay>(env, info);
    return (me != nullptr) ? me->OnHasImmersiveWindow(env, info) : nullptr;
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

napi_value JsDisplay::OnHasImmersiveWindow(napi_env env, napi_callback_info info)
{
    WLOGI("OnHasImmersiveWindow is called");
    NapiAsyncTask::CompleteCallback complete =
        [this](napi_env env, NapiAsyncTask& task, int32_t status) {
            bool immersive = false;
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(display_->HasImmersiveWindow(immersive));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, CreateJsValue(env, immersive));
                WLOGI("JsDisplay::OnHasImmersiveWindow success - immersive window exists: %{public}d", immersive);
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ret), "JsDisplay::OnHasImmersiveWindow failed."));
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
    NapiAsyncTask::Schedule("JsDisplay::OnHasImmersiveWindow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsDisplay::GetSupportedColorSpaces(napi_env env, napi_callback_info info)
{
    WLOGI("GetSupportedColorSpaces is called");
    JsDisplay* me = CheckParamsAndGetThis<JsDisplay>(env, info);
    return (me != nullptr) ? me->OnGetSupportedColorSpaces(env, info) : nullptr;
}

static napi_value CreateJsColorSpaceArray(napi_env env, const std::vector<uint32_t>& colorSpaces)
{
    WLOGI("JsDisplay::CreateJsColorSpaceArray is called");
    std::set<uint32_t> nativeColorSpaces;
    for (const auto colorSpace : colorSpaces) {
        GraphicCM_ColorSpaceType colorSpaceValue = static_cast<GraphicCM_ColorSpaceType>(colorSpace);
        if (NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.count(colorSpaceValue) == 0) {
            WLOGFE("Get color space name %{public}u, but not in api type", colorSpace);
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(DMError::DM_ERROR_DEVICE_NOT_SUPPORT)));
            return NapiGetUndefined(env);
        }
        nativeColorSpaces.insert(static_cast<uint32_t>(NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP.at(colorSpaceValue)));
    }
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, nativeColorSpaces.size(), &arrayValue);
    if (arrayValue == nullptr) {
        WLOGFE("Failed to create color space array");
        return NapiGetUndefined(env);
    }
    uint32_t index = 0;
    for (const auto nativeColorSpace : nativeColorSpaces) {
        napi_set_element(env, arrayValue, index++, CreateJsValue(env, nativeColorSpace));
    }
    return arrayValue;
}

napi_value JsDisplay::OnGetSupportedColorSpaces(napi_env env, napi_callback_info info)
{
    WLOGI("OnGetSupportedColorSpaces is called");
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            std::vector<uint32_t> colorSpaces;
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(display_->GetSupportedColorSpaces(colorSpaces));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, CreateJsColorSpaceArray(env, colorSpaces));
                WLOGI("OnGetSupportedColorSpaces success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                                               "JsDisplay::OnGetSupportedColorSpaces failed."));
                WLOGFE("OnGetSupportedColorSpaces failed");
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
    NapiAsyncTask::Schedule("JsDisplay::OnGetSupportedColorSpaces",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsDisplay::GetSupportedHDRFormats(napi_env env, napi_callback_info info)
{
    WLOGI("GetSupportedHDRFormats is called");
    JsDisplay* me = CheckParamsAndGetThis<JsDisplay>(env, info);
    return (me != nullptr) ? me->OnGetSupportedHDRFormats(env, info) : nullptr;
}

static napi_value CreateJsHDRFormatArray(napi_env env, const std::vector<uint32_t>& hdrFormats)
{
    WLOGI("JsDisplay::CreateJsHDRFormatArray is called");
    std::set<uint32_t> nativeHDRFormats;
    for (const auto hdrFormat : hdrFormats) {
        ScreenHDRFormat hdrFormatValue = static_cast<ScreenHDRFormat>(hdrFormat);
        if (NATIVE_TO_JS_HDR_FORMAT_TYPE_MAP.count(hdrFormatValue) == 0) {
            WLOGFE("Get HDR format name %{public}u, but not in api type", hdrFormat);
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(DMError::DM_ERROR_DEVICE_NOT_SUPPORT)));
            return NapiGetUndefined(env);
        }
        nativeHDRFormats.insert(static_cast<uint32_t>(NATIVE_TO_JS_HDR_FORMAT_TYPE_MAP.at(hdrFormatValue)));
    }
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, hdrFormats.size(), &arrayValue);
    if (arrayValue == nullptr) {
        WLOGFE("Failed to create HDR format array");
        return NapiGetUndefined(env);
    }
    uint32_t index = 0;
    for (const auto nativeHDRFormat : nativeHDRFormats) {
        napi_set_element(env, arrayValue, index++, CreateJsValue(env, nativeHDRFormat));
    }
    return arrayValue;
}

napi_value JsDisplay::OnGetSupportedHDRFormats(napi_env env, napi_callback_info info)
{
    WLOGI("OnGetSupportedHDRFormats is called");
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            std::vector<uint32_t> hdrFormats;
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(display_->GetSupportedHDRFormats(hdrFormats));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, CreateJsHDRFormatArray(env, hdrFormats));
                WLOGI("OnGetSupportedHDRFormats success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                                               "JsDisplay::OnGetSupportedHDRFormats failed."));
                WLOGFE("OnGetSupportedHDRFormats failed");
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
    NapiAsyncTask::Schedule("JsDisplay::OnGetSupportedHDRFormats",
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

void NapiSetNamedProperty(napi_env env, napi_value objValue, sptr<DisplayInfo> info)
{
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
        BindNativeFunction(env, objValue, "hasImmersiveWindow", "JsDisplay", JsDisplay::HasImmersiveWindow);
        BindNativeFunction(env, objValue, "getSupportedColorSpaces", "JsDisplay", JsDisplay::GetSupportedColorSpaces);
        BindNativeFunction(env, objValue, "getSupportedHDRFormats", "JsDisplay", JsDisplay::GetSupportedHDRFormats);
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

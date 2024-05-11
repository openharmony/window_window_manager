
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
#include "window_ffi.h"
#include <transaction/rs_interfaces.h>
#include "pixel_map_impl.h"
#include "window_manager_hilog.h"

using namespace OHOS::FFI;
using namespace OHOS::Media;

namespace OHOS {
namespace Rosen {
extern "C" {
const int32_t WM_ERROR_STATE_ABNORMALLY = 1300002;

RetDataI64 FfiOHOSCreateWindow(char* name, uint32_t windowType, OHOS::AbilityRuntime::Context* context,
    int64_t displayId, int64_t parentId)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] CreateWindow start");
    std::string nm = name;
    RetDataI64 ret;
    WindowParameters window;
    window.name = nm;
    window.winType = windowType;
    window.context = context;
    window.displayId = displayId;
    window.parentId = parentId;
    window.windowId = &ret.data;
    ret.code = OHOS::Rosen::WindowManagerImpl::CreateWindow(window);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] CreateWindow success");
    return ret;
}

int32_t FfiOHOSWindowSetWindowLayoutMode(uint32_t mode)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowLayoutMode start");
    int32_t ret = OHOS::Rosen::WindowManagerImpl::SetWindowLayoutMode(mode);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowLayoutMode success");
    return ret;
}

int32_t FfiOHOSWindowMinimizeAll(int64_t displayId)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] MinimizeAll start");
    int32_t ret = OHOS::Rosen::WindowManagerImpl::MinimizeAll(displayId);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] MinimizeAll success");
    return ret;
}

RetDataI64 FfiOHOSGetLastWindow(OHOS::AbilityRuntime::Context* ctx)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] GetLastWindow start");
    RetDataI64 ret;
    ret.code = OHOS::Rosen::WindowManagerImpl::GetLastWindow(ctx, ret.data);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] GetLastWindow success");
    return ret;
}

// window
int32_t FfiOHOSWindowHide(int64_t id)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] Hide start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->Hide();
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] Hide success");
    return res;
}

int32_t FfiOHOSWindowHideWithAnimation(int64_t id)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] HideWithAnimation start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->HideWithAnimation();
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] HideWithAnimation success");
    return res;
}

int32_t FfiOHOSWindowShowWindow(int64_t id)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] ShowWindow start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->ShowWindow();
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] ShowWindow success");
    return res;
}

int32_t FfiOHOSWindowShowWithAnimation(int64_t id)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] ShowWithAnimation start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->ShowWithAnimation();
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] ShowWithAnimation success");
    return res;
}

int32_t FfiOHOSWindowDestroyWindow(int64_t id)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] DestroyWindow start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->DestroyWindow();
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] DestroyWindow success");
    return res;
}

int32_t FfiOHOSWindowMoveWindowTo(int64_t id, int32_t x, int32_t y)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] MoveWindowTo start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->MoveWindowTo(x, y);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] MoveWindowTo success");
    return res;
}

int32_t FfiOHOSWindowResize(int64_t id, uint32_t width, uint32_t height)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] Resize start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->Resize(width, height);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] Resize success");
    return res;
}

int32_t FfiOHOSWindowSetWindowMode(int64_t id, uint32_t mode)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowMode start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWindowMode(mode);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowMode success");
    return res;
}

CWindowProperties FfiOHOSWindowGetWindowProperties(int64_t id, int32_t* errCode)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] GetWindowProperties start");
    CWindowProperties res;
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        *errCode = WM_ERROR_STATE_ABNORMALLY;
        return res;
    }
    res = instance->GetWindowProperties(errCode);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] GetWindowProperties success");
    return res;
}

int32_t FfiOHOSWindowSetWindowLayoutFullScreen(int64_t id, bool isLayoutFullScreen)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowLayoutFullScreen start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWindowLayoutFullScreen(isLayoutFullScreen);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowLayoutFullScreen success");
    return res;
}

int32_t FfiOHOSSetWindowBackgroundColor(int64_t id, const char* color)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowBackgroundColor start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t ret = instance->SetWindowBackgroundColor(color);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowBackgroundColor success");
    return ret;
}

int32_t FfiOHOSWindowSetWindowBrightness(int64_t id, float brightness)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowBrightness start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWindowBrightness(brightness);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowBrightness success");
    return res;
}

int32_t FfiOHOSWindowSetWindowFocusable(int64_t id, bool focusable)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowFocusable start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWindowFocusable(focusable);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowFocusable success");
    return res;
}

int32_t FfiOHOSWindowSetWindowKeepScreenOn(int64_t id, bool keepScreenOn)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowKeepScreenOn start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWindowKeepScreenOn(keepScreenOn);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowKeepScreenOn success");
    return res;
}

int32_t FfiOHOSWindowSetWindowPrivacyMode(int64_t id, bool isPrivacyMode)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowPrivacyMode start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWindowPrivacyMode(isPrivacyMode);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowPrivacyMode success");
    return res;
}

int32_t FfiOHOSWindowSetWindowTouchable(int64_t id, bool touchable)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowTouchable start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWindowTouchable(touchable);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetWindowTouchable success");
    return res;
}

int32_t FfiOHOSWindowSetForbidSplitMove(int64_t id, bool isForbidSplitMove)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetForbidSplitMove start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetForbidSplitMove(isForbidSplitMove);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] SetForbidSplitMove success");
    return res;
}

bool FfiOHOSWindowIsWindowSupportWideGamut(int64_t id, int32_t* errCode)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] IsWindowSupportWideGamut start");
    bool res = false;
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        *errCode = WM_ERROR_STATE_ABNORMALLY;
        return res;
    }
    res = instance->IsWindowSupportWideGamut(errCode);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] IsWindowSupportWideGamut success");
    return res;
}

bool FfiOHOSWindowIsWindowShowing(int64_t id, int32_t* errCode)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] IsWindowShowing start");
    bool res = false;
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] instance not exist %{public}lld", id);
        *errCode = WM_ERROR_STATE_ABNORMALLY;
        return res;
    }
    res = instance->IsWindowShowing(errCode);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] IsWindowShowing success");
    return res;
}

RetDataI64 FFiOHOSWindowFindWindow(char* name)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowFindWindow start");
    RetDataI64 ret;
    ret.code = WindowManagerImpl::FindWindow(name, ret.data);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowFindWindow success");
    return ret;
}

int32_t FFiOHOSWindowToggleShownStateForAllAppWindows()
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowToggleShownStateForAllAppWindows start");
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<WindowManager>().ToggleShownStateForAllAppWindows());
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowToggleShownStateForAllAppWindows error %{public}d", ret);
        return static_cast<int32_t>(ret);
    }
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowToggleShownStateForAllAppWindows success");
    return SUCCESS_CODE;
}

int32_t FFiOHOSWindowSetGestureNavigationEnabled(bool enable)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetGestureNavigationEnabled start");
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<WindowManager>().SetGestureNavigaionEnabled(enable));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetGestureNavigationEnabled error %{public}d", ret);
        return static_cast<int32_t>(ret);
    }
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetGestureNavigationEnabled success");
    return SUCCESS_CODE;
}

int32_t FFiOHOSWindowSetWaterMarkImage(int64_t pixelMapId, bool enable)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetWaterMarkImage start");
    auto pixelMapImpl = FFIData::GetData<PixelMapImpl>(pixelMapId);
    if (pixelMapImpl == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    auto pixelMap = pixelMapImpl->GetRealPixelMap();
    RSInterfaces::GetInstance().ShowWatermark(pixelMap, enable);
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetWaterMarkImage success");
    return SUCCESS_CODE;
}

int32_t FFiOHOSWindowSetBackdropBlurStyle(int64_t id, uint32_t blurStyle)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetBackdropBlurStyle start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (instance == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetBackdropBlurStyle(blurStyle);
    if (res == SUCCESS_CODE) {
        TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetBackdropBlurStyle success");
    }
    return res;
}

int32_t FFiOHOSWindowSetPreferredOrientation(int64_t id, uint32_t orientation)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetPreferredOrientation start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (instance == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetPreferredOrientation(orientation);
    if (res == SUCCESS_CODE) {
        TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetPreferredOrientation success");
    }
    return res;
}

int32_t FFiOHOSWindowGetWindowAvoidArea(int64_t id, uint32_t areaType,  OHOS::Rosen::CAvoidArea *retPtr)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowGetWindowAvoidArea start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (instance == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->GetWindowAvoidArea(areaType, retPtr);
    if (res == SUCCESS_CODE) {
        TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowGetWindowAvoidArea success");
    }
    return res;
}

int32_t FFiOHOSWindowSetWaterMarkFlag(int64_t id, bool enable)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetWaterMarkFlag start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (instance == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetWaterMarkFlag(enable);
    if (res == SUCCESS_CODE) {
        TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetWaterMarkFlag success");
    }
    return res;
}

int32_t FFiOHOSWindowSetAspectRatio(int64_t id, double ratio)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetAspectRatio start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (instance == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetAspectRatio(ratio);
    if (res == SUCCESS_CODE) {
        TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetAspectRatio success");
    }
    return res;
}

int32_t FFiOHOSWindowSetShadowRadius(int64_t id, double radius)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetShadowRadius start");
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (instance == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetShadowRadius(radius);
    if (res == SUCCESS_CODE) {
        TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetShadowRadius success");
    }
    return res;
}

int32_t FFiOHOSWindowSetShadowColor(int64_t id, char* color)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetShadowColor start");
    std::string windowColor = color;
    auto instance = FFIData::GetData<CJWindowImpl>(id);
    if (instance == nullptr) {
        return WM_ERROR_STATE_ABNORMALLY;
    }
    int32_t res = instance->SetShadowColor(windowColor);
    if (res == SUCCESS_CODE) {
        TLOGI(WmsLogTag::WMS_DIALOG, "[Window] FFiOHOSWindowSetShadowColor success");
    }
    return res;
}
} // extern "C"
}
}

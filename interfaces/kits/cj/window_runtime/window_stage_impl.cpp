/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "window_stage_impl.h"
#include "permission.h"
#include "window_impl.h"
#include "window_manager_hilog.h"
#include "window_utils.h"

namespace OHOS {
namespace Rosen {
void CreateCjSubWindowArrayObject(std::vector<sptr<Window>> &vec, RetStruct &ret)
{
    int64_t *windowList = static_cast<int64_t *>(malloc(sizeof(int64_t) * vec.size()));
    if (windowList == nullptr) {
        ret.code = static_cast<int32_t>(WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
        return;
    }
    ret.data = windowList;
    ret.len = 0;
    for (auto& window : vec) {
        if (window == nullptr) {
            continue;
        }
        sptr<CJWindowImpl> windowImpl = CreateCjWindowObject(window);
        if (windowImpl == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "[createCjSubWindowArrayObject] windowImpl is nullptr");
            continue;
        }
        windowList[ret.len] = windowImpl->GetID();
        ret.len += 1;
    }
    return;
}

int32_t CJWindowStageImpl::GetMainWindow(int64_t &windowId)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[getMainWindow] Window scene is nullptr");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[getMainWindow] Window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto windowImpl = CreateCjWindowObject(window);
    if (windowImpl == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[getMainWindow] windowImpl is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    windowId = windowImpl->GetID();
    return static_cast<int32_t>(WmErrorCode::WM_OK);
}

int32_t CJWindowStageImpl::CreateSubWindow(std::string name, int64_t &windowId)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[createSubWindow] Window scene is nullptr");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<Rosen::WindowOption> windowOption = new(std::nothrow) Rosen::WindowOption();
    if (windowOption == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[createSubWindow] Create option failed");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    auto window = weakScene->CreateWindow(name, windowOption);
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[createSubWindow] Create window failed");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<CJWindowImpl> windowImpl = CreateCjWindowObject(window);
    if (windowImpl == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[createSubWindow] windowImpl is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    windowId = windowImpl->GetID();
    TLOGI(WmsLogTag::WMS_DIALOG, "[createSubWindow] Create sub window %{public}s end", name.c_str());
    return static_cast<int32_t>(WmErrorCode::WM_OK);
}

RetStruct CJWindowStageImpl::GetSubWindow()
{
    RetStruct ret = {.code = static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), .len = 0, .data = nullptr};
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[getSubWindow] Window scene is nullptr");
        ret.code = static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ret;
    }
    auto subWindowVec = weakScene->GetSubWindow();
    CreateCjSubWindowArrayObject(subWindowVec, ret);
    return ret;
}

int32_t CJWindowStageImpl::OnLoadContent(const std::string &contexUrl,
    const std::string &storageJson, bool isLoadedByName)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowStage] WindowScene is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY);
    }
    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowStage] Window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WMError ret;
    if (isLoadedByName) {
        ret = window->SetUIContentByName(contexUrl, nullptr, nullptr);
    } else {
        ret = window->NapiSetUIContent(contexUrl, nullptr, nullptr);
    }
    TLOGI(WmsLogTag::WMS_DIALOG,
        "[WindowStage] LoadContent [%{public}u, %{public}s] load content end, ret = %{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), ret);
    return static_cast<int32_t>(ret);
}

int32_t CJWindowStageImpl::DisableWindowDecor()
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowStage] WindowScene is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY);
    }
    auto naWindow = weakScene->GetMainWindow();
    if (naWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowStage] Window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(naWindow->DisableAppWindowDecor());
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowStage] Window [%{public}u, %{public}s] disable app window decor end",
            naWindow->GetWindowId(), naWindow->GetWindowName().c_str());
        return static_cast<int32_t>(ret);
    }
    return static_cast<int32_t>(WmErrorCode::WM_OK);
}

/** @note @window.hierarchy */
int32_t CJWindowStageImpl::SetShowOnLockScreen(bool showOnLockScreen)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "Set show on lock screen permission denied!");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
    }
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr || weakScene->GetMainWindow() == nullptr) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[WindowStage] WindowScene is null or window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto window = weakScene->GetMainWindow();
    WmErrorCode ret;
    if (showOnLockScreen) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            window->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            window->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    }
    TLOGI(WmsLogTag::WMS_HIERARCHY,
        "[WindowStage] Window [%{public}u, %{public}s] SetShowOnLockScreen %{public}u, ret = %{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), showOnLockScreen, ret);
    return static_cast<int32_t>(ret);
}

std::shared_ptr<CJWindowStageImpl> CJWindowStageImpl::CreateCJWindowStage(
    std::shared_ptr<WindowScene> windowScene)
{
    auto ptr = FFIData::Create<CJWindowStageImpl>(windowScene);
    if (ptr == nullptr) {
        return nullptr;
    }
    auto windowStagePtr = std::shared_ptr<CJWindowStageImpl>(ptr.GetRefPtr());
    return windowStagePtr;
}

extern "C" {
FFI_EXPORT int64_t OHOS_CreateCJWindowStage(const std::shared_ptr<WindowScene>& windowScene)
{
    auto ptr = OHOS::FFI::FFIData::Create<OHOS::Rosen::CJWindowStageImpl>(windowScene);
    if (ptr == nullptr) {
        return 0;
    }
    return ptr->GetID();
}
}
}
}

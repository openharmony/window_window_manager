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
#include <cstdint>
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
    ret.code = static_cast<int32_t>(WmErrorCode::WM_OK);
    for (auto& window : vec) {
        if (window == nullptr) {
            continue;
        }
        sptr<CJWindowImpl> windowImpl = CreateCjWindowObject(window);
        if (windowImpl == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "windowImpl is nullptr");
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
        TLOGE(WmsLogTag::WMS_DIALOG, "Window scene is nullptr");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto windowImpl = CreateCjWindowObject(window);
    if (windowImpl == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "windowImpl is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    windowId = windowImpl->GetID();
    return static_cast<int32_t>(WmErrorCode::WM_OK);
}

int32_t CJWindowStageImpl::CreateSubWindow(std::string name, int64_t &windowId)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Window scene is nullptr");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<Rosen::WindowOption> windowOption = new(std::nothrow) Rosen::WindowOption();
    if (windowOption == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Create option failed");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    auto window = weakScene->CreateWindow(name, windowOption);
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Create window failed");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<CJWindowImpl> windowImpl = CreateCjWindowObject(window);
    if (windowImpl == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "windowImpl is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    windowId = windowImpl->GetID();
    TLOGI(WmsLogTag::WMS_DIALOG, "Create sub window %{public}s end", name.c_str());
    return static_cast<int32_t>(WmErrorCode::WM_OK);
}

RetStruct CJWindowStageImpl::GetSubWindow()
{
    RetStruct ret = {.code = static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), .len = 0, .data = nullptr};
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Window scene is nullptr");
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
        ret = window->NapiSetUIContent(contexUrl, (napi_env)nullptr, nullptr);
    }
    TLOGI(WmsLogTag::WMS_DIALOG,
        "[WindowStage] [%{public}u, %{public}s] end, ret=%{public}d",
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
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowStage] Window [%{public}u, %{public}s] end",
            naWindow->GetWindowId(), naWindow->GetWindowName().c_str());
        return static_cast<int32_t>(ret);
    }
    return static_cast<int32_t>(WmErrorCode::WM_OK);
}

/** @note @window.hierarchy */
int32_t CJWindowStageImpl::SetShowOnLockScreen(bool showOnLockScreen)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "permission denied!");
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
        "[WindowStage] Window [%{public}u, %{public}s] %{public}u, ret=%{public}u",
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

int32_t CJWindowStageImpl::SetDefaultDensityEnabled(bool enabled)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[WindowStage] WindowScene is null or window is null");
            return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[WindowStage] Window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetDefaultDensityEnabled(enabled));
    return static_cast<int32_t>(ret);
}

int32_t CJWindowStageImpl::OnEvent(int64_t funcId)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowStage] WindowScene is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowStage] Window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WmErrorCode ret = registerManager_->RegisterListener(window,
        WINDOW_STAGE_EVENT_CB, CaseType::CASE_STAGE, funcId, 0);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::DEFAULT, "Register failed, window stage [%{public}u, %{public}s] type: %{public}s",
            window->GetWindowId(), window->GetWindowName().c_str(), WINDOW_STAGE_EVENT_CB.c_str());
    }
    return static_cast<int32_t>(ret);
}

int32_t CJWindowStageImpl::OffEvent(int64_t funcId)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowStage] WindowScene is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowStage] Window is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    WmErrorCode ret = registerManager_->UnregisterListener(window,
        WINDOW_STAGE_EVENT_CB, CaseType::CASE_STAGE, funcId);
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::DEFAULT, "Unregister failed, window stage [%{public}u, %{public}s] type: %{public}s",
            window->GetWindowId(), window->GetWindowName().c_str(), WINDOW_STAGE_EVENT_CB.c_str());
    }
    return static_cast<int32_t>(ret);
}

int32_t CJWindowStageImpl::CreateSubWindowWithOptions(int64_t &windowId,
    const std::string& name, const std::string& title, bool decorEnabled, bool isModal)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[createSubWindowWithOptions] Window scene is nullptr");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<Rosen::WindowOption> windowOption = new(std::nothrow) Rosen::WindowOption();
    if (windowOption == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[createSubWindowWithOptions] Create option failed");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    if (isModal) {
        windowOption->AddWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL);
    }
    windowOption->SetSubWindowTitle(title);
    windowOption->SetSubWindowDecorEnable(decorEnabled);
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    auto window = weakScene->CreateWindow(name, windowOption);
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[createSubWindowWithOptions] Create window failed");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<CJWindowImpl> windowImpl = CreateCjWindowObject(window);
    if (windowImpl == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[createSubWindowWithOptions] windowImpl is null");
        return static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    windowId = windowImpl->GetID();
    TLOGI(WmsLogTag::WMS_SUB, "[createSubWindowWithOptions] Create sub window %{public}s end", name.c_str());
    return static_cast<int32_t>(WmErrorCode::WM_OK);
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

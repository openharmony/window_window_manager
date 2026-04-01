/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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
#include "float_bind_manager.h"

#include <random>
#include <chrono>

#include "window.h"
#include "window_manager_hilog.h"
 
namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t MAX_RANDOM = 999999;
}

std::mutex FloatBindManager::relationMutex_;
std::map<sptr<FloatViewController>, sptr<FloatingBallController>> FloatBindManager::floatViewToFloatingBallMap_ = {};
std::map<sptr<FloatingBallController>, sptr<FloatViewController>> FloatBindManager::floatingBallToFloatViewMap_ = {};

std::string FloatBindManager::GetControllerId()
{
    auto time = std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, MAX_RANDOM);
    auto randomNumber = distr(gen);
    std::stringstream ss;
    ss << time << "_" << randomNumber;
    return ss.str();
}

WMError FloatBindManager::Bind(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController, const FbOption &fbOption)
{
    std::lock_guard<std::mutex> lock(relationMutex_);
    if (!IsFloatingBallStateValid(fbController) || fbController->IsBind()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "floating ball state not supported or has been bound");
        return WMError::WM_ERROR_FB_INVALID_STATE;
    }
    if (!IsFloatViewStateValid(fvController) || fvController->IsBind()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "float view state not supported or has been bound");
        return WMError::WM_ERROR_FV_INVALID_STATE;
    }
    fbController->SetOption(sptr<FbOption>::MakeSptr(fbOption));
    fbController->SetBindState(true);
    fvController->SetBindState(true);
    AddRelation(fvController, fbController);
    return WMError::WM_OK;
}

bool FloatBindManager::IsFloatingBallStateValid(const sptr<FloatingBallController> &fbController)
{
    auto fbState = fbController->GetCurState();
    return fbState == FbWindowState::STATE_UNDEFINED || fbState == FbWindowState::STATE_STOPPED;
}

bool FloatBindManager::IsFloatViewStateValid(const sptr<FloatViewController> &fvController)
{
    auto fvState = fvController->GetCurState();
    return fvState == FvWindowState::FV_STATE_UNDEFINED || fvState == FvWindowState::FV_STATE_STOPPED;
}

WMError FloatBindManager::UnBind(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController)
{
    std::lock_guard<std::mutex> lock(relationMutex_);
    if (!IsFloatingBallStateValid(fbController) || !fbController->IsBind()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "floating ball state not supported or has not been bound");
        return WMError::WM_ERROR_FB_INVALID_STATE;
    }
    if (!IsFloatViewStateValid(fvController) || !fvController->IsBind()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "float view state not supported or has not been bound");
        return WMError::WM_ERROR_FV_INVALID_STATE;
    }
    fbController->SetBindState(false);
    fvController->SetBindState(false);
    fbController->SetBindWindowId(INVALID_WINDOW_ID);
    fvController->SetBindWindowId(INVALID_WINDOW_ID);
    RemoveRelation(fvController, fbController);
    return WMError::WM_OK;
}

void FloatBindManager::AddRelation(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController)
{
    floatViewToFloatingBallMap_[fvController] = fbController;
    floatingBallToFloatViewMap_[fbController] = fvController;
}

void FloatBindManager::RemoveRelation(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController)
{
    if (floatViewToFloatingBallMap_.find(fvController) != floatViewToFloatingBallMap_.end()) {
        floatViewToFloatingBallMap_.erase(fvController);
    }
    if (floatingBallToFloatViewMap_.find(fbController) != floatingBallToFloatViewMap_.end()) {
        floatingBallToFloatViewMap_.erase(fbController);
    }
}

WMError FloatBindManager::StartBindFloatView(const wptr<FloatViewController> &fvControllerWeak)
{
    auto fvController = fvControllerWeak.promote();
    if (fvController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fvController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::mutex> lock(relationMutex_);
    auto it = floatViewToFloatingBallMap_.find(fvController);
    if (it == floatViewToFloatingBallMap_.end() || it->second == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "can not find floating ball by float view or null floating ball controller");
        return WMError::WM_DO_NOTHING;
    }
    auto fbController = it->second;

    auto ret = fvController->StartFloatViewSingle(true);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "start float view failed when bind");
        return ret;
    }
    auto fvWindow = fvController->GetWindow();
    if (fvWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "float view window is nullptr");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fbController->SetBindWindowId(fvWindow->GetWindowId());
    ret = fbController->StartFloatingBallSingle(fbController->GetOption(), false);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "start floating ball failed when bind");
        fbController->SetBindWindowId(INVALID_WINDOW_ID);
        return WMError::WM_DO_NOTHING;
    }
    auto fbWindow = fbController->GetFbWindow();
    if (fbWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "floating ball window is nullptr");
        fbController->SetBindWindowId(INVALID_WINDOW_ID);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fvController->SetBindWindowId(fbWindow->GetWindowId());
    return WMError::WM_OK;
}

WMError FloatBindManager::StartBindFloatingBall(const wptr<FloatingBallController> &fbControllerWeak,
    const sptr<FbOption> &option)
{
    auto fbController = fbControllerWeak.promote();
    if (fbController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fbController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::mutex> lock(relationMutex_);
    auto it = floatingBallToFloatViewMap_.find(fbController);
    if (it == floatingBallToFloatViewMap_.end() || it->second == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "can not find float view by floating ball or null float view controller");
        return WMError::WM_DO_NOTHING;
    }
    auto fvController = it->second;

    auto ret = fbController->StartFloatingBallSingle(option, true);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "start floating ball failed when bind");
        return ret;
    }
    auto fbWindow = fbController->GetFbWindow();
    if (fbWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "floating ball window is nullptr");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fvController->SetBindWindowId(fbWindow->GetWindowId());

    ret = fvController->StartFloatViewSingle(false);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "start float view failed when bind");
        fvController->SetBindWindowId(INVALID_WINDOW_ID);
        return WMError::WM_DO_NOTHING;
    }
    auto fvWindow = fvController->GetWindow();
    if (fvWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "float view window is nullptr");
        fvController->SetBindWindowId(INVALID_WINDOW_ID);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fbController->SetBindWindowId(fvWindow->GetWindowId());
    return WMError::WM_OK;
}

WMError FloatBindManager::StopBindFloatView(const wptr<FloatViewController> &fvControllerWeak)
{
    auto fvController = fvControllerWeak.promote();
    if (fvController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fvController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::mutex> lock(relationMutex_);
    auto it = floatViewToFloatingBallMap_.find(fvController);
    if (it == floatViewToFloatingBallMap_.end() || it->second == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "can not find floating ball by float view or null floating ball controller");
        return WMError::WM_DO_NOTHING;
    }
    auto fbController = it->second;
    auto ret = fvController->StopFloatViewFromClientSingle();
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "stop float view failed when bind");
        return ret;
    }
    ret = fbController->StopFloatingBallFromClientSingle();
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "stop floating ball failed when bind");
        return WMError::WM_DO_NOTHING;
    }
    return WMError::WM_OK;
}

WMError FloatBindManager::StopBindFloatingBall(const wptr<FloatingBallController> &fbControllerWeak)
{
    auto fbController = fbControllerWeak.promote();
    if (fbController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fbController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::mutex> lock(relationMutex_);
    auto it = floatingBallToFloatViewMap_.find(fbController);
    if (it == floatingBallToFloatViewMap_.end() || it->second == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "can not find float view by floating ball or null float view controller");
        return WMError::WM_DO_NOTHING;
    }
    auto fvController = it->second;
    auto ret = fbController->StopFloatingBallFromClientSingle();
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "stop floating ball failed when bind");
        return ret;
    }
    ret = fvController->StopFloatViewFromClientSingle();
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "stop float view failed when bind");
        return WMError::WM_DO_NOTHING;
    }
    return WMError::WM_OK;
}

} // namespace Rosen
} // namespace OHOS

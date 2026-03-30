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
#include "float_window_manager.h"

#include <random>
#include <chrono>

#include "window.h"
#include "window_manager_hilog.h"

#include "float_view_manager.h"
#include "floating_ball_manager.h"
#include "picture_in_picture_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t MAX_RANDOM = 999999;
constexpr int64_t TOKEN_TIMEOUT_MS = 500;
}

std::recursive_mutex FloatWindowManager::relationMutex_;
std::map<sptr<FloatViewController>, sptr<FloatingBallController>> FloatWindowManager::floatViewToFloatingBallMap_ = {};
std::map<sptr<FloatingBallController>, sptr<FloatViewController>> FloatWindowManager::floatingBallToFloatViewMap_ = {};

std::condition_variable FloatWindowManager::tokenCv_;
std::mutex FloatWindowManager::tokenMutex_;
uint64_t FloatWindowManager::tokenOwner_ = 0;
uint64_t FloatWindowManager::tokenSeq_ = 0;

std::string FloatWindowManager::GetControllerId()
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

WMError FloatWindowManager::Bind(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController, const FbOption &fbOption)
{
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
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

bool FloatWindowManager::IsFloatingBallStateValid(const sptr<FloatingBallController> &fbController)
{
    auto fbState = fbController->GetCurState();
    return fbState == FbWindowState::STATE_UNDEFINED || fbState == FbWindowState::STATE_STOPPED;
}

bool FloatWindowManager::IsFloatViewStateValid(const sptr<FloatViewController> &fvController)
{
    auto fvState = fvController->GetCurState();
    return fvState == FvWindowState::FV_STATE_UNDEFINED || fvState == FvWindowState::FV_STATE_STOPPED;
}

WMError FloatWindowManager::UnBind(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController)
{
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
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

void FloatWindowManager::AddRelation(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController)
{
    floatViewToFloatingBallMap_[fvController] = fbController;
    floatingBallToFloatViewMap_[fbController] = fvController;
}

void FloatWindowManager::RemoveRelation(const sptr<FloatViewController> &fvController,
    const sptr<FloatingBallController> &fbController)
{
    if (floatViewToFloatingBallMap_.find(fvController) != floatViewToFloatingBallMap_.end()) {
        floatViewToFloatingBallMap_.erase(fvController);
    }
    if (floatingBallToFloatViewMap_.find(fbController) != floatingBallToFloatViewMap_.end()) {
        floatingBallToFloatViewMap_.erase(fbController);
    }
}

WMError FloatWindowManager::StartBindFloatView(const wptr<FloatViewController> &fvControllerWeak)
{
    auto fvController = fvControllerWeak.promote();
    if (fvController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fvController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
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
        fvController->StopFloatViewFromClientSingle();
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fbController->SetBindWindowId(fvWindow->GetWindowId());
    ret = fbController->StartFloatingBallSingle(fbController->GetOption(), false);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "start floating ball failed when bind");
        fbController->SetBindWindowId(INVALID_WINDOW_ID);
        fvController->StopFloatViewFromClientSingle();
        return WMError::WM_DO_NOTHING;
    }
    auto fbWindow = fbController->GetFbWindow();
    if (fbWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "floating ball window is nullptr");
        fbController->SetBindWindowId(INVALID_WINDOW_ID);
        fvController->StopFloatViewFromClientSingle();
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fvController->SetBindWindowId(fbWindow->GetWindowId());
    return WMError::WM_OK;
}

WMError FloatWindowManager::StartBindFloatingBall(const wptr<FloatingBallController> &fbControllerWeak,
    const sptr<FbOption> &option)
{
    auto fbController = fbControllerWeak.promote();
    if (fbController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fbController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
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
        fbController->StopFloatingBallFromClientSingle();
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fvController->SetBindWindowId(fbWindow->GetWindowId());

    ret = fvController->StartFloatViewSingle(false);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "start float view failed when bind");
        fvController->SetBindWindowId(INVALID_WINDOW_ID);
        fbController->StopFloatingBallFromClientSingle();
        return WMError::WM_DO_NOTHING;
    }
    auto fvWindow = fvController->GetWindow();
    if (fvWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "float view window is nullptr");
        fvController->SetBindWindowId(INVALID_WINDOW_ID);
        fbController->StopFloatingBallFromClientSingle();
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    fbController->SetBindWindowId(fvWindow->GetWindowId());
    return WMError::WM_OK;
}

WMError FloatWindowManager::StopBindFloatView(const wptr<FloatViewController> &fvControllerWeak)
{
    auto fvController = fvControllerWeak.promote();
    if (fvController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fvController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
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

WMError FloatWindowManager::StopBindFloatingBall(const wptr<FloatingBallController> &fbControllerWeak)
{
    auto fbController = fbControllerWeak.promote();
    if (fbController == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "fbController is null");
        return WMError::WM_DO_NOTHING;
    }
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
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

uint64_t FloatWindowManager::AcquireToken()
{
    std::unique_lock<std::mutex> lock(tokenMutex_);
    bool status = tokenCv_.wait_for(lock, std::chrono::milliseconds(TOKEN_TIMEOUT_MS), [] { return tokenOwner_ == 0; });
    if (!status) {
        return 0;
    }
    if (tokenSeq_ == UINT64_MAX) {
        tokenSeq_ = 0;
    }
    tokenOwner_ = ++tokenSeq_;
    return tokenOwner_;
}

void FloatWindowManager::ReleaseToken(uint64_t token)
{
    if (token == 0) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(tokenMutex_);
        if (tokenOwner_ != token) {
            return;
        }
        tokenOwner_ = 0;
    }
    tokenCv_.notify_one();
}

bool FloatWindowManager::IsFloatViewConflict(const wptr<FloatViewController>& selfController)
{
    if (PictureInPictureManager::HasRunningControllerStartingOrStarted()) {
        return true;
    }
    if (!FloatingBallManager::HasActiveController()) {
        return false;
    }
    auto selfFv = selfController.promote();
    if (selfFv != nullptr) {
        auto boundFb = GetBoundFloatingBall(selfFv);
        auto activeFb = FloatingBallManager::GetActiveController();
        if (boundFb != nullptr && activeFb != nullptr && boundFb.GetRefPtr() == activeFb.GetRefPtr()) {
            return false;
        }
    }
    return true;
}

bool FloatWindowManager::IsFloatingBallConflict(const wptr<FloatingBallController>& selfController)
{
    if (PictureInPictureManager::HasRunningControllerStartingOrStarted()) {
        return true;
    }
    if (!FloatViewManager::HasActiveController()) {
        return false;
    }
    auto selfFb = selfController.promote();
    if (selfFb != nullptr) {
        auto boundFv = GetBoundFloatView(selfFb);
        auto activeFv = FloatViewManager::GetActiveController();
        if (boundFv != nullptr && activeFv != nullptr && boundFv.GetRefPtr() == activeFv.GetRefPtr()) {
            return false;
        }
    }
    return true;
}

bool FloatWindowManager::IsPipConflict()
{
    return FloatViewManager::HasActiveController() || FloatingBallManager::HasActiveController();
}

sptr<FloatingBallController> FloatWindowManager::GetBoundFloatingBall(const sptr<FloatViewController>& fvController)
{
    if (fvController == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
    auto it = floatViewToFloatingBallMap_.find(fvController);
    if (it == floatViewToFloatingBallMap_.end()) {
        return nullptr;
    }
    return it->second;
}

sptr<FloatViewController> FloatWindowManager::GetBoundFloatView(const sptr<FloatingBallController>& fbController)
{
    if (fbController == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::recursive_mutex> lock(relationMutex_);
    auto it = floatingBallToFloatViewMap_.find(fbController);
    if (it == floatingBallToFloatViewMap_.end()) {
        return nullptr;
    }
    return it->second;
}

} // namespace Rosen
} // namespace OHOS

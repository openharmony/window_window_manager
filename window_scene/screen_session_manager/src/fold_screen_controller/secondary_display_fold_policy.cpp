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
#include "fold_screen_controller/secondary_display_fold_policy.h"
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <transaction/rs_interfaces.h>
#include <parameters.h>
#include "rs_adapter.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"
#include "screen_scene_config.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "fold_screen_state_internel.h"

#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
const ScreenId SCREEN_ID_FULL = 0;
const ScreenId SCREEN_ID_MAIN = 5;
const int32_t REMOVE_DISPLAY_NODE = 0;
const int32_t ADD_DISPLAY_NODE = 1;
const int32_t MAIN_STATUS_WIDTH = 0;
const int32_t FULL_STATUS_WIDTH = 1;
const int32_t GLOBAL_FULL_STATUS_WIDTH = 2;
const int32_t SCREEN_HEIGHT = 3;
const int32_t FULL_STATUS_OFFSET_X = 4;
constexpr uint32_t HALF_DIVIDER = 2;
constexpr float MAIN_DISPLAY_ROTATION_DEGREE = 180;
constexpr float ROTATION_TRANSLATE_X = 612;
constexpr float ROTATION_TRANSLATE_Y = -612;
const int32_t FOLD_CREASE_RECT_SIZE = 8;
const std::string g_FoldScreenRect = system::GetParameter("const.display.foldscreen.crease_region", "");
#ifdef TP_FEATURE_ENABLE
const int32_t TP_TYPE = 12;
const char* STATUS_MAIN = "version:3+main";
const char* STATUS_GLOBAL_FULL = "version:3+whole";
const char* STATUS_FULL = "version:3+minor";
#endif
} // namespace

SecondaryDisplayFoldPolicy::SecondaryDisplayFoldPolicy(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler)
    : displayInfoMutex_(displayInfoMutex), screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    InitScreenParams();
    std::vector<int32_t> screenParamsSigned;
    for (uint32_t ele : screenParams_) {
        screenParamsSigned.emplace_back(static_cast<int32_t>(ele));
    }
    ScreenId screenIdFull = 0;
    int32_t foldCreaseRegionABPosX = 0;
    int32_t foldCreaseRegionABPosY = screenParamsSigned[MAIN_STATUS_WIDTH]; // 1008
    int32_t foldCreaseRegionABPosHeight = screenParamsSigned[GLOBAL_FULL_STATUS_WIDTH] -
        screenParamsSigned[FULL_STATUS_WIDTH] - screenParamsSigned[MAIN_STATUS_WIDTH]; // 128
    int32_t foldCreaseRegionABPosWidth = screenParamsSigned[SCREEN_HEIGHT]; // 2232
    int32_t foldCreaseRegionBCPosX = 0;
    // 2224
    int32_t foldCreaseRegionBCPosY =
        screenParamsSigned[GLOBAL_FULL_STATUS_WIDTH] -
        ((screenParamsSigned[FULL_STATUS_WIDTH] / HALF_DIVIDER) - (foldCreaseRegionABPosHeight / HALF_DIVIDER));
    int32_t foldCreaseRegionBCPosWidth = screenParamsSigned[SCREEN_HEIGHT]; // 2232
    int32_t foldCreaseRegionBCPosHeight = foldCreaseRegionABPosHeight; // 128
    TLOGW(WmsLogTag::DMS, "created, screenIdFull = %{public}" PRIu64", foldCreaseRegionABPosX = %{public}d,\
        foldCreaseRegionABPosY = %{public}d, foldCreaseRegionABPosHeight = %{public}d,\
        foldCreaseRegionABPosWidth = %{public}d, foldCreaseRegionBCPosX = %{public}d,\
        foldCreaseRegionBCPosY = %{public}d, foldCreaseRegionBCPosWidth = %{public}d,\
        foldCreaseRegionBCPosHeight = %{public}d", screenIdFull, foldCreaseRegionABPosX, foldCreaseRegionABPosY,
        foldCreaseRegionABPosHeight, foldCreaseRegionABPosWidth, foldCreaseRegionBCPosX, foldCreaseRegionBCPosY,
        foldCreaseRegionBCPosWidth, foldCreaseRegionBCPosHeight);
    std::vector<DMRect> rect = {
        {
            foldCreaseRegionABPosX, foldCreaseRegionABPosY,
            static_cast<uint32_t>(foldCreaseRegionABPosWidth), static_cast<uint32_t>(foldCreaseRegionABPosHeight)
        },
        {
            foldCreaseRegionBCPosX, foldCreaseRegionBCPosY,
            static_cast<uint32_t>(foldCreaseRegionBCPosWidth), static_cast<uint32_t>(foldCreaseRegionBCPosHeight)
        }
    };
    currentFoldCreaseRegion_ = new FoldCreaseRegion(screenIdFull, rect);
}

sptr<FoldCreaseRegion> SecondaryDisplayFoldPolicy::GetStatusFullVerticalFoldCreaseRect()
{
    std::vector<int32_t> foldRect = FoldScreenStateInternel::StringFoldRectSplitToInt(g_FoldScreenRect, ",;");
    if (foldRect.size() != FOLD_CREASE_RECT_SIZE) {
        // ccm numbers of parameter on the current device is 8
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return nullptr;
    }

    ScreenId screenIdFull = 0;
    int32_t liveCreaseRegionPosX = foldRect[0]; // ccm PosX
    int32_t liveCreaseRegionPosY = foldRect[1]; // ccm PosY
    int32_t liveCreaseRegionPosWidth = foldRect[2]; // ccm PosWidth
    int32_t liveCreaseRegionPosHeight = foldRect[3]; // ccm PosHeight

    std::vector<DMRect> foldCreaseRect = {
        {
            liveCreaseRegionPosX, liveCreaseRegionPosY,
            liveCreaseRegionPosWidth, liveCreaseRegionPosHeight
        }
    };
    return new FoldCreaseRegion(screenIdFull, foldCreaseRect);
}

sptr<FoldCreaseRegion> SecondaryDisplayFoldPolicy::GetStatusFullHorizontalFoldCreaseRect()
{
    std::vector<int32_t> foldRect = FoldScreenStateInternel::StringFoldRectSplitToInt(g_FoldScreenRect, ",;");
    if (foldRect.size() != FOLD_CREASE_RECT_SIZE) {
        // ccm numbers of parameter on the current device is 8
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return nullptr;
    }

    ScreenId screenIdFull = 0;
    int32_t liveCreaseRegionPosX = foldRect[1]; // ccm PosX
    int32_t liveCreaseRegionPosY = foldRect[0]; // ccm PosY
    int32_t liveCreaseRegionPosWidth = foldRect[3]; // ccm PosWidth
    int32_t liveCreaseRegionPosHeight = foldRect[2]; // ccm PosHeight

    std::vector<DMRect> foldCreaseRect = {
        {
            liveCreaseRegionPosX, liveCreaseRegionPosY,
            liveCreaseRegionPosWidth, liveCreaseRegionPosHeight
        }
    };
    return new FoldCreaseRegion(screenIdFull, foldCreaseRect);
}

sptr<FoldCreaseRegion> SecondaryDisplayFoldPolicy::GetStatusGlobalFullVerticalFoldCreaseRect()
{
    std::vector<int32_t> foldRect = FoldScreenStateInternel::StringFoldRectSplitToInt(g_FoldScreenRect, ",;");
    if (foldRect.size() != FOLD_CREASE_RECT_SIZE) {
        // ccm numbers of parameter on the current device is 8
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return nullptr;
    }

    ScreenId screenIdFull = 0;
    int32_t liveCreaseRegionABPosX = foldRect[0]; // ccm ABPosx
    int32_t liveCreaseRegionABPosY = foldRect[1]; // ccm ABPosY
    int32_t liveCreaseRegionABPosWidth = foldRect[2]; // ccm ABPosWidth
    int32_t liveCreaseRegionABPosHeight = foldRect[3]; // ccm ABPosHeight
    int32_t liveCreaseRegionBCPosX = foldRect[4]; // ccm BCPosX
    int32_t liveCreaseRegionBCPosY = foldRect[5]; // ccm BCPosY
    int32_t liveCreaseRegionBCPosWidth = foldRect[6]; // ccm BCPosWidth
    int32_t liveCreaseRegionBCPosHeight = foldRect[7]; // ccm BCPosHeight

    std::vector<DMRect> foldCreaseRect = {
        {
            liveCreaseRegionABPosX, liveCreaseRegionABPosY,
            static_cast<uint32_t>(liveCreaseRegionABPosWidth), static_cast<uint32_t>(liveCreaseRegionABPosHeight)
        },
        {
            liveCreaseRegionBCPosX, liveCreaseRegionBCPosY,
            static_cast<uint32_t>(liveCreaseRegionBCPosWidth), static_cast<uint32_t>(liveCreaseRegionBCPosHeight)
        }
    };
    return new FoldCreaseRegion(screenIdFull, foldCreaseRect);
}

sptr<FoldCreaseRegion> SecondaryDisplayFoldPolicy::GetStatusGlobalFullHorizontalFoldCreaseRect()
{
    std::vector<int32_t> foldRect = FoldScreenStateInternel::StringFoldRectSplitToInt(g_FoldScreenRect, ",;");
    if (foldRect.size() != FOLD_CREASE_RECT_SIZE) {
        // ccm numbers of parameter on the current device is 8
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return nullptr;
    }

    ScreenId screenIdFull = 0;
    int32_t liveCreaseRegionABPosX = foldRect[1]; // ccm ABPosX
    int32_t liveCreaseRegionABPosY = foldRect[0]; // ccm ABPosY
    int32_t liveCreaseRegionABPosWidth = foldRect[3]; // ccm ABPosWidth
    int32_t liveCreaseRegionABPosHeight = foldRect[2]; // ccm ABPosHeight
    int32_t liveCreaseRegionBCPosX = foldRect[5]; // ccm ABPosx
    int32_t liveCreaseRegionBCPosY = foldRect[4]; // ccm ABPosY
    int32_t liveCreaseRegionBCPosWidth = foldRect[7]; // ccm ABPosWidth
    int32_t liveCreaseRegionBCPosHeight = foldRect[6]; // ccm ABPosHeight

    std::vector<DMRect> foldCreaseRect = {
        {
            liveCreaseRegionABPosX, liveCreaseRegionABPosY,
            static_cast<uint32_t>(liveCreaseRegionABPosWidth), static_cast<uint32_t>(liveCreaseRegionABPosHeight)
        },
        {
            liveCreaseRegionBCPosX, liveCreaseRegionBCPosY,
            static_cast<uint32_t>(liveCreaseRegionBCPosWidth), static_cast<uint32_t>(liveCreaseRegionBCPosHeight)
        }
    };
    return new FoldCreaseRegion(screenIdFull, foldCreaseRect);
}

void SecondaryDisplayFoldPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode, DisplayModeChangeReason reason)
{
    SetLastCacheDisplayMode(displayMode);
    if (GetModeChangeRunningStatus()) {
        TLOGW(WmsLogTag::DMS, "last process not complete, skip mode: %{public}d", displayMode);
        return;
    }
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        lastDisplayMode_ = displayMode;
    }
    if (displayMode == FoldDisplayMode::UNKNOWN) {
        TLOGW(WmsLogTag::DMS, "displayMode is unknown");
    } else {
        ChangeSuperScreenDisplayMode(screenSession, displayMode);
    }
    ReportFoldDisplayModeChange(displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        currentDisplayMode_ = displayMode;
    }
    if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
        TLOGW(WmsLogTag::DMS, "Set device status to STATUS_GLOBAL_FULL");
        SetDeviceStatus(static_cast<uint32_t>(DMDeviceStatus::STATUS_GLOBAL_FULL));
        system::SetParameter("persist.dms.device.status",
            std::to_string(static_cast<uint32_t>(DMDeviceStatus::STATUS_GLOBAL_FULL)));
    } else {
        TLOGW(WmsLogTag::DMS, "Set device status to UNKNOWN");
        SetDeviceStatus(static_cast<uint32_t>(DMDeviceStatus::UNKNOWN));
        system::SetParameter("persist.dms.device.status",
            std::to_string(static_cast<uint32_t>(DMDeviceStatus::UNKNOWN)));
    }
    ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
}

void SecondaryDisplayFoldPolicy::SendSensorResult(FoldStatus foldStatus)
{
    TLOGW(WmsLogTag::DMS, "FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    ChangeScreenDisplayMode(displayMode);
}

void SecondaryDisplayFoldPolicy::CloseCoordinationScreen()
{
    std::lock_guard<std::mutex> lock(coordinationMutex_);
    if (!ScreenSessionManager::GetInstance().GetCoordinationFlag()) {
        TLOGW(WmsLogTag::DMS, "CloseCoordinationScreen skipped, current coordination flag is false");
        return;
    }
    TLOGI(WmsLogTag::DMS, "Close Coordination Screen current mode=%{public}d", currentDisplayMode_);
    
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_MAIN, REMOVE_DISPLAY_NODE);
 
    ScreenSessionManager::GetInstance().OnScreenChange(SCREEN_ID_MAIN, ScreenEvent::DISCONNECTED);
    ScreenSessionManager::GetInstance().SetCoordinationFlag(false);
}
 
void SecondaryDisplayFoldPolicy::ChangeScreenDisplayModeToCoordination()
{
    std::lock_guard<std::mutex> lock(coordinationMutex_);
    if (ScreenSessionManager::GetInstance().GetCoordinationFlag()) {
        TLOGW(WmsLogTag::DMS, "change displaymode to coordination skipped, current coordination flag is true");
        return;
    }
    TLOGI(WmsLogTag::DMS, "change displaymode to coordination current mode=%{public}d", currentDisplayMode_);
    
    ScreenSessionManager::GetInstance().SetCoordinationFlag(true);
    ScreenSessionManager::GetInstance().OnScreenChange(SCREEN_ID_MAIN, ScreenEvent::CONNECTED);
 
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_MAIN, ADD_DISPLAY_NODE);
}
 
void SecondaryDisplayFoldPolicy::AddOrRemoveDisplayNodeToTree(ScreenId screenId, int32_t command)
{
    TLOGI(WmsLogTag::DMS, "AddOrRemoveDisplayNodeToTree, screenId: %{public}" PRIu64 ", command: %{public}d",
        screenId, command);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "AddOrRemoveDisplayNodeToTree, screenSession is null");
        return;
    }
    std::shared_ptr<RSDisplayNode> displayNode = screenSession->GetDisplayNode();
    if (displayNode == nullptr) {
        TLOGE(WmsLogTag::DMS, "AddOrRemoveDisplayNodeToTree, displayNode is null");
        return;
    }
 
    UpdateDisplayNodeBasedOnScreenId(screenId, displayNode);
    if (command == ADD_DISPLAY_NODE) {
        displayNode->AddDisplayNodeToTree();
    } else if (command == REMOVE_DISPLAY_NODE) {
        displayNode->RemoveDisplayNodeFromTree();
    }
    displayNode = nullptr;
    TLOGI(WmsLogTag::DMS, "add or remove displayNode");
    RSTransactionAdapter::FlushImplicitTransaction(screenSession->GetRSUIContext());
}
 
void SecondaryDisplayFoldPolicy::UpdateDisplayNodeBasedOnScreenId(ScreenId screenId,
    std::shared_ptr<RSDisplayNode> displayNode)
{
    if (screenId != SCREEN_ID_MAIN) {
        return;
    }
    TLOGW(WmsLogTag::DMS, "UpdateDisplayNodeBasedOnScreenId: %{public}" PRIu64, screenId);
    displayNode->SetScreenId(SCREEN_ID_FULL);
    displayNode->SetRotation(MAIN_DISPLAY_ROTATION_DEGREE);
    displayNode->SetTranslate({ROTATION_TRANSLATE_X, ROTATION_TRANSLATE_Y});
    displayNode->SetFrame(0, 0, screenParams_[MAIN_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT]);
    displayNode->SetBounds(0, 0, screenParams_[MAIN_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT]);
}
 
void SecondaryDisplayFoldPolicy::ExitCoordination()
{
    std::lock_guard<std::mutex> lock(coordinationMutex_);
    if (!ScreenSessionManager::GetInstance().GetCoordinationFlag()) {
        TLOGW(WmsLogTag::DMS, "ExitCoordination skipped, current coordination flag is false");
        return;
    }
    ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_MAIN, REMOVE_DISPLAY_NODE);
    ScreenSessionManager::GetInstance().OnScreenChange(SCREEN_ID_MAIN, ScreenEvent::DISCONNECTED);
    ScreenSessionManager::GetInstance().SetCoordinationFlag(false);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    currentDisplayMode_ = displayMode;
    lastDisplayMode_ = displayMode;
    TLOGI(WmsLogTag::DMS, "Exit coordination, current display mode:%{public}d", displayMode);
    ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
}

void SecondaryDisplayFoldPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    TLOGW(WmsLogTag::DMS, "onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        TLOGW(WmsLogTag::DMS, "when boot animation finished, change display mode");
        isChangeScreenWhenBootCompleted = true;
        RecoverWhenBootAnimationExit();
    }
}

void SecondaryDisplayFoldPolicy::RecoverWhenBootAnimationExit()
{
    TLOGW(WmsLogTag::DMS, "CurrentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    ChangeScreenDisplayMode(displayMode);
}

FoldDisplayMode SecondaryDisplayFoldPolicy::GetModeMatchStatus()
{
    FoldDisplayMode displayMode = FoldDisplayMode::MAIN;
    switch (currentFoldStatus_) {
        case FoldStatus::EXPAND: {
            displayMode = FoldDisplayMode::FULL;
            break;
        }
        case FoldStatus::FOLDED: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::HALF_FOLD: {
            displayMode = FoldDisplayMode::FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_HALF_FOLDED: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_EXPAND: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_HALF_FOLDED: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_EXPAND: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_HALF_FOLDED: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        default: {
            TLOGW(WmsLogTag::DMS, "FoldStatus is invalid");
        }
    }
    TLOGW(WmsLogTag::DMS, "displayMode: %{public}d", displayMode);
    return displayMode;
}

void SecondaryDisplayFoldPolicy::ChangeSuperScreenDisplayMode(sptr<ScreenSession> screenSession,
    FoldDisplayMode displayMode)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:ChangeScreenDisplayMode(displayMode = %" PRIu64")", displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode && !isChangeScreenWhenBootCompleted) {
            TLOGW(WmsLogTag::DMS, "already in displayMode %{public}d", displayMode);
            return;
        }
        if (isChangeScreenWhenBootCompleted) {
            isChangeScreenWhenBootCompleted = false;
        }
    }
    SetSecondaryDisplayModeChangeStatus(true);
    SendPropertyChangeResult(screenSession, SCREEN_ID_FULL, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND,
        displayMode);
    if (currentDisplayMode_ != displayMode) {
        if ((currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL && displayMode == FoldDisplayMode::FULL) ||
            (currentDisplayMode_ == FoldDisplayMode::FULL && displayMode == FoldDisplayMode::MAIN) ||
            (currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL && displayMode == FoldDisplayMode::MAIN)) {
                return;
        }
    }
    auto taskScreenOnFullOn = [=] {
        screenId_ = SCREEN_ID_FULL;
        PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
        SetdisplayModeChangeStatus(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnFullOn, "screenOnFullOnTask");
}

void SecondaryDisplayFoldPolicy::SendPropertyChangeResult(sptr<ScreenSession> screenSession, ScreenId screenId,
    ScreenPropertyChangeReason reason, FoldDisplayMode displayMode)
{
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSession is null");
        return;
    }

    if (displayMode == FoldDisplayMode::COORDINATION && currentDisplayMode_ == FoldDisplayMode::FULL) {
        SendCoordinationPropertyChangeResultSync(screenSession, screenId, reason, displayMode);
        return;
    }

    if (displayMode == FoldDisplayMode::MAIN) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
    }
    auto taskScreenOnFullOn = [this, displayMode] {
        switch (displayMode) {
            case FoldDisplayMode::FULL: {
                if (currentDisplayMode_ == FoldDisplayMode::COORDINATION) {
                    CloseCoordinationScreen();
                }
                SetStatusFullActiveRectAndTpFeature();
                break;
            }
            case FoldDisplayMode::MAIN: {
                if (currentDisplayMode_ == FoldDisplayMode::COORDINATION) {
                    CloseCoordinationScreen();
                }
                SetStatusMainActiveRectAndTpFeature();
                break;
            }
            case FoldDisplayMode::GLOBAL_FULL: {
                if (currentDisplayMode_ == FoldDisplayMode::COORDINATION) {
                    CloseCoordinationScreen();
                }
                SetStatusGlobalFullActiveRectAndTpFeature();
                break;
            }
            default: {
                TLOGW(WmsLogTag::DMS, "unKnown displayMode");
                break;
            }
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(std::move(taskScreenOnFullOn), __func__);
    HandleScreenOnFullOn(screenSession, screenId, reason, displayMode);
}

void SecondaryDisplayFoldPolicy::HandleScreenOnFullOn(sptr<ScreenSession> screenSession, ScreenId screenId,
    ScreenPropertyChangeReason reason, FoldDisplayMode displayMode)
{
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSession is null");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    if (displayMode == FoldDisplayMode::FULL) {
        auto fullStatusScreenBounds = RRect({ 0, screenParams_[FULL_STATUS_OFFSET_X],
            screenParams_[FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT] }, 0.0f, 0.0f);
        screenProperty_.SetBounds(fullStatusScreenBounds);
        screenSession->SetScreenAreaOffsetY(screenParams_[FULL_STATUS_OFFSET_X]);
        screenSession->SetScreenAreaHeight(screenParams_[FULL_STATUS_WIDTH]);
    } else if (displayMode == FoldDisplayMode::MAIN) {
        auto mainStatusScreenBounds =
            RRect({ 0, 0, screenParams_[MAIN_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT] }, 0.0f, 0.0f);
        screenProperty_.SetBounds(mainStatusScreenBounds);
        screenSession->SetScreenAreaOffsetY(0);
        screenSession->SetScreenAreaHeight(screenParams_[MAIN_STATUS_WIDTH]);
    } else if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
        auto globalFullStatusScreenBounds =
            RRect({ 0, 0, screenParams_[GLOBAL_FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT] }, 0.0f, 0.0f);
        screenProperty_.SetBounds(globalFullStatusScreenBounds);
        screenSession->SetScreenAreaOffsetY(0);
        screenSession->SetScreenAreaHeight(screenParams_[GLOBAL_FULL_STATUS_WIDTH]);
    } else {
        TLOGW(WmsLogTag::DMS, "unKnown displayMode");
    }
    screenSession->UpdatePropertyByFoldControl(screenProperty_, displayMode);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    if (displayMode == FoldDisplayMode::MAIN) {
        screenSession->SetRotationAndScreenRotationOnly(Rotation::ROTATION_0);
    }
    TLOGI(WmsLogTag::DMS, "screenBounds : width= %{public}f, height= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
}

void SecondaryDisplayFoldPolicy::SendCoordinationPropertyChangeResultSync(sptr<ScreenSession> screenSession,
    ScreenId screenId, ScreenPropertyChangeReason reason, FoldDisplayMode displayMode)
{
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSession is null");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    TLOGW(WmsLogTag::DMS, "COORDINATION");
    ChangeScreenDisplayModeToCoordination();
    SetStatusConditionalActiveRectAndTpFeature(screenProperty_);
 
    screenSession->UpdatePropertyByFoldControl(screenProperty_, displayMode);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    if (displayMode == FoldDisplayMode::MAIN) {
        screenSession->SetRotationAndScreenRotationOnly(Rotation::ROTATION_0);
    }
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
}

void SecondaryDisplayFoldPolicy::SetStatusFullActiveRectAndTpFeature()
{
    OHOS::Rect rectCur{
        .x = 0,
        .y = screenParams_[FULL_STATUS_OFFSET_X],
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[FULL_STATUS_WIDTH],
    };
    if (!onBootAnimation_) {
        RSInterfaces::GetInstance().NotifyScreenSwitched();
        auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
        TLOGI(WmsLogTag::DMS, "rs response is %{public}ld", static_cast<long>(response));
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    SetSecondaryDisplayModeChangeStatus(false);
}

void SecondaryDisplayFoldPolicy::SetStatusMainActiveRectAndTpFeature()
{
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[MAIN_STATUS_WIDTH],
    };
    if (!onBootAnimation_) {
        RSInterfaces::GetInstance().NotifyScreenSwitched();
        auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
        TLOGI(WmsLogTag::DMS, "rs response is %{public}ld", static_cast<long>(response));
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_MAIN, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    SetSecondaryDisplayModeChangeStatus(false);
}

void SecondaryDisplayFoldPolicy::SetStatusGlobalFullActiveRectAndTpFeature()
{
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[GLOBAL_FULL_STATUS_WIDTH],
    };
    if (!onBootAnimation_) {
        RSInterfaces::GetInstance().NotifyScreenSwitched();
        auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
        TLOGI(WmsLogTag::DMS, "rs response is %{public}ld", static_cast<long>(response));
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_GLOBAL_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    SetSecondaryDisplayModeChangeStatus(false);
}

void SecondaryDisplayFoldPolicy::SetStatusConditionalActiveRectAndTpFeature(ScreenProperty &screenProperty)
{
    auto globalFullStatusScreenBounds = RRect({0, screenParams_[FULL_STATUS_OFFSET_X],
        screenParams_[FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT]}, 0.0f, 0.0f);
    screenProperty.SetBounds(globalFullStatusScreenBounds);
    OHOS::Rect rectCur {
        .x = 0,
        .y = 0,
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[GLOBAL_FULL_STATUS_WIDTH],
    };
    if (!onBootAnimation_) {
        RSInterfaces::GetInstance().NotifyScreenSwitched();
        auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
        TLOGI(WmsLogTag::DMS, "rs response is %{public}d", static_cast<uint32_t>(response));
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_GLOBAL_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    SetSecondaryDisplayModeChangeStatus(false);
}

void SecondaryDisplayFoldPolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
{
    int32_t mode = static_cast<int32_t>(displayMode);
    TLOGW(WmsLogTag::DMS, "displayMode: %{public}d", mode);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "DISPLAY_MODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FOLD_DISPLAY_MODE", mode);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "write HiSysEvent error, ret: %{public}d", ret);
    }
}

sptr<FoldCreaseRegion> SecondaryDisplayFoldPolicy::GetCurrentFoldCreaseRegion()
{
    return currentFoldCreaseRegion_;
}

sptr<FoldCreaseRegion> SecondaryDisplayFoldPolicy::GetLiveCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "enter");
    FoldDisplayMode displayMode = GetScreenDisplayMode();
    if (displayMode == FoldDisplayMode::UNKNOWN || displayMode == FoldDisplayMode::MAIN) {
        return nullptr;
    }
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return nullptr;
    }
    DisplayOrientation displayOrientation = screenSession->GetScreenProperty().GetDisplayOrientation();
    if (displayMode == FoldDisplayMode::FULL || displayMode == FoldDisplayMode::COORDINATION) {
        switch (displayOrientation) {
            case DisplayOrientation::PORTRAIT:
            case DisplayOrientation::PORTRAIT_INVERTED: {
                liveCreaseRegion_ = GetStatusFullVerticalFoldCreaseRect();
                break;
            }
            case DisplayOrientation::LANDSCAPE:
            case DisplayOrientation::LANDSCAPE_INVERTED: {
                liveCreaseRegion_ = GetStatusFullHorizontalFoldCreaseRect();
                break;
            }
            default: {
                TLOGE(WmsLogTag::DMS, "displayOrientation is invalid");
            }
        }
    }
    if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
        switch (displayOrientation) {
            case DisplayOrientation::PORTRAIT:
            case DisplayOrientation::PORTRAIT_INVERTED: {
                liveCreaseRegion_ = GetStatusGlobalFullHorizontalFoldCreaseRect();
                break;
            }
            case DisplayOrientation::LANDSCAPE:
            case DisplayOrientation::LANDSCAPE_INVERTED: {
                liveCreaseRegion_ = GetStatusGlobalFullVerticalFoldCreaseRect();
                break;
            }
            default: {
                TLOGE(WmsLogTag::DMS, "displayOrientation is invalid");
            }
        }
    }
    return liveCreaseRegion_;
}

void SecondaryDisplayFoldPolicy::InitScreenParams()
{
    std::vector<DisplayPhysicalResolution> resolutions = ScreenSceneConfig::GetAllDisplayPhysicalConfig();
    for (auto &resolution : resolutions) {
        if (FoldDisplayMode::MAIN == resolution.foldDisplayMode_) {
            screenParams_.push_back(resolution.physicalWidth_);
        } else if (FoldDisplayMode::FULL == resolution.foldDisplayMode_) {
            screenParams_.push_back(resolution.physicalWidth_);
        } else if (FoldDisplayMode::GLOBAL_FULL == resolution.foldDisplayMode_) {
            screenParams_.push_back(resolution.physicalWidth_);
            screenParams_.push_back(resolution.physicalHeight_);
        } else {
            TLOGW(WmsLogTag::DMS, "unKnown displayMode");
        }
    }
    screenParams_.push_back(screenParams_[GLOBAL_FULL_STATUS_WIDTH] - screenParams_[FULL_STATUS_WIDTH]);
    TLOGI(WmsLogTag::DMS,
        "PhysicalResolution : mainStatusWidth_= %{public}d, fullStatusWidth_= %{public}d, gloablFullStatusWidth_="
        "%{public}d, screenHeight_= %{public}d, fullStatusOffsetX_= %{public}d",
          screenParams_[MAIN_STATUS_WIDTH], screenParams_[FULL_STATUS_WIDTH],
          screenParams_[GLOBAL_FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT],
          screenParams_[FULL_STATUS_OFFSET_X]);
}

std::vector<uint32_t> SecondaryDisplayFoldPolicy::GetScreenParams()
{
    return screenParams_;
}

void SecondaryDisplayFoldPolicy::SetSecondaryDisplayModeChangeStatus(bool status)
{
    if (status) {
        secondaryPengdingTask_ = SECONDARY_FOLD_TO_EXPAND_TASK_NUM;
        startTimePoint_ = std::chrono::steady_clock::now();
        displayModeChangeRunning_ = status;
    } else {
        secondaryPengdingTask_ --;
        if (secondaryPengdingTask_ > 0) {
            return;
        }
        displayModeChangeRunning_ = false;
        endTimePoint_ = std::chrono::steady_clock::now();
        if (lastCachedisplayMode_.load() != GetScreenDisplayMode()) {
            ScreenSessionManager::GetInstance().TriggerDisplayModeUpdate(lastCachedisplayMode_.load());
        }
    }
}

} // namespace OHOS::Rosen

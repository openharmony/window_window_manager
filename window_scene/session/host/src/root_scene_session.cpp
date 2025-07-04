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

#include "session/host/include/root_scene_session.h"

#include "display_manager.h"

namespace OHOS::Rosen {
void RootSceneSession::SetLoadContentFunc(const LoadContentFunc& loadContentFunc)
{
    loadContentFunc_ = loadContentFunc;
}

void RootSceneSession::LoadContent(
    const std::string& contentUrl, napi_env env, napi_value storage, AbilityRuntime::Context* context)
{
    if (loadContentFunc_) {
        loadContentFunc_(contentUrl, env, storage, context);
    }
}

void RootSceneSession::GetSystemAvoidAreaForRoot(const WSRect& rect, AvoidArea& avoidArea)
{
    std::vector<sptr<SceneSession>> statusBarVector;
    DisplayId displayId = GetSessionProperty()->GetDisplayId();
    if (specificCallback_ != nullptr && specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_) {
        statusBarVector = specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_(
            WindowType::WINDOW_TYPE_STATUS_BAR, displayId);
    }
    for (auto& statusBar : statusBarVector) {
        bool isVisible = statusBar->IsVisible();
        if (onGetStatusBarConstantlyShowFunc_) {
            onGetStatusBarConstantlyShowFunc_(displayId, isVisible);
            TLOGD(WmsLogTag::WMS_IMMS, "displayId %{public}" PRIu64 " constantly isVisible %{public}d",
                displayId, isVisible);
        }
        if (!isVisible) {
            TLOGI(WmsLogTag::WMS_IMMS, "root scene status bar not visible");
            continue;
        }
        WSRect statusBarRect = statusBar->GetSessionRect();
        if (onGetStatusBarAvoidHeightFunc_) {
            onGetStatusBarAvoidHeightFunc_(displayId, statusBarRect);
        }
        CalculateAvoidAreaRect(rect, statusBarRect, avoidArea);
        TLOGI(WmsLogTag::WMS_IMMS, "displayId %{public}" PRIu64 " root scene %{public}s "
              "status bar %{public}s area %{public}s",
              displayId, rect.ToString().c_str(), statusBarRect.ToString().c_str(), avoidArea.ToString().c_str());
    }
}

void RootSceneSession::GetKeyboardAvoidAreaForRoot(const WSRect& rect, AvoidArea& avoidArea)
{
    std::vector<sptr<SceneSession>> inputMethodVector;
    if (specificCallback_ != nullptr && specificCallback_->onGetSceneSessionVectorByType_) {
        inputMethodVector = specificCallback_->onGetSceneSessionVectorByType_(
            WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    }
    for (auto& inputMethod : inputMethodVector) {
        if (inputMethod == nullptr || (inputMethod->GetSessionState() != SessionState::STATE_FOREGROUND &&
                inputMethod->GetSessionState() != SessionState::STATE_ACTIVE)) {
            continue;
        }
        SessionGravity gravity = inputMethod->GetKeyboardGravity();
        if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT || !inputMethod->IsKeyboardAvoidAreaActive()) {
            continue;
        }
        if (isKeyboardPanelEnabled_) {
            WSRect keyboardRect;
            if (inputMethod && inputMethod->GetKeyboardPanelSession()) {
                keyboardRect = inputMethod->GetKeyboardPanelSession()->GetSessionRect();
                inputMethod->RecalculatePanelRectForAvoidArea(keyboardRect);
            }
            CalculateAvoidAreaRect(rect, keyboardRect, avoidArea);
            TLOGI(WmsLogTag::WMS_IMMS, "root scene %{public}s keyboard %{public}s area %{public}s",
                  rect.ToString().c_str(), keyboardRect.ToString().c_str(), avoidArea.ToString().c_str());
        } else {
            WSRect inputMethodRect = inputMethod->GetSessionRect();
            CalculateAvoidAreaRect(rect, inputMethodRect, avoidArea);
            TLOGI(WmsLogTag::WMS_IMMS, "root scene %{public}s input method %{public}s area %{public}s",
                  rect.ToString().c_str(), inputMethodRect.ToString().c_str(), avoidArea.ToString().c_str());
        }
    }
}

void RootSceneSession::GetCutoutAvoidAreaForRoot(const WSRect& rect, AvoidArea& avoidArea)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(GetSessionProperty()->GetDisplayId());
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "Failed to get display");
        return;
    }
    sptr<CutoutInfo> cutoutInfo = display->GetCutoutInfo();
    if (cutoutInfo == nullptr) {
        TLOGI(WmsLogTag::WMS_IMMS, "There is no CutoutInfo");
        return;
    }
    std::vector<DMRect> cutoutAreas = cutoutInfo->GetBoundingRects();
    if (cutoutAreas.empty()) {
        TLOGI(WmsLogTag::WMS_IMMS, "There is no cutoutAreas");
        return;
    }
    for (auto& cutoutArea : cutoutAreas) {
        WSRect cutoutAreaRect = {
            cutoutArea.posX_, cutoutArea.posY_,
            cutoutArea.width_, cutoutArea.height_
        };
        CalculateAvoidAreaRect(rect, cutoutAreaRect, avoidArea);
        TLOGI(WmsLogTag::WMS_IMMS, "root scene %{public}s cutout %{public}s area %{public}s",
              rect.ToString().c_str(), cutoutAreaRect.ToString().c_str(), avoidArea.ToString().c_str());
    }
}

void RootSceneSession::GetAINavigationBarAreaForRoot(const WSRect& rect, AvoidArea& avoidArea)
{
    WSRect barArea;
    if (specificCallback_ != nullptr && specificCallback_->onGetAINavigationBarArea_) {
        barArea = specificCallback_->onGetAINavigationBarArea_(GetSessionProperty()->GetDisplayId());
    }
    CalculateAvoidAreaRect(rect, barArea, avoidArea);
    TLOGI(WmsLogTag::WMS_IMMS, "root scene %{public}s AI bar %{public}s area %{public}s",
          rect.ToString().c_str(), barArea.ToString().c_str(), avoidArea.ToString().c_str());
}

AvoidArea RootSceneSession::GetAvoidAreaByType(AvoidAreaType type, const WSRect& rect, int32_t apiVersion)
{
    auto task = [weakThis = wptr(this), type]() -> AvoidArea {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_IMMS, "session is null");
            return {};
        }

        AvoidArea avoidArea;
        WSRect sessionRect = session->GetSessionRect();
        switch (type) {
            case AvoidAreaType::TYPE_SYSTEM: {
                session->GetSystemAvoidAreaForRoot(sessionRect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_CUTOUT: {
                session->GetCutoutAvoidAreaForRoot(sessionRect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_SYSTEM_GESTURE: {
                return avoidArea;
            }
            case AvoidAreaType::TYPE_KEYBOARD: {
                session->GetKeyboardAvoidAreaForRoot(sessionRect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_NAVIGATION_INDICATOR: {
                session->GetAINavigationBarAreaForRoot(sessionRect, avoidArea);
                return avoidArea;
            }
            default: {
                TLOGNE(WmsLogTag::WMS_IMMS, "cannot find type %{public}u, id %{public}d",
                    type, session->GetPersistentId());
                return avoidArea;
            }
        }
    };
    return PostSyncTask(task, __func__);
}

void RootSceneSession::SetRootSessionRect(const WSRect& rect)
{
    if (!rect.IsInvalid() && winRect_ != rect) {
        winRect_ = rect;
        TLOGI(WmsLogTag::WMS_IMMS, "root session update rect: %{public}s", winRect_.ToString().c_str());
        if (specificCallback_ != nullptr && specificCallback_->onUpdateAvoidArea_) {
            specificCallback_->onUpdateAvoidArea_(GetPersistentId());
        }
    }
}

WSError RootSceneSession::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    if (specificCallback_ == nullptr || specificCallback_->onNotifyAvoidAreaChange_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "callback is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    specificCallback_->onNotifyAvoidAreaChange_(avoidArea, type);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen

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
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "failed to get session property");
        return;
    }
    std::vector<sptr<SceneSession>> statusBarVector;
    if (specificCallback_ != nullptr && specificCallback_->onGetSceneSessionVectorByType_) {
        statusBarVector = specificCallback_->onGetSceneSessionVectorByType_(
            WindowType::WINDOW_TYPE_STATUS_BAR, sessionProperty->GetDisplayId());
    }
    for (auto& statusBar : statusBarVector) {
        if (!statusBar->IsVisible()) {
            continue;
        }
        WSRect statusBarRect = statusBar->GetSessionRect();
        CalculateAvoidAreaRect(rect, statusBarRect, avoidArea);
        TLOGI(WmsLogTag::WMS_IMMS, "root scene %{public}s status bar %{public}s area %{public}s",
              rect.ToString().c_str(), statusBarRect.ToString().c_str(), avoidArea.ToString().c_str());
    }
}

void RootSceneSession::GetKeyboardAvoidAreaForRoot(const WSRect& rect, AvoidArea& avoidArea)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "failed to get session property");
        return;
    }
    std::vector<sptr<SceneSession>> inputMethodVector;
    if (specificCallback_ != nullptr && specificCallback_->onGetSceneSessionVectorByType_) {
        inputMethodVector = specificCallback_->onGetSceneSessionVectorByType_(
            WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT, sessionProperty->GetDisplayId());
    }
    for (auto& inputMethod : inputMethodVector) {
        if (inputMethod->IsVisible()) {
            continue;
        }
        SessionGravity gravity = inputMethod->GetKeyboardGravity();
        if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
            continue;
        }
        if (isKeyboardPanelEnabled_) {
            WSRect keyboardRect;
            if (inputMethod && inputMethod->GetKeyboardPanelSession()) {
                keyboardRect = inputMethod->GetKeyboardPanelSession()->GetSessionRect();
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
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "failed to get session property");
        return;
    }
    auto display = DisplayManager::GetInstance().GetDisplayById(sessionProperty->GetDisplayId());
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
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "failed to get session property");
        return;
    }
    WSRect barArea;
    if (specificCallback_ != nullptr && specificCallback_->onGetAINavigationBarArea_) {
        barArea = specificCallback_->onGetAINavigationBarArea_(sessionProperty->GetDisplayId());
    }
    CalculateAvoidAreaRect(rect, barArea, avoidArea);
    TLOGI(WmsLogTag::WMS_IMMS, "root scene %{public}s AI bar %{public}s area %{public}s",
          rect.ToString().c_str(), barArea.ToString().c_str(), avoidArea.ToString().c_str());
}

AvoidArea RootSceneSession::GetAvoidAreaByType(AvoidAreaType type)
{
    auto task = [weakThis = wptr(this), type]() -> AvoidArea {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_IMMS, "session is null");
            return {};
        }

        AvoidArea avoidArea;
        WSRect rect = session->GetSessionRect();
        switch (type) {
            case AvoidAreaType::TYPE_SYSTEM: {
                session->GetSystemAvoidAreaForRoot(rect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_CUTOUT: {
                session->GetCutoutAvoidAreaForRoot(rect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_SYSTEM_GESTURE: {
                return avoidArea;
            }
            case AvoidAreaType::TYPE_KEYBOARD: {
                session->GetKeyboardAvoidAreaForRoot(rect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_NAVIGATION_INDICATOR: {
                session->GetAINavigationBarAreaForRoot(rect, avoidArea);
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
} // namespace OHOS::Rosen

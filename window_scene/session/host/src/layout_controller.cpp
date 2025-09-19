/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "session/host/include/layout_controller.h"

#include <cinttypes>
#include <transaction/rs_transaction.h>
#include <ui/rs_surface_node.h>

#include "display_manager.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/session_utils.h"
#include "session_helper.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "LayoutController"};
}

LayoutController::LayoutController(const sptr<WindowSessionProperty>& property)
{
    sessionProperty_ = property;
}

// LCOV_EXCL_START
bool LayoutController::SetSessionGlobalRect(const WSRect& rect)
{
    std::lock_guard<std::mutex> lock(globalRectMutex_);
    if (globalRect_ == rect) {
        return false;
    }
    globalRect_ = rect;
    return true;
}
// LCOV_EXCL_STOP

WSRect LayoutController::GetSessionGlobalRect() const
{
    if (Session::IsScbCoreEnabled()) {
        std::lock_guard<std::mutex> lock(globalRectMutex_);
        return globalRect_;
    }
    return winRect_;
}

void LayoutController::SetClientRect(const WSRect& rect)
{
    if (clientRect_ == rect) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "id:%{public}d skip same rect", GetSessionPersistentId());
        return;
    }
    clientRect_ = rect;
    TLOGD(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, update client rect:%{public}s",
        GetSessionPersistentId(), rect.ToString().c_str());
}

WSRect LayoutController::GetClientRect() const
{
    return clientRect_;
}

void LayoutController::GetGlobalScaledRect(Rect& globalScaledRect)
{
    WSRect scaledRect = GetSessionGlobalRect();
    scaledRect.width_ *= GetScaleX();
    scaledRect.height_ *= GetScaleY();
    globalScaledRect = { scaledRect.posX_, scaledRect.posY_, scaledRect.width_, scaledRect.height_ };
}

// LCOV_EXCL_START
WSRect LayoutController::ConvertRelativeRectToGlobal(const WSRect& relativeRect, DisplayId currentDisplayId) const
{
    sptr<ScreenSession> screenSession =
        ScreenSessionManagerClient::GetInstance().GetScreenSessionById(currentDisplayId);
    if (!screenSession) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Screen session is null, displayId:%{public}" PRIu64, currentDisplayId);
        screenSession =
            ScreenSessionManagerClient::GetInstance().GetScreenSessionById(sessionProperty_->GetDisplayId());
        if (!screenSession) {
            TLOGW(WmsLogTag::WMS_LAYOUT, "Screen session is null, displayId:%{public}" PRIu64,
                sessionProperty_->GetDisplayId());
            return relativeRect;
        }
    }
    const ScreenProperty& screenProperty = screenSession->GetScreenProperty();
    int32_t currentDisplayOffsetX = static_cast<int32_t>(screenProperty.GetStartX());
    int32_t currentDisplayOffsetY = static_cast<int32_t>(screenProperty.GetStartY());
    TLOGI(WmsLogTag::WMS_LAYOUT, "id:%{public}d, relativeRect:%{public}s, offsetX:%{public}d, offsetY:%{public}d",
        GetSessionPersistentId(), relativeRect.ToString().c_str(), currentDisplayOffsetX, currentDisplayOffsetY);
    return {relativeRect.posX_ + currentDisplayOffsetX,
            relativeRect.posY_ + currentDisplayOffsetY,
            relativeRect.width_, relativeRect.height_};
}

WSRect LayoutController::ConvertGlobalRectToRelative(const WSRect& globalRect, DisplayId targetDisplayId) const
{
    sptr<ScreenSession> screenSession =
        ScreenSessionManagerClient::GetInstance().GetScreenSessionById(targetDisplayId);
    if (!screenSession) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Screen session is null, displayId:%{public}" PRIu64, targetDisplayId);
        screenSession =
            ScreenSessionManagerClient::GetInstance().GetScreenSessionById(sessionProperty_->GetDisplayId());
        if (!screenSession) {
            TLOGW(WmsLogTag::WMS_LAYOUT, "Screen session is null, displayId:%{public}" PRIu64,
                sessionProperty_->GetDisplayId());
            return globalRect;
        }
    }
    const ScreenProperty& screenProperty = screenSession->GetScreenProperty();
    int32_t targetDisplayOffsetX = static_cast<int32_t>(screenProperty.GetStartX());
    int32_t targetDisplayOffsetY = static_cast<int32_t>(screenProperty.GetStartY());
    TLOGI(WmsLogTag::WMS_LAYOUT, "id:%{public}d, globalRect:%{public}s, offsetX:%{public}d, offsetY:%{public}d",
        GetSessionPersistentId(), globalRect.ToString().c_str(), targetDisplayOffsetX, targetDisplayOffsetY);
    return {globalRect.posX_ - targetDisplayOffsetX,
            globalRect.posY_ - targetDisplayOffsetY,
            globalRect.width_, globalRect.height_};
}

int32_t LayoutController::GetSessionPersistentId() const
{
    return sessionProperty_->GetPersistentId();
}

WSRect LayoutController::AdjustRectByAspectRatio(const WSRect& rect, const WindowDecoration& decoration)
{
    if (sessionProperty_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "sessionProperty is null");
        return rect;
    }
    auto windowId = GetSessionPersistentId();
    auto mode = sessionProperty_->GetWindowMode();
    auto type = sessionProperty_->GetWindowType();
    if (mode != WindowMode::WINDOW_MODE_FLOATING || !WindowHelper::IsMainWindow(type)) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Skip adjustment, windowId: %{public}d, mode: %{public}u, type: %{public}u",
            windowId, static_cast<uint32_t>(mode), static_cast<uint32_t>(type));
        return rect;
    }
    auto displayId = sessionProperty_->GetDisplayId();
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    if (!display) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "display is null, displayId: %{public}" PRIu64, displayId);
        return rect;
    }
    float vpr = display->GetVirtualPixelRatio();
    auto limits = sessionProperty_->GetWindowLimits();
    SessionUtils::CalcFloatWindowRectLimits(getSystemConfigFunc_().maxFloatingWindowSize_, vpr, limits);
    const WSRect& adjustedRect = SessionUtils::AdjustRectByAspectRatio(rect, limits, decoration, aspectRatio_);
    TLOGD(WmsLogTag::WMS_LAYOUT, "After adjustment, windowId: %{public}d, rect: %{public}s",
        windowId, adjustedRect.ToString().c_str());
    return adjustedRect;
}

void LayoutController::SetScale(float scaleX, float scaleY, float pivotX, float pivotY)
{
    scaleX_ = scaleX;
    scaleY_ = scaleY;
    pivotX_ = pivotX;
    pivotY_ = pivotY;
}

void LayoutController::SetClientScale(float scaleX, float scaleY, float pivotX, float pivotY)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, preScaleX:%{public}f, preScaleY:%{public}f, "
        "newScaleX:%{public}f, newScaleY:%{public}f", GetSessionPersistentId(),
        clientScaleX_, clientScaleY_, scaleX, scaleY);
    clientScaleX_ = scaleX;
    clientScaleY_ = scaleY;
    clientPivotX_ = pivotX;
    clientPivotY_ = pivotY;
}

bool LayoutController::IsTransformNeedUpdate(float scaleX, float scaleY, float pivotX, float pivotY)
{
    bool nearEqual = NearEqual(scaleX_, scaleX) && NearEqual(scaleY_, scaleY) &&
                     NearEqual(pivotX_, pivotX) && NearEqual(pivotY_, pivotY) &&
                     NearEqual(clientScaleX_, scaleX) && NearEqual(clientScaleY_, scaleY) &&
                     NearEqual(clientPivotX_, pivotX) && NearEqual(clientPivotY_, pivotY);
    return !nearEqual;
}
// LCOV_EXCL_STOP

void LayoutController::SetSystemConfigFunc(GetSystemConfigFunc&& func)
{
    if (!func) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "func is null");
        return;
    }
    getSystemConfigFunc_ = std::move(func);
}

}  // namespace OHOS::Rosen

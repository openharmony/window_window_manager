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

#include "session/host/include/scb_system_session.h"

#include <hisysevent.h>
#include "key_event.h"
#include "pointer_event.h"
#include <ui/rs_surface_node.h>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SCBSystemSession" };
} // namespace

SCBSystemSession::SCBSystemSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SceneSession(info, specificCallback)
{
    auto name = sessionInfo_.bundleName_;
    auto pos = name.find_last_of('.');
    name = (pos == std::string::npos) ? name : name.substr(pos + 1); // skip '.'
    if (sessionInfo_.isSystem_) {
        RSSurfaceNodeConfig config;
        config.SurfaceNodeName = name;
        config.surfaceWindowType = SurfaceWindowType::SYSTEM_SCB_WINDOW;
        surfaceNode_ = Rosen::RSSurfaceNode::Create(config, Rosen::RSSurfaceNodeType::APP_WINDOW_NODE);
    }
    WLOGFD("Create SCBSystemSession");
}

SCBSystemSession::~SCBSystemSession()
{
    WLOGD("~SCBSystemSession, id: %{public}d", GetPersistentId());
}

void SCBSystemSession::RegisterBufferAvailableCallback(const SystemSessionBufferAvailableCallback& func)
{
    if (surfaceNode_) {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "Set buffer available callback");
        surfaceNode_->SetBufferAvailableCallback(func);
    } else {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "surfaceNode_ is null");
    }
}

WSError SCBSystemSession::ProcessPointDownSession(int32_t posX, int32_t posY)
{
    const auto& id = GetPersistentId();
    const auto& type = GetWindowType();
    WLOGFI("id: %{public}d, type: %{public}d", id, type);
    PresentFocusIfPointDown();
    return SceneSession::ProcessPointDownSession(posX, posY);
}

WSError SCBSystemSession::NotifyClientToUpdateRect(const std::string& updateReason,
    std::shared_ptr<RSTransaction> rsTransaction)
{
    auto task = [weakThis = wptr(this), rsTransaction, updateReason]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WSError ret = session->NotifyClientToUpdateRectTask(updateReason, rsTransaction);
        if (session->specificCallback_ != nullptr && session->specificCallback_->onUpdateAvoidArea_ != nullptr &&
            session->specificCallback_->onClearDisplayStatusBarTemporarilyFlags_ != nullptr) {
            if (Session::IsScbCoreEnabled()) {
                session->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
            } else {
                session->specificCallback_->onUpdateAvoidArea_(session->GetPersistentId());
            }
            session->specificCallback_->onClearDisplayStatusBarTemporarilyFlags_();
        }
        if (session->GetWindowType() == WindowType::WINDOW_TYPE_KEYBOARD_PANEL &&
            session->keyboardPanelRectUpdateCallback_ && session->isKeyboardPanelEnabled_) {
            session->keyboardPanelRectUpdateCallback_();
        }
        // clear after use
        if (session->reason_ != SizeChangeReason::DRAG) {
            session->reason_ = SizeChangeReason::UNDEFINED;
            session->dirtyFlags_ &= ~static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
        }
        return ret;
    };
    PostTask(task, "NotifyClientToUpdateRect");
    return WSError::WS_OK;
}

void SCBSystemSession::SetKeyboardPanelRectUpdateCallback(const KeyboardPanelRectUpdateCallback& func)
{
    keyboardPanelRectUpdateCallback_ = func;
}

void SCBSystemSession::BindKeyboardSession(sptr<SceneSession> session)
{
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr");
        return;
    }
    keyboardSession_ = session;
    KeyboardPanelRectUpdateCallback onKeyboardPanelRectUpdate = [this]() {
        if (this->keyboardSession_ != nullptr) {
            this->keyboardSession_->OnKeyboardPanelUpdated();
        }
    };
    SetKeyboardPanelRectUpdateCallback(onKeyboardPanelRectUpdate);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Success, id: %{public}d", keyboardSession_->GetPersistentId());
}

sptr<SceneSession> SCBSystemSession::GetKeyboardSession() const
{
    return keyboardSession_;
}

void SCBSystemSession::PresentFocusIfPointDown()
{
    WLOGFI("PresentFocusIfPointDown, id: %{public}d, type: %{public}d", GetPersistentId(), GetWindowType());
    if (!isFocused_ && GetFocusable()) {
        FocusChangeReason reason = FocusChangeReason::CLICK;
        NotifyRequestFocusStatusNotifyManager(true, false, reason);
        NotifyClick();
    }
}

WSError SCBSystemSession::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }

    WSError ret = Session::TransferKeyEvent(keyEvent);
    return ret;
}

void SCBSystemSession::PresentFoucusIfNeed(int32_t pointerAction)
{
    WLOGFD("OnClick down, id: %{public}d", GetPersistentId());
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        if (!isFocused_ && GetFocusable()) {
            FocusChangeReason reason = FocusChangeReason::CLICK;
            NotifyRequestFocusStatusNotifyManager(true, false, reason);
            NotifyClick();
        }
    }
}

WSError SCBSystemSession::UpdateFocus(bool isFocused)
{
    if (isFocused_ == isFocused) {
        TLOGD(WmsLogTag::WMS_FOCUS, "Session focus do not change");
        return WSError::WS_DO_NOTHING;
    }
    isFocused_ = isFocused;
    // notify scb arkui focus
    if (isFocused) {
        HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "FOCUS_WINDOW",
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "PID", getpid(),
            "UID", getuid(),
            "BUNDLE_NAME", sessionInfo_.bundleName_);
        NotifyUIRequestFocus();
    } else {
        NotifyUILostFocus();
    }
    return WSError::WS_OK;
}

WSError SCBSystemSession::UpdateWindowMode(WindowMode mode)
{
    WLOGFD("session is system, id: %{public}d, mode: %{public}d, name: %{public}s, state: %{public}u",
        GetPersistentId(), static_cast<int32_t>(mode), sessionInfo_.bundleName_.c_str(), GetSessionState());
    return WSError::WS_ERROR_INVALID_SESSION;
}

WSError SCBSystemSession::SetSystemSceneBlockingFocus(bool blocking)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "Session set blocking focus, id: %{public}d, mode: %{public}d",
        GetPersistentId(), blocking);
    blockingFocus_ = blocking;
    return WSError::WS_OK;
}

void SCBSystemSession::UpdatePointerArea(const WSRect& rect)
{
    // scb system session do nothing
    return;
}

void SCBSystemSession::SetSkipSelfWhenShowOnVirtualScreen(bool isSkip)
{
    TLOGD(WmsLogTag::WMS_SCB, "Set Skip Self, isSkip: %{public}d", isSkip);
    auto task = [weakThis = wptr(this), isSkip]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_SCB, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        std::shared_ptr<RSSurfaceNode> surfaceNode = session->GetSurfaceNode();
        if (!surfaceNode) {
            TLOGE(WmsLogTag::WMS_SCB, "surfaceNode_ is null");
            return WSError::WS_OK;
        }
        if (session->specificCallback_ != nullptr
            && session->specificCallback_->onSetSkipSelfWhenShowOnVirtualScreen_ != nullptr) {
            session->specificCallback_->onSetSkipSelfWhenShowOnVirtualScreen_(surfaceNode->GetId(), isSkip);
        }
        return WSError::WS_OK;
    };
    PostTask(task, "SetSkipSelf");
}

std::shared_ptr<RSSurfaceNode> SCBSystemSession::GetSurfaceNode()
{
    if (!surfaceNode_) {
        TLOGE(WmsLogTag::WMS_SCB, "surfaceNode_ is null");
        return nullptr;
    }
    return surfaceNode_;
}

bool SCBSystemSession::IsVisibleForeground() const
{
    return isVisible_;
}

void SCBSystemSession::NotifyClientToUpdateAvoidArea()
{
    SceneSession::NotifyClientToUpdateAvoidArea();
    if (GetWindowType() == WindowType::WINDOW_TYPE_KEYBOARD_PANEL &&
        keyboardPanelRectUpdateCallback_ && isKeyboardPanelEnabled_) {
        keyboardPanelRectUpdateCallback_();
    }
}
} // namespace OHOS::Rosen

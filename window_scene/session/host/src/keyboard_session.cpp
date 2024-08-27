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

#include "session/host/include/keyboard_session.h"
#include "session/host/include/session.h"
#include "common/include/session_permission.h"
#include "display_manager.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "session_helper.h"
#include <parameters.h>
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

KeyboardSession::KeyboardSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback,
    const sptr<KeyboardSessionCallback>& keyboardCallback)
    : SystemSession(info, specificCallback)
{
    keyboardCallback_ = keyboardCallback;
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Create KeyboardSession");
}

KeyboardSession::~KeyboardSession()
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "~KeyboardSession");
}

void KeyboardSession::BindKeyboardPanelSession(sptr<SceneSession> panelSession)
{
    if (panelSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "panelSession is nullptr");
        return;
    }
    keyboardPanelSession_ = panelSession;
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Success, panelId: %{public}d", panelSession->GetPersistentId());
}

sptr<SceneSession> KeyboardSession::GetKeyboardPanelSession() const
{
    return keyboardPanelSession_;
}

SessionGravity KeyboardSession::GetKeyboardGravity() const
{
    SessionGravity gravity = SessionGravity::SESSION_GRAVITY_DEFAULT;
    uint32_t percent = 0;
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty) {
        sessionProperty->GetSessionGravity(gravity, percent);
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "gravity: %{public}d", gravity);
    return gravity;
}

WSError KeyboardSession::Show(sptr<WindowSessionProperty> property)
{
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Session property is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!CheckPermissionWithPropertyAnimation(property)) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null, show keyboard failed");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->UseFocusIdIfCallingSessionIdInvalid();
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Show keyboard session, id: %{public}d, calling session id: %{public}d",
            session->GetPersistentId(), session->GetCallingSessionId());
        session->MoveAndResizeKeyboard(property->GetKeyboardLayoutParams(), property, true);
        session->NotifySessionRectChange(session->GetSessionRequestRect(), SizeChangeReason::UNDEFINED);
        return session->SceneSession::Foreground(property);
    };
    PostTask(task, "Show");
    return WSError::WS_OK;
}

WSError KeyboardSession::Hide()
{
    if (!CheckPermissionWithPropertyAnimation(GetSessionProperty())) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null, hide keyboard failed!");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }

        TLOGI(WmsLogTag::WMS_KEYBOARD, "Hide keyboard session, set callingSessionId to 0, id: %{public}d",
            session->GetPersistentId());
        auto ret = session->SetActive(false);
        if (ret != WSError::WS_OK) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Set session active state failed, ret: %{public}d", ret);
            return ret;
        }
        ret = session->SceneSession::Background();
        WSRect rect = {0, 0, 0, 0};
        session->NotifyKeyboardPanelInfoChange(rect, false);
        if (session->systemConfig_.uiType_ == "pc" || session->GetSessionScreenName() == "HiCar"
            || session->GetSessionScreenName() == "SuperLauncher") {
            TLOGD(WmsLogTag::WMS_KEYBOARD, "pc or virtual screen, restore calling session");
            session->RestoreCallingSession();
            auto sessionProperty = session->GetSessionProperty();
            if (sessionProperty) {
                sessionProperty->SetCallingSessionId(INVALID_WINDOW_ID);
            }
        }
        return ret;
    };
    PostTask(task, "Hide");
    return WSError::WS_OK;
}

WSError KeyboardSession::Disconnect(bool isFromClient)
{
    auto task = [weakThis = wptr(this), isFromClient]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null, disconnect keyboard session failed!");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Disconnect keyboard session, id: %{public}d, isFromClient: %{public}d",
            session->GetPersistentId(), isFromClient);
        session->SceneSession::Disconnect(isFromClient);
        WSRect rect = {0, 0, 0, 0};
        session->NotifyKeyboardPanelInfoChange(rect, false);
        session->RestoreCallingSession();
        auto sessionProperty = session->GetSessionProperty();
        if (sessionProperty) {
            sessionProperty->SetCallingSessionId(INVALID_WINDOW_ID);
        }
        return WSError::WS_OK;
    };
    PostTask(task, "Disconnect");
    return WSError::WS_OK;
}

WSError KeyboardSession::NotifyClientToUpdateRect(std::shared_ptr<RSTransaction> rsTransaction)
{
    auto task = [weakThis = wptr(this), rsTransaction]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WSError ret = session->NotifyClientToUpdateRectTask(rsTransaction);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        if (session->reason_ != SizeChangeReason::DRAG) {
            session->reason_ = SizeChangeReason::UNDEFINED;
            session->dirtyFlags_ &= ~static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
        }
        return ret;
    };
    PostTask(task, "NotifyClientToUpdateRect");
    return WSError::WS_OK;
}

void KeyboardSession::OnKeyboardPanelUpdated()
{
    if (!isKeyboardPanelEnabled_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "KeyboardPanel is not enabled");
        return;
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d", GetPersistentId());
    WSRect panelRect = { 0, 0, 0, 0 };
    panelRect = (keyboardPanelSession_ == nullptr) ? panelRect : keyboardPanelSession_->GetSessionRect();
    RaiseCallingSession(panelRect);
    if (specificCallback_ != nullptr && specificCallback_->onUpdateAvoidArea_ != nullptr) {
        if (Session::IsScbCoreEnabled()) {
            dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
        } else {
            specificCallback_->onUpdateAvoidArea_(GetPersistentId());
        }
    }
}

WSError KeyboardSession::SetKeyboardSessionGravity(SessionGravity gravity, uint32_t percent)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "keyboardId: %{public}d, gravity: %{public}d, percent: %{public}d",
        GetPersistentId(), gravity, percent);
    if (sessionChangeCallback_ && sessionChangeCallback_->onKeyboardGravityChange_) {
        sessionChangeCallback_->onKeyboardGravityChange_(gravity);
    }
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty) {
        sessionProperty->SetKeyboardSessionGravity(gravity, percent);
    }
    RelayoutKeyBoard();
    if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        SetWindowAnimationFlag(false);
        if (IsSessionForeground()) {
            RestoreCallingSession();
        }
    } else {
        SetWindowAnimationFlag(true);
    }
    return WSError::WS_OK;
}

void KeyboardSession::SetCallingSessionId(uint32_t callingSessionId)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session id: %{public}d", callingSessionId);
    UpdateCallingSessionIdAndPosition(callingSessionId);
    if (keyboardCallback_ == nullptr || keyboardCallback_->onCallingSessionIdChange_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "KeyboardCallback_, callingSessionId: %{public}d",
            callingSessionId);
        return;
    }
    keyboardCallback_->onCallingSessionIdChange_(GetCallingSessionId());
}

uint32_t KeyboardSession::GetCallingSessionId()
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Session property is null");
        return INVALID_SESSION_ID;
    }
    return sessionProperty->GetCallingSessionId();
}

WSError KeyboardSession::AdjustKeyboardLayout(const KeyboardLayoutParams& params)
{
    auto task = [weakThis = wptr(this), params]() -> WSError {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboard session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        auto sessionProperty = session->GetSessionProperty();
        if (sessionProperty == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session property is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        sessionProperty->SetKeyboardLayoutParams(params);
        session->MoveAndResizeKeyboard(params, sessionProperty, false);
        session->SetKeyboardSessionGravity(static_cast<SessionGravity>(params.gravity_), 0);
        session->NotifySessionRectChange(session->GetSessionRequestRect(), SizeChangeReason::UNDEFINED);
        TLOGI(WmsLogTag::WMS_KEYBOARD, "adjust keyboard layout, keyboardId: %{public}d, gravity: %{public}u, "
            "LandscapeKeyboardRect: %{public}s, PortraitKeyboardRect: %{public}s, LandscapePanelRect: %{public}s, "
            "PortraitPanelRect: %{public}s, requestRect: %{public}s", session->GetPersistentId(),
            static_cast<uint32_t>(params.gravity_), params.LandscapeKeyboardRect_.ToString().c_str(),
            params.PortraitKeyboardRect_.ToString().c_str(), params.LandscapePanelRect_.ToString().c_str(),
            params.PortraitPanelRect_.ToString().c_str(), session->GetSessionRequestRect().ToString().c_str());
        if (session->adjustKeyboardLayoutFunc_) {
            session->adjustKeyboardLayoutFunc_(params);
        }
        return WSError::WS_OK;
    };
    PostTask(task, "AdjustKeyboardLayout");
    return WSError::WS_OK;
}

sptr<SceneSession> KeyboardSession::GetSceneSession(uint32_t persistentId)
{
    if (keyboardCallback_ == nullptr || keyboardCallback_->onGetSceneSession_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Get scene session failed, persistentId: %{public}d", persistentId);
        return nullptr;
    }
    return keyboardCallback_->onGetSceneSession_(persistentId);
}

int32_t KeyboardSession::GetFocusedSessionId()
{
    if (keyboardCallback_ == nullptr || keyboardCallback_->onGetFocusedSessionId_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboardCallback_ is nullptr, get focusedSessionId failed!");
        return INVALID_WINDOW_ID;
    }
    return keyboardCallback_->onGetFocusedSessionId_();
}

void KeyboardSession::NotifyOccupiedAreaChangeInfo(const sptr<SceneSession>& callingSession, const WSRect& rect,
    const WSRect& occupiedArea, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    // if keyboard will occupy calling, notify calling window the occupied area and safe height
    const WSRect& safeRect = SessionHelper::GetOverlap(occupiedArea, rect, 0, 0);
    const WSRect& lastSafeRect = callingSession->GetLastSafeRect();
    if (lastSafeRect == safeRect) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "SafeRect is same to lastSafeRect: %{public}s", safeRect.ToString().c_str());
        return;
    }
    callingSession->SetLastSafeRect(safeRect);
    double textFieldPositionY = 0.0;
    double textFieldHeight = 0.0;
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        textFieldPositionY = sessionProperty->GetTextFieldPositionY();
        textFieldHeight = sessionProperty->GetTextFieldHeight();
    }
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT,
        SessionHelper::TransferToRect(safeRect), safeRect.height_, textFieldPositionY, textFieldHeight);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "lastSafeRect: %{public}s, safeRect: %{public}s, keyboardRect: %{public}s, "
        "textFieldPositionY_: %{public}f, textFieldHeight_: %{public}f", lastSafeRect.ToString().c_str(),
        safeRect.ToString().c_str(), occupiedArea.ToString().c_str(), textFieldPositionY, textFieldHeight);
    callingSession->NotifyOccupiedAreaChangeInfo(info, rsTransaction);
}

void KeyboardSession::NotifyKeyboardPanelInfoChange(WSRect rect, bool isKeyboardPanelShow)
{
    if (!isKeyboardPanelEnabled_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "KeyboardPanel is not enabled");
        return;
    }
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "sessionStage_ is nullptr, notify keyboard panel rect change failed");
        return;
    }
    KeyboardPanelInfo keyboardPanelInfo;
    keyboardPanelInfo.rect_ = SessionHelper::TransferToRect(rect);
    keyboardPanelInfo.gravity_ = static_cast<WindowGravity>(GetKeyboardGravity());
    keyboardPanelInfo.isShowing_ = isKeyboardPanelShow;

    sessionStage_->NotifyKeyboardPanelInfoChange(keyboardPanelInfo);
}

bool KeyboardSession::CheckIfNeedRaiseCallingSession(sptr<SceneSession> callingSession, bool isCallingSessionFloating)
{
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is nullptr");
        return false;
    }

    SessionGravity gravity = GetKeyboardGravity();
    if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No need to raise calling session, gravity: %{public}d", gravity);
        return false;
    }
    bool isMainOrParentFloating = WindowHelper::IsMainWindow(callingSession->GetWindowType()) ||
        (WindowHelper::IsSubWindow(callingSession->GetWindowType()) && callingSession->GetParentSession() != nullptr &&
         callingSession->GetParentSession()->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);
    bool isFreeMultiWindowMode = callingSession->IsFreeMultiWindowMode();
    if (isCallingSessionFloating && isMainOrParentFloating &&
        (systemConfig_.uiType_ == "phone" || (systemConfig_.uiType_ == "pad" && !isFreeMultiWindowMode))) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No need to raise calling session in float window.");
        return false;
    }

    return true;
}

void KeyboardSession::RaiseCallingSession(const WSRect& keyboardPanelRect,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    sptr<SceneSession> callingSession = GetSceneSession(GetCallingSessionId());
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is nullptr");
        return;
    }
    NotifyKeyboardPanelInfoChange(keyboardPanelRect, true);

    bool isCallingSessionFloating = (callingSession->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);
    if (!CheckIfNeedRaiseCallingSession(callingSession, isCallingSessionFloating)) {
        return;
    }

    WSRect callingSessionRect = callingSession->GetSessionRect();
    int32_t oriPosYBeforeRaisedBykeyboard = callingSession->GetOriPosYBeforeRaisedByKeyboard();
    if (oriPosYBeforeRaisedBykeyboard != 0 && isCallingSessionFloating) {
        callingSessionRect.posY_ = oriPosYBeforeRaisedBykeyboard;
    }
    if (SessionHelper::IsEmptyRect(SessionHelper::GetOverlap(keyboardPanelRect, callingSessionRect, 0, 0)) &&
        oriPosYBeforeRaisedBykeyboard == 0) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No overlap area, keyboardRect: %{public}s, callingRect: %{public}s",
            keyboardPanelRect.ToString().c_str(), callingSessionRect.ToString().c_str());
        NotifyOccupiedAreaChangeInfo(callingSession, callingSessionRect, keyboardPanelRect, rsTransaction);
        return;
    }

    WSRect newRect = callingSessionRect;
    int32_t statusHeight = callingSession->GetStatusBarHeight();
    if (isCallingSessionFloating && callingSessionRect.posY_ > statusHeight) {
        if (oriPosYBeforeRaisedBykeyboard == 0) {
            oriPosYBeforeRaisedBykeyboard = callingSessionRect.posY_;
            callingSession->SetOriPosYBeforeRaisedByKeyboard(callingSessionRect.posY_);
        }
        // calculate new rect of calling session
        newRect.posY_ = std::max(keyboardPanelRect.posY_ - newRect.height_, statusHeight);
        newRect.posY_ = std::min(oriPosYBeforeRaisedBykeyboard, newRect.posY_);
        NotifyOccupiedAreaChangeInfo(callingSession, newRect, keyboardPanelRect, rsTransaction);
        callingSession->UpdateSessionRect(newRect, SizeChangeReason::UNDEFINED);
    } else {
        NotifyOccupiedAreaChangeInfo(callingSession, newRect, keyboardPanelRect, rsTransaction);
    }

    TLOGI(WmsLogTag::WMS_KEYBOARD, "keyboardRect: %{public}s, CallSession OriRect: %{public}s, NewRect: %{public}s"
        ", OriPosYBeforeRaisedBykeyboard: %{public}d, isCallingSessionFloating: %{public}d",
        keyboardPanelRect.ToString().c_str(), callingSessionRect.ToString().c_str(), newRect.ToString().c_str(),
        oriPosYBeforeRaisedBykeyboard, isCallingSessionFloating);
}

void KeyboardSession::RestoreCallingSession(const std::shared_ptr<RSTransaction>& rsTransaction)
{
    sptr<SceneSession> callingSession = GetSceneSession(GetCallingSessionId());
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is nullptr");
        return;
    }
    const WSRect& emptyRect = { 0, 0, 0, 0 };
    int32_t oriPosYBeforeRaisedBykeyboard = callingSession->GetOriPosYBeforeRaisedByKeyboard();
    NotifyOccupiedAreaChangeInfo(callingSession, emptyRect, emptyRect, rsTransaction);
    if (oriPosYBeforeRaisedBykeyboard != 0 &&
        callingSession->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
        WSRect callingSessionRestoringRect = callingSession->GetSessionRect();
        if (oriPosYBeforeRaisedBykeyboard != 0) {
            callingSessionRestoringRect.posY_ = oriPosYBeforeRaisedBykeyboard;
        }
        TLOGI(WmsLogTag::WMS_KEYBOARD, "oriPosYBeforeRaisedBykeyboard: %{public}d, sessionMode: %{public}d",
            oriPosYBeforeRaisedBykeyboard, callingSession->GetWindowMode());
        callingSession->UpdateSessionRect(callingSessionRestoringRect, SizeChangeReason::UNDEFINED);
    }
    callingSession->SetOriPosYBeforeRaisedByKeyboard(0); // 0: default value
}

// Use focused session id when calling session id is invalid.
void KeyboardSession::UseFocusIdIfCallingSessionIdInvalid()
{
    if (GetSceneSession(GetCallingSessionId()) != nullptr) {
        return;
    }
    uint32_t focusedSessionId = static_cast<uint32_t>(GetFocusedSessionId());
    if (GetSceneSession(focusedSessionId) == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Focused session is null, id: %{public}d", focusedSessionId);
    } else {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Using focusedSession id: %{public}d", focusedSessionId);
            GetSessionProperty()->SetCallingSessionId(focusedSessionId);
    }
}

void KeyboardSession::UpdateCallingSessionIdAndPosition(uint32_t callingSessionId)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Session property is nullptr.");
        return;
    }
    uint32_t curSessionId = sessionProperty->GetCallingSessionId();
    // When calling window id changes, restore the old calling session, raise the new calling session.
    if (curSessionId != INVALID_WINDOW_ID && callingSessionId != curSessionId && IsSessionForeground()) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "CallingSession curId: %{public}d, newId: %{public}d",
            curSessionId, callingSessionId);
        RestoreCallingSession();

        sessionProperty->SetCallingSessionId(callingSessionId);
        UseFocusIdIfCallingSessionIdInvalid();
        WSRect panelRect = { 0, 0, 0, 0 };
        panelRect = (keyboardPanelSession_ == nullptr) ? panelRect : keyboardPanelSession_->GetSessionRect();
        RaiseCallingSession(panelRect);
    } else {
        sessionProperty->SetCallingSessionId(callingSessionId);
    }
}

void KeyboardSession::RelayoutKeyBoard()
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Session property is nullptr, relayout keyboard failed");
        return;
    }

    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    SessionGravity gravity = SessionGravity::SESSION_GRAVITY_DEFAULT;
    uint32_t percent = 0;
    sessionProperty->GetSessionGravity(gravity, percent);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Gravity: %{public}d, percent: %{public}d", gravity, percent);
    if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        return;
    }
    if (!GetScreenWidthAndHeightFromClient(sessionProperty, screenWidth, screenHeight)) {
        return;
    }

    auto requestRect = sessionProperty->GetRequestRect();
    if (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM) {
        requestRect.width_ = screenWidth;
        requestRect.posX_ = 0;
        if (percent != 0) {
            // 100: for calc percent.
            requestRect.height_ = static_cast<uint32_t>(screenHeight) * percent / 100u;
        }
    }
    requestRect.posY_ = static_cast<int32_t>(screenHeight - requestRect.height_);
    sessionProperty->SetRequestRect(requestRect);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Id: %{public}d, rect: %{public}s", GetPersistentId(),
        SessionHelper::TransferToWSRect(requestRect).ToString().c_str());
}

void KeyboardSession::OpenKeyboardSyncTransaction()
{
    if (isKeyboardSyncTransactionOpen_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Keyboard sync transaction is already open");
        return;
    }
    isKeyboardSyncTransactionOpen_ = true;
    auto task = []() {
        auto transactionController = RSSyncTransactionController::GetInstance();
        if (transactionController) {
            transactionController->OpenSyncTransaction();
        }
        return WSError::WS_OK;
    };
    PostSyncTask(task);
}

void KeyboardSession::CloseKeyboardSyncTransaction(const WSRect& keyboardPanelRect,
    bool isKeyboardShow, bool isRotating)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "keyboardPanelRect: %{public}s, isKeyboardShow: %{public}d, isRotating: %{public}d",
        keyboardPanelRect.ToString().c_str(), isKeyboardShow, isRotating);
    auto task = [weakThis = wptr(this), keyboardPanelRect, isKeyboardShow, isRotating]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboard session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }

        std::shared_ptr<RSTransaction> rsTransaction = nullptr;
        if (!isRotating && session->isKeyboardSyncTransactionOpen_) {
            rsTransaction = session->GetRSTransaction();
        }
        if (isKeyboardShow) {
            session->RaiseCallingSession(keyboardPanelRect, rsTransaction);
            if (session->specificCallback_ != nullptr && session->specificCallback_->onUpdateAvoidArea_ != nullptr) {
                if (Session::IsScbCoreEnabled()) {
                    session->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
                } else {
                    session->specificCallback_->onUpdateAvoidArea_(session->GetPersistentId());
                }
            }
        } else {
            session->RestoreCallingSession(rsTransaction);
            auto sessionProperty = session->GetSessionProperty();
            if (sessionProperty) {
                sessionProperty->SetCallingSessionId(INVALID_WINDOW_ID);
            }
        }

        if (!session->isKeyboardSyncTransactionOpen_) {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Keyboard sync transaction is closed");
            return WSError::WS_OK;
        }
        session->isKeyboardSyncTransactionOpen_ = false;
        auto transactionController = RSSyncTransactionController::GetInstance();
        if (transactionController) {
            transactionController->CloseSyncTransaction();
        }
        return WSError::WS_OK;
    };
    PostTask(task, "CloseKeyboardSyncTransaction");
}

std::shared_ptr<RSTransaction> KeyboardSession::GetRSTransaction()
{
    auto transactionController = RSSyncTransactionController::GetInstance();
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    if (transactionController) {
        rsTransaction = transactionController->GetRSTransaction();
    }
    return rsTransaction;
}

std::string KeyboardSession::GetSessionScreenName()
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        auto displayId = sessionProperty->GetDisplayId();
        auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(displayId);
        if (screenSession != nullptr) {
            return screenSession->GetName();
        }
    }
    return "";
}
} // namespace OHOS::Rosen

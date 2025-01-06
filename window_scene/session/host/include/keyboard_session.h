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

#ifndef OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_SESSION_H

#include "session/host/include/system_session.h"
#include "transaction/rs_sync_transaction_controller.h"

namespace OHOS::Rosen {
enum class SystemKeyboardAvoidChangeReason : uint32_t {
    KEYBOARD_BEGIN,
    KEYBOARD_CREATED,
    KEYBOARD_SHOW,
    KEYBOARD_HIDE,
    KEYBOARD_DISCONNECT,
    KEYBOARD_GRAVITY_BOTTOM,
    KEYBOARD_GRAVITY_FLOAT,
    KEYBOARD_END,
};

using OnGetSceneSessionCallback = std::function<sptr<SceneSession>(uint32_t callingSessionId)>;
using OnGetFocusedSessionIdCallback = std::function<int32_t()>;
using OnCallingSessionIdChangeCallback = std::function<void(uint32_t callingSessionId)>;
using OnSystemKeyboardAvoidChangeCallback = std::function<void(DisplayId displayId,
    SystemKeyboardAvoidChangeReason reason)>;

class KeyboardSession : public SystemSession {
public:
    // callback for notify SceneSessionManager
    struct KeyboardSessionCallback : public RefBase {
        OnGetSceneSessionCallback onGetSceneSession;
        OnGetFocusedSessionIdCallback onGetFocusedSessionId;
        OnCallingSessionIdChangeCallback onCallingSessionIdChange;
        OnSystemKeyboardAvoidChangeCallback onSystemKeyboardAvoidChange;
    };
    KeyboardSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback,
        const sptr<KeyboardSessionCallback>& keyboardCallback);
    ~KeyboardSession();

    WSError Show(sptr<WindowSessionProperty> property) override;
    WSError Hide() override;
    WSError Disconnect(bool isFromClient = false, const std::string& identityToken = "") override;
    WSError NotifyClientToUpdateRect(const std::string& updateReason,
        std::shared_ptr<RSTransaction> rsTransaction) override;

    void BindKeyboardPanelSession(sptr<SceneSession> panelSession) override;
    sptr<SceneSession> GetKeyboardPanelSession() const override;
    SessionGravity GetKeyboardGravity() const override;
    void OnKeyboardPanelUpdated() override;
    void OnCallingSessionUpdated() override;
    void OpenKeyboardSyncTransaction() override;
    void CloseKeyboardSyncTransaction(const WSRect& keyboardPanelRect, bool isKeyboardShow, bool isRotating) override;
    bool IsVisibleForeground() const override;
    uint32_t GetCallingSessionId() override;
    void RecalculatePanelRectForAvoidArea(WSRect& panelRect) override;

protected:
    void EnableCallingSessionAvoidArea() override;
    void RestoreCallingSession(const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;

private:
    sptr<SceneSession> GetSceneSession(uint32_t persistentId);
    int32_t GetFocusedSessionId();

    void SetCallingSessionId(uint32_t callingSessionId) override;

    void NotifyOccupiedAreaChangeInfo(const sptr<SceneSession>& callingSession, const WSRect& rect,
        const WSRect& occupiedArea, const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void RaiseCallingSession(const WSRect& keyboardPanelRect, bool needCheckVisible,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void UpdateKeyboardAvoidArea();
    void UseFocusIdIfCallingSessionIdInvalid();
    void UpdateCallingSessionIdAndPosition(uint32_t newCallingSessionId);
    void NotifyKeyboardPanelInfoChange(WSRect rect, bool isKeyboardPanelShow);
    bool CheckIfNeedRaiseCallingSession(sptr<SceneSession> callingSession, bool isCallingSessionFloating);
    WSError AdjustKeyboardLayout(const KeyboardLayoutParams& params) override;
    std::shared_ptr<RSTransaction> GetRSTransaction();
    std::string GetSessionScreenName();
    void MoveAndResizeKeyboard(const KeyboardLayoutParams& params, const sptr<WindowSessionProperty>& sessionProperty,
        bool isShow);
    void NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason reason);

    sptr<KeyboardSessionCallback> keyboardCallback_ = nullptr;
    bool isKeyboardSyncTransactionOpen_ = false;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_SESSION_H
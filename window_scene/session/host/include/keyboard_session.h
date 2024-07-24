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
#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>
#include "transaction/rs_sync_transaction_controller.h"

namespace OHOS::Rosen {
using OnGetSceneSessionCallback = std::function<sptr<SceneSession>(uint32_t callingSessionId)>;
using OnGetFocusedSessionIdCallback = std::function<int32_t()>;
using OnCallingSessionIdChangeCallback = std::function<void(uint32_t callingSessionId)>;

class KeyboardSession : public SystemSession {
public:
    // callback for notify SceneSessionManager
    struct KeyboardSessionCallback : public RefBase {
        OnGetSceneSessionCallback onGetSceneSession_;
        OnGetFocusedSessionIdCallback onGetFocusedSessionId_;
        OnCallingSessionIdChangeCallback onCallingSessionIdChange_;
    };
    KeyboardSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback,
        const sptr<KeyboardSessionCallback>& keyboardCallback);
    ~KeyboardSession();

    WSError Show(sptr<WindowSessionProperty> property) override;
    WSError Hide() override;
    WSError Disconnect(bool isFromClient = false) override;
    WSError NotifyClientToUpdateRect(std::shared_ptr<RSTransaction> rsTransaction) override;

    void BindKeyboardPanelSession(sptr<SceneSession> panelSession) override;
    sptr<SceneSession> GetKeyboardPanelSession() const override;
    SessionGravity GetKeyboardGravity() const override;
    void OnKeyboardPanelUpdated() override;
    void OpenKeyboardSyncTransaction() override;
    void CloseKeyboardSyncTransaction(const WSRect& keyboardPanelRect, bool isKeyboardShow, bool isRotating) override;

private:
    sptr<SceneSession> GetSceneSession(uint32_t persistentId);
    int32_t GetFocusedSessionId();

    WSError SetKeyboardSessionGravity(SessionGravity gravity, uint32_t percent) override;
    void SetCallingSessionId(uint32_t callingSessionId) override;
    uint32_t GetCallingSessionId();

    void NotifyOccupiedAreaChangeInfo(const sptr<SceneSession>& callingSession, const WSRect& rect,
        const WSRect& occupiedArea, const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void RaiseCallingSession(const WSRect& keyboardPanelRect,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void RestoreCallingSession(const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void UseFocusIdIfCallingSessionIdInvalid();
    void UpdateCallingSessionIdAndPosition(uint32_t callingSessionId);
    void RelayoutKeyBoard();
    void NotifyKeyboardPanelInfoChange(WSRect rect, bool isKeyboardPanelShow);
    bool CheckIfNeedRaiseCallingSession(sptr<SceneSession> callingSession, bool isCallingSessionFloating);
    WSError AdjustKeyboardLayout(const KeyboardLayoutParams& params) override;
    std::shared_ptr<RSTransaction> GetRSTransaction();

    sptr<KeyboardSessionCallback> keyboardCallback_ = nullptr;
    bool isKeyboardSyncTransactionOpen_ = false;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_SESSION_H
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_H
#include <future>

#include "key_event.h"
#include "want.h"

#include "session/host/include/session.h"

namespace OHOS::Rosen {
class WindowEventChannelListener : public IRemoteStub<IWindowEventChannelListener> {
public:
    explicit WindowEventChannelListener() = default;
    void SetTransferKeyEventForConsumedParams(const std::shared_ptr<std::promise<bool>>& isConsumedPromise,
        const std::shared_ptr<WSError>& retCode);
    void ResetTransferKeyEventForConsumedParams();
    void OnTransferKeyEventForConsumed(bool isConsumed, WSError retCode) override;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    std::mutex transferKeyEventForConsumedMutex_;
    std::shared_ptr<std::promise<bool>> isConsumedPromise_ = nullptr;
    std::shared_ptr<WSError> retCode_ = nullptr;
};

class ChannelDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit ChannelDeathRecipient(const sptr<WindowEventChannelListener>& listener): listener_(listener) {}
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
private:
    sptr<WindowEventChannelListener> listener_ = nullptr;
};

using NotifyTransferAbilityResultFunc = std::function<void(uint32_t resultCode, const AAFwk::Want& want)>;
using NotifyTransferExtensionDataFunc = std::function<void(const AAFwk::WantParams& wantParams)>;
using NotifyRemoteReadyFunc = std::function<void()>;
using NotifySyncOnFunc = std::function<void()>;
using NotifyAsyncOnFunc = std::function<void()>;
using NotifyGetAvoidAreaByTypeFunc = std::function<AvoidArea(AvoidAreaType type)>;
using NotifyBindModalFunc = std::function<void()>;
class ExtensionSession : public Session {
public:
    struct ExtensionSessionEventCallback : public RefBase {
        NotifyTransferAbilityResultFunc transferAbilityResultFunc_;
        NotifyTransferExtensionDataFunc transferExtensionDataFunc_;
        NotifyRemoteReadyFunc notifyRemoteReadyFunc_;
        NotifySyncOnFunc notifySyncOnFunc_;
        NotifyAsyncOnFunc notifyAsyncOnFunc_;
        NotifyGetAvoidAreaByTypeFunc notifyGetAvoidAreaByTypeFunc_;
        NotifyBindModalFunc notifyBindModalFunc_;
    };

    explicit ExtensionSession(const SessionInfo& info);
    virtual ~ExtensionSession();

    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property, sptr<IRemoteObject> token, int32_t pid, int32_t uid,
        const std::string& identityToken = "") override;

    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;

    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) override;
    WSError TransferExtensionData(const AAFwk::WantParams& wantParams) override;
    WSError TransferComponentData(const AAFwk::WantParams& wantParams);
    WSErrorCode TransferComponentDataSync(const AAFwk::WantParams& wantParams,
                                          AAFwk::WantParams& reWantParams);
    WSError TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) override;
    void NotifyRemoteReady() override;
    void NotifySyncOn() override;
    void NotifyAsyncOn() override;
    WSError NotifyDensityFollowHost(bool isFollowHost, float densityValue = 1.0f);
    void TriggerBindModalUIExtension() override;
    void RegisterExtensionSessionEventCallback(const sptr<ExtensionSessionEventCallback>& extSessionEventCallback);
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
        bool& isTimeOut, bool isPreImeEvent = false);
    WSError TransferKeyEventAsync(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isPreImeEvent = false);
    sptr<ExtensionSessionEventCallback> GetExtensionSessionEventCallback();
    WSError Background(bool isFromClient = false) override;

private:
    sptr<ExtensionSessionEventCallback> extSessionEventCallback_ = nullptr;
    bool isFirstTriggerBindModal_ = true;
    sptr<ChannelDeathRecipient> channelDeath_ = nullptr;
    sptr<WindowEventChannelListener> channelListener_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_H

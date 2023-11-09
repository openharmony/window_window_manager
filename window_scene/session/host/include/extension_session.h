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

#include "want.h"

#include "session/host/include/session.h"

namespace OHOS::Rosen {
using NotifyTransferAbilityResultFunc = std::function<void(uint32_t resultCode, const AAFwk::Want& want)>;
using NotifyTransferExtensionDataFunc = std::function<void(const AAFwk::WantParams& wantParams)>;
using NotifyRemoteReadyFunc = std::function<void()>;
using NotifySyncOnFunc = std::function<void()>;
using NotifyAsyncOnFunc = std::function<void()>;
class ExtensionSession : public Session {
public:
    struct ExtensionSessionEventCallback : public RefBase {
        NotifyTransferAbilityResultFunc transferAbilityResultFunc_;
        NotifyTransferExtensionDataFunc transferExtensionDataFunc_;
        NotifyRemoteReadyFunc notifyRemoteReadyFunc_;
        NotifySyncOnFunc notifySyncOnFunc_;
        NotifyAsyncOnFunc notifyAsyncOnFunc_;
    };

    explicit ExtensionSession(const SessionInfo& info);
    virtual ~ExtensionSession() = default;

    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property, sptr<IRemoteObject> token, int32_t pid, int32_t uid) override;

    WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) override;
    WSError TransferExtensionData(const AAFwk::WantParams& wantParams) override;
    WSError TransferComponentData(const AAFwk::WantParams& wantParams);
    WSErrorCode TransferComponentDataSync(const AAFwk::WantParams& wantParams,
                                          AAFwk::WantParams& reWantParams);
    void NotifyRemoteReady() override;
    void NotifySyncOn() override;
    void NotifyAsyncOn() override;
    void RegisterExtensionSessionEventCallback(const sptr<ExtensionSessionEventCallback>& extSessionEventCallback);
    sptr<ExtensionSessionEventCallback> GetExtensionSessionEventCallback();

private:
    sptr<ExtensionSessionEventCallback> extSessionEventCallback_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_H

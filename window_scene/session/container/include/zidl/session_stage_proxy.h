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

#ifndef OHOS_WINDOW_SCENE_SESSION_STAGE_RPOXY_H
#define OHOS_WINDOW_SCENE_SESSION_STAGE_RPOXY_H

#include <iremote_proxy.h>
#include <transaction/rs_transaction.h>

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"

namespace OHOS::Rosen {
class SessionStageProxy : public IRemoteProxy<ISessionStage> {
public:
    explicit SessionStageProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISessionStage>(impl) {};

    ~SessionStageProxy() {};

    WSError SetActive(bool active) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    void UpdateDensity() override;
    WSError HandleBackEvent() override;
    WSError MarkProcessed(int32_t eventId) override;
    WSError UpdateFocus(bool isFocused) override;
    WSError NotifyDestroy() override;
    WSError NotifyCloseExistPipWindow() override;
    WSError NotifyTransferComponentData(const AAFwk::WantParams& wantParams) override;
    WSErrorCode NotifyTransferComponentDataSync(const AAFwk::WantParams& wantParams,
                                                AAFwk::WantParams& reWantParams) override;
    void NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info) override;
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    void NotifyScreenshot() override;
    void DumpSessionElementInfo(const std::vector<std::string>& params)  override;
    WSError NotifyTouchOutside() override;
    WSError NotifyWindowVisibility(bool isVisible) override;
    WSError UpdateWindowMode(WindowMode mode) override;
    void NotifyForegroundInteractiveStatus(bool interactive) override;
    WSError UpdateMaximizeMode(MaximizeMode mode) override;
    void NotifySessionForeground(uint32_t reason, bool withAnimation) override;
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WSError UpdateTitleInTargetPos(bool isShow, int32_t height) override;
    void OnTransformChange(const Transform& transform) override;

private:
    static inline BrokerDelegator<SessionStageProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_SESSION_STAGE_RPOXY_H

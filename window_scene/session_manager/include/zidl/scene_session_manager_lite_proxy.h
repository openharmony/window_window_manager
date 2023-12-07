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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_LITE_PROXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_LITE_PROXY_H

#include <iremote_proxy.h>

#include "session_manager/include/zidl/scene_session_manager_lite_interface.h"

namespace OHOS::Rosen {
class SceneSessionManagerLiteProxy : public IRemoteProxy<ISceneSessionManagerLite> {
public:
    explicit SceneSessionManagerLiteProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<ISceneSessionManagerLite>(impl) {}
    virtual ~SceneSessionManagerLiteProxy() = default;

    WSError SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label) override;
    WSError SetSessionIcon(const sptr<IRemoteObject> &token, const std::shared_ptr<Media::PixelMap> &icon) override;
    WSError IsValidSessionIds(const std::vector<int32_t> &sessionIds, std::vector<bool> &results) override;
private:
    template<typename T>
    WSError GetParcelableInfos(MessageParcel& reply, std::vector<T>& parcelableInfos);
    static inline BrokerDelegator<SceneSessionManagerLiteProxy> delegator_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_LITE_PROXY_H

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

#ifndef OHOS_WINDOW_SCENE_SESSION_STAGE_INTERFACE_H
#define OHOS_WINDOW_SCENE_SESSION_STAGE_INTERFACE_H

#include <iremote_broker.h>

#include "utils/include/window_scene_common.h"

namespace OHOS::Rosen {
class ISessionStage : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISessionStage");

    enum class SessionStageMessage : uint32_t {
        TRANS_ID_SET_ACTIVE,
        TRANS_ID_NOTIFY_SIZE_CHANGE,
    };

    virtual WSError SetActive(bool active) = 0;
    virtual WSError UpdateSessionRect(const WSRect& rect, SessionSizeChangeReason reason) = 0;
};
}
#endif // OHOS_WINDOW_SCENE_SESSION_STAGE_INTERFACE_H
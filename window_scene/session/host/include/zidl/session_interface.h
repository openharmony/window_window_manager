/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_SESSION_INTERFACE_H
#define OHOS_SESSION_INTERFACE_H

#include <iremote_broker.h>
#include "window_scene_common.h"

namespace OHOS::Rosen {
class ISession : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISession");

    enum class SessionMessage : uint32_t {
        TRANS_ID_FOREGROUND = 0,
        TRANS_ID_BACKGROUND,
        TRANS_ID_DISCONNECT,
    };
    virtual WSError Foreground() = 0;
    virtual WSError Background() = 0;
    virtual WSError Disconnect() = 0;
};
}
#endif // OHOS_SESSION_INTERFACE_H
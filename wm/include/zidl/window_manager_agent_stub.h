/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_MANAGER_AGENT_STUB_H
#define OHOS_WINDOW_MANAGER_AGENT_STUB_H

#include "window_manager_agent_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Rosen {
class WindowManagerAgentStub : public IRemoteStub<IWindowManagerAgent> {
public:
    WindowManagerAgentStub() = default;
    ~WindowManagerAgentStub() = default;

    virtual int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& option) override;

private:
    bool ReadWindowInfoList(MessageParcel& data,
        std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>>& windowInfoList);
    bool ReadWindowInfo(MessageParcel& data,
        std::unordered_map<WindowInfoKey, WindowChangeInfoType>& windowInfo);
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_MANAGER_AGENT_STUB_H

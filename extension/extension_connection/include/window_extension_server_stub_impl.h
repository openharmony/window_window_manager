/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef WINDOW_EXTENSION_SERVER_STUB_IMPL_H
#define WINDOW_EXTENSION_SERVER_STUB_IMPL_H

#include "window_extension_connection.h"
#include "window_extension_server_stub.h"

namespace OHOS {
namespace Rosen {
class WindowExtensionServerStubImpl : public WindowExtensionServerStub {
public:
    explicit WindowExtensionServerStubImpl(sptr<IWindowExtensionCallback>& componentCallback)
    : componentCallback_(componentCallback) {};
    ~WindowExtensionServerStubImpl() = default;

    void OnWindowReady(std::shared_ptr<RSSurfaceNode>& surfaceNode) override;
    void OnBackPress() override;
    void OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    void OnPointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
private:
    sptr<IWindowExtensionCallback> componentCallback_;
};
} // namespcce Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_SERVER_STUB_IMPL_H
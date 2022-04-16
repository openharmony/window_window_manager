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

#ifndef WINDOW_EXTENSION_CLIENT_STUB_IMPL_H
#define WINDOW_EXTENSION_CLIENT_STUB_IMPL_H

#include "wm_common.h"

#include <string>

#include "window.h"
#include "window_extension_client_stub.h"
#include "window_extension_server_interface.h"

namespace OHOS {
namespace Rosen {
class WindowExtensionClientStubImpl : public WindowExtensionClientStub {
public:
    WindowExtensionClientStubImpl(const std::string& windowName);
    ~WindowExtensionClientStubImpl();

    virtual void Resize(Rect rect) override;
    virtual void Hide() override;
    virtual void Show() override;
    virtual void RequestFocus() override;
    virtual void ConnectToClient(sptr<IWindowExtensionServer>& token) override;

    std::shared_ptr<RSSurfaceNode> CreateWindow(Rect& rect);
private:

    sptr<IDispatchInputEventListener> dispatchInputEventListener_;
    sptr<Window> window_;
    std::string windowName_;
    sptr<IWindowExtensionServer> token_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_CLIENT_STUB_IMPL_H
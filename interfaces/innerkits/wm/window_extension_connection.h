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

#ifndef WINDOW_EXTENSION_CONNECTION_H
#define WINDOW_EXTENSION_CONNECTION_H

#include <string>

#include <i_input_event_consumer.h>
#include <key_event.h>
#include <memory>

#include "element_name.h"
#include "input_manager.h"
#include "pointer_event.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    const static std::string RECT_FORM_KEY_POS_X = "ext_pos_x";
    const static std::string RECT_FORM_KEY_POS_Y = "ext_pos_y";
    const static std::string RECT_FORM_KEY_HEIGHT = "ext_pos_heigh";
    const static std::string RECT_FORM_KEY_WIDTH = "ext_pos_width";
}

class RSSurfaceNode;
class IWindowExtensionCallback : virtual public RefBase {
public:
    virtual void OnWindowReady(std::shared_ptr<RSSurfaceNode>& rsSurfaceNode) = 0;
    virtual void OnExtensionDisconnected() = 0;
    virtual void OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& event) = 0;
    virtual void OnPointerEvent(std::shared_ptr<MMI::PointerEvent>& event) = 0;
    virtual void OnBackPress() = 0;
};

class WindowExtensionConnection : public RefBase {
public:
    //static sptr<WindowExtensionConnection> Init();
    virtual void ConnectExtension(const AppExecFwk::ElementName &element, Rect rect,
        uint32_t uid, sptr<IWindowExtensionCallback>& callback) = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void RequestFocus() = 0;

private:
    sptr<WindowExtensionConnection> impl_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_CONNECTION_H
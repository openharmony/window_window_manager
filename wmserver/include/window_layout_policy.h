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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H

#include <map>
#include <refbase.h>
#include <set>

#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowLayoutPolicy : public RefBase {
public:
    WindowLayoutPolicy() = default;
    ~WindowLayoutPolicy() = default;
    WMError UpdateDisplayInfo(const Rect& displayRect);
    WMError LayoutWindow(sptr<WindowNode>& node);

private:
    Rect displayRect_ = {0, 0, 0, 0};
    Rect limitRect_ = {0, 0, 0, 0};
    std::map<uint32_t, sptr<WindowNode>> aviodNodes_;
    const std::set<WindowType> aviodTypes_ {
        WindowType::WINDOW_TYPE_STATUS_BAR,
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
    };
    void UpdateLimitRect(const sptr<WindowNode>& node);
    void RecordAvoidRect(const sptr<WindowNode>& node);
    bool UpdateLayoutRects(sptr<WindowNode>& node);
    bool IsNeedAvoidNode(const sptr<WindowNode>& node);
    bool IsFullScreenNode(const sptr<WindowNode>& node);
    bool IsParentLimitNode(const sptr<WindowNode>& node);
    bool IsRectChanged(const Rect& l, const Rect& r);
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H

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

#ifndef OHOS_ROSEN_REMOTE_ANIMATION_H
#define OHOS_ROSEN_REMOTE_ANIMATION_H

#include <refbase.h>
#include <rs_iwindow_animation_controller.h>
#include "surface_draw.h"
#include "wm_common.h"
#include "window_node.h"
#include "window_transition_info.h"

namespace OHOS {
namespace Rosen {
enum class TransitionEvent : uint32_t {
    COLD_START,
    HOT_START,
    BACK,
    HOME,
    MINIMIZE,
    CLOSE,
    UNKNOWN,
};

class RemoteAnimation : public RefBase {
public:
    RemoteAnimation() = delete;
    ~RemoteAnimation() = default;

    static bool CheckTransition(sptr<WindowTransitionInfo> info);
    static TransitionEvent GetTransitionEvent(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode);
    static sptr<WindowNode> CreateWindowNode(sptr<WindowTransitionInfo> info, uint32_t winId);
    static void HandleClientWindowCreate(sptr<WindowNode>& node, sptr<IWindow>& window,
        uint32_t& windowId, const std::shared_ptr<RSSurfaceNode>& surfaceNode);
    static void NotifyAnimationTransition(sptr<WindowTransitionInfo> srcInfo, sptr<WindowTransitionInfo> dstInfo,
        const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode, bool needMinimizeSrcNode);
    static void DrawStartingWindow(sptr<WindowNode>& node);
    static void UpdateRSTree(sptr<WindowNode>& node);
    static WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller);

private:
    static SurfaceDraw surfaceDraw_;
    static sptr<RSIWindowAnimationController> windowAnimationController_;
    static WMError CreateLeashAndStartingSurfaceNode(sptr<WindowNode>& node);
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_REMOTE_ANIMATION_H

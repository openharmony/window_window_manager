/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_STARTING_WINDOW_H
#define OHOS_ROSEN_STARTING_WINDOW_H

#include <bundlemgr/launcher_service.h>
#include <refbase.h>
#include <resource_manager.h>

#include "ability_info.h"
#include "animation_config.h"
#include "bundle_info.h"
#include "image_source.h"
#include "pixel_map.h"
#include "surface_draw.h"
#include "window_node.h"
#include "window_root.h"
#include "window_transition_info.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
struct ResInfoShowState {
    uint32_t frameIdx = 0;
    uint32_t frameCount = 0;
    std::vector<int32_t> delay;
    std::chrono::steady_clock::time_point next;
};
struct StartingWindowShowInfo {
    wptr<WindowNode> node;
    Rect rect;
    std::shared_ptr<Rosen::StartingWindowPageDrawInfo> info;
    float vpRatio;
    std::array<ResInfoShowState, size_t(StartWindowResType::Count)> resStates {};
    std::array<uint32_t, size_t(StartWindowResType::Count)> frameIndex {};
};
class StartingWindow : public RefBase {
public:
    StartingWindow() = delete;
    ~StartingWindow() = default;

    static sptr<WindowNode> CreateWindowNode(const sptr<WindowTransitionInfo>& info, uint32_t winId);
    static void HandleClientWindowCreate(sptr<WindowNode>& node, sptr<IWindow>& window,
        uint32_t& windowId, const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowProperty>& property,
        int32_t pid, int32_t uid);
    static WMError DrawStartingWindow(sptr<WindowNode>& node, std::shared_ptr<Media::PixelMap> pixelMap,
        uint32_t bkgColor, bool isColdStart);
    static void AddNodeOnRSTree(sptr<WindowNode>& node, bool isMultiDisplay);
    static void ReleaseStartWinSurfaceNode(sptr<WindowNode>& node);
    static void SetDefaultWindowMode(WindowMode defaultMode);
    static void SetAnimationConfig(AnimationConfig config);
    static bool transAnimateEnable_;
    static WindowUIType windowUIType_;
    
private:
    static WMError CreateLeashAndStartingSurfaceNode(sptr<WindowNode>& node);
    static WMError SetStartingWindowAnimation(wptr<WindowNode> weak);
    static void ChangePropertyByApiVersion(const sptr<WindowTransitionInfo>& info,
        const Orientation orientation, sptr<WindowProperty>& property);
    static std::recursive_mutex mutex_;
    static WindowMode defaultMode_;
    static AnimationConfig animationConfig_;
    static bool IsWindowFollowParent(WindowType type);
    static sptr<WindowProperty> InitializeWindowProperty(const sptr<WindowTransitionInfo>& info, uint32_t winId);
    static void UpdateRSTree(sptr<WindowNode>& node, bool isMultiDisplay);
    static sptr<AppExecFwk::IBundleMgr> GetBundleManager();
    static std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfoFromBMS(const sptr<WindowNode>& node,
        const sptr<AppExecFwk::IBundleMgr>& bundleMgr);
    static std::shared_ptr<Global::Resource::ResourceManager> CreateResourceManager(
        const std::shared_ptr<AppExecFwk::AbilityInfo>& abilityInfo);
    static std::shared_ptr<Rosen::ResourceInfo> GetPixelMapListInfo(uint32_t mediaDataId,
        const std::shared_ptr<Global::Resource::ResourceManager>& resourceMgr,
        const std::shared_ptr<AppExecFwk::AbilityInfo>& abilityInfo);
    static std::shared_ptr<Rosen::StartingWindowPageDrawInfo> GetCustomStartingWindowInfo(
        const sptr<WindowNode>& node, const sptr<AppExecFwk::IBundleMgr>& bundleMgr);
    static std::shared_ptr<Rosen::StartingWindowPageDrawInfo> DoGetCustomStartingWindowInfo(
        const std::shared_ptr<AppExecFwk::AbilityInfo>& abilityInfo,
        const std::shared_ptr<Global::Resource::ResourceManager>& resourceMgr);
    static WMError DrawStartingWindow(const std::shared_ptr<Rosen::StartingWindowPageDrawInfo>& info,
        const sptr<WindowNode>& node, const Rect& rect);
    static void RegisterStartingWindowShowInfo(const sptr<WindowNode>& node, const Rect& rect,
        const std::shared_ptr<Rosen::StartingWindowPageDrawInfo>& info, float vpRatio);
    static void UnRegisterStartingWindowShowInfo();
    static void UpdateWindowShowInfo(StartingWindowShowInfo& startingWindowShowInfo, bool& needRedraw);
    static void DrawStartingWindowShowInfo();
    static StartingWindowShowInfo startingWindowShowInfo_;
    static std::atomic<bool> startingWindowShowRunning_;
    static std::thread startingWindowShowThread_;
    static std::mutex firstFrameMutex_;
    static std::condition_variable firstFrameCondition_;
    static std::atomic<bool> firstFrameCompleted_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_STARTING_WINDOW_H
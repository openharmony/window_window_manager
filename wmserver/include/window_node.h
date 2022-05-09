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

#ifndef OHOS_ROSEN_WINDOW_NODE_H
#define OHOS_ROSEN_WINDOW_NODE_H

#include <ipc_skeleton.h>
#include <refbase.h>
#include <running_lock.h>
#include <ui/rs_surface_node.h>
#include "zidl/window_interface.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class WindowNode : public RefBase {
public:
    WindowNode(const sptr<WindowProperty>& property, const sptr<IWindow>& window,
        std::shared_ptr<RSSurfaceNode> surfaceNode)
        : surfaceNode_(surfaceNode), property_(property), windowToken_(window)
    {
        callingPid_ = IPCSkeleton::GetCallingPid();
        callingUid_ = IPCSkeleton::GetCallingUid();
    }
    WindowNode() : property_(new WindowProperty())
    {
        callingPid_ = IPCSkeleton::GetCallingPid();
        callingUid_ = IPCSkeleton::GetCallingUid();
    }
    explicit WindowNode(const sptr<WindowProperty>& property) : property_(property)
    {
        callingPid_ = IPCSkeleton::GetCallingPid();
        callingUid_ = IPCSkeleton::GetCallingUid();
    }
    ~WindowNode() = default;

    void SetDisplayId(DisplayId displayId);
    void SetHotZoneRect(const Rect& rect);
    void SetWindowRect(const Rect& rect);
    void SetDecoStatus(bool decoStatus);
    void SetRequestRect(const Rect& rect);
    void SetWindowProperty(const sptr<WindowProperty>& property);
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& property);
    void SetWindowMode(WindowMode mode);
    void SetWindowBackgroundBlur(WindowBlurLevel level);
    void SetAlpha(float alpha);
    void SetBrightness(float brightness);
    void SetFocusable(bool focusable);
    void SetTouchable(bool touchable);
    void SetTurnScreenOn(bool turnScreenOn);
    void SetKeepScreenOn(bool keepScreenOn);
    void SetCallingWindow(uint32_t windowId);
    void SetCallingPid();
    void SetCallingUid();
    void SetWindowToken(sptr<IWindow> window);
    uint32_t GetCallingWindow() const;
    void SetWindowSizeChangeReason(WindowSizeChangeReason reason);
    void SetRequestedOrientation(Orientation orientation);
    const sptr<IWindow>& GetWindowToken() const;
    uint32_t GetWindowId() const;
    uint32_t GetParentId() const;
    const std::string& GetWindowName() const;
    DisplayId GetDisplayId() const;
    Rect GetHotZoneRect() const;
    Rect GetWindowRect() const;
    bool GetDecoStatus() const;
    Rect GetRequestRect() const;
    WindowType GetWindowType() const;
    WindowMode GetWindowMode() const;
    WindowBlurLevel GetWindowBackgroundBlur() const;
    float GetAlpha() const;
    float GetBrightness() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    uint32_t GetWindowFlags() const;
    const sptr<WindowProperty>& GetWindowProperty() const;
    int32_t GetCallingPid() const;
    int32_t GetCallingUid() const;
    const std::unordered_map<WindowType, SystemBarProperty>& GetSystemBarProperty() const;
    bool IsSplitMode() const;
    WindowSizeChangeReason GetWindowSizeChangeReason() const;
    Orientation GetRequestedOrientation() const;
    void ResetWindowSizeChangeReason();

    sptr<WindowNode> parent_;
    std::vector<sptr<WindowNode>> children_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSSurfaceNode> leashWinSurfaceNode_ = nullptr;
    std::shared_ptr<RSSurfaceNode> startingWinSurfaceNode_ = nullptr;
    sptr<IRemoteObject> abilityToken_ = nullptr;
    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;
    int32_t priority_ { 0 };
    bool requestedVisibility_ { false };
    bool currentVisibility_ { false };
    bool isCovered_ { true }; // initial value true to ensure notification when this window is shown
    bool isPlayAnimationShow_ { false };
    bool isPlayAnimationHide_ { false };
    bool startingWindowShown_ { false };
private:
    sptr<WindowProperty> property_ = nullptr;
    sptr<IWindow> windowToken_ = nullptr;
    Rect hotZoneRect_ { 0, 0, 0, 0 };
    int32_t callingPid_;
    int32_t callingUid_;
    WindowSizeChangeReason windowSizeChangeReason_ {WindowSizeChangeReason::UNDEFINED};
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_WINDOW_NODE_H

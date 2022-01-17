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

#ifndef OHOS_ROSEN_WINDOW_IMPL_H
#define OHOS_ROSEN_WINDOW_IMPL_H
#include <map>
#include <ability_context.h>
#include <refbase.h>
#include <ui_content.h>
#include "input_transfer_station.h"
#include "vsync_station.h"
#include "window.h"
#include "window_property.h"

namespace OHOS {
namespace Rosen {
class WindowImpl : public Window {
#define CALL_LIFECYCLE_LISTENER(windowLifecycleCb, uiContentCb) \
    do { \
        if (lifecycleListener_ != nullptr) { \
            lifecycleListener_->windowLifecycleCb(); \
        } \
        if (uiContent_ != nullptr) { \
            uiContent_->uiContentCb(); \
        } \
    } while (0);

public:
    WindowImpl(const sptr<WindowOption>& option);
    ~WindowImpl();

    static sptr<Window> Find(const std::string& id);

    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const override;
    virtual Rect GetRect() const override;
    virtual WindowType GetType() const override;
    virtual WindowMode GetMode() const override;
    virtual const std::string& GetWindowName() const override;
    virtual uint32_t GetWindowId() override;
    virtual uint32_t GetWindowFlags() override;
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) override;
    virtual WMError SetWindowType(WindowType type) override;
    virtual WMError SetWindowMode(WindowMode mode) override;
    virtual WMError AddWindowFlag(WindowFlag flag) override;
    virtual WMError RemoveWindowFlag(WindowFlag flag) override;
    virtual WMError SetWindowFlags(uint32_t flags) override;
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) override;

    WMError Create(const std::string& parentName,
        const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    virtual WMError Destroy() override;
    virtual WMError Show() override;
    virtual WMError Hide() override;
    virtual WMError MoveTo(int32_t x, int32_t y) override;
    virtual WMError Resize(uint32_t width, uint32_t height) override;

    virtual WMError RequestFocus() const override;
    virtual void AddInputEventListener(std::shared_ptr<MMI::IInputEventConsumer>& inputEventListener) override;

    virtual void RegisterLifeCycleListener(sptr<IWindowLifeCycle>& listener) override;
    virtual void RegisterWindowChangeListener(sptr<IWindowChangeListener>& listener) override;
    virtual void RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    virtual void UnregisterAvoidAreaChangeListener() override;

    void UpdateRect(const struct Rect& rect);
    void UpdateMode(WindowMode mode);
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) override;
    virtual void ConsumePointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) override;
    void ConsumeDividerPointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent);
    virtual void RequestFrame() override;
    void UpdateFocusStatus(bool focused);
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;
    void UpdateAvoidArea(const std::vector<Rect>& avoidAreas);

    virtual WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isdistributed) override;
    virtual const std::string& GetContentInfo() override;

private:
    inline void NotifyAfterForeground() const
    {
        CALL_LIFECYCLE_LISTENER(AfterForeground, Foreground);
    }
    inline void NotifyAfterBackground() const
    {
        CALL_LIFECYCLE_LISTENER(AfterBackground, Background);
    }
    inline void NotifyAfterFocused() const
    {
        CALL_LIFECYCLE_LISTENER(AfterFocused, Focus);
    }
    inline void NotifyAfterUnFocused() const
    {
        CALL_LIFECYCLE_LISTENER(AfterUnfocused, UnFocus);
    }
    inline void NotifyBeforeDestroy() const
    {
        if (uiContent_ != nullptr) {
            uiContent_->Destroy();
        }
    }
    void SetDefaultOption(); // for api7
    bool IsWindowValid() const;
    void OnVsync(int64_t timeStamp);

    enum WindowState {
        STATE_INITIAL,
        STATE_CREATED,
        STATE_SHOWN,
        STATE_HIDDEN,
        STATE_DESTROYED,
        STATE_BOTTOM = STATE_DESTROYED,
    };

    std::shared_ptr<VsyncStation::VsyncCallback> callback_ =
        std::make_shared<VsyncStation::VsyncCallback>(VsyncStation::VsyncCallback());
    static std::map<std::string, std::pair<uint32_t, sptr<Window>>> windowMap_;
    static std::map<uint32_t, std::vector<sptr<Window>>> subWindowMap_;
    sptr<WindowProperty> property_;
    WindowState state_ { STATE_INITIAL };
    sptr<IWindowLifeCycle> lifecycleListener_;
    sptr<IWindowChangeListener> windowChangeListener_;
    sptr<IAvoidAreaChangedListener> avoidAreaChangeListener_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::string name_;
    std::unique_ptr<Ace::UIContent> uiContent_;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_; // give up when context offer getToken
    std::shared_ptr<AbilityRuntime::Context> context_;
    const float STATUS_BAR_RATIO = 0.07;
    const float NAVIGATION_BAR_RATIO = 0.07;
    const float SYSTEM_ALARM_WINDOW_WIDTH_RATIO = 0.8;
    const float SYSTEM_ALARM_WINDOW_HEIGHT_RATIO = 0.3;
};
}
}
#endif // OHOS_ROSEN_WINDOW_IMPL_H

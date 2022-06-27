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

#ifndef OHOS_ROSEN_INNER_WINDOW_H
#define OHOS_ROSEN_INNER_WINDOW_H

#include <unordered_map>
#include <memory>
#include "window.h"
#include "window_manager_hilog.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
enum class InnerWindowState : uint32_t {
    INNER_WINDOW_STATE_NLL,
    INNER_WINDOW_STATE_CRATED,
    INNER_WINDOW_STATE_DESTROYED
};
class IInnerWindow : virtual public RefBase {
public:
    virtual void Create() = 0;
    virtual void Destroy() = 0;
    InnerWindowState GetState() {
        return state_;
    };
protected:
    InnerWindowState state_ = InnerWindowState::INNER_WINDOW_STATE_NLL;
};

class PlaceHolderWindow : public IInnerWindow, public IWindowLifeCycle, public ITouchOutsideListener,
    public IInputEventListener {
public:
    PlaceHolderWindow(std::string name, DisplayId displyId, const Rect& rect, WindowMode mode) : name_(name),
        displayId_(displyId), rect_(rect), mode_(mode) {};
    ~PlaceHolderWindow();
    void Create();
    void Destroy();

private:
    virtual void OnTouchOutside();
    virtual void OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent);
    virtual void OnPointerInputEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    virtual void AfterForeground() {};
    virtual void AfterBackground() {};
    virtual void AfterFocused() {};
    virtual void AfterInactive() {};
    virtual void AfterUnfocused();
    void UnRegitsterWindowListener();
    void RegitsterWindowListener();

private:
    std::string name_;
    DisplayId displayId_;
    Rect rect_;
    WindowMode mode_;
    sptr<OHOS::Rosen::Window> window_;
    std::string contentImgPath_ = "/etc/window/resources/bg_place_holder.png";
};

class DividerWindow : public IInnerWindow {
public:
    DividerWindow(std::string name, DisplayId displyId, const Rect& rect) : name_(name),
        displayId_(displyId), rect_(rect) {};
    ~DividerWindow();
    void Create();
    void Destroy();

private:
    std::string name_;
    DisplayId displayId_;
    Rect rect_;
    int32_t dialogId_ = -1;

};

class InnerWindowFactory : public RefBase {
WM_DECLARE_SINGLE_INSTANCE(InnerWindowFactory);
public:
    WMError CreateInnerWindow(std::string name, DisplayId displyId, Rect rect, WindowType type, WindowMode mode);
    WMError DestroyInnerWindow(DisplayId displyId, WindowType type);
private:
    std::unordered_map<WindowType, std::unique_ptr<IInnerWindow>> innerWindowMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_INNER_WINDOW_H
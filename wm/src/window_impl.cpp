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

#include "window_impl.h"
#include "display_manager.h"
#include "singleton_container.h"
#include "window_adapter.h"
#include "window_agent.h"
#include "window_manager_hilog.h"
#ifndef _NEW_RENDERSERVER_
#include "adapter.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowImpl"};
}

std::map<std::string, std::pair<uint32_t, sptr<Window>>> WindowImpl::windowMap_;

WindowImpl::WindowImpl(const sptr<WindowOption>& option)
{
    property_ = new WindowProperty();
    property_->SetWindowRect(option->GetWindowRect());
    property_->SetWindowType(option->GetWindowType());
    property_->SetWindowMode(option->GetWindowMode());
    property_->SetFullScreen(option->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    property_->SetFocusable(option->GetFocusable());
    property_->SetTouchable(option->GetTouchable());
    property_->SetDisplayId(option->GetDisplayId());
    property_->SetWindowFlags(option->GetWindowFlags());
    name_ = option->GetWindowName();

#ifdef _NEW_RENDERSERVER_
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    surfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
#endif
}

WindowImpl::~WindowImpl()
{
    Destroy();
}

sptr<Window> WindowImpl::Find(const std::string& name)
{
    auto iter = windowMap_.find(name);
    if (iter == windowMap_.end()) {
        return nullptr;
    }
    return iter->second.second;
}

std::shared_ptr<RSSurfaceNode> WindowImpl::GetSurfaceNode() const
{
    return surfaceNode_;
}

Rect WindowImpl::GetRect() const
{
    return property_->GetWindowRect();
}

WindowType WindowImpl::GetType() const
{
    return property_->GetWindowType();
}

WindowMode WindowImpl::GetMode() const
{
    return property_->GetWindowMode();
}

const std::string& WindowImpl::GetWindowName() const
{
    return name_;
}

uint32_t WindowImpl::GetWindowId()
{
    return property_->GetWindowId();
}

WMError WindowImpl::SetWindowType(WindowType type)
{
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_CREATED) {
        property_->SetWindowType(type);
        return WMError::WM_OK;
    }
    if (property_->GetWindowType() != type) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowMode(WindowMode mode)
{
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_CREATED || state_ == STATE_HIDDEN) {
        property_->SetWindowMode(mode);
        return WMError::WM_OK;
    }
    if (property_->GetWindowMode() != mode) {
        // TODO
    }
    return WMError::WM_OK;
}

WMError WindowImpl::SetUIContent(std::shared_ptr<AbilityRuntime::AbilityContext> context,
    std::string& url, NativeEngine* engine, NativeValue* storage)
{
    WLOGFI("SetUIContent");
    uiContent_ = Ace::UIContent::Create(context.get(), engine);
    if (uiContent_ == nullptr) {
        WLOGFE("fail to SetUIContent id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->Initialize(this, url, storage);
    return WMError::WM_OK;
}

WMError WindowImpl::Create(const std::string& parentName, const sptr<IRemoteObject>& abilityToken)
{
#ifdef _NEW_RENDERSERVER_
    // check window name, same window names are forbidden
    if (windowMap_.find(name_) != windowMap_.end()) {
        WLOGFE("WindowName(%{public}s) already exists.", name_.c_str());
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    // check parent name, if create sub window and there is not exist parent Window, then return
    if (parentName != "") {
        if (windowMap_.find(parentName) == windowMap_.end()) {
            WLOGFE("ParentName is empty or valid. ParentName is %{public}s", parentName.c_str());
            return WMError::WM_ERROR_INVALID_PARAM;
        } else {
            uint32_t parentId = windowMap_[parentName].first;
            property_->SetParentId(parentId);
        }
    }

    sptr<WindowImpl> window(this);
    sptr<IWindow> windowAgent(new WindowAgent(window));
    uint32_t windowId = 0;
    WMError ret = SingletonContainer::Get<WindowAdapter>()->CreateWindow(windowAgent, property_, surfaceNode_,
        windowId);
    property_->SetWindowId(windowId);

    if (ret != WMError::WM_OK) {
        WLOGFE("create window failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return ret;
    }
    if (abilityToken != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>()->SaveAbilityToken(abilityToken, windowId);
        if (ret != WMError::WM_OK) {
            WLOGFE("SaveAbilityToken failed with errCode:%{public}d", static_cast<int32_t>(ret));
            return ret;
        }
    }
    windowMap_.insert({ name_, std::pair<uint32_t, sptr<Window>>(windowId, this) });
    state_ = STATE_CREATED;
    WLOGFI("create window success with winId:%{public}d", windowId);
    InputTransferStation::GetInstance()->AddInputWindow(this);
    return ret;
#else
    /* weston adapter */
    return WMError::WM_OK;
#endif
}

WMError WindowImpl::Destroy()
{
    NotifyBeforeDestroy();
#ifdef _NEW_RENDERSERVER_
    // should destroy surface here
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    WLOGFI("destroy window id: %{public}d", property_->GetWindowId());
    WMError ret = SingletonContainer::Get<WindowAdapter>()->DestroyWindow(property_->GetWindowId());
    windowMap_.erase(GetWindowName());
    state_ = STATE_DESTROYED;
    InputTransferStation::GetInstance()->RemoveInputWindow(this);
    return ret;
#else
    InputTransferStation::GetInstance()->RemoveInputWindow(this);
    Adapter::DestroyWestonWindow();
#endif
    return WMError::WM_OK;
}

WMError WindowImpl::Show()
{
#ifdef _NEW_RENDERSERVER_
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_SHOWN) {
        WLOGFI("window is already shown id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    SetDefaultOption();
    WMError ret = SingletonContainer::Get<WindowAdapter>()->AddWindow(property_);
    if (ret == WMError::WM_OK || ret == WMError::WM_ERROR_DEATH_RECIPIENT) {
        Rect rect = property_->GetWindowRect();
        WLOGFI("show x: %{public}d ; y: %{public}d; width: %{public}d; height: %{public}d, winId:%{public}d;",
            rect.posX_, rect.posY_, rect.width_, rect.height_, property_->GetWindowId());
        state_ = STATE_SHOWN;
        NotifyAfterForeground();
    }
    WLOGFE("show errCode:%{public}d for winId:%{public}d", static_cast<int32_t>(ret), property_->GetWindowId());
    return ret;
#else
    /* weston adapter */
    WMError rtn = Adapter::Show();
    if (rtn == WMError::WM_OK) {
        NotifyAfterForeground();
        NotifyAfterFocused();
        WLOGFI("Show AfterForeground was invoked");
    } else {
        WLOGFE("Show error=%d", static_cast<int>(rtn));
    }
    InputTransferStation::GetInstance()->AddInputWindow(this);
    return rtn;
#endif
}

WMError WindowImpl::Hide()
{
#ifdef _NEW_RENDERSERVER_
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_HIDDEN || state_ == STATE_CREATED) {
        WLOGFI("window is already hidden id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    WMError ret = SingletonContainer::Get<WindowAdapter>()->RemoveWindow(property_->GetWindowId());
    WLOGFI("hide errCode:%{public}d for winId:%{public}d", static_cast<int32_t>(ret), property_->GetWindowId());
    if (ret != WMError::WM_OK) {
        return ret;
    }
    state_ = STATE_HIDDEN;
    NotifyAfterBackground();
    return ret;
#else
    /* weston adapter */
    WMError rtn = Adapter::Hide();
    if (rtn == WMError::WM_OK) {
        NotifyAfterUnFocused();
        NotifyAfterBackground();
        WLOGFI("WindowImpl::Show AfterBackground was ivoked");
    } else {
        WLOGFE("WindowImpl::Show error=%d", static_cast<int>(rtn));
    }
    InputTransferStation::GetInstance()->RemoveInputWindow(this);
    return rtn;
#endif
}

WMError WindowImpl::MoveTo(int32_t x, int32_t y)
{
    /* weston adapter */
#ifdef _NEW_RENDERSERVER_
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_HIDDEN || state_ == STATE_CREATED) {
        Rect rect = GetRect();
        property_->SetWindowRect({ x, y, rect.width_, rect.height_ });
        WLOGFI("window is hidden or created! id: %{public}d, rect: [%{public}d, %{public}d, %{public}d, %{public}d]",
               property_->GetWindowId(), rect.posX_, rect.posY_, x, y);
        return WMError::WM_OK;
    }
    return SingletonContainer::Get<WindowAdapter>()->MoveTo(property_->GetWindowId(), x, y);
#else
    return Adapter::MoveTo(x, y);
#endif
}

WMError WindowImpl::Resize(uint32_t width, uint32_t height)
{
    /* weston adapter */
#ifdef _NEW_RENDERSERVER_
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_HIDDEN || state_ == STATE_CREATED) {
        Rect rect = GetRect();
        property_->SetWindowRect({ rect.posX_, rect.posY_, width, height });
        WLOGFI("window is hidden or created! id: %{public}d, rect: [%{public}d, %{public}d, %{public}d, %{public}d]",
               property_->GetWindowId(), rect.posX_, rect.posY_, width, height);
        return WMError::WM_OK;
    }
    return SingletonContainer::Get<WindowAdapter>()->Resize(property_->GetWindowId(), width, height);
#else
    return Adapter::Resize(width, height);
#endif
}

WMError WindowImpl::RequestFocus() const
{
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>()->RequestFocus(property_->GetWindowId());
}

void WindowImpl::AddInputEventListener(std::shared_ptr<MMI::IInputEventConsumer>& inputEventListener)
{
    InputTransferStation::GetInstance()->SetInputListener(GetWindowId(), inputEventListener);
}

void WindowImpl::RegisterLifeCycleListener(sptr<IWindowLifeCycle>& listener)
{
    lifecycleListener_ = listener;
}

void WindowImpl::RegisterWindowChangeListener(sptr<IWindowChangeListener>& listener)
{
    windowChangeListener_ = listener;
}

void WindowImpl::UpdateRect(const struct Rect& rect)
{
    property_->SetWindowRect(rect);
    if (windowChangeListener_ != nullptr) {
        windowChangeListener_->OnSizeChange(rect);
    }
}

void WindowImpl::UpdateMode(WindowMode mode)
{
    property_->SetWindowMode(mode);
}

void WindowImpl::ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    uiContent_->ProcessKeyEvent(keyEvent);
}
void WindowImpl::ConsumePointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    uiContent_->ProcessPointerEvent(pointerEvent);
}

void WindowImpl::UpdateFocusStatus(bool focused)
{
    WLOGFI("window focus status: %{public}d, id: %{public}d", focused, property_->GetWindowId());
    if (focused) {
        NotifyAfterFocused();
    } else {
        NotifyAfterUnFocused();
    }
}

void WindowImpl::SetDefaultOption()
{
    auto display = DisplayManager::GetInstance()->GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}d, window id:%{public}u", property_->GetDisplayId(),
            property_->GetWindowId());
        return;
    }
    uint32_t width = display->GetWidth();
    uint32_t height = display->GetHeight();
    WLOGFI("width:%{public}u, height:%{public}u, displayId:%{public}d", width, height, property_->GetDisplayId());

    Rect rect;
    switch (property_->GetWindowType()) {
        case WindowType::WINDOW_TYPE_STATUS_BAR: {
            rect = { 0, 0, width, static_cast<uint32_t>((static_cast<float>(height) * 0.07)) };
            property_->SetWindowRect(rect);
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_NAVIGATION_BAR: {
            uint32_t navHeight = static_cast<uint32_t>((static_cast<float>(height) * 0.07));
            rect = { 0, static_cast<int32_t>(height - navHeight), width, navHeight };
            property_->SetWindowRect(rect);
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW: {
            uint32_t alarmWidth = static_cast<uint32_t>((static_cast<float>(width) * 0.8));
            uint32_t alarmHeight = static_cast<uint32_t>((static_cast<float>(height) * 0.3));

            rect = { static_cast<int32_t>((width - alarmWidth) / 2), static_cast<int32_t>((height - alarmHeight) / 2),
                     alarmWidth, alarmHeight };
            property_->SetWindowRect(rect);
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        default:
            break;
    }

}
bool WindowImpl::IsWindowValid() const
{
    return ((state_ > STATE_INITIAL) && (state_ < STATE_BOTTOM));
}
}
}
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

#include <cmath>
#include <ui/rs_surface_node.h>

#include "display_manager.h"
#include "singleton_container.h"
#include "window_adapter.h"
#include "window_agent.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowImpl"};
}

std::map<std::string, std::pair<uint32_t, sptr<Window>>> WindowImpl::windowMap_;
std::map<uint32_t, std::vector<sptr<Window>>> WindowImpl::subWindowMap_;

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
    auto& sysBarPropMap = option->GetSystemBarProperty();
    for (auto it : sysBarPropMap) {
        property_->SetSystemBarProperty(it.first, it.second);
    }
    name_ = option->GetWindowName();
    callback_->onCallback = std::bind(&WindowImpl::OnVsync, this, std::placeholders::_1);

    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    surfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
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

uint32_t WindowImpl::GetWindowFlags()
{
    return property_->GetWindowFlags();
}

SystemBarProperty WindowImpl::GetSystemBarPropertyByType(WindowType type)
{
    auto curProperties = property_->GetSystemBarProperty();
    return curProperties[type];
}

WMError WindowImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    WLOGFI("GetAvoidAreaByType  Search Type: %{public}u", static_cast<uint32_t>(type));
    std::vector<Rect> avoidAreaVec;
    uint32_t windowId = property_->GetWindowId();
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetAvoidAreaByType(windowId, type, avoidAreaVec);
    if (ret != WMError::WM_OK || avoidAreaVec.size() != 4) {    // 4: the avoid area num (left, top, right, bottom)
        WLOGFE("GetAvoidAreaByType errCode:%{public}d winId:%{public}u Type is :%{public}u." \
            "Or avoidArea Size != 4. Current size of avoid area: %{public}u", static_cast<int32_t>(ret),
            property_->GetWindowId(), static_cast<uint32_t>(type), static_cast<uint32_t>(avoidAreaVec.size()));
        return ret;
    }
    avoidArea = {avoidAreaVec[0], avoidAreaVec[1], avoidAreaVec[2], avoidAreaVec[3]}; // 0:left 1:top 2:right 3:bottom
    return ret;
}

WMError WindowImpl::SetWindowType(WindowType type)
{
    WLOGFI("window id: %{public}d, type:%{public}d", property_->GetWindowId(), static_cast<uint32_t>(type));
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_CREATED) {
        if (!(WindowHelper::IsAppWindow(type) || WindowHelper::IsSystemWindow(type))) {
            WLOGFE("window type is invalid %{public}d", type);
            return WMError::WM_ERROR_INVALID_PARAM;
        }
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
    WLOGFI("[Client] Window %{public}d mode %{public}d", property_->GetWindowId(), static_cast<uint32_t>(mode));
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_CREATED || state_ == STATE_HIDDEN) {
        property_->SetWindowMode(mode);
    } else if (state_ == STATE_SHOWN) {
        property_->SetWindowMode(mode);
        return SingletonContainer::Get<WindowAdapter>().SetWindowMode(property_->GetWindowId(), mode);
    }
    if (property_->GetWindowMode() != mode) {
        WLOGFE("set window mode filed! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    return WMError::WM_OK;
}

WMError WindowImpl::AddWindowFlag(WindowFlag flag)
{
    uint32_t updateFlags = property_->GetWindowFlags() | (static_cast<uint32_t>(flag));
    return SetWindowFlags(updateFlags);
}

WMError WindowImpl::RemoveWindowFlag(WindowFlag flag)
{
    uint32_t updateFlags = property_->GetWindowFlags() & (~(static_cast<uint32_t>(flag)));
    return SetWindowFlags(updateFlags);
}

WMError WindowImpl::SetWindowFlags(uint32_t flags)
{
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (property_->GetWindowFlags() == flags) {
        return WMError::WM_OK;
    }
    property_->SetWindowFlags(flags);
    if (state_ == STATE_CREATED || state_ == STATE_HIDDEN) {
        return WMError::WM_OK;
    }
    WMError ret = SingletonContainer::Get<WindowAdapter>().SetWindowFlags(property_->GetWindowId(), flags);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetWindowFlags errCode:%{public}d winId:%{public}d",
            static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

WMError WindowImpl::SetUIContent(const std::string& contentInfo,
    NativeEngine* engine, NativeValue* storage, bool isdistributed)
{
    WLOGFI("SetUIContent");
    if (context_.get() == nullptr) {
        WLOGFE("SetUIContent context_ is nullptr id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("contentInfo: %{public}s, context_:%{public}p", contentInfo.c_str(), context_.get());
    uiContent_ = Ace::UIContent::Create(context_.get(), engine);
    if (uiContent_ == nullptr) {
        WLOGFE("fail to SetUIContent id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    if (isdistributed) {
        uiContent_->Restore(this, contentInfo, storage);
    } else {
        uiContent_->Initialize(this, contentInfo, storage);
    }
    return WMError::WM_OK;
}

const std::string& WindowImpl::GetContentInfo()
{
    WLOGFI("GetContentInfo");
    if (uiContent_ == nullptr) {
        WLOGFE("fail to GetContentInfo id: %{public}d", property_->GetWindowId());
        return "";
    }
    return uiContent_->GetContentInfo();
}

WMError WindowImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (GetSystemBarPropertyByType(type) == property) {
        return WMError::WM_OK;
    }
    property_->SetSystemBarProperty(type, property);
    if (state_ == STATE_CREATED || state_ == STATE_HIDDEN) {
        return WMError::WM_OK;
    }
    WMError ret = SingletonContainer::Get<WindowAdapter>().SetSystemBarProperty(property_->GetWindowId(),
        type, property);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}d",
            static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

WMError WindowImpl::Create(const std::string& parentName, const std::shared_ptr<AbilityRuntime::Context>& context)
{
    WLOGFI("[Client] Window Create");
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
    WMError ret = SingletonContainer::Get<WindowAdapter>().CreateWindow(windowAgent, property_, surfaceNode_,
        windowId);
    property_->SetWindowId(windowId);

    if (ret != WMError::WM_OK) {
        WLOGFE("create window failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return ret;
    }
    context_ = context;
    // FIX ME: use context_
    abilityContext_ = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context);
    if (abilityContext_ != nullptr) {
        ret = SingletonContainer::Get<WindowAdapter>().SaveAbilityToken(abilityContext_->GetAbilityToken(), windowId);
        if (ret != WMError::WM_OK) {
            WLOGFE("SaveAbilityToken failed with errCode:%{public}d", static_cast<int32_t>(ret));
            return ret;
        }
    }
    windowMap_.insert({ name_, std::pair<uint32_t, sptr<Window>>(windowId, this) });
    if (parentName != "") { // add to subWindowMap_
        subWindowMap_[property_->GetParentId()].push_back(this);
    }
    state_ = STATE_CREATED;
    InputTransferStation::GetInstance().AddInputWindow(this);
    return ret;
}

WMError WindowImpl::Destroy()
{
    NotifyBeforeDestroy();
    WLOGFI("[Client] Window %{public}d Destroy", property_->GetWindowId());
    // should destroy surface here
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    WLOGFI("destroy window id: %{public}d", property_->GetWindowId());
    WMError ret = SingletonContainer::Get<WindowAdapter>().DestroyWindow(property_->GetWindowId());
    windowMap_.erase(GetWindowName());
    if (subWindowMap_.count(property_->GetParentId()) > 0) { // remove from subWindowMap_
        std::vector<sptr<Window>>& subWindows = subWindowMap_.at(property_->GetParentId());
        for (auto iter = subWindows.begin(); iter < subWindows.end(); ++iter) {
            if ((*iter)->GetWindowId() == GetWindowId()) {
                subWindows.erase(iter);
                break;
            }
        }
    }
    subWindowMap_.erase(GetWindowId());
    state_ = STATE_DESTROYED;
    InputTransferStation::GetInstance().RemoveInputWindow(this);
    return ret;
}

WMError WindowImpl::Show()
{
    WLOGFI("[Client] Window %{public}d Show", property_->GetWindowId());
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_SHOWN && property_->GetWindowType() == WindowType::WINDOW_TYPE_WALLPAPER) {
        WLOGFI("Minimize all app window");
        WMError ret = SingletonContainer::Get<WindowAdapter>().MinimizeAllAppNodeAbility(property_->GetWindowId());
        if (ret != WMError::WM_OK) {
            WLOGFE("Minimize all app errCode:%{public}d for winId:%{public}d",
                static_cast<int32_t>(ret), property_->GetWindowId());
        }
        return ret;
    }
    if (state_ == STATE_SHOWN) {
        WLOGFI("window is already shown id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    SetDefaultOption();
    WMError ret = SingletonContainer::Get<WindowAdapter>().AddWindow(property_);
    if (ret == WMError::WM_OK || ret == WMError::WM_ERROR_DEATH_RECIPIENT) {
        state_ = STATE_SHOWN;
        NotifyAfterForeground();
    } else {
        WLOGFE("show errCode:%{public}d for winId:%{public}d", static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

WMError WindowImpl::Hide()
{
    WLOGFI("[Client] Window %{public}d Hide", property_->GetWindowId());
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == STATE_HIDDEN || state_ == STATE_CREATED) {
        WLOGFI("window is already hidden id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    WMError ret = SingletonContainer::Get<WindowAdapter>().RemoveWindow(property_->GetWindowId());
    if (ret != WMError::WM_OK) {
        WLOGFE("hide errCode:%{public}d for winId:%{public}d", static_cast<int32_t>(ret), property_->GetWindowId());
        return ret;
    }
    state_ = STATE_HIDDEN;
    NotifyAfterBackground();
    return ret;
}

WMError WindowImpl::MoveTo(int32_t x, int32_t y)
{
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
    return SingletonContainer::Get<WindowAdapter>().MoveTo(property_->GetWindowId(), x, y);
}

WMError WindowImpl::Resize(uint32_t width, uint32_t height)
{
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
    return SingletonContainer::Get<WindowAdapter>().Resize(property_->GetWindowId(), width, height);
}

WMError WindowImpl::RequestFocus() const
{
    if (!IsWindowValid()) {
        WLOGFI("window is already destroyed or not created! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().RequestFocus(property_->GetWindowId());
}

void WindowImpl::AddInputEventListener(std::shared_ptr<MMI::IInputEventConsumer>& inputEventListener)
{
    InputTransferStation::GetInstance().SetInputListener(GetWindowId(), inputEventListener);
}

void WindowImpl::RegisterLifeCycleListener(sptr<IWindowLifeCycle>& listener)
{
    lifecycleListener_ = listener;
}

void WindowImpl::RegisterWindowChangeListener(sptr<IWindowChangeListener>& listener)
{
    windowChangeListener_ = listener;
}

void WindowImpl::RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    if (avoidAreaChangeListener_ != nullptr) {
        WLOGFE("RegisterAvoidAreaChangeListener failed. AvoidAreaChangeListene is not nullptr");
        return;
    }
    avoidAreaChangeListener_ = listener;
}

void WindowImpl::UnregisterAvoidAreaChangeListener()
{
    if (avoidAreaChangeListener_ == nullptr) {
        WLOGFE("UnregisterAvoidAreaChangeListener failed. AvoidAreaChangeListene is nullptr");
        return;
    }
    avoidAreaChangeListener_ = nullptr;
}

void WindowImpl::UpdateRect(const struct Rect& rect)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}d, window id:%{public}u", property_->GetDisplayId(),
            property_->GetWindowId());
        return;
    }
    float virtualPixelRatio = display->GetVirtualPixelRatio();
    WLOGFI("winId:%{public}d, rect[%{public}d, %{public}d, %{public}d, %{public}d], vpr:%{public}f",
        GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_, virtualPixelRatio);
    property_->SetWindowRect(rect);
    if (windowChangeListener_ != nullptr) {
        windowChangeListener_->OnSizeChange(rect);
    }
    if (uiContent_ != nullptr) {
        Ace::ViewportConfig config;
        config.SetSize(rect.width_, rect.height_);
        config.SetDensity(virtualPixelRatio);
        uiContent_->UpdateViewportConfig(config);
        WLOGFI("notify uiContent window size change end");
    }
}

void WindowImpl::UpdateMode(WindowMode mode)
{
    property_->SetWindowMode(mode);
}

void WindowImpl::ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    int32_t keyCode = keyEvent->GetKeyCode();
    int32_t keyAction = keyEvent->GetKeyAction();
    WLOGFI("ConsumeKeyEvent: enter GetKeyCode: %{public}d, action: %{public}d", keyCode, keyAction);
    if (keyCode == MMI::KeyEvent::KEYCODE_BACK) {
        if (keyAction != MMI::KeyEvent::KEY_ACTION_UP) {
            return;
        }
        if (uiContent_ != nullptr && uiContent_->ProcessBackPressed()) {
            WLOGI("ConsumeKeyEvent keyEvent is consumed");
            return;
        }
        // FIX ME: use context_
        if (abilityContext_ != nullptr) {
            WLOGI("ConsumeKeyEvent ability TerminateSelf");
            abilityContext_->TerminateSelf();
        } else {
            WLOGI("ConsumeKeyEvent destroy window");
            Destroy();
        }
    } else {
        if (uiContent_ == nullptr) {
            WLOGE("ConsumeKeyEvent uiContent is nullptr");
            return;
        }
        if (!uiContent_->ProcessKeyEvent(keyEvent)) {
            WLOGI("ConsumeKeyEvent no comsumer window exit");
        }
    }
}

void WindowImpl::ConsumeDividerPointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    static int32_t diffX = 0;
    static int32_t diffY = 0;
    static bool beginMove = false;
    int32_t action = pointerEvent->GetPointerAction();
    MMI::PointerEvent::PointerItem pointerItem;
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN: {
            if (pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
                beginMove = true;
                diffX = abs(pointerItem.GetGlobalX() - GetRect().posX_);
                diffY = abs(pointerItem.GetGlobalY() - GetRect().posY_);
                WLOGFI("point down divider, diff: [%{public}d, %{public}d]", diffX, diffY);
            }
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            if (beginMove && (pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem))) {
                int32_t targetX = pointerItem.GetGlobalX() - diffX;
                int32_t targetY = pointerItem.GetGlobalY() - diffY;
                auto res = MoveTo(targetX, targetY);
                if (res != WMError::WM_OK) {
                    WLOGFE("move divider failed");
                }
            }
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL:
            beginMove = false;
            break;
        default:
            break;
    }
}

void WindowImpl::ConsumePointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (GetType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        ConsumeDividerPointerEvent(pointerEvent);
        return;
    }
    if (uiContent_ == nullptr) {
        WLOGE("ConsumePointerEvent uiContent is nullptr");
        return;
    }
    uiContent_->ProcessPointerEvent(pointerEvent);
}

void WindowImpl::OnVsync(int64_t timeStamp)
{
    uiContent_->ProcessVsyncEvent(timeStamp);
}

void WindowImpl::RequestFrame()
{
    VsyncStation::GetInstance().RequestVsync(VsyncStation::CallbackType::CALLBACK_FRAME, callback_);
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

void WindowImpl::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_ != nullptr) {
        WLOGFD("notify ace winId:%{public}d", GetWindowId());
        uiContent_->UpdateConfiguration(configuration);
    }
    if (subWindowMap_.count(GetWindowId()) == 0) {
        return;
    }
    for (auto& subWindow : subWindowMap_.at(GetWindowId())) {
        subWindow->UpdateConfiguration(configuration);
    }
}

void WindowImpl::UpdateAvoidArea(const std::vector<Rect>& avoidArea)
{
    if (avoidAreaChangeListener_ != nullptr) {
        avoidAreaChangeListener_->OnAvoidAreaChanged(avoidArea);
    }
}

void WindowImpl::SetDefaultOption()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(property_->GetDisplayId());
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
            rect = { 0, 0, width, static_cast<uint32_t>((static_cast<float>(height) * STATUS_BAR_RATIO)) };
            property_->SetWindowRect(rect);
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_NAVIGATION_BAR: {
            uint32_t navHeight = static_cast<uint32_t>((static_cast<float>(height) * NAVIGATION_BAR_RATIO));
            rect = { 0, static_cast<int32_t>(height - navHeight), width, navHeight };
            property_->SetWindowRect(rect);
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW: {
            uint32_t alarmWidth = static_cast<uint32_t>((static_cast<float>(width) *
                SYSTEM_ALARM_WINDOW_WIDTH_RATIO));
            uint32_t alarmHeight = static_cast<uint32_t>((static_cast<float>(height) *
                SYSTEM_ALARM_WINDOW_HEIGHT_RATIO));

            rect = { static_cast<int32_t>((width - alarmWidth) / 2), static_cast<int32_t>((height - alarmHeight) / 2),
                     alarmWidth, alarmHeight };
            property_->SetWindowRect(rect);
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_KEYGUARD: {
            RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
            break;
        }
        case WindowType::WINDOW_TYPE_DRAGGING_EFFECT: {
            property_->SetWindowFlags(0);
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

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
#include "wm_common.h"

#include <ability_manager_client.h>

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowImpl"};
}

std::map<std::string, std::pair<uint32_t, sptr<Window>>> WindowImpl::windowMap_;
std::map<uint32_t, std::vector<sptr<Window>>> WindowImpl::subWindowMap_;
std::map<uint32_t, std::vector<sptr<Window>>> WindowImpl::appFloatingWindowMap_;

WindowImpl::WindowImpl(const sptr<WindowOption>& option)
{
    property_ = new WindowProperty();
    property_->SetWindowName(option->GetWindowName());
    property_->SetWindowRect(option->GetWindowRect());
    property_->SetWindowType(option->GetWindowType());
    property_->SetWindowMode(option->GetWindowMode());
    property_->SetWindowBackgroundBlur(option->GetWindowBackgroundBlur());
    property_->SetAlpha(option->GetAlpha());
    property_->SetFullScreen(option->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    property_->SetFocusable(option->GetFocusable());
    property_->SetTouchable(option->GetTouchable());
    property_->SetDisplayId(option->GetDisplayId());
    property_->SetWindowFlags(option->GetWindowFlags());
    property_->SetHitOffset(option->GetHitOffset());
    AdjustWindowAnimationFlag();
    auto& sysBarPropMap = option->GetSystemBarProperty();
    for (auto it : sysBarPropMap) {
        property_->SetSystemBarProperty(it.first, it.second);
    }
    name_ = option->GetWindowName();
    callback_->onCallback = std::bind(&WindowImpl::OnVsync, this, std::placeholders::_1);

    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = property_->GetWindowName();
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

const std::shared_ptr<AbilityRuntime::Context> WindowImpl::GetContext() const
{
    return context_;
}

sptr<Window> WindowImpl::FindTopWindow(uint32_t topWinId)
{
    if (windowMap_.empty()) {
        WLOGFE("Please create mainWindow First!");
        return nullptr;
    }
    for (auto iter = windowMap_.begin(); iter != windowMap_.end(); iter++) {
        if (topWinId == iter->second.first) {
            WLOGFI("FindTopWindow id: %{public}d", topWinId);
            return iter->second.second;
        }
    }
    WLOGFE("Cannot find topWindow!");
    return nullptr;
}

sptr<Window> WindowImpl::GetTopWindowWithId(uint32_t mainWinId)
{
    uint32_t topWinId = INVALID_WINDOW_ID;
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetTopWindowId(mainWinId, topWinId);
    if (ret != WMError::WM_OK) {
        WLOGFE("GetTopWindowId failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return nullptr;
    }
    return FindTopWindow(topWinId);
}

sptr<Window> WindowImpl::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    if (windowMap_.empty()) {
        WLOGFE("Please create mainWindow First!");
        return nullptr;
    }
    uint32_t mainWinId = INVALID_WINDOW_ID;
    for (auto iter = windowMap_.begin(); iter != windowMap_.end(); iter++) {
        auto win = iter->second.second;
        if (context.get() == win->GetContext().get() && WindowHelper::IsMainWindow(win->GetType())) {
            mainWinId = win->GetWindowId();
            WLOGFI("GetTopWindow Find MainWinId:%{public}d with context %{public}p!", mainWinId, context.get());
            break;
        }
    }
    WLOGFI("GetTopWindowfinal MainWinId:%{public}d!", mainWinId);
    if (!mainWinId) {
        WLOGFE("Cannot find topWindow!");
        return nullptr;
    }
    uint32_t topWinId = INVALID_WINDOW_ID;
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetTopWindowId(mainWinId, topWinId);
    if (ret != WMError::WM_OK) {
        WLOGFE("GetTopWindowId failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return nullptr;
    }
    return FindTopWindow(topWinId);
}

std::vector<sptr<Window>> WindowImpl::GetSubWindow(uint32_t parentId)
{
    if (subWindowMap_.find(parentId) == subWindowMap_.end()) {
        WLOGFE("Cannot parentWindow with id: %{public}d!", parentId);
        return std::vector<sptr<Window>>();
    }
    return subWindowMap_[parentId];
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

WindowBlurLevel WindowImpl::GetWindowBackgroundBlur() const
{
    return property_->GetWindowBackgroundBlur();
}

float WindowImpl::GetAlpha() const
{
    return property_->GetAlpha();
}

bool WindowImpl::GetShowState() const
{
    return state_ == WindowState::STATE_SHOWN;
}

bool WindowImpl::GetFocusable() const
{
    return property_->GetFocusable();
}

bool WindowImpl::GetTouchable() const
{
    return property_->GetTouchable();
}

const std::string& WindowImpl::GetWindowName() const
{
    return name_;
}

uint32_t WindowImpl::GetWindowId() const
{
    return property_->GetWindowId();
}

uint32_t WindowImpl::GetWindowFlags() const
{
    return property_->GetWindowFlags();
}

SystemBarProperty WindowImpl::GetSystemBarPropertyByType(WindowType type) const
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
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_CREATED) {
        if (!(WindowHelper::IsAppWindow(type) || WindowHelper::IsSystemWindow(type))) {
            WLOGFE("window type is invalid %{public}d", type);
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        property_->SetWindowType(type);
        AdjustWindowAnimationFlag();
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
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_CREATED || state_ == WindowState::STATE_HIDDEN) {
        property_->SetWindowMode(mode);
    } else if (state_ == WindowState::STATE_SHOWN) {
        property_->SetWindowMode(mode);
        WMError ret = SingletonContainer::Get<WindowAdapter>().SetWindowMode(property_->GetWindowId(), mode);
        if (ret == WMError::WM_OK) {
            WLOGFD("notify window mode changed");
            for (auto& listener : windowChangeListeners_) {
                if (listener != nullptr) {
                    listener->OnModeChange(mode);
                }
            }
        }
        return ret;
    }
    if (property_->GetWindowMode() != mode) {
        WLOGFE("set window mode filed! id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowBackgroundBlur(WindowBlurLevel level)
{
    WLOGFI("[Client] Window %{public}d blurlevel %{public}u", property_->GetWindowId(), static_cast<uint32_t>(level));
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetWindowBackgroundBlur(level);
    return SingletonContainer::Get<WindowAdapter>().SetWindowBackgroundBlur(property_->GetWindowId(), level);
}

WMError WindowImpl::SetAlpha(float alpha)
{
    WLOGFI("[Client] Window %{public}d alpha %{public}f", property_->GetWindowId(), alpha);
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetAlpha(alpha);
    return SingletonContainer::Get<WindowAdapter>().SetAlpha(property_->GetWindowId(), alpha);
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
    WLOGFI("[Client] Window %{public}d flags %{public}u", property_->GetWindowId(), flags);
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (property_->GetWindowFlags() == flags) {
        return WMError::WM_OK;
    }
    property_->SetWindowFlags(flags);
    if (state_ == WindowState::STATE_CREATED || state_ == WindowState::STATE_HIDDEN) {
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
    WLOGFI("contentInfo: %{public}s, context_:%{public}p", contentInfo.c_str(), context_.get());
    uiContent_ = Ace::UIContent::Create(context_.get(), engine);
    if (uiContent_ == nullptr) {
        WLOGFE("fail to SetUIContent id: %{public}d", property_->GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        property_->SetDecorEnable(true);
    } else {
        property_->SetDecorEnable(false);
    }
    if (isdistributed) {
        uiContent_->Restore(this, contentInfo, storage);
    } else {
        uiContent_->Initialize(this, contentInfo, storage);
    }
    return WMError::WM_OK;
}

std::string WindowImpl::GetContentInfo()
{
    WLOGFI("GetContentInfo");
    if (uiContent_ == nullptr) {
        WLOGFE("fail to GetContentInfo id: %{public}d", property_->GetWindowId());
        return "";
    }
    return uiContent_->GetContentInfo();
}

bool WindowImpl::IsSupportWideGamut()
{
    return SingletonContainer::Get<WindowAdapter>().IsSupportWideGamut(property_->GetWindowId());
}

void WindowImpl::SetColorSpace(ColorSpace colorSpace)
{
    SingletonContainer::Get<WindowAdapter>().SetColorSpace(property_->GetWindowId(), colorSpace);
}

ColorSpace WindowImpl::GetColorSpace()
{
    return SingletonContainer::Get<WindowAdapter>().GetColorSpace(property_->GetWindowId());
}

void WindowImpl::DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info)
{
    WLOGFI("Ace:DumpInfo");
    if (uiContent_ != nullptr) {
        uiContent_->DumpInfo(params, info);
    }
}

WMError WindowImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    WLOGFI("[Client] Window %{public}d SetSystemBarProperty type %{public}d " \
        "enable:%{public}d, backgroundColor:%{public}x, contentColor:%{public}x ",
        property_->GetWindowId(), static_cast<uint32_t>(type), property.enable_,
        property.backgroundColor_, property.contentColor_);
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (GetSystemBarPropertyByType(type) == property) {
        return WMError::WM_OK;
    }
    property_->SetSystemBarProperty(type, property);
    if (state_ == WindowState::STATE_CREATED || state_ == WindowState::STATE_HIDDEN) {
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

WMError WindowImpl::SetLayoutFullScreen(bool status)
{
    WLOGFI("[Client] Window %{public}d SetLayoutFullScreen: %{public}d", property_->GetWindowId(), status);
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WMError ret = SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetWindowMode errCode:%{public}d winId:%{public}d",
            static_cast<int32_t>(ret), property_->GetWindowId());
        return ret;
    }
    if (status) {
        ret = RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
        if (ret != WMError::WM_OK) {
            WLOGFE("RemoveWindowFlag errCode:%{public}d winId:%{public}d",
                static_cast<int32_t>(ret), property_->GetWindowId());
            return ret;
        }
    } else {
        ret = AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
        if (ret != WMError::WM_OK) {
            WLOGFE("AddWindowFlag errCode:%{public}d winId:%{public}d",
                static_cast<int32_t>(ret), property_->GetWindowId());
            return ret;
        }
    }
    return ret;
}

WMError WindowImpl::SetFullScreen(bool status)
{
    WLOGFI("[Client] Window %{public}d SetFullScreen: %{public}d", property_->GetWindowId(), status);
    WMError ret = SetLayoutFullScreen(status);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetLayoutFullScreen errCode:%{public}d winId:%{public}d",
            static_cast<int32_t>(ret), property_->GetWindowId());
        return ret;
    }
    SystemBarProperty statusProperty = GetSystemBarPropertyByType(
        WindowType::WINDOW_TYPE_STATUS_BAR);
    SystemBarProperty naviProperty = GetSystemBarPropertyByType(
        WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    if (status) {
        statusProperty.enable_ = false;
        naviProperty.enable_ = false;
    } else {
        statusProperty.enable_ = true;
        naviProperty.enable_ = true;
    }
    ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusProperty);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}d",
            static_cast<int32_t>(ret), property_->GetWindowId());
        return ret;
    }
    ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, naviProperty);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}d",
            static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

void WindowImpl::MapFloatingWindowToAppIfNeeded()
{
    if (GetType() != WindowType::WINDOW_TYPE_FLOAT || context_.get() == nullptr) {
        return;
    }

    for (auto& winPair : windowMap_) {
        auto win = winPair.second.second;
        if (win->GetType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
            context_.get() == win->GetContext().get()) {
            appFloatingWindowMap_[win->GetWindowId()].push_back(this);
            WLOGFI("Map FloatingWindow %{public}d to AppMainWindow %{public}d", GetWindowId(), win->GetWindowId());
            return;
        }
    }
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
        ret = SingletonContainer::Get<WindowAdapter>().SaveAbilityToken(abilityContext_->GetToken(), windowId);
        if (ret != WMError::WM_OK) {
            WLOGFE("SaveAbilityToken failed with errCode:%{public}d", static_cast<int32_t>(ret));
            return ret;
        }
    }
    windowMap_.insert(std::make_pair(name_, std::pair<uint32_t, sptr<Window>>(windowId, this)));
    if (parentName != "") { // add to subWindowMap_
        subWindowMap_[property_->GetParentId()].push_back(this);
    }

    MapFloatingWindowToAppIfNeeded();

    state_ = WindowState::STATE_CREATED;
    InputTransferStation::GetInstance().AddInputWindow(this);
    return ret;
}

WMError WindowImpl::Destroy()
{
    if (!IsWindowValid()) {
        return WMError::WM_OK;
    }

    WLOGFI("[Client] Window %{public}d Destroy", property_->GetWindowId());

    // FixMe: Remove "NotifyBeforeDestroy()" because of ACE bug, add when fixed
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

    // Destroy app floating window if exist
    if (appFloatingWindowMap_.count(GetWindowId()) > 0) {
        for (auto& floatingWindow : appFloatingWindowMap_.at(GetWindowId())) {
            WMError fltRet = SingletonContainer::Get<WindowAdapter>().DestroyWindow(floatingWindow->GetWindowId());
            if (fltRet == WMError::WM_OK) {
                WLOGFI("Destroy App %{public}d FltWindow %{public}d", GetWindowId(), floatingWindow->GetWindowId());
            } else {
                WLOGFE("Floating Window %{public}d Destroy Failed", floatingWindow->GetWindowId());
            }
        }
        appFloatingWindowMap_.erase(GetWindowId());
    }

    state_ = WindowState::STATE_DESTROYED;
    InputTransferStation::GetInstance().RemoveInputWindow(this);
    return ret;
}

WMError WindowImpl::Show()
{
    WLOGFI("[Client] Window [name:%{public}s, id:%{public}d] Show", name_.c_str(), property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN && property_->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
        WLOGFI("Minimize all app windows");
        SingletonContainer::Get<WindowAdapter>().MinimizeAllAppWindows(property_->GetDisplayId());
        for (auto& listener : lifecycleListeners_) {
            if (listener != nullptr) {
                listener->AfterForeground();
            }
        }
        return WMError::WM_OK;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        WLOGFI("window is already shown id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    SetDefaultOption();
    WMError ret = SingletonContainer::Get<WindowAdapter>().AddWindow(property_);
    if (ret == WMError::WM_OK || ret == WMError::WM_ERROR_DEATH_RECIPIENT) {
        state_ = WindowState::STATE_SHOWN;
        NotifyAfterForeground();
    } else {
        WLOGFE("show errCode:%{public}d for winId:%{public}d", static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

WMError WindowImpl::Hide()
{
    WLOGFI("[Client] Window [name:%{public}s, id:%{public}d] Hide", name_.c_str(), property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFI("window is already hidden id: %{public}d", property_->GetWindowId());
        return WMError::WM_OK;
    }
    WMError ret = SingletonContainer::Get<WindowAdapter>().RemoveWindow(property_->GetWindowId());
    if (ret != WMError::WM_OK) {
        WLOGFE("hide errCode:%{public}d for winId:%{public}d", static_cast<int32_t>(ret), property_->GetWindowId());
        return ret;
    }
    state_ = WindowState::STATE_HIDDEN;
    NotifyAfterBackground();
    return ret;
}

WMError WindowImpl::MoveTo(int32_t x, int32_t y)
{
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    Rect rect = GetRect();
    Rect moveRect = { x, y, rect.width_, rect.height_ };
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFI("window is hidden or created! id: %{public}d, oriPos: [%{public}d, %{public}d, "
               "movePos: [%{public}d, %{public}d]", property_->GetWindowId(), rect.posX_, rect.posY_, x, y);
        property_->SetWindowRect(moveRect);
        return WMError::WM_OK;
    }
    return SingletonContainer::Get<WindowAdapter>().ResizeRect(property_->GetWindowId(),
        moveRect, WindowSizeChangeReason::MOVE);
}

WMError WindowImpl::Resize(uint32_t width, uint32_t height)
{
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    Rect rect = GetRect();
    Rect resizeRect = { rect.posX_, rect.posY_, width, height };
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFI("window is hidden or created! id: %{public}d, oriRect: [%{public}u, %{public}u], "
               "resizeRect: [%{public}u, %{public}u]", property_->GetWindowId(), rect.width_,
               rect.height_, width, height);
        property_->SetWindowRect(resizeRect);
        return WMError::WM_OK;
    }
    return SingletonContainer::Get<WindowAdapter>().ResizeRect(property_->GetWindowId(),
        resizeRect, WindowSizeChangeReason::RESIZE);
}

WMError WindowImpl::Drag(const Rect& rect)
{
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().ResizeRect(property_->GetWindowId(),
        rect, WindowSizeChangeReason::DRAG);
}

bool WindowImpl::IsDecorEnable() const
{
    return property_->GetDecorEnable();
}

WMError WindowImpl::Maximize()
{
    WLOGFI("[Client] Window %{public}d Maximize", property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        SetFullScreen(true);
    }
    return WMError::WM_OK;
}

WMError WindowImpl::Minimize()
{
    WLOGFI("[Client] Window %{public}d Minimize", property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        if (abilityContext_ != nullptr) {
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(abilityContext_->GetToken(), true);
        } else {
            Hide();
        }
    }
    return WMError::WM_OK;
}

WMError WindowImpl::Recover()
{
    WLOGFI("[Client] Window %{public}d Normalize", property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    }
    return WMError::WM_OK;
}

WMError WindowImpl::Close()
{
    WLOGFI("[Client] Window %{public}d Close", property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        if (abilityContext_ != nullptr) {
            abilityContext_->CloseAbility();
        } else {
            Destroy();
        }
    }
    return WMError::WM_OK;
}

void WindowImpl::StartMove()
{
    if (!WindowHelper::IsMainFloatingWindow(GetType(), GetMode())) {
        WLOGFI("[StartMove] current window can not be moved, windowId %{public}u", GetWindowId());
        return;
    }
    startMoveFlag_ = true;
    WLOGFI("[StartMove] windowId %{public}u", GetWindowId());
    return;
}

WMError WindowImpl::RequestFocus() const
{
    if (!IsWindowValid()) {
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
    if (listener == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    lifecycleListeners_.emplace_back(listener);
}

void WindowImpl::RegisterWindowChangeListener(sptr<IWindowChangeListener>& listener)
{
    if (listener == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    windowChangeListeners_.emplace_back(listener);
}

void WindowImpl::UnregisterLifeCycleListener(sptr<IWindowLifeCycle>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    lifecycleListeners_.erase(std::remove_if(lifecycleListeners_.begin(), lifecycleListeners_.end(),
        [listener](sptr<IWindowLifeCycle> registeredListener) {
            return registeredListener == listener;
        }), lifecycleListeners_.end());
}

void WindowImpl::UnregisterWindowChangeListener(sptr<IWindowChangeListener>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    windowChangeListeners_.erase(std::remove_if(windowChangeListeners_.begin(), windowChangeListeners_.end(),
        [listener](sptr<IWindowChangeListener> registeredListener) {
            return registeredListener == listener;
        }), windowChangeListeners_.end());
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

void WindowImpl::RegisterDragListener(sptr<IWindowDragListener>& listener)
{
    if (listener == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    windowDragListeners_.emplace_back(listener);
}

void WindowImpl::UnregisterDragListener(sptr<IWindowDragListener>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(windowDragListeners_.begin(), windowDragListeners_.end(), listener);
    if (iter == windowDragListeners_.end()) {
        WLOGFE("could not find this listener");
        return;
    }
    windowDragListeners_.erase(iter);
}

void WindowImpl::UpdateRect(const struct Rect& rect, WindowSizeChangeReason reason)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64", window id:%{public}u", property_->GetDisplayId(),
            property_->GetWindowId());
        return;
    }
    float virtualPixelRatio = display->GetVirtualPixelRatio();
    WLOGFI("winId:%{public}u, rect[%{public}d, %{public}d, %{public}u, %{public}u], vpr:%{public}f, reason:%{public}u",
        GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_, virtualPixelRatio, reason);
    property_->SetWindowRect(rect);
    for (auto& listener : windowChangeListeners_) {
        if (listener != nullptr) {
            listener->OnSizeChange(rect, reason);
        }
    }

    if (uiContent_ != nullptr) {
        Ace::ViewportConfig config;
        config.SetSize(rect.width_, rect.height_);
        config.SetDensity(virtualPixelRatio);
        uiContent_->UpdateViewportConfig(config, reason);
        WLOGFI("notify uiContent window size change end");
    }
}

void WindowImpl::UpdateMode(WindowMode mode)
{
    WLOGI("UpdateMode %{public}d", mode);
    property_->SetWindowMode(mode);
    for (auto& listener : windowChangeListeners_) {
        if (listener != nullptr) {
            listener->OnModeChange(mode);
        }
    }
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

void WindowImpl::HandleMoveEvent(int32_t posX, int32_t posY, int32_t pointId)
{
    if (!startMoveFlag_ || (pointId != startPointerId_)) {
        return;
    }
    int32_t targetX = startPointRect_.posX_ + (posX - startPointPosX_);
    int32_t targetY = startPointRect_.posY_ + (posY - startPointPosY_);
    auto res = MoveTo(targetX, targetY);
    if (res != WMError::WM_OK) {
        WLOGFE("move window: %{public}u failed", GetWindowId());
    }
}

void WindowImpl::HandleDragEvent(int32_t posX, int32_t posY, int32_t pointId)
{
    if (!startDragFlag_ || (pointId != startPointerId_)) {
        return;
    }
    int32_t diffX = posX - startPointPosX_;
    int32_t diffY = posY - startPointPosY_;
    Rect newRect = startPointRect_;
    if (startPointPosX_ <= startPointRect_.posX_) {
        if (diffX > static_cast<int32_t>(startPointRect_.width_)) {
            diffX = static_cast<int32_t>(startPointRect_.width_);
        }
        newRect.posX_ += diffX;
        newRect.width_ = static_cast<uint32_t>(static_cast<int32_t>(newRect.width_) - diffX);
    } else if (startPointPosX_ >= startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_)) {
        if (diffX < 0 && (-diffX > static_cast<int32_t>(startPointRect_.width_))) {
            diffX = -(static_cast<int32_t>(startPointRect_.width_));
        }
        newRect.width_ = static_cast<uint32_t>(static_cast<int32_t>(newRect.width_) + diffX);
    }
    if (startPointPosY_ <= startPointRect_.posY_) {
        if (diffY > static_cast<int32_t>(startPointRect_.height_)) {
            diffY = static_cast<int32_t>(startPointRect_.height_);
        }
        newRect.posY_ += diffY;
        newRect.height_ = static_cast<uint32_t>(static_cast<int32_t>(newRect.height_) - diffY);
    } else if (startPointPosY_ >= startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_)) {
        if (diffY < 0 && (-diffY > static_cast<int32_t>(startPointRect_.height_))) {
            diffY = -(static_cast<int32_t>(startPointRect_.height_));
        }
        newRect.height_ = static_cast<uint32_t>(static_cast<int32_t>(newRect.height_) + diffY);
    }
    auto res = Drag(newRect);
    if (res != WMError::WM_OK) {
        WLOGFE("drag window: %{public}u failed", GetWindowId());
    }
}

void WindowImpl::EndMoveOrDragWindow(int32_t pointId)
{
    if (pointId != startPointerId_) {
        return;
    }
    startDragFlag_ = false;
    startMoveFlag_ = false;
    pointEventStarted_ = false;
    return;
}

void WindowImpl::ReadyToMoveOrDragWindow(int32_t globalX, int32_t globalY, int32_t pointId, const Rect& rect)
{
    if (pointEventStarted_) {
        return;
    }
    startPointRect_ = rect;
    startPointPosX_ = globalX;
    startPointPosY_ = globalY;
    startPointerId_ = pointId;
    pointEventStarted_ = true;
    if (GetType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        startMoveFlag_ = true;
    } else if (!WindowHelper::IsPointInWindow(startPointPosX_, startPointPosY_, startPointRect_)) {
        startDragFlag_ = true;
    }
    return;
}

void WindowImpl::ConsumeMoveOrDragEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    MMI::PointerEvent::PointerItem pointerItem;
    int32_t pointId = pointerEvent->GetPointerId();
    if (!pointerEvent->GetPointerItem(pointId, pointerItem)) {
        WLOGFW("Point item is invalid");
        return;
    }
    int32_t pointGlobalX = pointerItem.GetGlobalX();
    int32_t pointGlobalY = pointerItem.GetGlobalY();
    int32_t action = pointerEvent->GetPointerAction();
    switch (action) {
        // Ready to move or drag
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN: {
            Rect rect = GetRect();
            ReadyToMoveOrDragWindow(pointGlobalX, pointGlobalY, pointId, rect);
            WLOGFI("[Point Down]: windowId: %{public}u, action: %{public}d, hasPointStarted: %{public}d, "
                   "startMove: %{public}d, startDrag: %{public}d, pointPos: [%{public}d, %{public}d], "
                   "winRect: [%{public}d, %{public}d, %{public}u, %{public}u]",
                   GetWindowId(), action, pointEventStarted_, startMoveFlag_, startDragFlag_,
                   pointGlobalX, pointGlobalY, rect.posX_, rect.posY_, rect.width_, rect.height_);
            break;
        }
        // Start to move or darg
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            HandleMoveEvent(pointGlobalX, pointGlobalY, pointId);
            HandleDragEvent(pointGlobalX, pointGlobalY, pointId);
            break;
        }
        // End move or drag
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            EndMoveOrDragWindow(pointId);
            WLOGFI("[Point Up/Cancle]: windowId: %{public}u, action: %{public}d, startMove: %{public}d, "
                   "startDrag: %{public}d", GetWindowId(), action, startMoveFlag_, startDragFlag_);
            break;
        }
        default:
            break;
    }
}

bool WindowImpl::IsPointerEventConsumed()
{
    return startDragFlag_ || startMoveFlag_;
}

void WindowImpl::AdjustWindowAnimationFlag()
{
    WindowType winType = property_->GetWindowType();
    if (!WindowHelper::IsAppWindow(winType)) {
        property_->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::NONE));
    }
}

void WindowImpl::ConsumePointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    int32_t action = pointerEvent->GetPointerAction();
    if (action == MMI::PointerEvent::POINTER_ACTION_DOWN || action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        WLOGI("WMS process point down, windowId: %{public}u, action: %{public}d", GetWindowId(), action);
        SingletonContainer::Get<WindowAdapter>().ProcessWindowTouchedEvent(property_->GetWindowId());
    }
    if (WindowHelper::IsMainFloatingWindow(GetType(), GetMode()) ||
        GetType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        ConsumeMoveOrDragEvent(pointerEvent);
    }

    if (IsPointerEventConsumed()) {
        return;
    }
    if (uiContent_ == nullptr) {
        WLOGE("ConsumePointerEvent uiContent is nullptr, windowId: %{public}u", GetWindowId());
        return;
    }
    WLOGFI("Transer pointer event to ACE");
    uiContent_->ProcessPointerEvent(pointerEvent);
}

void WindowImpl::OnVsync(int64_t timeStamp)
{
    uiContent_->ProcessVsyncEvent(static_cast<uint64_t>(timeStamp));
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
        NotifyAfterUnfocused();
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
    WLOGFI("Window Update AvoidArea, id: %{public}d", property_->GetWindowId());
    if (avoidAreaChangeListener_ != nullptr) {
        avoidAreaChangeListener_->OnAvoidAreaChanged(avoidArea);
    }
}

void WindowImpl::UpdateWindowState(WindowState state)
{
    WLOGFI("[Client] Window %{public}u, WindowState to set:%{public}u", GetWindowId(), state);
    if (!IsWindowValid()) {
        return;
    }
    switch (state) {
        case WindowState::STATE_FROZEN: {
            state_ = WindowState::STATE_HIDDEN;
            if (abilityContext_ != nullptr) {
                WLOGFD("DoAbilityBackground KEYGUARD, id: %{public}d", GetWindowId());
                AAFwk::AbilityManagerClient::GetInstance()->DoAbilityBackground(abilityContext_->GetToken(),
                    static_cast<uint32_t>(WindowStateChangeReason::KEYGUARD));
            }
            NotifyAfterBackground();
            break;
        }
        case WindowState::STATE_UNFROZEN: {
            state_ = WindowState::STATE_SHOWN;
            if (abilityContext_ != nullptr) {
                WLOGFD("DoAbilityForeground KEYGUARD, id: %{public}d", GetWindowId());
                AAFwk::AbilityManagerClient::GetInstance()->DoAbilityForeground(abilityContext_->GetToken(),
                    static_cast<uint32_t>(WindowStateChangeReason::KEYGUARD));
            }
            NotifyAfterForeground();
            break;
        }
        default: {
            WLOGFE("windowState to set is invalid");
            break;
        }
    }
}

void WindowImpl::UpdateDragEvent(const PointInfo& point, DragEvent event)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& iter : windowDragListeners_) {
        iter->OnDrag(point.x, point.y, event);
    }
}

void WindowImpl::SetDefaultOption()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64", window id:%{public}u", property_->GetDisplayId(),
            property_->GetWindowId());
        return;
    }
    uint32_t width = static_cast<uint32_t>(display->GetWidth());
    uint32_t height = static_cast<uint32_t>(display->GetHeight());
    WLOGFI("width:%{public}u, height:%{public}u, displayId:%{public}" PRIu64"",
        width, height, property_->GetDisplayId());

    Rect rect;
    switch (property_->GetWindowType()) {
        case WindowType::WINDOW_TYPE_STATUS_BAR: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            property_->SetFocusable(false);
            break;
        }
        case WindowType::WINDOW_TYPE_NAVIGATION_BAR: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            property_->SetFocusable(false);
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
        case WindowType::WINDOW_TYPE_VOLUME_OVERLAY: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::SYSTEM_WINDOW_END: // SYSTEM_WINDOW_END is for boot animation, is unfocusable.
        case WindowType::WINDOW_TYPE_POINTER: {
            property_->SetFocusable(false);
            break;
        }
        default:
            break;
    }
}
bool WindowImpl::IsWindowValid() const
{
    bool res = ((state_ > WindowState::STATE_INITIAL) && (state_ < WindowState::STATE_BOTTOM));
    if (!res) {
        WLOGFI("window is already destroyed or not created! id: %{public}u", GetWindowId());
    }
    return res;
}

bool WindowImpl::IsLayoutFullScreen() const
{
    uint32_t flags = GetWindowFlags();
    auto mode = GetMode();
    bool needAvoid = (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    if (mode == WindowMode::WINDOW_MODE_FULLSCREEN && !needAvoid) {
        return true;
    }
    return false;
}

bool WindowImpl::IsFullScreen() const
{
    auto statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto naviProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    if (IsLayoutFullScreen() && !statusProperty.enable_ && !naviProperty.enable_) {
        return true;
    }
    return false;
}
}
}

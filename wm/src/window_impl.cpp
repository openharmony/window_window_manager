/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <ability_manager_client.h>
#include <power_mgr_client.h>

#include "color_parser.h"
#include "display_manager.h"
#include "singleton_container.h"
#include "window_adapter.h"
#include "window_agent.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowImpl"};
}

const WindowImpl::ColorSpaceConvertMap WindowImpl::colorSpaceConvertMap[] = {
    { ColorSpace::COLOR_SPACE_DEFAULT, COLOR_GAMUT_SRGB },
    { ColorSpace::COLOR_SPACE_WIDE_GAMUT, COLOR_GAMUT_DCI_P3 },
};

std::map<std::string, std::pair<uint32_t, sptr<Window>>> WindowImpl::windowMap_;
std::map<uint32_t, std::vector<sptr<WindowImpl>>> WindowImpl::subWindowMap_;
std::map<uint32_t, std::vector<sptr<WindowImpl>>> WindowImpl::appFloatingWindowMap_;

WindowImpl::WindowImpl(const sptr<WindowOption>& option)
{
    property_ = new (std::nothrow) WindowProperty();
    property_->SetWindowName(option->GetWindowName());
    property_->SetRequestRect(option->GetWindowRect());
    property_->SetWindowType(option->GetWindowType());
    property_->SetWindowMode(option->GetWindowMode());
    property_->SetWindowBackgroundBlur(option->GetWindowBackgroundBlur());
    property_->SetAlpha(option->GetAlpha());
    property_->SetFullScreen(option->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    property_->SetFocusable(option->GetFocusable());
    property_->SetTouchable(option->GetTouchable());
    property_->SetDisplayId(option->GetDisplayId());
    property_->SetCallingWindow(option->GetCallingWindow());
    property_->SetWindowFlags(option->GetWindowFlags());
    property_->SetHitOffset(option->GetHitOffset());
    property_->SetRequestedOrientation(option->GetRequestedOrientation());
    windowTag_ = option->GetWindowTag();
    keepScreenOn_ = option->IsKeepScreenOn();
    property_->SetTurnScreenOn(option->IsTurnScreenOn());
    brightness_ = option->GetBrightness();
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
            WLOGFI("FindTopWindow id: %{public}u", topWinId);
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
            WLOGFI("GetTopWindow Find MainWinId:%{public}u.", mainWinId);
            break;
        }
    }
    WLOGFI("GetTopWindowfinal MainWinId:%{public}u!", mainWinId);
    if (mainWinId == INVALID_WINDOW_ID) {
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
        WLOGFE("Cannot parentWindow with id: %{public}u!", parentId);
        return std::vector<sptr<Window>>();
    }
    return std::vector<sptr<Window>>(subWindowMap_[parentId].begin(), subWindowMap_[parentId].end());
}

std::shared_ptr<RSSurfaceNode> WindowImpl::GetSurfaceNode() const
{
    return surfaceNode_;
}

Rect WindowImpl::GetRect() const
{
    return property_->GetWindowRect();
}

Rect WindowImpl::GetRequestRect() const
{
    return property_->GetRequestRect();
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

void WindowImpl::SetFocusable(bool isFocusable)
{
    if (!IsWindowValid()) {
        return;
    }
    property_->SetFocusable(isFocusable);
    UpdateProperty(PropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
}

bool WindowImpl::GetFocusable() const
{
    return property_->GetFocusable();
}

void WindowImpl::SetTouchable(bool isTouchable)
{
    if (!IsWindowValid()) {
        return;
    }
    property_->SetTouchable(isTouchable);
    UpdateProperty(PropertyChangeAction::ACTION_UPDATE_TOUCHABLE);
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
    WLOGFI("window id: %{public}u, type:%{public}u.", property_->GetWindowId(), static_cast<uint32_t>(type));
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_CREATED) {
        if (!(WindowHelper::IsAppWindow(type) || WindowHelper::IsSystemWindow(type))) {
            WLOGFE("window type is invalid %{public}u.", type);
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        property_->SetWindowType(type);
        property_->SetDecorEnable(WindowHelper::IsMainWindow(property_->GetWindowType()));
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
    WLOGFI("[Client] Window %{public}u mode %{public}u", property_->GetWindowId(), static_cast<uint32_t>(mode));
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_CREATED || state_ == WindowState::STATE_HIDDEN) {
        UpdateMode(mode);
    } else if (state_ == WindowState::STATE_SHOWN) {
        property_->SetWindowMode(mode);
        WMError ret = UpdateProperty(PropertyChangeAction::ACTION_UPDATE_MODE);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        // set client window mode if success.
        UpdateMode(mode);
    }
    if (property_->GetWindowMode() != mode) {
        WLOGFE("set window mode filed! id: %{public}u.", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowBackgroundBlur(WindowBlurLevel level)
{
    WLOGFI("[Client] Window %{public}u blurlevel %{public}u", property_->GetWindowId(), static_cast<uint32_t>(level));
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetWindowBackgroundBlur(level);
    return SingletonContainer::Get<WindowAdapter>().SetWindowBackgroundBlur(property_->GetWindowId(), level);
}

WMError WindowImpl::SetAlpha(float alpha)
{
    WLOGFI("[Client] Window %{public}u alpha %{public}f", property_->GetWindowId(), alpha);
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
    WLOGFI("[Client] Window %{public}u flags %{public}u", property_->GetWindowId(), flags);
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
    WMError ret = UpdateProperty(PropertyChangeAction::ACTION_UPDATE_FLAGS);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetWindowFlags errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

void WindowImpl::OnNewWant(const AAFwk::Want& want)
{
    WLOGFI("[Client] Window [name:%{public}s, id:%{public}u] OnNewWant", name_.c_str(), property_->GetWindowId());
    if (uiContent_ != nullptr) {
        uiContent_->OnNewWant(want);
    }
}

WMError WindowImpl::SetUIContent(const std::string& contentInfo,
    NativeEngine* engine, NativeValue* storage, bool isdistributed, AppExecFwk::Ability* ability)
{
    WLOGFI("SetUIContent contentInfo: %{public}s", contentInfo.c_str());
    if (ability != nullptr) {
        uiContent_ = Ace::UIContent::Create(ability);
    } else {
        if (context_.get() == nullptr) {
            WLOGFE("fail to SetUIContent id: %{public}u in StageMode because null context", property_->GetWindowId());
            return WMError::WM_ERROR_NULLPTR;
        }
        uiContent_ = Ace::UIContent::Create(context_.get(), engine);
    }
    if (uiContent_ == nullptr) {
        WLOGFE("fail to SetUIContent id: %{public}u", property_->GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    if (isdistributed) {
        uiContent_->Restore(this, contentInfo, storage);
    } else {
        uiContent_->Initialize(this, contentInfo, storage);
    }
    if (state_ == WindowState::STATE_SHOWN) {
        Ace::ViewportConfig config;
        Rect rect = GetRect();
        config.SetSize(rect.width_, rect.height_);
        config.SetPosition(rect.posX_, rect.posY_);
        auto display = DisplayManager::GetInstance().GetDisplayById(property_->GetDisplayId());
        if (display == nullptr) {
            WLOGFE("get display failed displayId:%{public}" PRIu64", window id:%{public}u", property_->GetDisplayId(),
                property_->GetWindowId());
            return WMError::WM_ERROR_NULLPTR;
        }
        float virtualPixelRatio = display->GetVirtualPixelRatio();
        config.SetDensity(virtualPixelRatio);
        uiContent_->UpdateViewportConfig(config, WindowSizeChangeReason::UNDEFINED);
        WLOGFI("notify uiContent window size change end");
    }
    return WMError::WM_OK;
}

Ace::UIContent* WindowImpl::GetUIContent() const
{
    return uiContent_.get();
}

std::string WindowImpl::GetContentInfo()
{
    WLOGFI("GetContentInfo");
    if (uiContent_ == nullptr) {
        WLOGFE("fail to GetContentInfo id: %{public}u", property_->GetWindowId());
        return "";
    }
    return uiContent_->GetContentInfo();
}

ColorSpace WindowImpl::GetColorSpaceFromSurfaceGamut(ColorGamut ColorGamut)
{
    for (auto item: colorSpaceConvertMap) {
        if (item.sufaceColorGamut == ColorGamut) {
            return item.colorSpace;
        }
    }
    return ColorSpace::COLOR_SPACE_DEFAULT;
}

ColorGamut WindowImpl::GetSurfaceGamutFromColorSpace(ColorSpace colorSpace)
{
    for (auto item: colorSpaceConvertMap) {
        if (item.colorSpace == colorSpace) {
            return item.sufaceColorGamut;
        }
    }
    return ColorGamut::COLOR_GAMUT_SRGB;
}

bool WindowImpl::IsSupportWideGamut()
{
    return true;
}

void WindowImpl::SetColorSpace(ColorSpace colorSpace)
{
    auto surfaceGamut = GetSurfaceGamutFromColorSpace(colorSpace);
    surfaceNode_->SetColorSpace(surfaceGamut);
}

ColorSpace WindowImpl::GetColorSpace()
{
    auto surfaceGamut = surfaceNode_->GetColorSpace();
    return GetColorSpaceFromSurfaceGamut(surfaceGamut);
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
    WLOGFI("[Client] Window %{public}u SetSystemBarProperty type %{public}u " \
        "enable:%{public}u, backgroundColor:%{public}x, contentColor:%{public}x ",
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
    WMError ret = UpdateProperty(PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

WMError WindowImpl::SetLayoutFullScreen(bool status)
{
    WLOGFI("[Client] Window %{public}u SetLayoutFullScreen: %{public}u", property_->GetWindowId(), status);
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WMError ret = SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetWindowMode errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), property_->GetWindowId());
        return ret;
    }
    if (status) {
        ret = RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
        if (ret != WMError::WM_OK) {
            WLOGFE("RemoveWindowFlag errCode:%{public}d winId:%{public}u",
                static_cast<int32_t>(ret), property_->GetWindowId());
            return ret;
        }
    } else {
        ret = AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
        if (ret != WMError::WM_OK) {
            WLOGFE("AddWindowFlag errCode:%{public}d winId:%{public}u",
                static_cast<int32_t>(ret), property_->GetWindowId());
            return ret;
        }
    }
    return ret;
}

WMError WindowImpl::SetFullScreen(bool status)
{
    WLOGFI("[Client] Window %{public}u SetFullScreen: %{public}d", property_->GetWindowId(), status);
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
    WMError ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusProperty);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), property_->GetWindowId());
    }
    ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, naviProperty);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), property_->GetWindowId());
    }
    ret = SetLayoutFullScreen(status);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetLayoutFullScreen errCode:%{public}d winId:%{public}u",
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
            WLOGFI("Map FloatingWindow %{public}u to AppMainWindow %{public}u", GetWindowId(), win->GetWindowId());
            return;
        }
    }
}

WMError WindowImpl::UpdateProperty(PropertyChangeAction action)
{
    return SingletonContainer::Get<WindowAdapter>().UpdateProperty(property_, action);
}

void WindowImpl::HandleKeepScreenOn(bool keepScreenOn)
{
    if (keepScreenOn && keepScreenLock_ == nullptr) {
        keepScreenLock_ = PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(name_,
            PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
    }
    if (keepScreenLock_ == nullptr) {
        return;
    }
    WLOGFI("handle keep screen on: operation: %{public}d", keepScreenOn);
    ErrCode res;
    if (keepScreenOn) {
        res = keepScreenLock_->Lock();
    } else {
        res = keepScreenLock_->UnLock();
    }
    if (res != ERR_OK) {
        WLOGFE("handle keep screen running lock failed: [operation: %{public}d, err: %{public}d]", keepScreenOn, res);
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
    context_ = context;
    sptr<WindowImpl> window(this);
    sptr<IWindow> windowAgent(new WindowAgent(window));
    uint32_t windowId = 0;
    sptr<IRemoteObject> token = nullptr;
    if (context_ != nullptr) {
        token = context_->GetToken();
        if (token != nullptr) {
            property_->SetTokenState(true);
        }
    }
    WMError ret = SingletonContainer::Get<WindowAdapter>().CreateWindow(windowAgent, property_, surfaceNode_,
        windowId, token);
    property_->SetWindowId(windowId);
    property_->SetDecorEnable(WindowHelper::IsMainWindow(property_->GetWindowType()));

    if (ret != WMError::WM_OK) {
        WLOGFE("create window failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return ret;
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

void WindowImpl::DestroyFloatingWindow()
{
    // remove from appFloatingWindowMap_
    for (auto& floatingWindows: appFloatingWindowMap_) {
        for (auto iter = floatingWindows.second.begin(); iter != floatingWindows.second.end(); ++iter) {
            if ((*iter)->GetWindowId() == GetWindowId()) {
                floatingWindows.second.erase(iter);
                break;
            }
        }
    }

    // Destroy app floating window if exist
    if (appFloatingWindowMap_.count(GetWindowId()) > 0) {
        for (auto& floatingWindow : appFloatingWindowMap_.at(GetWindowId())) {
            floatingWindow->Destroy();
        }
        appFloatingWindowMap_.erase(GetWindowId());
    }
}

void WindowImpl::DestroySubWindow()
{
    if (subWindowMap_.count(property_->GetParentId()) > 0) { // remove from subWindowMap_
        std::vector<sptr<WindowImpl>>& subWindows = subWindowMap_.at(property_->GetParentId());
        for (auto iter = subWindows.begin(); iter < subWindows.end(); ++iter) {
            if ((*iter)->GetWindowId() == GetWindowId()) {
                subWindows.erase(iter);
                break;
            }
        }
    }

    if (subWindowMap_.count(GetWindowId()) > 0) { // remove from subWindowMap_ and windowMap_
        std::vector<sptr<WindowImpl>>& subWindows = subWindowMap_.at(GetWindowId());
        for (auto& subWindow : subWindows) {
            subWindow->Destroy(false);
        }
        subWindowMap_[GetWindowId()].clear();
        subWindowMap_.erase(GetWindowId());
    }
}

WMError WindowImpl::Destroy()
{
    return Destroy(true);
}

WMError WindowImpl::Destroy(bool needNotifyServer)
{
    if (!IsWindowValid()) {
        return WMError::WM_OK;
    }

    WLOGFI("[Client] Window %{public}u Destroy", property_->GetWindowId());
    InputTransferStation::GetInstance().RemoveInputWindow(this);
    WMError ret = WMError::WM_OK;
    if (needNotifyServer) {
        NotifyBeforeDestroy(GetWindowName());
        if (subWindowMap_.count(GetWindowId()) > 0) {
            for (auto& subWindow : subWindowMap_.at(GetWindowId())) {
                NotifyBeforeSubWindowDestroy(subWindow);
            }
        }
        ret = SingletonContainer::Get<WindowAdapter>().DestroyWindow(property_->GetWindowId());
        if (ret != WMError::WM_OK) {
            WLOGFE("destroy window failed with errCode:%{public}d", static_cast<int32_t>(ret));
            return ret;
        }
    } else {
        WLOGFI("Do not need to notify server to destroy window");
    }

    windowMap_.erase(GetWindowName());
    DestroySubWindow();
    DestroyFloatingWindow();
    HandleKeepScreenOn(false);
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        state_ = WindowState::STATE_DESTROYED;
        VsyncStation::GetInstance().RemoveCallback(VsyncStation::CallbackType::CALLBACK_FRAME, callback_);
    }
    return ret;
}

WMError WindowImpl::Show(uint32_t reason)
{
    WLOGFI("[Client] Window [name:%{public}s, id:%{public}u] Show", name_.c_str(), property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WindowStateChangeReason stateChangeReason = static_cast<WindowStateChangeReason>(reason);
    if (stateChangeReason == WindowStateChangeReason::KEYGUARD) {
        state_ = WindowState::STATE_SHOWN;
        NotifyAfterForeground();
        return WMError::WM_OK;
    }
    if (state_ == WindowState::STATE_FROZEN) {
        WLOGFE("window is frozen, can not be shown, windowId: %{public}u", property_->GetWindowId());
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        if (property_->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
            WLOGFI("desktop window [id:%{public}u] is shown, minimize all app windows", property_->GetWindowId());
            SingletonContainer::Get<WindowAdapter>().MinimizeAllAppWindows(property_->GetDisplayId());
        } else {
            WLOGFI("window is already shown id: %{public}u, raise to top", property_->GetWindowId());
            SingletonContainer::Get<WindowAdapter>().ProcessPointDown(property_->GetWindowId());
        }
        for (auto& listener : lifecycleListeners_) {
            if (listener != nullptr) {
                listener->AfterForeground();
            }
        }
        return WMError::WM_OK;
    }
    SetDefaultOption();
    WMError ret = SingletonContainer::Get<WindowAdapter>().AddWindow(property_);
    if (ret == WMError::WM_OK || ret == WMError::WM_ERROR_DEATH_RECIPIENT) {
        state_ = WindowState::STATE_SHOWN;
        NotifyAfterForeground();
        HandleKeepScreenOn(keepScreenOn_);
    } else {
        WLOGFE("show errCode:%{public}d for winId:%{public}u", static_cast<int32_t>(ret), property_->GetWindowId());
    }
    return ret;
}

WMError WindowImpl::Hide(uint32_t reason)
{
    WLOGFI("[Client] Window [name:%{public}s, id:%{public}u] Hide", name_.c_str(), property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WindowStateChangeReason stateChangeReason = static_cast<WindowStateChangeReason>(reason);
    if (stateChangeReason == WindowStateChangeReason::KEYGUARD) {
        state_ = WindowState::STATE_FROZEN;
        NotifyAfterBackground();
        return WMError::WM_OK;
    }
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFI("window is already hidden id: %{public}u", property_->GetWindowId());
        return WMError::WM_OK;
    }
    WMError ret = SingletonContainer::Get<WindowAdapter>().RemoveWindow(property_->GetWindowId());
    if (ret != WMError::WM_OK) {
        WLOGFE("hide errCode:%{public}d for winId:%{public}u", static_cast<int32_t>(ret), property_->GetWindowId());
        return ret;
    }
    state_ = WindowState::STATE_HIDDEN;
    NotifyAfterBackground();
    HandleKeepScreenOn(false);
    return ret;
}

WMError WindowImpl::MoveTo(int32_t x, int32_t y)
{
    WLOGFI("[Client] Window [name:%{public}s, id:%{public}d] MoveTo %{public}d %{public}d",
        name_.c_str(), property_->GetWindowId(), x, y);
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    Rect rect = property_->GetRequestRect();
    Rect moveRect = { x, y, rect.width_, rect.height_ }; // must keep w/h, which may maintain stashed resize info
    property_->SetRequestRect(moveRect);
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFI("window is hidden or created! id: %{public}u, oriPos: [%{public}d, %{public}d, "
               "movePos: [%{public}d, %{public}d]", property_->GetWindowId(), rect.posX_, rect.posY_, x, y);
        return WMError::WM_OK;
    }
    property_->SetWindowSizeChangeReason(WindowSizeChangeReason::MOVE);
    return UpdateProperty(PropertyChangeAction::ACTION_UPDATE_RECT);
}

WMError WindowImpl::Resize(uint32_t width, uint32_t height)
{
    WLOGFI("[Client] Window [name:%{public}s, id:%{public}d] Resize %{public}u %{public}u",
        name_.c_str(), property_->GetWindowId(), width, height);
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    Rect rect = property_->GetRequestRect();
    Rect resizeRect = { rect.posX_, rect.posY_, width, height };
    property_->SetRequestRect(resizeRect);
    property_->SetDecoStatus(false);
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFI("window is hidden or created! id: %{public}u, oriRect: [%{public}u, %{public}u], "
               "resizeRect: [%{public}u, %{public}u]", property_->GetWindowId(), rect.width_,
               rect.height_, width, height);
        return WMError::WM_OK;
    }
    property_->SetWindowSizeChangeReason(WindowSizeChangeReason::RESIZE);
    return UpdateProperty(PropertyChangeAction::ACTION_UPDATE_RECT);
}

void WindowImpl::SetKeepScreenOn(bool keepScreenOn)
{
    keepScreenOn_ = keepScreenOn;
    if (state_ == WindowState::STATE_SHOWN) {
        HandleKeepScreenOn(keepScreenOn);
    }
}

bool WindowImpl::IsKeepScreenOn() const
{
    return keepScreenOn_;
}

void WindowImpl::SetTurnScreenOn(bool turnScreenOn)
{
    if (!IsWindowValid()) {
        return;
    }
    property_->SetTurnScreenOn(turnScreenOn);
    if (state_ == WindowState::STATE_SHOWN) {
        UpdateProperty(PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON);
    }
}

bool WindowImpl::IsTurnScreenOn() const
{
    return property_->IsTurnScreenOn();
}

void WindowImpl::SetBackgroundColor(const std::string& color)
{
    if (uiContent_ == nullptr) {
        WLOGFE("invalid operation, uiContent is nullptr, windowId: %{public}u", GetWindowId());
        return;
    }
    uint32_t colorValue;
    if (ColorParser::Parse(color, colorValue)) {
        uiContent_->SetBackgroundColor(colorValue);
        return;
    }
    WLOGFE("invalid color string: %{public}s", color.c_str());
}

void WindowImpl::SetTransparent(bool isTransparent)
{
    if (uiContent_ == nullptr) {
        WLOGFE("invalid operation, uiContent is nullptr, windowId: %{public}u", GetWindowId());
        return;
    }
    ColorParam backgroundColor;
    backgroundColor.value = uiContent_->GetBackgroundColor();
    if (isTransparent) {
        backgroundColor.argb.alpha = 0x00; // 0x00: completely transparent
        uiContent_->SetBackgroundColor(backgroundColor.value);
    } else {
        backgroundColor.value = uiContent_->GetBackgroundColor();
        if (backgroundColor.argb.alpha == 0x00) {
            backgroundColor.argb.alpha = 0xff; // 0xff: completely opaque
            uiContent_->SetBackgroundColor(backgroundColor.value);
        }
    }
}

bool WindowImpl::IsTransparent() const
{
    if (uiContent_ == nullptr) {
        WLOGFE("invalid operation, uiContent is nullptr, windowId: %{public}u", GetWindowId());
        return false;
    }
    ColorParam backgroundColor;
    backgroundColor.value = uiContent_->GetBackgroundColor();
    return backgroundColor.argb.alpha == 0x00; // 0x00: completely transparent
}

void WindowImpl::SetBrightness(float brightness)
{
    if (!IsWindowValid()) {
        return;
    }
    if (brightness < MINIMUM_BRIGHTNESS || brightness > MAXIMUM_BRIGHTNESS) {
        WLOGFE("invalid brightness value: %{public}f", brightness);
        return;
    }
    if (!WindowHelper::IsAppWindow(GetType())) {
        WLOGFE("non app window does not support set brightness, type: %{public}u", GetType());
        return;
    }
    SingletonContainer::Get<WindowAdapter>().SetBrightness(GetWindowId(), brightness);
    brightness_ = brightness;
}

float WindowImpl::GetBrightness() const
{
    return brightness_;
}

void WindowImpl::SetCallingWindow(uint32_t windowId)
{
    if (!IsWindowValid()) {
        return;
    }
    property_->SetCallingWindow(windowId);
    UpdateProperty(PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW);
}

void WindowImpl::SetPrivacyMode(bool isPrivacyMode)
{
    property_->SetPrivacyMode(isPrivacyMode);
    surfaceNode_->SetSecurityLayer(isPrivacyMode);
}

bool WindowImpl::IsPrivacyMode() const
{
    return property_->GetPrivacyMode();
}

WMError WindowImpl::Drag(const Rect& rect)
{
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    Rect requestRect = rect;
    property_->SetRequestRect(requestRect);
    property_->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
    return UpdateProperty(PropertyChangeAction::ACTION_UPDATE_RECT);
}

bool WindowImpl::IsDecorEnable() const
{
    return property_->GetDecorEnable();
}

WMError WindowImpl::Maximize()
{
    WLOGFI("[Client] Window %{public}u Maximize", property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        return SetFullScreen(true);
    } else {
        WLOGFI("Maximize Window failed. The window is not main window");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
}

WMError WindowImpl::Minimize()
{
    WLOGFI("[Client] Window %{public}u Minimize", property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        if (context_ != nullptr) {
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(context_->GetToken(), true);
        } else {
            Hide();
        }
    }
    return WMError::WM_OK;
}

WMError WindowImpl::Recover()
{
    WLOGFI("[Client] Window %{public}u Normalize", property_->GetWindowId());
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
    WLOGFI("[Client] Window %{public}u Close", property_->GetWindowId());
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
        if (abilityContext != nullptr) {
            abilityContext->CloseAbility();
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
}

WMError WindowImpl::RequestFocus() const
{
    if (!IsWindowValid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().RequestFocus(property_->GetWindowId());
}

void WindowImpl::AddInputEventListener(const std::shared_ptr<MMI::IInputEventConsumer>& inputEventListener)
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
    if (listener == nullptr) {
        WLOGFE("RegisterAvoidAreaChangeListener failed. AvoidAreaChangeListener is nullptr");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    avoidAreaChangeListeners_.emplace_back(listener);
}

void WindowImpl::UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    avoidAreaChangeListeners_.erase(std::remove_if(avoidAreaChangeListeners_.begin(), avoidAreaChangeListeners_.end(),
        [listener](sptr<IAvoidAreaChangedListener> registeredListener) {
            return registeredListener == listener;
        }), avoidAreaChangeListeners_.end());
}

void WindowImpl::RegisterDragListener(const sptr<IWindowDragListener>& listener)
{
    if (listener == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    windowDragListeners_.emplace_back(listener);
}

void WindowImpl::UnregisterDragListener(const sptr<IWindowDragListener>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(windowDragListeners_.begin(), windowDragListeners_.end(), listener);
    if (iter == windowDragListeners_.end()) {
        WLOGFE("could not find this listener");
        return;
    }
    windowDragListeners_.erase(iter);
}

void WindowImpl::RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    if (listener == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayMoveListeners_.emplace_back(listener);
}

void WindowImpl::UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(displayMoveListeners_.begin(), displayMoveListeners_.end(), listener);
    if (iter == displayMoveListeners_.end()) {
        WLOGFE("could not find the listener");
        return;
    }
    displayMoveListeners_.erase(iter);
}

void WindowImpl::RegisterInputEventListener(sptr<IInputEventListener>& listener)
{
    if (listener == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    inputEventListeners_.emplace_back(listener);
}

void WindowImpl::UnregisterInputEventListener(sptr<IInputEventListener>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(inputEventListeners_.begin(), inputEventListeners_.end(), listener);
    if (iter == inputEventListeners_.end()) {
        WLOGFE("could not find the listener");
        return;
    }
    inputEventListeners_.erase(iter);
}

void WindowImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    WLOGFI("JS RegisterWindowDestroyedListener the listener");
    notifyNativefunc_ = std::move(func);
}

void WindowImpl::RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE(" listener is nullptr");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    occupiedAreaChangeListeners_.emplace_back(listener);
}

void WindowImpl::UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    occupiedAreaChangeListeners_.erase(std::remove_if(occupiedAreaChangeListeners_.begin(),
        occupiedAreaChangeListeners_.end(), [listener](sptr<IOccupiedAreaChangeListener> registeredListener) {
            return registeredListener == listener;
        }), occupiedAreaChangeListeners_.end());
}

void WindowImpl::UpdateRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason)
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
    property_->SetDecoStatus(decoStatus);
    if (reason == WindowSizeChangeReason::HIDE) {
        property_->SetRequestRect(rect);
        return;
    }
    property_->SetWindowRect(rect);
    WLOGFI("sizeChange callback size: %{public}lu", (unsigned long)windowChangeListeners_.size());
    for (auto& listener : windowChangeListeners_) {
        if (listener != nullptr) {
            listener->OnSizeChange(rect, reason);
        }
    }

    if (uiContent_ != nullptr) {
        Ace::ViewportConfig config;
        config.SetSize(rect.width_, rect.height_);
        config.SetPosition(rect.posX_, rect.posY_);
        config.SetDensity(virtualPixelRatio);
        uiContent_->UpdateViewportConfig(config, reason);
        WLOGFI("notify uiContent window size change end");
    }
}

void WindowImpl::UpdateMode(WindowMode mode)
{
    WLOGI("UpdateMode %{public}u", mode);
    property_->SetWindowMode(mode);
    for (auto& listener : windowChangeListeners_) {
        if (listener != nullptr) {
            listener->OnModeChange(mode);
        }
    }

    if (uiContent_ != nullptr) {
        uiContent_->UpdateWindowMode(mode);
        WLOGFI("notify uiContent window mode change end");
    }
}

void WindowImpl::ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    for (auto& listener : inputEventListeners_) {
        if (listener != nullptr) {
            WLOGI("ConsumeKeyEvent keyEvent is old api consumed");
            listener->OnKeyEvent(keyEvent);
        }
    }
    if (uiContent_ == nullptr) {
        WLOGE("ConsumeKeyEvent uiContent is nullptr");
        return;
    }
    int32_t keyCode = keyEvent->GetKeyCode();
    int32_t keyAction = keyEvent->GetKeyAction();
    WLOGFI("ConsumeKeyEvent: enter GetKeyCode: %{public}d, action: %{public}d", keyCode, keyAction);
    if (keyCode == MMI::KeyEvent::KEYCODE_BACK) {
        if (keyAction != MMI::KeyEvent::KEY_ACTION_UP) {
            return;
        }
        if (uiContent_->ProcessBackPressed()) {
            WLOGI("ConsumeKeyEvent keyEvent is consumed");
            return;
        }
        auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
        if (abilityContext != nullptr) {
            WLOGI("ConsumeKeyEvent ability TerminateSelf");
            abilityContext->TerminateSelf();
        } else {
            WLOGI("ConsumeKeyEvent destroy window");
            Destroy();
        }
    } else {
        if (!uiContent_->ProcessKeyEvent(keyEvent)) {
            WLOGI("ConsumeKeyEvent no consumer window exit");
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

    Rect hotZoneRect;
    if ((startPointPosX_ > startRectExceptCorner_.posX_ &&
        (startPointPosX_ < startRectExceptCorner_.posX_ + static_cast<int32_t>(startRectExceptCorner_.width_))) &&
        (startPointPosY_ > startRectExceptCorner_.posY_ &&
        (startPointPosY_ < startRectExceptCorner_.posY_ + static_cast<int32_t>(startRectExceptCorner_.height_)))) {
        hotZoneRect = startRectExceptFrame_; // drag type: left/right/top/bottom
    } else {
        hotZoneRect = startRectExceptCorner_; // drag type: left_top/right_top/left_bottom/right_bottom
    }

    if (startPointPosX_ <= hotZoneRect.posX_) {
        if (diffX > static_cast<int32_t>(startPointRect_.width_)) {
            diffX = static_cast<int32_t>(startPointRect_.width_);
        }
        newRect.posX_ += diffX;
        newRect.width_ = static_cast<uint32_t>(static_cast<int32_t>(newRect.width_) - diffX);
    } else if (startPointPosX_ >= hotZoneRect.posX_ + static_cast<int32_t>(hotZoneRect.width_)) {
        if (diffX < 0 && (-diffX > static_cast<int32_t>(startPointRect_.width_))) {
            diffX = -(static_cast<int32_t>(startPointRect_.width_));
        }
        newRect.width_ = static_cast<uint32_t>(static_cast<int32_t>(newRect.width_) + diffX);
    }
    if (startPointPosY_ <= hotZoneRect.posY_) {
        if (diffY > static_cast<int32_t>(startPointRect_.height_)) {
            diffY = static_cast<int32_t>(startPointRect_.height_);
        }
        newRect.posY_ += diffY;
        newRect.height_ = static_cast<uint32_t>(static_cast<int32_t>(newRect.height_) - diffY);
    } else if (startPointPosY_ >= hotZoneRect.posY_ + static_cast<int32_t>(hotZoneRect.height_)) {
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

    if (startDragFlag_) {
        SingletonContainer::Get<WindowAdapter>().ProcessPointUp(GetWindowId());
        startDragFlag_ = false;
    }

    if (startMoveFlag_) {
        if (GetType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            SingletonContainer::Get<WindowAdapter>().ProcessPointUp(GetWindowId());
        }
        startMoveFlag_ = false;
    }
    pointEventStarted_ = false;
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

    // calculate window inner rect except frame
    auto display = DisplayManager::GetInstance().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64", window id:%{public}u", property_->GetDisplayId(),
            property_->GetWindowId());
        return;
    }
    float virtualPixelRatio = display->GetVirtualPixelRatio();

    startRectExceptFrame_.posX_ = startPointRect_.posX_ +
        static_cast<int32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    startRectExceptFrame_.posY_ = startPointRect_.posY_ +
        static_cast<int32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    startRectExceptFrame_.width_ = startPointRect_.width_ -
        static_cast<uint32_t>((WINDOW_FRAME_WIDTH + WINDOW_FRAME_WIDTH) * virtualPixelRatio);
    startRectExceptFrame_.height_ = startPointRect_.height_ -
        static_cast<uint32_t>((WINDOW_FRAME_WIDTH + WINDOW_FRAME_WIDTH) * virtualPixelRatio);

    startRectExceptCorner_.posX_ = startPointRect_.posX_ +
        static_cast<int32_t>(WINDOW_FRAME_CORNER_WIDTH * virtualPixelRatio);
    startRectExceptCorner_.posY_ = startPointRect_.posY_ +
        static_cast<int32_t>(WINDOW_FRAME_CORNER_WIDTH * virtualPixelRatio);
    startRectExceptCorner_.width_ = startPointRect_.width_ -
        static_cast<uint32_t>((WINDOW_FRAME_CORNER_WIDTH + WINDOW_FRAME_CORNER_WIDTH) * virtualPixelRatio);
    startRectExceptCorner_.height_ = startPointRect_.height_ -
        static_cast<uint32_t>((WINDOW_FRAME_CORNER_WIDTH + WINDOW_FRAME_CORNER_WIDTH) * virtualPixelRatio);

    if (GetType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        startMoveFlag_ = true;
        SingletonContainer::Get<WindowAdapter>().ProcessPointDown(property_->GetWindowId(), true);
    } else if (!WindowHelper::IsPointInTargetRect(startPointPosX_, startPointPosY_, startRectExceptFrame_) ||
        (WindowHelper::IsPointInTargetRect(startPointPosX_, startPointPosY_, startRectExceptFrame_) &&
        (!WindowHelper::IsPointInWindowExceptCorner(startPointPosX_, startPointPosY_, startRectExceptCorner_)))) {
        startDragFlag_ = true;
        SingletonContainer::Get<WindowAdapter>().ProcessPointDown(property_->GetWindowId(), true);
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
        // Start to move or drag
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
            WLOGFI("[Point Up/Cancel]: windowId: %{public}u, action: %{public}d, startMove: %{public}d, "
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
        WLOGI("WMS process point down, window: [name:%{public}s, id:%{public}u], action: %{public}d",
            name_.c_str(), GetWindowId(), action);
        if (GetType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT) {
            MMI::PointerEvent::PointerItem pointerItem;
            if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
                WLOGFW("Point item is invalid");
                return;
            }
            if (!WindowHelper::IsPointInTargetRect(pointerItem.GetGlobalX(), pointerItem.GetGlobalY(), GetRect())) {
                NotifyListenerAfterUnfocused();
                return;
            }
        }
        SingletonContainer::Get<WindowAdapter>().ProcessPointDown(property_->GetWindowId());
    }

    if (WindowHelper::IsMainFloatingWindow(GetType(), GetMode()) ||
        GetType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        ConsumeMoveOrDragEvent(pointerEvent);
    }

    if (IsPointerEventConsumed()) {
        return;
    }
    for (auto& listener : inputEventListeners_) {
        if (listener != nullptr) {
            WLOGI("ConsumePointEvent pointerEvent is old api consumed");
            listener->OnPointerInputEvent(pointerEvent);
        }
    }
    if (uiContent_ == nullptr) {
        WLOGE("ConsumePointerEvent uiContent is nullptr, windowId: %{public}u", GetWindowId());
        return;
    }
    WLOGFI("Transfer pointer event to ACE");
    uiContent_->ProcessPointerEvent(pointerEvent);
}

void WindowImpl::OnVsync(int64_t timeStamp)
{
    uiContent_->ProcessVsyncEvent(static_cast<uint64_t>(timeStamp));
}

void WindowImpl::RequestFrame()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (state_ == WindowState::STATE_DESTROYED) {
        WLOGFE("RequestFrame failed, window is destroyed");
        return;
    }
    VsyncStation::GetInstance().RequestVsync(VsyncStation::CallbackType::CALLBACK_FRAME, callback_);
}

void WindowImpl::UpdateFocusStatus(bool focused)
{
    WLOGFI("window focus status: %{public}d, id: %{public}u", focused, property_->GetWindowId());
    if (focused) {
        NotifyAfterFocused();
    } else {
        NotifyAfterUnfocused();
    }
}

void WindowImpl::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_ != nullptr) {
        WLOGFD("notify ace winId:%{public}u", GetWindowId());
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
    WLOGFI("Window Update AvoidArea, id: %{public}u", property_->GetWindowId());
    for (auto& listener : avoidAreaChangeListeners_) {
        if (listener != nullptr) {
            listener->OnAvoidAreaChanged(avoidArea);
        }
    }
}

void WindowImpl::UpdateWindowState(WindowState state)
{
    WLOGFI("[Client] Window %{public}u, WindowState to set:%{public}u", GetWindowId(), state);
    if (!IsWindowValid()) {
        return;
    }
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
    switch (state) {
        case WindowState::STATE_FROZEN: {
            if (abilityContext != nullptr && windowTag_ == WindowTag::MAIN_WINDOW) {
                WLOGFD("DoAbilityBackground KEYGUARD, id: %{public}u", GetWindowId());
                AAFwk::AbilityManagerClient::GetInstance()->DoAbilityBackground(abilityContext->GetToken(),
                    static_cast<uint32_t>(WindowStateChangeReason::KEYGUARD));
            } else {
                state_ = WindowState::STATE_FROZEN;
                NotifyAfterBackground();
                HandleKeepScreenOn(false);
            }
            break;
        }
        case WindowState::STATE_UNFROZEN: {
            if (abilityContext != nullptr && windowTag_ == WindowTag::MAIN_WINDOW) {
                WLOGFD("DoAbilityForeground KEYGUARD, id: %{public}u", GetWindowId());
                AAFwk::AbilityManagerClient::GetInstance()->DoAbilityForeground(abilityContext->GetToken(),
                    static_cast<uint32_t>(WindowStateChangeReason::KEYGUARD));
            } else {
                state_ = WindowState::STATE_SHOWN;
                NotifyAfterForeground();
                HandleKeepScreenOn(keepScreenOn_);
            }
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
    std::vector<sptr<IWindowDragListener>> windowDragListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        windowDragListeners = windowDragListeners_;
    }
    for (auto& iter : windowDragListeners) {
        iter->OnDrag(point.x - GetRect().posX_, point.y - GetRect().posY_, event);
    }
}

void WindowImpl::UpdateDisplayId(DisplayId from, DisplayId to)
{
    WLOGFD("update displayId. win %{public}u", GetWindowId());
    for (auto& listener : displayMoveListeners_) {
        if (listener != nullptr) {
            listener->OnDisplayMove(from, to);
        }
    }
    property_->SetDisplayId(to);
}

void WindowImpl::UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info)
{
    WLOGFI("Window Update OccupiedArea, id: %{public}u", property_->GetWindowId());
    for (auto& listener : occupiedAreaChangeListeners_) {
        if (listener != nullptr) {
            listener->OnSizeChange(info);
        }
    }
}

void WindowImpl::UpdateActiveStatus(bool isActive)
{
    WLOGFI("window active status: %{public}d, id: %{public}u", isActive, property_->GetWindowId());
    if (isActive) {
        NotifyAfterActive();
    } else {
        NotifyAfterInactive();
    }
}

Rect WindowImpl::GetSystemAlarmWindowDefaultSize(Rect defaultRect)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64", window id:%{public}u", property_->GetDisplayId(),
            property_->GetWindowId());
        return defaultRect;
    }
    uint32_t width = static_cast<uint32_t>(display->GetWidth());
    uint32_t height = static_cast<uint32_t>(display->GetHeight());
    WLOGFI("width:%{public}u, height:%{public}u, displayId:%{public}" PRIu64"",
        width, height, property_->GetDisplayId());
    Rect rect;
    uint32_t alarmWidth = static_cast<uint32_t>((static_cast<float>(width) *
        SYSTEM_ALARM_WINDOW_WIDTH_RATIO));
    uint32_t alarmHeight = static_cast<uint32_t>((static_cast<float>(height) *
        SYSTEM_ALARM_WINDOW_HEIGHT_RATIO));

    rect = { static_cast<int32_t>((width - alarmWidth) / 2), static_cast<int32_t>((height - alarmHeight) / 2),
                alarmWidth, alarmHeight }; // devided by 2 to middle the window
    return rect;
}

void WindowImpl::SetDefaultOption()
{
    switch (property_->GetWindowType()) {
        case WindowType::WINDOW_TYPE_STATUS_BAR:
        case WindowType::WINDOW_TYPE_NAVIGATION_BAR: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            property_->SetFocusable(false);
            break;
        }
        case WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW: {
            property_->SetRequestRect(GetSystemAlarmWindowDefaultSize(property_->GetRequestRect()));
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
        case WindowType::WINDOW_TYPE_TOAST:
        case WindowType::WINDOW_TYPE_FLOAT:
        case WindowType::WINDOW_TYPE_LAUNCHER_DOCK:
        case WindowType::WINDOW_TYPE_SEARCHING_BAR: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_VOLUME_OVERLAY: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            property_->SetFocusable(false);
            break;
        }
        case WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            property_->SetFocusable(false);
            break;
        }
        case WindowType::WINDOW_TYPE_BOOT_ANIMATION:
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
    return (mode == WindowMode::WINDOW_MODE_FULLSCREEN && !needAvoid);
}

bool WindowImpl::IsFullScreen() const
{
    auto statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto naviProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    return (IsLayoutFullScreen() && !statusProperty.enable_ && !naviProperty.enable_);
}

void WindowImpl::SetRequestedOrientation(Orientation orientation)
{
    if (property_->GetRequestedOrientation() == orientation) {
        return;
    }
    property_->SetRequestedOrientation(orientation);
    UpdateProperty(PropertyChangeAction::ACTION_UPDATE_ORIENTATION);
}

Orientation WindowImpl::GetRequestedOrientation()
{
    return property_->GetRequestedOrientation();
}
} // namespace Rosen
} // namespace OHOS

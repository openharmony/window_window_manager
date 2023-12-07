/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "window_manager_hilog.h"
#include "window_helper.h"
#include "window_option.h"
#include "viewport_config.h"
#include "singleton_container.h"
#include "vsync_station.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowImpl"};
}
std::map<std::string, std::pair<uint32_t, sptr<Window>>> WindowImpl::windowMap_;
std::map<uint32_t, std::vector<sptr<WindowImpl>>> WindowImpl::subWindowMap_;
static int constructorCnt = 0;
static int deConstructorCnt = 0;
WindowImpl::WindowImpl(const sptr<WindowOption>& option)
{
    if (option != nullptr) {
        name_ = option->GetWindowName();
    } else {
        name_ = "main_window";
    }
    WLOGFI("WindowImpl constructorCnt: %{public}d",
        ++constructorCnt);
}

WindowImpl::~WindowImpl()
{
    WLOGFI("windowName: %{public}s, windowId: %{public}d, deConstructorCnt: %{public}d",
        GetWindowName().c_str(), GetWindowId(), ++deConstructorCnt);
    Destroy();
}

void WindowImpl::CreateSurfaceNode(const std::string name, const SendRenderDataCallback& callback)
{
    WLOGFI("CreateSurfaceNode");
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = name;
    rsSurfaceNodeConfig.additionalData = reinterpret_cast<void*>(callback);
    surfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
}

void WindowImpl::SetContentInfoCallback(const ContentInfoCallback& callback)
{
    contentInfoCallback_ = callback;
}

sptr<Window> WindowImpl::Find(const std::string& name)
{
    return nullptr;
}

const std::shared_ptr<AbilityRuntime::Context> WindowImpl::GetContext() const
{
    return nullptr;
}

sptr<Window> WindowImpl::GetTopWindowWithId(uint32_t mainWinId)
{
    return nullptr;
}

sptr<Window> WindowImpl::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    return nullptr;
}

std::vector<sptr<Window>> WindowImpl::GetSubWindow(uint32_t parentId)
{
    return std::vector<sptr<Window>>();
}

void WindowImpl::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    for (const auto& winPair : windowMap_) {
        auto window = winPair.second.second;
        window->UpdateConfiguration(configuration);
    }
}

std::shared_ptr<RSSurfaceNode> WindowImpl::GetSurfaceNode() const
{
    return surfaceNode_;
}

Rect WindowImpl::GetRect() const
{
    return Rect{0, 0, 0, 0};
}

Rect WindowImpl::GetRequestRect() const
{
    return Rect{0, 0, 0, 0};
}

WindowType WindowImpl::GetType() const
{
    return WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
}

WindowMode WindowImpl::GetMode() const
{
    return WindowMode::WINDOW_MODE_UNDEFINED;
}

float WindowImpl::GetAlpha() const
{
    return 0.0;
}

WindowState WindowImpl::GetWindowState() const
{
    return state_;
}

WMError WindowImpl::SetFocusable(bool isFocusable)
{
    return WMError::WM_OK;
}

bool WindowImpl::GetFocusable() const
{
    return false;
}

WMError WindowImpl::SetTouchable(bool isTouchable)
{
    return WMError::WM_OK;
}

bool WindowImpl::GetTouchable() const
{
    return false;
}

const std::string& WindowImpl::GetWindowName() const
{
    return name_;
}

uint32_t WindowImpl::GetWindowId() const
{
    return 0;
}

uint32_t WindowImpl::GetWindowFlags() const
{
    return 0;
}

uint32_t WindowImpl::GetRequestModeSupportInfo() const
{
    return 0;
}

bool WindowImpl::IsMainHandlerAvailable() const
{
    return true;
}

SystemBarProperty WindowImpl::GetSystemBarPropertyByType(WindowType type) const
{
    return SystemBarProperty();
}

WMError WindowImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowType(WindowType type)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowMode(WindowMode mode)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetAlpha(float alpha)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetTransform(const Transform& trans)
{
    return WMError::WM_OK;
}

const Transform& WindowImpl::GetTransform() const
{
    return transform_;
}

WMError WindowImpl::AddWindowFlag(WindowFlag flag)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RemoveWindowFlag(WindowFlag flag)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowFlags(uint32_t flags)
{
    return WMError::WM_OK;
}

void WindowImpl::OnNewWant(const AAFwk::Want& want)
{
    return;
}

WMError WindowImpl::NapiSetUIContent(const std::string& contentInfo,
    napi_env env, napi_value storage, bool isdistributed, sptr<IRemoteObject> token, AppExecFwk::Ability* ability)
{
    WLOGFD("NapiSetUIContent: %{public}s", contentInfo.c_str());
    if (uiContent_) {
        uiContent_->Destroy();
    }
    std::unique_ptr<Ace::UIContent> uiContent;
    if (ability != nullptr) {
        uiContent = Ace::UIContent::Create(ability);
    } else {
        uiContent = Ace::UIContent::Create(context_.get(), reinterpret_cast<NativeEngine*>(env));
    }
    if (uiContent == nullptr) {
        WLOGFE("fail to NapiSetUIContent");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (isdistributed) {
        uiContent->Restore(this, contentInfo, storage);
    } else {
        uiContent->Initialize(this, contentInfo, storage);
    }
    uiContent_ = std::move(uiContent);
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is NULL");
        return WMError::WM_ERROR_NULLPTR;
    }
    UpdateViewportConfig();
    if (contentInfoCallback_) {
        contentInfoCallback_(contentInfo);
    }
    return WMError::WM_OK;
}


Ace::UIContent* WindowImpl::GetUIContent() const
{
    return uiContent_.get();
}

std::string WindowImpl::GetContentInfo()
{
    return "";
}

bool WindowImpl::IsSupportWideGamut()
{
    return true;
}

void WindowImpl::SetColorSpace(ColorSpace colorSpace)
{
    return;
}

ColorSpace WindowImpl::GetColorSpace()
{
    return ColorSpace::COLOR_SPACE_DEFAULT;
}

std::shared_ptr<Media::PixelMap> WindowImpl::Snapshot()
{
    return nullptr;
}

void WindowImpl::DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info)
{
    return;
}

WMError WindowImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetSpecificBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetLayoutFullScreen(bool status)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetFullScreen(bool status)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Create(uint32_t parentId, const std::shared_ptr<AbilityRuntime::Context>& context)
{
    WLOGFI("[Client] Window [name:%{public}s] Create", name_.c_str());
    context_ = context;
    return WMError::WM_OK;
}

WMError WindowImpl::BindDialogTarget(sptr<IRemoteObject> targetToken)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Destroy()
{
    if (uiContent_) {
        uiContent_->Destroy();
    }
    return WMError::WM_OK;
}

WMError WindowImpl::UpdateSurfaceNodeAfterCustomAnimation(bool isAdd)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Show(uint32_t reason, bool withAnimation)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    return WMError::WM_OK;
}

WMError WindowImpl::MoveTo(int32_t x, int32_t y)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Resize(uint32_t width, uint32_t height)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowGravity(WindowGravity gravity, uint32_t percent)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetKeepScreenOn(bool keepScreenOn)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsKeepScreenOn() const
{
    return false;
}

WMError WindowImpl::SetTurnScreenOn(bool turnScreenOn)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsTurnScreenOn() const
{
    return false;
}

WMError WindowImpl::SetBackgroundColor(const std::string& color)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetTransparent(bool isTransparent)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsTransparent() const
{
    return true;
}

WMError WindowImpl::SetBrightness(float brightness)
{
    return WMError::WM_OK;
}

float WindowImpl::GetBrightness() const
{
    return 0.0;
}

WMError WindowImpl::SetCallingWindow(uint32_t windowId)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetPrivacyMode(bool isPrivacyMode)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsPrivacyMode() const
{
    return false;
}

void WindowImpl::SetSystemPrivacyMode(bool isSystemPrivacyMode)
{
}

WMError WindowImpl::SetSnapshotSkip(bool isSkip)
{
    return WMError::WM_OK;
}

WMError WindowImpl::DisableAppWindowDecor()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Maximize()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Minimize()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Recover()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Close()
{
    return WMError::WM_OK;
}

void WindowImpl::StartMove()
{
    return;
}

WMError WindowImpl::RequestFocus() const
{
    return WMError::WM_OK;
}

bool WindowImpl::IsFocused() const
{
    return true;
}

void WindowImpl::SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer)
{
    return;
}

WMError WindowImpl::RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterDragListener(const sptr<IWindowDragListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterDragListener(const sptr<IWindowDragListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    return WMError::WM_OK;
}

void WindowImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    return;
}

WMError WindowImpl::RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    return WMError::WM_OK;
}

void WindowImpl::RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    return;
}

void WindowImpl::UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    return;
}

void WindowImpl::SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler)
{
    return;
}

void WindowImpl::SetRequestModeSupportInfo(uint32_t modeSupportInfo)
{
    return;
}

void WindowImpl::ConsumeKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (uiContent_ == nullptr) {
        WLOGFE("ConsumeKeyEvent to uiContent failed,uiContent_ is null");
        return;
    }
    uiContent_->ProcessKeyEvent(keyEvent);
}

void WindowImpl::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (uiContent_ == nullptr) {
        WLOGFE("ConsumePointerEvent to uiContent failed,uiContent_ is null");
        return;
    }
    (void)uiContent_->ProcessPointerEvent(pointerEvent);
}


void WindowImpl::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    VsyncStation::GetInstance().RequestVsync(vsyncCallback);
}

int64_t WindowImpl::GetVSyncPeriod()
{
    return 0;
}

void WindowImpl::FlushFrameRate(uint32_t rate)
{
    return;
}

void WindowImpl::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_ != nullptr) {
        WLOGFD("notify ace winId:%{public}u", GetWindowId());
        uiContent_->UpdateConfiguration(configuration);
    }
}

void WindowImpl::NotifyTouchDialogTarget()
{
    return;
}

void WindowImpl::SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel)
{
    needRemoveWindowInputChannel_ = needRemoveWindowInputChannel;
}

bool WindowImpl::IsLayoutFullScreen() const
{
    return true;
}

bool WindowImpl::IsFullScreen() const
{
    return true;
}

void WindowImpl::SetRequestedOrientation(Orientation orientation)
{
}

Orientation WindowImpl::GetRequestedOrientation()
{
    return Orientation::UNSPECIFIED;
}

WMError WindowImpl::SetTouchHotAreas(const std::vector<Rect>& rects)
{
    return WMError::WM_OK;
}
void WindowImpl::GetRequestedTouchHotAreas(std::vector<Rect>& rects) const
{
}

WMError WindowImpl::SetAPPWindowLabel(const std::string& label)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowTitle(label);
    return WMError::WM_OK;
}

WMError WindowImpl::SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    if (icon == nullptr) {
        WLOGFE("window icon is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowIcon(icon);
    return WMError::WM_OK;
}

WMError WindowImpl::SetCornerRadius(float cornerRadius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowRadius(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowColor(std::string color)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowOffsetX(float offsetX)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowOffsetY(float offsetY)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBlur(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBackdropBlur(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBackdropBlurStyle(WindowBlurStyle blurStyle)
{
    return WMError::WM_OK;
}

WMError WindowImpl::NotifyMemoryLevel(int32_t level)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsAllowHaveSystemSubWindow()
{
    return true;
}

WmErrorCode WindowImpl::RaiseToAppTop()
{
    return WmErrorCode::WM_OK;
}

WMError WindowImpl::SetAspectRatio(float ratio)
{
    return WMError::WM_OK;
}

WMError WindowImpl::ResetAspectRatio()
{
    return WMError::WM_OK;
}

KeyboardAnimationConfig WindowImpl::GetKeyboardAnimationConfig()
{
    return keyboardAnimationConfig_;
}

void WindowImpl::SetNeedDefaultAnimation(bool needDefaultAnimation)
{
    return;
}

void WindowImpl::SetViewportConfig(const Ace::ViewportConfig& config)
{
    bool isUpdate = false;
    if (width_ != config.Width()) {
        width_ = config.Width();
        isUpdate = true;
    }
    if (height_ != config.Height()) {
        height_ = config.Height();
        isUpdate = true;
    }
    if (abs(density_ - config.Density()) >= 1e-6) {
        density_ = config.Density();
        isUpdate = true;
    }
    if (orientation_ != config.Orientation()) {
        orientation_ = config.Orientation();
        isUpdate = true;
    }
    if (isUpdate) {
        UpdateViewportConfig();
    }
}

void WindowImpl::UpdateViewportConfig()
{
    Ace::ViewportConfig config;
    config.SetSize(width_, height_);
    config.SetPosition(0, 0);
    config.SetDensity(density_);
    config.SetOrientation(orientation_);
    if (uiContent_ != nullptr) {
        uiContent_->UpdateViewportConfig(config, WindowSizeChangeReason::UNDEFINED);
    }
}

void WindowImpl::SetOrientation(Orientation orientation)
{
    WLOGFD("SetOrientation : orientation=%{public}d", static_cast<int32_t>(orientation));
    if (orientation_ == static_cast<int32_t>(orientation)) {
        return;
    }
    orientation_ = static_cast<int32_t>(orientation);
    UpdateViewportConfig();
}

void WindowImpl::SetSize(int32_t width, int32_t height)
{
    WLOGFD("SetSize : width=%{public}d, height=%{public}d", width, height);
    if (width_ == width && height_ == height) {
        return;
    }
    width_ = width;
    height_ = height;
    UpdateViewportConfig();
}

void WindowImpl::SetDensity(float density)
{
    WLOGFD("SetDensity : density=%{public}f", density);
    if (abs(density_ - density) <= 0.000001) { // 0.000001: near zero.
        return;
    }
    density_ = density;
    UpdateViewportConfig();
}

WMError WindowImpl::SetResizeByDragEnabled(bool dragEnabled)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetRaiseByClickEnabled(bool raiseEnabled)
{
    return WMError::WM_OK;
}

WmErrorCode WindowImpl::RaiseAboveTarget(int32_t subWindowId)
{
    return WmErrorCode::WM_OK;
}

WMError WindowImpl::HideNonSystemFloatingWindows(bool shouldHide)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterWindowVisibilityChangeListener(const WindowVisibilityListenerSptr& listener)
{
    return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
}

WMError WindowImpl::UnregisterWindowVisibilityChangeListener(const WindowVisibilityListenerSptr& listener)
{
    return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
}
} // namespace Rosen
} // namespace OHOS

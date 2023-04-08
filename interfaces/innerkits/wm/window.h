/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_H
#define OHOS_ROSEN_WINDOW_H

#include <refbase.h>
#include <parcel.h>
#include <pixel_map.h>
#include <iremote_object.h>

#include "wm_common.h"
#include "window_option.h"

class NativeValue;
class NativeEngine;
namespace OHOS::MMI {
    class PointerEvent;
    class KeyEvent;
    class AxisEvent;
}
namespace OHOS::AppExecFwk {
    class Configuration;
    class Ability;
}

namespace OHOS::AbilityRuntime {
    class AbilityContext;
    class Context;
}

namespace OHOS::AAFwk {
    class Want;
}

namespace OHOS::Ace {
    class UIContent;
}

namespace OHOS {
namespace Rosen {
using NotifyNativeWinDestroyFunc = std::function<void(std::string windowName)>;
class RSSurfaceNode;
class RSTransaction;

/**
 * @class IWindowLifeCycle
 *
 * @brief IWindowLifeCycle is a listener used to notify caller that lifecycle of window.
 */
class IWindowLifeCycle : virtual public RefBase {
public:
    /**
     * @brief Notify caller that window is on the forground.
     */
    virtual void AfterForeground() {}
    /**
     * @brief Notify caller that window is on the background.
     */
    virtual void AfterBackground() {}
    /**
     * @brief Notify caller that window is focused.
     */
    virtual void AfterFocused() {}
    /**
     * @brief Notify caller that window is unfocused.
     */
    virtual void AfterUnfocused() {}
    /**
     * @brief Notify caller the error code when window go forground failed.
     *
     * @param ret error code when window go forground failed.
     */
    virtual void ForegroundFailed(int32_t ret) {}
    /**
     * @brief Notify caller that window is active.
     */
    virtual void AfterActive() {}
    /**
     * @brief Notify caller that window is inactive.
     */
    virtual void AfterInactive() {}
};

/**
 * @class IWindowChangeListener
 *
 * @brief IWindowChangeListener is used to observe the window size or window mode when window changed.
 */
class IWindowChangeListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when window size changed.
     *
     * @param Rect Rect of the current window.
     * @param reason Reason for window change.
     * @param rsTransaction Synchronization transaction for animation
     */
    virtual void OnSizeChange(Rect rect, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) {}
        const std::shared_ptr<RSTransaction> rsTransaction = nullptr) {}
    /**
     * @brief Notify caller when window mode changed.
     *
     * @param mode mode of the current window.
     * @param hasDeco window has decoration or not.
     */
    virtual void OnModeChange(WindowMode mode, bool hasDeco = true) {}
};

/**
 * @class IAvoidAreaChangedListener
 *
 * @brief IAvoidAreaChangedListener is used to observe the avoid area when avoid area size changed.
 */
class IAvoidAreaChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when avoid area size changed.
     *
     * @param avoidArea Area needed to be avoided.
     * @param type Type of avoid area.
     */
    virtual void OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type) {}
};

/**
 * @class IWindowDragListener
 *
 * @brief IWindowDragListener is used to observe the drag status when drag window.
 */
class IWindowDragListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when drag window.
     *
     * @param x X-axis when drag window.
     * @param y Y-axis when drag window.
     * @param event Drag type.
     */
    virtual void OnDrag(int32_t x, int32_t y, DragEvent event) {}
};

/**
 * @class IDisplayMoveListener
 *
 * @brief IDisplayMoveListener is used to observe display move status when display move.
 */
class IDisplayMoveListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when display move.
     *
     * @param from display id before display start move.
     * @param to display id after display move end.
     */
    virtual void OnDisplayMove(DisplayId from, DisplayId to) {}
};

/**
 * @class IDispatchInputEventListener
 *
 * @brief IDispatchInputEventListener is used to dispatch input event.
 */
class IDispatchInputEventListener : virtual public RefBase {
public:
    /**
     * @brief Dispatch PointerEvent.
     *
     * @param inputEvent Means PointerEvent.
     */
    virtual void OnDispatchPointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) = 0;
    /**
     * @brief Dispatch KeyEvent.
     *
     * @param inputEvent Means KeyEvent.
     */
    virtual void OnDispatchKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
};

/**
 * @class OccupiedAreaChangeInfo
 *
 * @brief Occupied area info when it changed.
 */
class OccupiedAreaChangeInfo : public Parcelable {
public:
    /**
     * @brief Default construct func of OccupiedAreaChangeInfo.
     */
    OccupiedAreaChangeInfo() = default;
    /**
     * @brief Construct func of OccupiedAreaChangeInfo.
     *
     * @param OccupiedAreaType
     * @param rect rect of Occupied area
     */
    OccupiedAreaChangeInfo(OccupiedAreaType type, Rect rect) : type_(type), rect_(rect) {};
    /**
     * @brief Deconstruct func of OccupiedAreaChangeInfo.
     */
    OccupiedAreaChangeInfo(OccupiedAreaType type, Rect rect, uint32_t safeHeight)
        : type_(type), rect_(rect), safeHeight_(safeHeight) {};
    ~OccupiedAreaChangeInfo() = default;

    /**
     * @brief Marshalling the data of OccupiedAreaChangeInfo.
     *
     * @param parcel Data of OccupiedAreaChangeInfo.
     */
    virtual bool Marshalling(Parcel& parcel) const override;
    /**
     * @brief Unmarshalling the data of OccupiedAreaChangeInfo.
     *
     * @param parcel Data of OccupiedAreaChangeInfo.
     */
    static OccupiedAreaChangeInfo* Unmarshalling(Parcel& parcel);

    OccupiedAreaType type_ = OccupiedAreaType::TYPE_INPUT;
    Rect rect_ = { 0, 0, 0, 0 };
    uint32_t safeHeight_ = 0;
};

/**
 * @class IOccupiedAreaChangeListener
 *
 * @brief IOccupiedAreaChangeListener is used to observe OccupiedArea change.
 */
class IOccupiedAreaChangeListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when OccupiedArea size change.
     *
     * @param info Occupied area info when occupied changed.
     */
    virtual void OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) {}
};

/**
 * @class IAceAbilityHandler
 *
 * @brief IAceAbilityHandler is used to control Ace Ability.
 */
class IAceAbilityHandler : virtual public RefBase {
public:
    /**
     * @brief Set BackgroundColor
     *
     * @param color Color of Background.
     */
    virtual void SetBackgroundColor(uint32_t color) = 0;
    /**
     * @brief Get BackgroundColor.
     *
     * @return Value of BackgroundColor.
     */
    virtual uint32_t GetBackgroundColor() = 0;
};

/**
 * @class IInputEventConsumer
 *
 * @brief IInputEventConsumer is a Listener to observe InputEvent consumed or not.
 */
class IInputEventConsumer {
public:
    /**
     * @brief Default construct func of IInputEventConsumer.
     */
    IInputEventConsumer() = default;
    /**
     * @brief Default Destructor func of IInputEventConsumer.
     */
    virtual ~IInputEventConsumer() = default;
    /**
     * @brief Observe KeyEvent of Multi-Model Input.
     *
     * @param keyEvent KeyEvent of Multi-Model Input.
     */
    virtual bool OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const = 0;
    /**
     * @brief Observe PointerEvent of Multi-Model Input.
     *
     * @param pointerEvent PointerEvent of Multi-Model Input.
     */
    virtual bool OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const = 0;
    /**
     * @brief Observe axisEvent of Multi-Model Input.
     *
     * @param axisEvent axisEvent of Multi-Model Input.
     */
    virtual bool OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const = 0;
};

/**
 * @class ITouchOutsideListener
 *
 * @brief ITouchOutsideListener is a Listener to observe event when touch outside the window.
 */
class ITouchOutsideListener : virtual public RefBase {
public:
    /**
     * @brief Observe the event when touch outside the window.
     */
    virtual void OnTouchOutside() const {}
};

/**
 * @class IAnimationTransitionController
 *
 * @brief IAnimationTransitionController is a Listener to observe event about animation.
 */
class IAnimationTransitionController : virtual public RefBase {
public:
    /**
     * @brief Observe the event when animation show.
     */
    virtual void AnimationForShown() = 0;
    /**
     * @brief Observe the event when animation hide.
     */
    virtual void AnimationForHidden() = 0;
};

/**
 * @class IScreenshotListener
 *
 * @brief IScreenshotListener is a Listener to observe event when screenshot happened.
 */
class IScreenshotListener : virtual public RefBase {
public:
    /**
     * @brief Observe event when screenshot happened.
     */
    virtual void OnScreenshot() {}
};

/**
 * @class IDialogTargetTouchListener
 *
 * @brief IDialogTargetTouchListener is a Listener to observe event when touch dialog window.
 */
class IDialogTargetTouchListener : virtual public RefBase {
public:
    /**
     * @brief Observe event when touch dialog window.
     */
    virtual void OnDialogTargetTouch() const {}
};

/**
 * @class IDialogDeathRecipientListener
 *
 * @brief IDialogDeathRecipientListener is a Listener to observe event when mainwindow(bind to dialog) destroyed.
 */
class IDialogDeathRecipientListener : virtual public RefBase {
public:
    /**
     * @brief Observe event when mainwindow(bind to dialog) destroyed.
     */
    virtual void OnDialogDeathRecipient() const = 0;
};

static WMError DefaultCreateErrCode = WMError::WM_OK;
class Window : public RefBase {
public:
    /**
     * @brief create window, include main_window/sub_window/system_window
     *
     * @param windowName window name, identify window instance
     * @param option window propertion
     * @param context ability context
     * @return sptr<Window> If create window success,return window instance;Otherwise, return nullptr
     */
    static sptr<Window> Create(const std::string& windowName,
        sptr<WindowOption>& option, const std::shared_ptr<AbilityRuntime::Context>& context = nullptr,
        WMError& errCode = DefaultCreateErrCode);
    /**
     * @brief find window by windowName
     *
     * @param windowName
     * @return sptr<Window> Return the window instance founded
     */
    static sptr<Window> Find(const std::string& windowName);
    /**
     * @brief Get the final show window by context. Its implemented in api8
     *
     * @param context Indicates the context on which the window depends
     * @return sptr<Window>
     */
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    /**
     * @brief Get the final show window by id. Its implemented in api8
     *
     * @param mainWinId main window id?
     * @return sptr<Window>
     */
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    /**
     * @brief Get the all sub windows by parent
     *
     * @param parentId parent window id
     * @return std::vector<sptr<Window>>
     */
    static std::vector<sptr<Window>> GetSubWindow(uint32_t parentId);

    /**
     * @brief Update configuration for all windows
     *
     * @param configuration configuration for app
     */
    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    /**
     * @brief Get surface node from RS
     *
     * @return Surface node from RS
     */
    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const = 0;
    /**
     * @brief Get ability context
     *
     * @return ability context from AbilityRuntime
     */
    virtual const std::shared_ptr<AbilityRuntime::Context> GetContext() const = 0;
    /**
     * @brief Get the window show rect
     *
     * @return Rect of window
     */
    virtual Rect GetRect() const = 0;
    /**
     * @brief Get window default rect from window property.
     *
     * @return Rect of window.
     */
    virtual Rect GetRequestRect() const = 0;
    /**
     * @brief Get the window type
     *
     * @return Type of window
     */
    virtual WindowType GetType() const = 0;
    /**
     * @brief Get the window mode.
     *
     * @return Mode of window.
     */
    virtual WindowMode GetMode() const = 0;
    /**
     * @brief Get alpha of window.
     *
     * @return Alpha of window.
     */
    virtual float GetAlpha() const = 0;
    /**
     * @brief Get the name of window.
     *
     * @return Name of window.
     */
    virtual const std::string& GetWindowName() const = 0;
    /**
     * @brief Get id of window.
     *
     * @return Id of window.
     */
    virtual uint32_t GetWindowId() const = 0;
    /**
     * @brief Get flag of window.
     *
     * @return flag of window.
     */
    virtual uint32_t GetWindowFlags() const = 0;
    /**
     * @brief Get state of window.
     *
     * @return Current state of window.
     */
    virtual WindowState GetWindowState() const = 0;
    /**
     * @brief Set focusable property of window.
     *
     * @param isFocusable Window can be focused or not.
     * @return Errorcode of window.
     */
    virtual WMError SetFocusable(bool isFocusable) = 0;
    /**
     * @brief Get focusable property of window.
     *
     * @return True means window can be focused, false means window cannot be focused.
     */
    virtual bool GetFocusable() const = 0;
    /**
     * @brief Set touchable property of window.
     *
     * @param isTouchable Window can be touched or not.
     * @return Errorcode of window.
     */
    virtual WMError SetTouchable(bool isTouchable) = 0;
    /**
     * @brief Get touchable property of window.
     *
     * @return True means window can be touched, false means window cannot be touched.
     */
    virtual bool GetTouchable() const = 0;
    /**
     * @brief Get SystemBarProperty By WindowType.
     *
     * @param type Type of window.
     * @return Property of system bar.
     */
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) const = 0;
    /**
     * @brief judge this window is full screen.
     *
     * @return true If SetFullScreen(true) is called , return true.
     * @return false default return false
     */
    virtual bool IsFullScreen() const = 0;
    /**
     * @brief judge window layout is full screen
     *
     * @return true this window layout is full screen
     * @return false this window layout is not full screen
     */
    virtual bool IsLayoutFullScreen() const = 0;
    /**
     * @brief Set the Window Type
     *
     * @param type window type
     * @return WMError
     */
    virtual WMError SetWindowType(WindowType type) = 0;
    /**
     * @brief Set the Window Mode
     *
     * @param mode window mode
     * @return WMError
     */
    virtual WMError SetWindowMode(WindowMode mode) = 0;
    /**
     * @brief Set alpha of window.
     *
     * @param alpha Alpha of window.
     * @return WM_OK means success, others means set failed.
     */
    virtual WMError SetAlpha(float alpha) = 0;
    /**
     * @brief Set transform of window property.
     *
     * @param trans Window Transform.
     * @return WMError
     */
    virtual WMError SetTransform(const Transform& trans) = 0;
    /**
     * @brief Get transform of window property.
     *
     * @return Property of transform.
     */
    virtual const Transform& GetTransform() const = 0;
    /**
     * @brief Add window flag.
     *
     * @param flag flag of window.
     * @return WM_OK means add success, others means failed.
     */
    virtual WMError AddWindowFlag(WindowFlag flag) = 0;
    /**
     * @brief Remove window flag.
     *
     * @param flag Flag of window
     * @return WM_OK means remove success, others means failed.
     */
    virtual WMError RemoveWindowFlag(WindowFlag flag) = 0;
    /**
     * @brief Set window flag.
     *
     * @param flags Flag of window
     * @return WM_OK means set success, others means failed.
     */
    virtual WMError SetWindowFlags(uint32_t flags) = 0;
    /**
     * @brief Set the System Bar(include status bar and nav bar) Property
     *
     * @param type WINDOW_TYPE_STATUS_BAR or WINDOW_TYPE_NAVIGATION_BAR
     * @param property system bar prop,include content color, background color
     * @return WMError
     */
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) = 0;
    /**
     * @brief Get the Avoid Area By Type object
     *
     * @param type avoid area type.@see reference
     * @param avoidArea
     * @return WMError
     */
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) = 0;
    /**
     * @brief Set this window layout full screen, with hide status bar and nav bar above on this window
     *
     * @param status
     * @return WMError
     */
    virtual WMError SetLayoutFullScreen(bool status) = 0;
    /**
     * @brief Set this window full screen, with hide status bar and nav bar
     *
     * @param status if true, hide status bar and nav bar; Otherwise, show status bar and nav bar
     * @return WMError
     */
    virtual WMError SetFullScreen(bool status) = 0;
    /**
     * @brief destroy window
     *
     * @return WMError
     */
    virtual WMError Destroy() = 0;
    /**
     * @brief Show window
     *
     * @param reason Reason for window state change.
     * @param withAnimation true means window show with animation, false means window show without animation.
     * @return WM_OK means window show success, others means failed.
     */
    virtual WMError Show(uint32_t reason = 0, bool withAnimation = false) = 0;
    /**
     * @brief Hide window
     *
     * @param reason Reason for window state change.
     * @param withAnimation true means window show with animation, false means window show without animation.
     * @param isFromInnerkits true means remove command is from inner kits.
     * @return WM_OK means window hide success, others means failed.
     */
    virtual WMError Hide(uint32_t reason = 0, bool withAnimation = false, bool isFromInnerkits = true) = 0;
    /**
     * @brief move the window to (x, y)
     *
     * @param x
     * @param y
     * @return WMError
     */
    virtual WMError MoveTo(int32_t x, int32_t y) = 0;
    /**
     * @brief resize the window instance (w,h)
     *
     * @param width
     * @param height
     * @return WMError
     */
    virtual WMError Resize(uint32_t width, uint32_t height) = 0;
    /**
     * @brief set the window gravity
     *
     * @param gravity
     * @param percent
     * @return WMError
     */
    virtual WMError SetWindowGravity(WindowGravity gravity, uint32_t percent) = 0;
    /**
     * @brief Set the screen always on
     *
     * @param keepScreenOn
     * @return WMError
     */
    virtual WMError SetKeepScreenOn(bool keepScreenOn) = 0;
    /**
     * @brief Get the screen is always on or not.
     *
     * @return true means screen is always on, false means the opposite.
     */
    virtual bool IsKeepScreenOn() const = 0;
    /**
     * @brief Set the screen on
     *
     * @param turnScreenOn true means turn screen on, false means the opposite.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetTurnScreenOn(bool turnScreenOn) = 0;
    /**
     * @brief Get the screen is on or not.
     *
     * @return true means screen is on, false means screen is off.
     */
    virtual bool IsTurnScreenOn() const = 0;
    /**
     * @brief Set Background color.
     *
     * @param color background color.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBackgroundColor(const std::string& color) = 0;
    /**
     * @brief Set transparent status.
     *
     * @param isTransparent true means set window transparent, false means the opposite.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetTransparent(bool isTransparent) = 0;
    /**
     * @brief Get transparent status.
     *
     * @return true means window is transparent, false means the opposite.
     */
    virtual bool IsTransparent() const = 0;
    /**
     * @brief Set brightness value of window.
     *
     * @param brightness brightness of window.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBrightness(float brightness) = 0;
    /**
     * @brief Get brightness value of window.
     *
     * @return brightness value of window.
     */
    virtual float GetBrightness() const = 0;
    /**
     * @brief Set calling window.
     *
     * @param windowId window id.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetCallingWindow(uint32_t windowId) = 0;
    /**
     * @brief Set privacy mode of window.
     *
     * @param isPrivacyMode true means set window private, false means not set window private.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetPrivacyMode(bool isPrivacyMode) = 0;
    /**
     * @brief Get privacy property of window.
     *
     * @return true means window is private and cannot be screenshot or recorded.
     */
    virtual bool IsPrivacyMode() const = 0;
    /**
     * @brief Set privacy mode by system.
     *
     * @param isSystemPrivacyMode true means set window private, false means not set window private.
     */
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) = 0;
    /**
     * @brief Bind Dialog window to target token.
     *
     * @param targetToken window token of target.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError BindDialogTarget(sptr<IRemoteObject> targetToken) = 0;
    /**
     * @brief Raise zorder of window to the top of APP Mainwindow.
     *
     * @return WM_OK means raise success, others means raise failed.
     */
    virtual WmErrorCode RaiseToAppTop() = 0;
    /**
     * @brief Set skip flag of snapshot.
     *
     * @param isSkip true means skip the snapshot, false means the opposite.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetSnapshotSkip(bool isSkip) = 0;

    // window effect
    /**
     * @brief Set corner radius of window.
     *
     * @param cornerRadius corner radius of window
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetCornerRadius(float cornerRadius) = 0;
    /**
     * @brief Set shadow radius of window.
     *
     * @param radius shadow radius of window
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowRadius(float radius) = 0;
    /**
     * @brief Set shadow color of window.
     *
     * @param color shadow color of window.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowColor(std::string color) = 0;
    /**
     * @brief Set shadow X offset.
     *
     * @param offsetX Shadow x-axis offset.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowOffsetX(float offsetX) = 0;
    /**
     * @brief Set shadow Y offset.
     *
     * @param offsetY Shadow y-axis offset.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowOffsetY(float offsetY) = 0;
    /**
     * @brief Set blur property.
     *
     * @param radius blur value.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBlur(float radius) = 0;
    /**
     * @brief Set Backdrop blur property.
     *
     * @param radius Backdrop blur value.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBackdropBlur(float radius) = 0;
    /**
     * @brief Set Backdrop blur style.
     *
     * @param blurStyle Backdrop blur value.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBackdropBlurStyle(WindowBlurStyle blurStyle) = 0;

    /**
     * @brief Request to get focus.
     *
     * @return WM_OK means request success, others means request failed.
     */
    virtual WMError RequestFocus() const = 0;
    /**
     * @brief Check current focus status.
     *
     * @return true means window is focused, false means window is unfocused.
     */
    virtual bool IsFocused() const = 0;
    /**
     * @brief Update surfaceNode after customAnimation.
     *
     * @param isAdd true means add custom animation, false means the opposite.
     * @return WM_OK means update success, others means update failed.
     */
    virtual WMError UpdateSurfaceNodeAfterCustomAnimation(bool isAdd) = 0;
    /**
     * @brief Set InputEvent Consumer.
     *
     * @param inputEventConsumer consume input event object.
     * @return WM_OK means set success, others means set failed.
     */
    virtual void SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer) = 0;
    /**
     * @brief Consume KeyEvent from MMI.
     *
     * @param inputEvent Keyboard input event.
     */
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) = 0;
    /**
     * @brief Consume PointerEvent from MMI.
     *
     * @param inputEvent Pointer input event
     */
    virtual void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& inputEvent) = 0;
    /**
     * @brief Request Vsync.
     *
     * @param vsyncCallback Callback of vsync.
     */
    virtual void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) = 0;
    /**
     * @brief Update Configuration.
     *
     * @param configuration Window configuration.
     */
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) = 0;
    /**
     * @brief register window lifecycle listener.
     *
     * @param listener WindowLifeCycle listener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) = 0;
    /**
     * @brief Unregister window lifecycle listener.
     *
     * @param listener WindowLifeCycle listener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) = 0;
    /**
     * @brief register window change listener.
     *
     * @param listener IWindowChangeListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) = 0;
    /**
     * @brief Unregister window change listener.
     *
     * @param listener IWindowChangeListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) = 0;
    /**
     * @brief register avoid area change listener.
     *
     * @param listener IAvoidAreaChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) = 0;
    /**
     * @brief Unregister avoid area change listener.
     *
     * @param listener IAvoidAreaChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) = 0;
    /**
     * @brief register window drag listener.
     *
     * @param listener IWindowDragListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterDragListener(const sptr<IWindowDragListener>& listener) = 0;
    /**
     * @brief Unregister window drag listener.
     *
     * @param listener IWindowDragListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterDragListener(const sptr<IWindowDragListener>& listener) = 0;
    /**
     * @brief register display move listener.
     *
     * @param listener IDisplayMoveListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) = 0;
    /**
     * @brief Unregister display move listener.
     *
     * @param listener IDisplayMoveListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) = 0;
    /**
     * @brief register window destroyed listener.
     *
     * @param func function to notify window destroyed. 
     */
    virtual void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) = 0;
    /**
     * @brief register Occupied Area Change listener.
     *
     * @param listener IOccupiedAreaChangeListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) = 0;
    /**
     * @brief Unregister occupied area change listener.
     *
     * @param listener IOccupiedAreaChangeListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) = 0;
    /**
     * @brief register touch outside listener.
     *
     * @param listener ITouchOutsideListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) = 0;
    /**
     * @brief Unregister touch outside listener.
     *
     * @param listener ITouchOutsideListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) = 0;
    /**
     * @brief register Animation Transition Controller listener.
     *
     * @param listener IAnimationTransitionController.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener) = 0;
    /**
     * @brief register screen shot listener.
     *
     * @param listener IScreenshotListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterScreenshotListener(const sptr<IScreenshotListener>& listener) = 0;
    /**
     * @brief Unregister screen shot listener.
     *
     * @param listener IScreenshotListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener) = 0;
    /**
     * @brief register dialog target touch listener.
     *
     * @param listener IDialogTargetTouchListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) = 0;
    /**
     * @brief Unregister dialog target touch listener.
     *
     * @param listener IDialogTargetTouchListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) = 0;
    /**
     * @brief register dialog death Recipient listener.
     *
     * @param listener IDialogDeathRecipientListener.
     */
    virtual void RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) = 0;
    /**
     * @brief Unregister window death recipient listener.
     *
     * @param listener IDialogDeathRecipientListener.
     */
    virtual void UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) = 0;
    /**
     * @brief Notify touch dialog target.
     */
    virtual void NotifyTouchDialogTarget() = 0;
    /**
     * @brief Set ace ability handler.
     *
     * @param handler ace ability handler.
     */
    virtual void SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler) = 0;
    /**
     * @brief set window ui content
     *
     * @param contentInfo content info path
     * @param engine
     * @param storage
     * @param isDistributed
     * @param ability
     * @return WMError
     */
    virtual WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isDistributed = false, AppExecFwk::Ability* ability = nullptr) = 0;
    /**
     * @brief Get ui content info.
     *
     * @return ui content info.
     */
    virtual std::string GetContentInfo() = 0;
    /**
     * @brief Get ui content object.
     *
     * @return UIContent object of ACE.
     */
    virtual Ace::UIContent* GetUIContent() const = 0;
    /**
     * @brief Window handle new want.
     *
     * @param want want object of AAFwk.
     */
    virtual void OnNewWant(const AAFwk::Want& want) = 0;
    /**
     * @brief Set requested orientation.
     *
     * @param Orientation screen orientation.
     */
    virtual void SetRequestedOrientation(Orientation) = 0;
    /**
     * @brief Get requested orientation.
     *
     * @return Orientation screen orientation.
     */
    virtual Orientation GetRequestedOrientation() = 0;
    /**
     * @brief Set requested mode support info.
     *
     * @param modeSupportInfo mode of window supported.
     */
    virtual void SetRequestModeSupportInfo(uint32_t modeSupportInfo) = 0;
    /**
     * @brief Get requested mode support info.
     *
     * @return Enumeration values under WindowModeSupport.
     */
    virtual uint32_t GetRequestModeSupportInfo() const = 0;
    /**
     * @brief Set touch hot areas.
     *
     * @param rects hot areas of touching.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetTouchHotAreas(const std::vector<Rect>& rects) = 0;
    /**
     * @brief Get requested touch hot areas.
     *
     * @param rects hot areas of touching.
     */
    virtual void GetRequestedTouchHotAreas(std::vector<Rect>& rects) const = 0;
    /**
     * @brief Main handler available or not.
     *
     * @return true means main handler is available, false means the opposite.
     */
    virtual bool IsMainHandlerAvailable() const = 0;
    /**
     * @brief Set window label name.
     *
     * @param label window label name.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetAPPWindowLabel(const std::string& label) = 0;
    /**
     * @brief Set window icon.
     *
     * @param icon window icon.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon) = 0;

    /**
     * @brief disable main window decoration. It must be callled before loadContent.
     *
     */
    virtual WMError DisableAppWindowDecor() = 0;
    /**
     * @brief return window decoration is enabled. It is called by ACE
     *
     * @return true means window decoration is enabled. Otherwise disabled
     */
    virtual bool IsDecorEnable() const = 0;
    /**
     * @brief maximize the main window. It is called by ACE when maximize button is clicked.
     *
     * @return WMError
     */
    virtual WMError Maximize() = 0;
    /**
     * @brief minimize the main window. It is called by ACE when minimize button is clicked.
     *
     * @return WMError
     */
    virtual WMError Minimize() = 0;
    /**
     * @brief recovery the main window. It is called by ACE when recovery button is clicked.
     *
     * @return WMError
     */
    virtual WMError Recover() = 0;
    /**
     * @brief close the main window. It is called by ACE when close button is clicked.
     *
     * @return WMError
     */
    virtual WMError Close() = 0;
    /**
     * @brief start move main window. It is called by ACE when title is moved.
     *
     */
    virtual void StartMove() = 0;
    /**
     * @brief Set flag that need remove window input channel.
     *
     * @param needRemoveWindowInputChannel true means remove input channel, false means not remove.
     */
    virtual void SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel) = 0;

    // colorspace, gamut
    /**
     * @brief Is support wide gamut or not.
     *
     * @return true means support wide gamut, false means not support.
     */
    virtual bool IsSupportWideGamut() = 0;
    /**
     * @brief Set color space.
     *
     * @param colorSpace ColorSpace object.
     */
    virtual void SetColorSpace(ColorSpace colorSpace) = 0;
    /**
     * @brief Get color space object.
     *
     * @return ColorSpace object.
     */
    virtual ColorSpace GetColorSpace() = 0;

    virtual void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) = 0;
    /**
     * @brief window snapshot
     *
     * @return std::shared_ptr<Media::PixelMap> snapshot pixel
     */
    virtual std::shared_ptr<Media::PixelMap> Snapshot() = 0;

    /**
     * @brief Handle and notify memory level.
     *
     * @param level memory level
     * @return the error code of window
     */
    virtual WMError NotifyMemoryLevel(int32_t level) const = 0;

    /**
     * @brief Update configuration for all windows
     *
     * @param configuration configuration for app
     */
    virtual bool IsAllowHaveSystemSubWindow() = 0;

    /**
     * @brief Set aspect ratio of this window
     *
     * @param ratio the aspect ratio of window except decoration
     * @return WMError
     */
    virtual WMError SetAspectRatio(float ratio) = 0;
    /**
     * @brief Unset aspect ratio
     * @return WMError
     */
    virtual WMError ResetAspectRatio() = 0;
    /**
     * @brief Get keyboard animation config
     * @return KeyboardAnimationConfig
     */
    virtual KeyboardAnimationConfig GetKeyboardAnimationConfig() = 0;
};
}
}
#endif // OHOS_ROSEN_WINDOW_H

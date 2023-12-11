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
#include <iremote_object.h>

#include "wm_common.h"
#include "window_option.h"
#include "occupied_area_change_info.h"

typedef struct napi_env__* napi_env;
typedef struct napi_value__* napi_value;
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
    class WantParams;
}

namespace OHOS::Ace {
    class UIContent;
}

namespace OHOS::Media {
    class PixelMap;
}

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
namespace OHOS {
namespace Rosen {
using NotifyNativeWinDestroyFunc = std::function<void(std::string windowName)>;
using NotifyTransferComponentDataFunc = std::function<void(const AAFwk::WantParams& wantParams)>;
using NotifyTransferComponentDataForResultFunc = std::function<AAFwk::WantParams(const AAFwk::WantParams& wantParams)>;
class RSSurfaceNode;
class RSTransaction;
class ISession;

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
     * @param ret Error code when window go forground failed.
     */
    virtual void ForegroundFailed(int32_t ret) {}
    /**
     * @brief Notify caller the error code when window go background failed.
     *
     * @param ret Error code when window go background failed.
     */
    virtual void BackgroundFailed(int32_t ret) {}
    /**
     * @brief Notify caller that window is active.
     */
    virtual void AfterActive() {}
    /**
     * @brief Notify caller that window is inactive.
     */
    virtual void AfterInactive() {}
    /**
     * @brief Notify caller that window is resumed.
     */
    virtual void AfterResumed() {}
    /**
     * @brief Notify caller that window is paused.
     */
    virtual void AfterPaused() {}
    /**
     * @brief Notify caller that window is destroyed.
     */
    virtual void AfterDestroyed() {}
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
    /**
     * @brief Notify caller when window mode changed.
     *
     * @param mode Mode of the current window.
     * @param hasDeco Window has decoration or not.
     */
    virtual void OnModeChange(WindowMode mode, bool hasDeco = true) {}
};

/**
 * @class IWindowStatusChangeListener
 *
 * @brief IWindowStatusChangeListener is used to observe the window status when window status changed.
 */
class IWindowStatusChangeListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when window status changed.
     *
     * @param status Mode of the current window.
     */
    virtual void OnWindowStatusChange(WindowStatus status) {}
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
     * @param from Display id before display start move.
     * @param to Display id after display move end.
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
    virtual void OnDispatchPointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) {}
    /**
     * @brief Dispatch KeyEvent.
     *
     * @param inputEvent Means KeyEvent.
     */
    virtual void OnDispatchKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent) {}
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
     * @param rsTransaction Animation transaction.
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
    virtual void SetBackgroundColor(uint32_t color) {}
    /**
     * @brief Get BackgroundColor.
     *
     * @return Value of BackgroundColor and default color is white.
     */
    virtual uint32_t GetBackgroundColor() { return 0xffffffff; }
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
    virtual bool OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const { return false; }
    /**
     * @brief Observe PointerEvent of Multi-Model Input.
     *
     * @param pointerEvent PointerEvent of Multi-Model Input.
     */
    virtual bool OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const { return false; }
    /**
     * @brief Observe axisEvent of Multi-Model Input.
     *
     * @param axisEvent AxisEvent of Multi-Model Input.
     */
    virtual bool OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const { return false; }
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
    virtual void AnimationForShown() {}
    /**
     * @brief Observe the event when animation hide.
     */
    virtual void AnimationForHidden() {}
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
    virtual void OnDialogDeathRecipient() const {}
};

/**
 * @class IWindowVisibilityChangedListener
 *
 * @brief Listener to observe one window visibility changed.
*/
class IWindowVisibilityChangedListener : virtual public RefBase {
public:
    virtual void OnWindowVisibilityChangedCallback(const bool isVisible) {};
};
using IWindowVisibilityListenerSptr = sptr<IWindowVisibilityChangedListener>;

static WMError DefaultCreateErrCode = WMError::WM_OK;
class Window : virtual public RefBase {
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
     * @brief create main/uiextension window with session
     *
     * @param option window propertion
     * @param context ability context
     * @param iSession session token of window session
     * @param errCode error code of create window
     * @return sptr<Window> If create window success, return window instance; Otherwise, return nullptr
     */
    static sptr<Window> Create(sptr<WindowOption>& option, const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<IRemoteObject>& iSession, WMError& errCode = DefaultCreateErrCode);

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
    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const { return nullptr; }
    /**
     * @brief Get ability context
     *
     * @return Ability context from AbilityRuntime
     */
    virtual const std::shared_ptr<AbilityRuntime::Context> GetContext() const { return nullptr; }
    /**
     * @brief Get the window show rect
     *
     * @return Rect of window
     */
    virtual Rect GetRect() const { return {}; }
    /**
     * @brief Get window default rect from window property.
     *
     * @return Rect of window.
     */
    virtual Rect GetRequestRect() const { return {}; }
    /**
     * @brief Get the window type
     *
     * @return Type of window
     */
    virtual WindowType GetType() const { return WindowType::WINDOW_TYPE_APP_MAIN_WINDOW; }
    /**
     * @brief Get the window mode.
     *
     * @return Mode of window.
     */
    virtual WindowMode GetMode() const { return WindowMode::WINDOW_MODE_UNDEFINED; }
    /**
     * @brief Get alpha of window.
     *
     * @return Alpha of window.
     */
    virtual float GetAlpha() const { return 0.0f; }
    /**
     * @brief Get the name of window.
     *
     * @return Name of window.
     */
    virtual const std::string& GetWindowName() const
    {
        static const std::string name;
        return name;
    }
    /**
     * @brief Get id of window.
     *
     * @return ID of window.
     */
    virtual uint32_t GetWindowId() const { return INVALID_WINDOW_ID; }
    /**
     * @brief Get flag of window.
     *
     * @return Flag of window.
     */
    virtual uint32_t GetWindowFlags() const { return 0; }
    /**
     * @brief Get state of window.
     *
     * @return Current state of window.
     */
    virtual WindowState GetWindowState() const { return WindowState::STATE_INITIAL; }
    /**
     * @brief Set focusable property of window.
     *
     * @param isFocusable Window can be focused or not.
     * @return Errorcode of window.
     */
    virtual WMError SetFocusable(bool isFocusable) { return WMError::WM_OK; }
    /**
     * @brief Get focusable property of window.
     *
     * @return True means window can be focused, false means window cannot be focused.
     */
    virtual bool GetFocusable() const { return false; }
    /**
     * @brief Set touchable property of window.
     *
     * @param isTouchable Window can be touched or not.
     * @return Errorcode of window.
     */
    virtual WMError SetTouchable(bool isTouchable) { return WMError::WM_OK; }
    /**
     * @brief Get touchable property of window.
     *
     * @return True means window can be touched, false means window cannot be touched.
     */
    virtual bool GetTouchable() const { return false; }
    /**
     * @brief Get SystemBarProperty By WindowType.
     *
     * @param type Type of window.
     * @return Property of system bar.
     */
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) const { return {}; }
    /**
     * @brief judge this window is full screen.
     *
     * @return true If SetFullScreen(true) is called , return true.
     * @return false default return false
     */
    virtual bool IsFullScreen() const { return false; }
    /**
     * @brief judge window layout is full screen
     *
     * @return true this window layout is full screen
     * @return false this window layout is not full screen
     */
    virtual bool IsLayoutFullScreen() const { return false; }
    /**
     * @brief Set the Window Type
     *
     * @param type window type
     * @return WMError
     */
    virtual WMError SetWindowType(WindowType type) { return WMError::WM_OK; }
    /**
     * @brief Set the Window Mode
     *
     * @param mode window mode
     * @return WMError
     */
    virtual WMError SetWindowMode(WindowMode mode) { return WMError::WM_OK; }
    /**
     * @brief Set alpha of window.
     *
     * @param alpha Alpha of window.
     * @return WM_OK means success, others means set failed.
     */
    virtual WMError SetAlpha(float alpha) { return WMError::WM_OK; }
    /**
     * @brief Set transform of window property.
     *
     * @param trans Window Transform.
     * @return WMError
     */
    virtual WMError SetTransform(const Transform& trans) { return WMError::WM_OK; }
    /**
     * @brief Get transform of window property.
     *
     * @return Property of transform.
     */
    virtual const Transform& GetTransform() const
    {
        static const Transform trans;
        return trans;
    }
    /**
     * @brief Add window flag.
     *
     * @param flag Flag of window.
     * @return WM_OK means add success, others means failed.
     */
    virtual WMError AddWindowFlag(WindowFlag flag) { return WMError::WM_OK; }
    /**
     * @brief Remove window flag.
     *
     * @param flag Flag of window
     * @return WM_OK means remove success, others means failed.
     */
    virtual WMError RemoveWindowFlag(WindowFlag flag) { return WMError::WM_OK; }
    /**
     * @brief Set window flag.
     *
     * @param flags Flag of window
     * @return WM_OK means set success, others means failed.
     */
    virtual WMError SetWindowFlags(uint32_t flags) { return WMError::WM_OK; }
    /**
     * @brief Set the System Bar(include status bar and nav bar) Property
     *
     * @param type WINDOW_TYPE_STATUS_BAR or WINDOW_TYPE_NAVIGATION_BAR
     * @param property system bar prop,include content color, background color
     * @return WMError
     */
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) { return WMError::WM_OK; }
    /**
     * @brief Get the Avoid Area By Type object
     *
     * @param type avoid area type.@see reference
     * @param avoidArea
     * @return WMError
     */
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) { return WMError::WM_OK; }
    /**
     * @brief Set this window layout full screen, with hide status bar and nav bar above on this window
     *
     * @param status
     * @return WMError
     */
    virtual WMError SetLayoutFullScreen(bool status) { return WMError::WM_OK; }
    /**
     * @brief Set this window full screen, with hide status bar and nav bar
     *
     * @param status if true, hide status bar and nav bar; Otherwise, show status bar and nav bar
     * @return WMError
     */
    virtual WMError SetFullScreen(bool status) { return WMError::WM_OK; }
    /**
     * @brief destroy window
     *
     * @return WMError
     */
    virtual WMError Destroy() { return WMError::WM_OK; }
    /**
     * @brief Show window
     *
     * @param reason Reason for window state change.
     * @param withAnimation True means window show with animation, false means window show without animation.
     * @return WM_OK means window show success, others means failed.
     */
    virtual WMError Show(uint32_t reason = 0, bool withAnimation = false) { return WMError::WM_OK; }
    /**
     * @brief Hide window
     *
     * @param reason Reason for window state change.
     * @param withAnimation True means window show with animation, false means window show without animation.
     * @param isFromInnerkits True means remove command is from inner kits.
     * @return WM_OK means window hide success, others means failed.
     */
    virtual WMError Hide(uint32_t reason = 0, bool withAnimation = false, bool isFromInnerkits = true)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief move the window to (x, y)
     *
     * @param x
     * @param y
     * @return WMError
     */
    virtual WMError MoveTo(int32_t x, int32_t y) { return WMError::WM_OK; }
    /**
     * @brief resize the window instance (w,h)
     *
     * @param width
     * @param height
     * @return WMError
     */
    virtual WMError Resize(uint32_t width, uint32_t height) { return WMError::WM_OK; }
    /**
     * @brief set the window gravity
     *
     * @param gravity
     * @param percent
     * @return WMError
     */
    virtual WMError SetWindowGravity(WindowGravity gravity, uint32_t percent) { return WMError::WM_OK; }
    /**
     * @brief Set the screen always on
     *
     * @param keepScreenOn
     * @return WMError
     */
    virtual WMError SetKeepScreenOn(bool keepScreenOn) { return WMError::WM_OK; }
    /**
     * @brief Get the screen is always on or not.
     *
     * @return True means screen is always on, false means the opposite.
     */
    virtual bool IsKeepScreenOn() const { return false; }
    /**
     * @brief Set the screen on
     *
     * @param turnScreenOn True means turn screen on, false means the opposite.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetTurnScreenOn(bool turnScreenOn) { return WMError::WM_OK; }
    /**
     * @brief Get the screen is on or not.
     *
     * @return True means screen is on, false means screen is off.
     */
    virtual bool IsTurnScreenOn() const { return false; }
    /**
     * @brief Set Background color.
     *
     * @param color Background color.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBackgroundColor(const std::string& color) { return WMError::WM_OK; }
    /**
     * @brief Set transparent status.
     *
     * @param isTransparent True means set window transparent, false means the opposite.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetTransparent(bool isTransparent) { return WMError::WM_OK; }
    /**
     * @brief Get transparent status.
     *
     * @return True means window is transparent, false means the opposite.
     */
    virtual bool IsTransparent() const { return false; }
    /**
     * @brief Set brightness value of window.
     *
     * @param brightness Brightness of window.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBrightness(float brightness) { return WMError::WM_OK; }
    /**
     * @brief Get brightness value of window.
     *
     * @return Brightness value of window.
     */
    virtual float GetBrightness() const { return 0.0f; }
    /**
     * @brief Set calling window.
     *
     * @param windowId Window id.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetCallingWindow(uint32_t windowId) { return WMError::WM_OK; }
    /**
     * @brief Set privacy mode of window.
     *
     * @param isPrivacyMode True means set window private, false means not set window private.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetPrivacyMode(bool isPrivacyMode) { return WMError::WM_OK; }
    /**
     * @brief Get privacy property of window.
     *
     * @return True means window is private and cannot be screenshot or recorded.
     */
    virtual bool IsPrivacyMode() const { return false; }
    /**
     * @brief Set privacy mode by system.
     *
     * @param isSystemPrivacyMode True means set window private, false means not set window private.
     */
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) {}
    /**
     * @brief Bind Dialog window to target token.
     *
     * @param targetToken Window token of target.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError BindDialogTarget(sptr<IRemoteObject> targetToken) { return WMError::WM_OK; }
    /**
     * @brief Raise zorder of window to the top of APP Mainwindow.
     *
     * @return WM_OK means raise success, others means raise failed.
     */
    virtual WmErrorCode RaiseToAppTop() { return WmErrorCode::WM_OK; }
    /**
     * @brief Set skip flag of snapshot.
     *
     * @param isSkip True means skip the snapshot, false means the opposite.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetSnapshotSkip(bool isSkip) { return WMError::WM_OK; }

    // window effect
    /**
     * @brief Set corner radius of window.
     *
     * @param cornerRadius Corner radius of window
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetCornerRadius(float cornerRadius) { return WMError::WM_OK; }
    /**
     * @brief Set shadow radius of window.
     *
     * @param radius Shadow radius of window
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowRadius(float radius) { return WMError::WM_OK; }
    /**
     * @brief Set shadow color of window.
     *
     * @param color Shadow color of window.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowColor(std::string color) { return WMError::WM_OK; }
    /**
     * @brief Set shadow X offset.
     *
     * @param offsetX Shadow x-axis offset.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowOffsetX(float offsetX) { return WMError::WM_OK; }
    /**
     * @brief Set shadow Y offset.
     *
     * @param offsetY Shadow y-axis offset.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetShadowOffsetY(float offsetY) { return WMError::WM_OK; }
    /**
     * @brief Set blur property.
     *
     * @param radius Blur value.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBlur(float radius) { return WMError::WM_OK; }
    /**
     * @brief Set Backdrop blur property.
     *
     * @param radius Backdrop blur value.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBackdropBlur(float radius) { return WMError::WM_OK; }
    /**
     * @brief Set Backdrop blur style.
     *
     * @param blurStyle Backdrop blur value.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetBackdropBlurStyle(WindowBlurStyle blurStyle) { return WMError::WM_OK; }

    /**
     * @brief Request to get focus.
     *
     * @return WM_OK means request success, others means request failed.
     */
    virtual WMError RequestFocus() const { return WMError::WM_OK; }
    /**
     * @brief Check current focus status.
     *
     * @return True means window is focused, false means window is unfocused.
     */
    virtual bool IsFocused() const { return false; }
    /**
     * @brief Update surfaceNode after customAnimation.
     *
     * @param isAdd True means add custom animation, false means the opposite.
     * @return WM_OK means update success, others means update failed.
     */
    virtual WMError UpdateSurfaceNodeAfterCustomAnimation(bool isAdd) { return WMError::WM_OK; }
    /**
     * @brief Set InputEvent Consumer.
     *
     * @param inputEventConsumer Consume input event object.
     * @return WM_OK means set success, others means set failed.
     */
    virtual void SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer) {}
    /**
     * @brief Consume KeyEvent from MMI.
     *
     * @param inputEvent Keyboard input event.
     */
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) {}
    /**
     * @brief Consume PointerEvent from MMI.
     *
     * @param inputEvent Pointer input event
     */
    virtual void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& inputEvent) {}
    /**
     * @brief Request Vsync.
     *
     * @param vsyncCallback Callback of vsync.
     */
    virtual void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) {}
    /**
     * @brief get vsync period.
     *
     * @return vsync period.
     */
    virtual int64_t GetVSyncPeriod() { return 0; }
    /**
     * @brief flush frame rate of linker.
     *
     * @param rate frame rate.
     */
    virtual void FlushFrameRate(uint32_t rate) {}
    /**
     * @brief Update Configuration.
     *
     * @param configuration Window configuration.
     */
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) {}
    /**
     * @brief Register window lifecycle listener.
     *
     * @param listener WindowLifeCycle listener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) { return WMError::WM_OK; }
    /**
     * @brief Unregister window lifecycle listener.
     *
     * @param listener WindowLifeCycle listener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) { return WMError::WM_OK; }
    /**
     * @brief Register window change listener.
     *
     * @param listener IWindowChangeListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Unregister window change listener.
     *
     * @param listener IWindowChangeListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Register avoid area change listener.
     *
     * @param listener IAvoidAreaChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Unregister avoid area change listener.
     *
     * @param listener IAvoidAreaChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Register window drag listener.
     *
     * @param listener IWindowDragListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterDragListener(const sptr<IWindowDragListener>& listener) { return WMError::WM_OK; }
    /**
     * @brief Unregister window drag listener.
     *
     * @param listener IWindowDragListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterDragListener(const sptr<IWindowDragListener>& listener) { return WMError::WM_OK; }
    /**
     * @brief Register display move listener.
     *
     * @param listener IDisplayMoveListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) { return WMError::WM_OK; }
    /**
     * @brief Unregister display move listener.
     *
     * @param listener IDisplayMoveListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) { return WMError::WM_OK; }
    /**
     * @brief Register window destroyed listener.
     *
     * @param func Function to notify window destroyed.
     */
    virtual void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) {}
    /**
     * @brief Register Occupied Area Change listener.
     *
     * @param listener IOccupiedAreaChangeListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Unregister occupied area change listener.
     *
     * @param listener IOccupiedAreaChangeListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Register touch outside listener.
     *
     * @param listener ITouchOutsideListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) { return WMError::WM_OK; }
    /**
     * @brief Unregister touch outside listener.
     *
     * @param listener ITouchOutsideListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Register Animation Transition Controller listener.
     *
     * @param listener IAnimationTransitionController.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Register screen shot listener.
     *
     * @param listener IScreenshotListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterScreenshotListener(const sptr<IScreenshotListener>& listener) { return WMError::WM_OK; }
    /**
     * @brief Unregister screen shot listener.
     *
     * @param listener IScreenshotListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener) { return WMError::WM_OK; }
    /**
     * @brief Register dialog target touch listener.
     *
     * @param listener IDialogTargetTouchListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Unregister dialog target touch listener.
     *
     * @param listener IDialogTargetTouchListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Register dialog death Recipient listener.
     *
     * @param listener IDialogDeathRecipientListener.
     */
    virtual void RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) {}
    /**
     * @brief Unregister window death recipient listener.
     *
     * @param listener IDialogDeathRecipientListener.
     */
    virtual void UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) {}
    /**
     * @brief Notify touch dialog target.
     */
    virtual void NotifyTouchDialogTarget() {}
    /**
     * @brief Set ace ability handler.
     *
     * @param handler Ace ability handler.
     */
    virtual void SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler) {}
    /**
     * @brief set window ui content
     *
     * @param contentInfo content info path
     * @param env
     * @param storage
     * @param isDistributed
     * @param ability
     * @return WMError
     */
    virtual WMError NapiSetUIContent(const std::string& contentInfo, napi_env env,
        napi_value storage, bool isDistributed = false, sptr<IRemoteObject> token = nullptr,
        AppExecFwk::Ability* ability = nullptr)
    {
        return WMError::WM_OK;
    }
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
    virtual WMError SetUIContentByName(const std::string& contentInfo, napi_env env, napi_value storage,
        AppExecFwk::Ability* ability = nullptr)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief set window ui content by abc
     *
     * @param abcPath abc path
     * @param env
     * @param storage
     * @param ability
     * @return WMError
     */
    virtual WMError SetUIContentByAbc(const std::string& abcPath, napi_env env, napi_value storage,
        AppExecFwk::Ability* ability = nullptr)
    {
        return WMError::WM_OK;
    }
    /**
     * @brief Get ui content info.
     *
     * @return UI content info.
     */
    virtual std::string GetContentInfo() { return std::string(); }
    /**
     * @brief Get ui content object.
     *
     * @return UIContent object of ACE.
     */
    virtual Ace::UIContent* GetUIContent() const { return nullptr; }
    /**
     * @brief Window handle new want.
     *
     * @param want Want object of AAFwk.
     */
    virtual void OnNewWant(const AAFwk::Want& want) {}
    /**
     * @brief Set requested orientation.
     *
     * @param Orientation Screen orientation.
     */
    virtual void SetRequestedOrientation(Orientation) {}
    /**
     * @brief Get requested orientation.
     *
     * @return Orientation screen orientation.
     */
    virtual Orientation GetRequestedOrientation() { return Orientation::UNSPECIFIED; }
    /**
     * @brief Set requested mode support info.
     *
     * @param modeSupportInfo Mode of window supported.
     */
    virtual void SetRequestModeSupportInfo(uint32_t modeSupportInfo) {}
    /**
     * @brief Get requested mode support info.
     *
     * @return Enumeration values under WindowModeSupport.
     */
    virtual uint32_t GetRequestModeSupportInfo() const { return 0; }
    /**
     * @brief Set touch hot areas.
     *
     * @param rects Hot areas of touching.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetTouchHotAreas(const std::vector<Rect>& rects) { return WMError::WM_OK; }
    /**
     * @brief Get requested touch hot areas.
     *
     * @param rects Hot areas of touching.
     */
    virtual void GetRequestedTouchHotAreas(std::vector<Rect>& rects) const {}
    /**
     * @brief Main handler available or not.
     *
     * @return True means main handler is available, false means the opposite.
     */
    virtual bool IsMainHandlerAvailable() const { return false; }
    /**
     * @brief Set window label name.
     *
     * @param label Window label name.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetAPPWindowLabel(const std::string& label) { return WMError::WM_OK; }
    /**
     * @brief Set window icon.
     *
     * @param icon Window icon.
     * @return WM_OK means set success, others means set failed.
     */
    virtual WMError SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon) { return WMError::WM_OK; }

    /**
     * @brief disable main window decoration. It must be callled before loadContent.
     *
     */
    virtual WMError DisableAppWindowDecor() { return WMError::WM_OK; }
    /**
     * @brief return window decoration is enabled. It is called by ACE
     *
     * @return true means window decoration is enabled. Otherwise disabled
     */
    virtual bool IsDecorEnable() const { return false; }
    /**
     * @brief maximize the main window. It is called by ACE when maximize button is clicked.
     *
     * @return WMError
     */
    virtual WMError Maximize() { return WMError::WM_OK; }
    /**
     * @brief maximize the main window according to MaximizeMode. called by ACE when maximize button is clicked.
     *
     * @return WMError
     */
    virtual WMError MaximizeFloating() {return WMError::WM_OK;}
    /**
     * @brief minimize the main window. It is called by ACE when minimize button is clicked.
     *
     * @return WMError
     */
    virtual WMError Minimize() { return WMError::WM_ERROR_DEVICE_NOT_SUPPORT; }
    /**
     * @brief recovery the main window. It is called by ACE when recovery button is clicked.
     *
     * @return WMError
     */
    virtual WMError Recover() { return WMError::WM_ERROR_DEVICE_NOT_SUPPORT; }
    /**
     * @brief close the main window. It is called by ACE when close button is clicked.
     *
     * @return WMError
     */
    virtual WMError Close() { return WMError::WM_OK; }
    /**
     * @brief start move main window. It is called by ACE when title is moved.
     *
     */
    virtual void StartMove() {}
    /**
     * @brief Set flag that need remove window input channel.
     *
     * @param needRemoveWindowInputChannel True means remove input channel, false means not remove.
     */
    virtual void SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel) {}
    /**
     * @brief set global window maximize mode. It is called by ACE when maximize mode changed.
     *
     * @param mode MODE_AVOID_SYSTEM_BAR - avoid statusbar and dockbar; MODE_FULL_FILL - fullfill the screen
     *
     * @return WMError
     */
    virtual WMError SetGlobalMaximizeMode(MaximizeMode mode) {return WMError::WM_OK;}
    /**
     * @brief get global window maximize mode.
     *
     * @return MaximizeMode
     */
    virtual MaximizeMode GetGlobalMaximizeMode() const {return MaximizeMode::MODE_FULL_FILL;}

    // colorspace, gamut
    /**
     * @brief Is support wide gamut or not.
     *
     * @return True means support wide gamut, false means not support.
     */
    virtual bool IsSupportWideGamut() { return false; }
    /**
     * @brief Set color space.
     *
     * @param colorSpace ColorSpace object.
     */
    virtual void SetColorSpace(ColorSpace colorSpace) {}
    /**
     * @brief Get color space object.
     *
     * @return ColorSpace object.
     */
    virtual ColorSpace GetColorSpace() { return ColorSpace::COLOR_SPACE_DEFAULT; }

    virtual void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) {}
    /**
     * @brief window snapshot
     *
     * @return std::shared_ptr<Media::PixelMap> snapshot pixel
     */
    virtual std::shared_ptr<Media::PixelMap> Snapshot() { return nullptr; }

    /**
     * @brief Handle and notify memory level.
     *
     * @param level memory level
     * @return the error code of window
     */
    virtual WMError NotifyMemoryLevel(int32_t level) { return WMError::WM_OK; }

    /**
     * @brief Update configuration for all windows
     *
     * @param configuration configuration for app
     */
    virtual bool IsAllowHaveSystemSubWindow() { return false; }

    /**
     * @brief Set aspect ratio of this window
     *
     * @param ratio the aspect ratio of window except decoration
     * @return WMError
     */
    virtual WMError SetAspectRatio(float ratio) { return WMError::WM_OK; }
    /**
     * @brief Unset aspect ratio
     * @return WMError
     */
    virtual WMError ResetAspectRatio() { return WMError::WM_OK; }
    /**
     * @brief Get keyboard animation config
     * @return KeyboardAnimationConfig
     */
    virtual KeyboardAnimationConfig GetKeyboardAnimationConfig() { return {}; }
    /**
     * @brief Set need default animation for window show and hide.
     *
     * @param needDefaultAnimation True means need default animation, false means not need.
     */
    virtual void SetNeedDefaultAnimation(bool needDefaultAnimation) {}
    /**
     * @brief Transfer Ability Result.
     * @return WMError
     */
    virtual WMError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) { return WMError::WM_OK; }
    /**
     * @brief Transfer UIExtension data to Extension Component.
     * @return WMError
     */
    virtual WMError TransferExtensionData(const AAFwk::WantParams& wantParams) { return WMError::WM_OK; }
    /**
     * @brief Register transfer component data callback.
     *
     * @param func Function to notify transfer component data.
     */
    virtual void RegisterTransferComponentDataListener(const NotifyTransferComponentDataFunc& func) {}
    /**
     * @brief Perform back event.
     *
     */
    virtual void PerformBack() {}
    /**
     * @brief Set the drag enabled flag of a window.
     *
     * @param dragEnabled true means the window can be resized by dragging, otherwise means the opposite.
     * @return Errorcode of window.
     */
    virtual WMError SetResizeByDragEnabled(bool dragEnabled) { return WMError::WM_ERROR_DEVICE_NOT_SUPPORT; }
    /**
     * @brief Set the raise enabled flag of a window.
     *
     * @param raiseEnabled true means the window can be raised by click, otherwise means the opposite.
     * @return Errorcode of window.
     */
    virtual WMError SetRaiseByClickEnabled(bool raiseEnabled) { return WMError::WM_ERROR_DEVICE_NOT_SUPPORT; }
    /**
     * @brief Raise one app sub window above another.
     *
     * @return WM_OK means raise success, others means raise failed.
     */
    virtual WmErrorCode RaiseAboveTarget(int32_t subWindowId) { return WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT; }

    /**
     * @brief Hide non-system floating windows.
     *
     * @param shouldHide true means the non-system windows should be hidden, otherwise means the opposite.
     * @return Errorcode of window.
     */
    virtual WMError HideNonSystemFloatingWindows(bool shouldHide) { return WMError::WM_OK; }

    /**
     * @brief Is floating window of app type or not.
     *
     * @return True means floating window of app type, false means the opposite.
     */
    virtual bool IsFloatingWindowAppType() const { return false; }

    /**
     * @brief Set Text Field Avoid Info.
     *
     * @return Errorcode of window.
     */
    virtual WMError SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) { return WMError::WM_OK; }

    /**
     * @brief Register transfer component data callback.
     *
     * @param func Function to notify transfer component data.
     */
    virtual void RegisterTransferComponentDataForResultListener(const NotifyTransferComponentDataForResultFunc& func) {}

    /**
     * @brief Transfer accessibility event data
     *
     * @param func Function to notify transfer component data.
    */
    virtual WMError TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        const std::vector<int32_t>& uiExtensionIdLevelVec) { return WMError::WM_OK; };

    /**
     * @brief Notify prepare to close window
     *
     * @return Errorcode of window.
     */
    virtual WMError NotifyPrepareClosePiPWindow() { return WMError::WM_OK; }

    /**
     * @brief update the pip window instance (w,h,r).
     *
     * @param width width of pip window.
     * @param height width of pip window.
     * @param reason reason of update.
     */
    virtual void UpdatePiPRect(const uint32_t width, const uint32_t height, PiPRectUpdateReason reason) {}

    /**
     * @brief Recovery pip main window.
     *
     * @param Rect of window.
     * @return Errorcode of window.
     */
    virtual WMError RecoveryPullPiPMainWindow(const Rect& rect) { return WMError::WM_OK; }

    /**
     * @brief When get focused, keep the keyboard created by other windows, support system window and app subwindow.
     *
     * @param keepKeyboardFlag true means the keyboard should be preserved, otherwise means the opposite.
     * @return WM_OK means set keep keyboard flag success, others means failed.
    */
    virtual WmErrorCode KeepKeyboardOnFocus(bool keepKeyboardFlag) { return WmErrorCode::WM_OK; }

    /**
     * @brief Get the window limits of current window.
     *
     * @param windowLimits.
     * @return WMError.
    */
    virtual WMError GetWindowLimits(WindowLimits& windowLimits) { return WMError::WM_ERROR_DEVICE_NOT_SUPPORT; }

    /**
     * @brief Set the window limits of current window.
     *
     * @param windowLimits.
     * @return WMError.
    */
    virtual WMError SetWindowLimits(WindowLimits& windowLimits) { return WMError::WM_ERROR_DEVICE_NOT_SUPPORT; }

    /**
     * @brief Register window visibility change listener.
     *
     * @param listener IWindowVisibilityChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterWindowVisibilityChangeListener(const IWindowVisibilityListenerSptr& listener)
    {
        return WMError::WM_OK;
    }

    /**
     * @brief Unregister window visibility change listener.
     *
     * @param listener IWindowVisibilityChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterWindowVisibilityChangeListener(const IWindowVisibilityListenerSptr& listener)
    {
        return WMError::WM_OK;
    }
    
    /**
     * @brief Register window status change listener.
     *
     * @param listener IWindowStatusChangeListener.
     * @return WM_OK means register success, others means register failed.
     */
    virtual WMError RegisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener)
    {
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    /**
     * @brief Unregister window status change listener.
     *
     * @param listener IWindowStatusChangeListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    virtual WMError UnregisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener)
    {
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }

    /**
     * @brief Set Specific System Bar(include status bar and nav bar) Property
     *
     * @param type WINDOW_TYPE_STATUS_BAR or WINDOW_TYPE_NAVIGATION_BAR
     * @param property system bar prop,include content color, background color
     * @return WMError
     */
    virtual WMError SetSpecificBarProperty(WindowType type, const SystemBarProperty& property)
    {
        return WMError::WM_OK;
    }
};
}
}
#endif // OHOS_ROSEN_WINDOW_H

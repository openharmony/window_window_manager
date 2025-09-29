/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SESSION_IMPL_H
#define OHOS_ROSEN_WINDOW_SESSION_IMPL_H

#include <atomic>

#include <shared_mutex>
#include <ability_context.h>
#include <event_handler.h>
#include <i_input_event_consumer.h>
#include <ui_content.h>
#include <ui/rs_surface_node.h>
#include "display_manager.h"
#include "singleton_container.h"

#include "common/include/window_session_property.h"
#include "display_info.h"
#include "future_callback.h"
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "session/container/include/zidl/session_stage_stub.h"
#include "session/host/include/zidl/session_interface.h"
#include "vsync_station.h"
#include "window.h"
#include "window_helper.h"
#include "window_option.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
template<typename T1, typename T2, typename Ret>
using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;

/*
 * DFX
 */
const std::string SET_UICONTENT_TIMEOUT_LISTENER_TASK_NAME = "SetUIContentTimeoutListener";
constexpr int64_t SET_UICONTENT_TIMEOUT_TIME_MS = 4000;
constexpr int64_t SET_UICONTENT_TIMEOUT_TIME_AFTER_FREEZE_MS = 5000;
}

struct WindowTitleVisibleFlags {
    bool isMaximizeVisible = true;
    bool isMinimizeVisible = true;
    bool isSplitVisible = true;
    bool isCloseVisible = true;
};

class WindowSessionImpl : public Window, public virtual SessionStageStub {
public:
    explicit WindowSessionImpl(const sptr<WindowOption>& option);
    ~WindowSessionImpl();

    static sptr<Window> Find(const std::string& name);
    static std::vector<sptr<Window>> GetSubWindow(int parentId);

    virtual WMError Create(const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<Rosen::ISession>& iSession,
        const std::string& identityToken = "") { return WMError::WM_OK; }

    /*
     * inherits from window
     */
    WMError Show(uint32_t reason = 0, bool withAnimation = false, bool withFocus = true) override;
    WMError Hide(uint32_t reason = 0, bool withAnimation = false, bool isFromInnerkits = true) override;
    WMError Destroy() override;
    virtual WMError Destroy(bool needNotifyServer, bool needClearListener = true);
    WMError NapiSetUIContent(const std::string& contentInfo, napi_env env, napi_value storage,
        BackupAndRestoreType type, sptr<IRemoteObject> token, AppExecFwk::Ability* ability) override;
    WMError SetUIContentByName(const std::string& contentInfo, napi_env env, napi_value storage,
        AppExecFwk::Ability* ability) override;
    WMError SetUIContentByAbc(const std::string& abcPath, napi_env env, napi_value storage,
        AppExecFwk::Ability* ability) override;
    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const override;
    const std::shared_ptr<AbilityRuntime::Context> GetContext() const override;
    Rect GetRequestRect() const override;
    WindowType GetType() const override;
    const std::string& GetWindowName() const override;
    WindowState GetWindowState() const override;
    WindowState GetRequestWindowState() const;
    WMError SetFocusable(bool isFocusable) override;
    WMError SetTouchable(bool isTouchable) override;

    /*
     * Window Hierarchy
     */
    WMError SetTopmost(bool topmost) override;
    bool IsTopmost() const override;
    WMError SetMainWindowTopmost(bool isTopmost) override;
    bool IsMainWindowTopmost() const override;
    void SetSubWindowZLevelToProperty();
    int32_t GetSubWindowZLevelByFlags(WindowType type, uint32_t windowFlags, bool isTopmost);

    WMError SetResizeByDragEnabled(bool dragEnabled) override;
    WMError SetRaiseByClickEnabled(bool raiseEnabled) override;
    WMError HideNonSystemFloatingWindows(bool shouldHide) override;
    WMError SetSingleFrameComposerEnabled(bool enable) override;
    bool IsFloatingWindowAppType() const override;

    /*
     * PC Window
     */
    bool IsPcWindow() const override;
    bool IsPcOrPadCapabilityEnabled() const override;
    bool IsPcOrPadFreeMultiWindowMode() const override;
    bool IsSceneBoardEnabled() const override;
    bool GetCompatibleModeInPc() const override;
    WMError SetWindowDelayRaiseEnabled(bool isEnabled) override;
    bool IsWindowDelayRaiseEnabled() const override;
    WMError SetTitleButtonVisible(bool isMaximizeVisible, bool isMinimizeVisible, bool isSplitVisible,
        bool isCloseVisible) override;
    WMError SetSubWindowModal(bool isModal, ModalityType modalityType = ModalityType::WINDOW_MODALITY) override;
    WMError SetWindowModal(bool isModal) override;
    void SetTargetAPIVersion(uint32_t targetAPIVersion);
    uint32_t GetTargetAPIVersion() const;

    WMError SetWindowType(WindowType type) override;
    WMError SetBrightness(float brightness) override;
    virtual float GetBrightness() const override;
    void SetRequestedOrientation(Orientation orientation, bool needAnimation = true) override;
    bool GetTouchable() const override;
    uint32_t GetWindowId() const override;
    uint64_t GetDisplayId() const override;
    Rect GetRect() const override;
    bool GetFocusable() const override;
    std::string GetContentInfo(BackupAndRestoreType type = BackupAndRestoreType::CONTINUATION) override;
    WMError SetRestoredRouterStack(const std::string& routerStack) override;
    Ace::UIContent* GetUIContent() const override;
    std::shared_ptr<Ace::UIContent> GetUIContentSharedPtr() const;
    Ace::UIContent* GetUIContentWithId(uint32_t winId) const override;
    void OnNewWant(const AAFwk::Want& want) override;
    void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) override;
    int64_t GetVSyncPeriod() override;
    void FlushFrameRate(uint32_t rate, int32_t animatorExpectedFrameRate, uint32_t rateType = 0) override;
    void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) override;
    bool PreNotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;

    /*
     * inherits from session stage
     */
    WSError SetActive(bool active) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const SceneAnimationConfig& config = { nullptr, ROTATE_ANIMATION_DURATION },
        const std::map<AvoidAreaType, AvoidArea>& avoidAreas = {}) override;
    void UpdateDensity() override;
    void SetUniqueVirtualPixelRatio(bool useUniqueDensity, float virtualPixelRatio) override;
    WSError UpdateOrientation() override;
    WSError UpdateDisplayId(uint64_t displayId) override;
    WSError UpdateFocus(bool focus) override;
    bool IsFocused() const override;
    WMError RequestFocus() const override;
    WMError RequestFocusByClient(bool isFocused) const override;
    WSError UpdateWindowMode(WindowMode mode) override;
    WSError HandleBackEvent() override;
    KeyboardAnimationConfig GetKeyboardAnimationConfig() override;
    bool NotifyOnKeyPreImeEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    void NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    void NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
        bool notifyInputMethod = true) override;
    void NotifyOccupiedAreaChangeInfoInner(sptr<OccupiedAreaChangeInfo> info);
    void NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info,
                                      const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    void NotifyKeyboardDidShow(const KeyboardPanelInfo& keyboardPanelInfo);
    void NotifyKeyboardDidHide(const KeyboardPanelInfo& keyboardPanelInfo);
    void NotifyKeyboardAnimationCompleted(const KeyboardPanelInfo& keyboardPanelInfo) override;
    void NotifyForegroundInteractiveStatus(bool interactive) override;
    void NotifyDisplayMove(DisplayId from, DisplayId to) override;
    void NotifyWindowCrossAxisChange(CrossAxisState state) override;

    WMError RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    WMError UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    WMError RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    WMError UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    WMError RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    WMError UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    WMError RegisterWindowCrossAxisListener(const sptr<IWindowCrossAxisListener>& listener) override;
    WMError UnregisterWindowCrossAxisListener(const sptr<IWindowCrossAxisListener>& listener) override;
    void RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    void UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    WMError RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    WMError UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    WMError RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    WMError UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    WMError RegisterKeyboardDidShowListener(const sptr<IKeyboardDidShowListener>& listener) override;
    WMError UnregisterKeyboardDidShowListener(const sptr<IKeyboardDidShowListener>& listener) override;
    WMError RegisterKeyboardDidHideListener(const sptr<IKeyboardDidHideListener>& listener) override;
    WMError UnregisterKeyboardDidHideListener(const sptr<IKeyboardDidHideListener>& listener) override;
    WMError RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    WMError UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    WMError RegisterWindowVisibilityChangeListener(const IWindowVisibilityListenerSptr& listener) override;
    WMError UnregisterWindowVisibilityChangeListener(const IWindowVisibilityListenerSptr& listener) override;
    WMError RegisterDisplayIdChangeListener(const IDisplayIdChangeListenerSptr& listener) override;
    WMError UnregisterDisplayIdChangeListener(const IDisplayIdChangeListenerSptr& listener) override;
    WMError RegisterSystemDensityChangeListener(const ISystemDensityChangeListenerSptr& listener) override;
    WMError UnregisterSystemDensityChangeListener(const ISystemDensityChangeListenerSptr& listener) override;
    WMError RegisterWindowNoInteractionListener(const IWindowNoInteractionListenerSptr& listener) override;
    WMError UnregisterWindowNoInteractionListener(const IWindowNoInteractionListenerSptr& listener) override;
    void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) override;
    void UnregisterWindowDestroyedListener() override { notifyNativeFunc_ = nullptr; }
    WMError RegisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    WMError UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    void SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler) override;
    void SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer) override;
    void GetExtensionConfig(AAFwk::WantParams& want) const override;
    void UpdateExtensionConfig(const std::shared_ptr<AAFwk::Want>& want) override;

    WMError SetBackgroundColor(const std::string& color) override;
    virtual Orientation GetRequestedOrientation() override;

    int32_t GetParentId() const;
    int32_t GetPersistentId() const override;
    sptr<WindowSessionProperty> GetProperty() const;
    SystemSessionConfig GetSystemSessionConfig() const;
    sptr<ISession> GetHostSession() const;
    int32_t GetFloatingWindowParentId();
    void NotifyAfterForeground(bool needNotifyListeners = true, bool needNotifyUiContent = true);
    void NotifyAfterBackground(bool needNotifyListeners = true, bool needNotifyUiContent = true);
    void NotifyAfterDidForeground(uint32_t reason = static_cast<uint32_t>(WindowStateChangeReason::NORMAL));
    void NotifyAfterDidBackground(uint32_t reason = static_cast<uint32_t>(WindowStateChangeReason::NORMAL));
    void NotifyForegroundFailed(WMError ret);
    void NotifyBackgroundFailed(WMError ret);
    WSError MarkProcessed(int32_t eventId) override;
    void UpdateTitleButtonVisibility();
    WSError NotifyDestroy() override;
    WSError NotifyTransferComponentData(const AAFwk::WantParams& wantParams) override;
    WSErrorCode NotifyTransferComponentDataSync(const AAFwk::WantParams& wantParams,
        AAFwk::WantParams& reWantParams) override;
    void NotifyTouchDialogTarget(int32_t posX = 0, int32_t posY = 0) override;
    void NotifyScreenshot() override;
    void DumpSessionElementInfo(const std::vector<std::string>& params) override;
    // colorspace, gamut
    virtual bool IsSupportWideGamut() override;
    virtual void SetColorSpace(ColorSpace colorSpace) override;
    virtual ColorSpace GetColorSpace() override;
    WSError NotifyTouchOutside() override;
    WMError SetLandscapeMultiWindow(bool isLandscapeMultiWindow) override;
    WSError NotifyWindowVisibility(bool isVisible) override;
    WSError NotifyNoInteractionTimeout(const IWindowNoInteractionListenerSptr& listener);
    WMError TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) override;
    WindowState state_ { WindowState::STATE_INITIAL };
    WindowState requestState_ { WindowState::STATE_INITIAL };
    WSError UpdateMaximizeMode(MaximizeMode mode) override;
    void NotifySessionForeground(uint32_t reason, bool withAnimation) override;
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WSError UpdateTitleInTargetPos(bool isShow, int32_t height) override;
    WSError NotifyDialogStateChange(bool isForeground) override;
    bool IsMainHandlerAvailable() const override;

    /*
     * PiP Window
     */
    WSError NotifyCloseExistPipWindow() override;
    WSError SetPipActionEvent(const std::string& action, int32_t status) override;
    WSError SetPiPControlEvent(WsPiPControlType controlType, WsPiPControlStatus status) override;
    WSError NotifyPipWindowSizeChange(double width, double height, double scale) override;
    void UpdatePiPRect(const Rect& rect, WindowSizeChangeReason reason) override;
    void UpdatePiPControlStatus(PiPControlType controlType, PiPControlStatus status) override;
    void SetAutoStartPiP(bool isAutoStart, uint32_t priority, uint32_t width, uint32_t height) override;

    void SetDrawingContentState(bool drawingContentState);
    WMError RegisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener) override;
    WMError UnregisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener) override;
    WMError SetSpecificBarProperty(WindowType type, const SystemBarProperty& property) override;
    void SetLayoutTransform(const Transform& trans);
    Transform GetLayoutTransform() const override;

    /*
     * Window Decor
     */
    WMError SetDecorVisible(bool isVisible) override;
    WMError GetDecorVisible(bool& isVisible) override;
    WMError SetWindowTitleMoveEnabled(bool enable) override;
    WMError SetDecorHeight(int32_t decorHeight) override;
    WMError GetDecorHeight(int32_t& height) override;
    WMError SetDecorButtonStyle(const DecorButtonStyle& decorButtonStyle) override;
    WMError GetDecorButtonStyle(DecorButtonStyle& decorButtonStyle) override;
    WMError GetTitleButtonArea(TitleButtonRect& titleButtonRect) override;
    WMError SetAPPWindowLabel(const std::string& label) override;
    WMError SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon) override;
    WMError SetWindowContainerColor(const std::string& activeColor, const std::string& inactiveColor) override;

    /*
     * Window Decor listener
     */
    WMError RegisterWindowTitleButtonRectChangeListener(
        const sptr<IWindowTitleButtonRectChangedListener>& listener) override;
    WMError UnregisterWindowTitleButtonRectChangeListener(
        const sptr<IWindowTitleButtonRectChangedListener>& listener) override;
    void NotifyWindowTitleButtonRectChange(TitleButtonRect titleButtonRect);
    WMError RegisterSubWindowCloseListeners(const sptr<ISubWindowCloseListener>& listener) override;
    WMError UnregisterSubWindowCloseListeners(const sptr<ISubWindowCloseListener>& listener) override;
    void NotifySubWindowClose(bool& terminateCloseProcess);
    WMError RegisterMainWindowCloseListeners(const sptr<IMainWindowCloseListener>& listener) override;
    WMError UnregisterMainWindowCloseListeners(const sptr<IMainWindowCloseListener>& listener) override;
    WMError NotifyMainWindowClose(bool& terminateCloseProcess);
    WMError RegisterWindowWillCloseListeners(const sptr<IWindowWillCloseListener>& listener) override;
    WMError UnRegisterWindowWillCloseListeners(const sptr<IWindowWillCloseListener>& listener) override;
    WMError NotifyWindowWillClose(sptr<Window> window);

    WSError GetUIContentRemoteObj(sptr<IRemoteObject>& uiContentRemoteObj) override;
    void RecoverSessionListener();
    void SetDefaultDisplayIdIfNeed();
    WMError RegisterWindowRectChangeListener(const sptr<IWindowRectChangeListener>& listener) override;
    WMError UnregisterWindowRectChangeListener(const sptr<IWindowRectChangeListener>& listener) override;
    virtual WMError GetCallingWindowWindowStatus(WindowStatus& windowStatus) const override;
    virtual WMError GetCallingWindowRect(Rect& rect) const override;
    virtual void SetUiDvsyncSwitch(bool dvsyncSwitch) override;
    WMError SetContinueState(int32_t continueState) override;

    /*
     * Multi Window
     */
    WSError SetSplitButtonVisible(bool isVisible) override;
    WMError GetIsMidScene(bool& isMidScene) override;
    WSError SendContainerModalEvent(const std::string& eventName, const std::string& eventValue) override;

    /*
     * Window Layout
     */
    WMError EnableDrag(bool enableDrag) override;
    WSError SetDragActivated(bool dragActivated) override;
    WSError SetEnableDragBySystem(bool enableDrag) override;
    bool IsWindowDraggable();
    float GetVirtualPixelRatio() override;
    CrossAxisState GetCrossAxisState() override;

    /*
     * Free Multi Window
     */
    WMError RegisterSwitchFreeMultiWindowListener(const sptr<ISwitchFreeMultiWindowListener>& listener) override;
    WMError UnregisterSwitchFreeMultiWindowListener(const sptr<ISwitchFreeMultiWindowListener>& listener) override;
    void SetFreeMultiWindowMode(bool enable)
    {
        windowSystemConfig_.freeMultiWindowEnable_ = enable;
    }

    /*
     * Window Immersive
     */
    WMError RegisterAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener) override;
    WMError UnregisterAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener) override;
    WMError SetAvoidAreaOption(uint32_t avoidAreaOption) override;
    WMError GetAvoidAreaOption(uint32_t& avoidAreaOption) override;
    void NotifyAvoidAreaChange(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    bool IsSystemWindow() const override { return WindowHelper::IsSystemWindow(GetType()); }
    bool IsAppWindow() const override { return WindowHelper::IsAppWindow(GetType()); }
    WindowType GetRootHostWindowType() const override { return rootHostWindowType_; }
    void SetRootHostWindowType(WindowType& rootHostWindowType) override { rootHostWindowType_ = rootHostWindowType; }
    WMError UpdateSystemBarProperties(const std::unordered_map<WindowType, SystemBarProperty>& systemBarProperties,
        const std::unordered_map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags) override;
    void UpdateSpecificSystemBarEnabled(bool systemBarEnable, bool systemBarEnableAnimation,
        SystemBarProperty& property) override;
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;

    /*
     * Window Property
     */
    WSError NotifyDisplayIdChange(DisplayId displayId);

    /*
     * Window Input Event
     */
    WMError NotifyWatchGestureConsumeResult(int32_t keyCode, bool isConsumed);
    WMError NotifyWatchFocusActiveChange(bool isActive);
    void RegisterWatchFocusActiveChangeCallback();
    void NotifyConsumeResultToFloatWindow(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isConsumed);

    /*
     * HightLight Window
     */
    bool GetExclusivelyHighlighted() const;
    WMError IsWindowHighlighted(bool& highlighted) const override;
    WMError SetExclusivelyHighlighted(bool isExclusivelyHighlighted) override;
    WMError RegisterWindowHighlightChangeListeners(const sptr<IWindowHighlightChangeListener>& listener) override;
    WMError UnregisterWindowHighlightChangeListeners(const sptr<IWindowHighlightChangeListener>& listener) override;
    WSError NotifyHighlightChange(bool isHighlight) override;

protected:
    WMError Connect();
    bool IsWindowSessionInvalid() const;
    void NotifyWindowAfterUnfocused();
    void NotifyWindowAfterFocused();
    void NotifyAfterActive();
    void NotifyAfterInactive();
    void NotifyBeforeDestroy(std::string windowName);
    void NotifyAfterDestroy();
    void ClearListenersById(int32_t persistentId);

    /*
     * Free Multi Window
     */
    void ClearSwitchFreeMultiWindowListenersById(int32_t persistentId);
    void NotifySwitchFreeMultiWindow(bool enable);

    void ClearVsyncStation();
    WMError WindowSessionCreateCheck();
    void UpdateDecorEnableToAce(bool isDecorEnable);
    void UpdateDecorEnable(bool needNotify = false, WindowMode mode = WindowMode::WINDOW_MODE_UNDEFINED);
    void NotifyModeChange(WindowMode mode, bool hasDeco = true);
    WMError UpdateProperty(WSPropertyChangeAction action);
    WMError SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;
    virtual WMError SetLayoutFullScreenByApiVersion(bool status);
    virtual float GetVirtualPixelRatio(const sptr<DisplayInfo>& displayInfo);
    void UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr,
        const sptr<DisplayInfo>& info = nullptr,
        const std::map<AvoidAreaType, AvoidArea>& avoidAreas = {});
    void NotifySizeChange(Rect rect, WindowSizeChangeReason reason);
    static sptr<Window> FindWindowById(uint32_t winId);
    void NotifyWindowStatusChange(WindowMode mode);
    void NotifyTransformChange(const Transform& transForm) override;
    void NotifySingleHandTransformChange(const SingleHandTransform& singleHandTransform) override;
    bool IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const;
    void DispatchKeyEventCallback(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed);
    bool IsVerticalOrientation(Orientation orientation) const;
    void CopyUniqueDensityParameter(sptr<WindowSessionImpl> parentWindow);
    sptr<WindowSessionImpl> FindMainWindowWithContext();
    sptr<WindowSessionImpl> FindExtensionWindowWithContext();

    WMError RegisterExtensionAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener);
    WMError UnregisterExtensionAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener);

    void RefreshNoInteractionTimeoutMonitor();
    WindowStatus GetWindowStatusInner(WindowMode mode);

    /*
     * PC Event Filter
     */
    bool FilterKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);
    bool FilterPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    WMError SetKeyEventFilter(KeyEventFilterFunc filter) override;
    WMError ClearKeyEventFilter() override;
    WMError SetMouseEventFilter(MouseEventFilterFunc filter) override;
    WMError ClearMouseEventFilter() override;
    WMError SetTouchEventFilter(TouchEventFilterFunc filter) override;
    WMError ClearTouchEventFilter() override;

    /*
     * Sub Window
     */
    void UpdateSubWindowStateAndNotify(int32_t parentPersistentId, const WindowState newState);
    void DestroySubWindow();
    bool IsSubWindowMaximizeSupported() const override;
    void UpdateSubWindowLevel(uint32_t subWindowLevel);
    void GetSubWidnows(int32_t parentPersistentId, std::vector<sptr<WindowSessionImpl>>& subWindows);
    void RemoveSubWindow(int32_t parentPersistentId);

    sptr<WindowOption> windowOption_;
    sptr<ISession> hostSession_;
    mutable std::mutex hostSessionMutex_;
    std::shared_ptr<Ace::UIContent> uiContent_;
    mutable std::shared_mutex uiContentMutex_;
    std::shared_ptr<AbilityRuntime::Context> context_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;

    sptr<WindowSessionProperty> property_;
    SystemSessionConfig windowSystemConfig_;
    NotifyNativeWinDestroyFunc notifyNativeFunc_;

    std::recursive_mutex mutex_;
    static std::map<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>> windowSessionMap_;
    // protect windowSessionMap_
    static std::shared_mutex windowSessionMutex_;
    static std::set<sptr<WindowSessionImpl>> windowExtensionSessionSet_;
    // protect windowExtensionSessionSet_
    static std::shared_mutex windowExtensionSessionMutex_;
    bool isSystembarPropertiesSet_ = false;
    bool isIgnoreSafeAreaNeedNotify_ = false;
    bool isIgnoreSafeArea_ = false;
    std::atomic_bool isFocused_ = false;
    std::atomic_bool isHighlighted_ = false;
    std::atomic_bool shouldReNotifyHighlight_ = false;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    bool shouldReNotifyFocus_ = false;
    std::shared_ptr<VsyncStation> vsyncStation_ = nullptr;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer_;
    bool useUniqueDensity_ { false };
    float virtualPixelRatio_ { 1.0f };
    bool escKeyEventTriggered_ = false;
    // Check whether the UIExtensionAbility process is started
    static bool isUIExtensionAbilityProcess_;
    WSError SwitchFreeMultiWindow(bool enable) override;
    std::string identityToken_ = { "" };
    void MakeSubOrDialogWindowDragableAndMoveble();
    bool IsFreeMultiWindowMode() const
    {
        return windowSystemConfig_.IsFreeMultiWindowMode();
    }

    /*
     * Sub Window
     */
    static std::recursive_mutex subWindowSessionMutex_;
    static std::map<int32_t, std::vector<sptr<WindowSessionImpl>>> subWindowSessionMap_;

    /*
     * Compatible Mode
     */
    std::atomic_bool isFullScreenInForceSplit_ { false };

    /*
     * DFX
     */
    void SetUIContentComplete();
    void AddSetUIContentTimeoutCheck();
    void NotifySetUIContentComplete();
    std::atomic_bool setUIContentCompleted_ { false };
    void SetUIExtensionDestroyComplete();
    void SetUIExtensionDestroyCompleteInSubWindow();
    void AddSetUIExtensionDestroyTimeoutCheck();
    std::atomic_bool setUIExtensionDestroyCompleted_ { false };
    std::atomic_bool startUIExtensionDestroyTimer_ { false };
    enum TimeoutErrorCode : int32_t {
        SET_UICONTENT_TIMEOUT = 1000,
        SET_UIEXTENSION_DESTROY_TIMEOUT
    };

    /*
     * Window Lifecycle
     */
    bool hasFirstNotifyInteractive_ = false;
    bool interactive_ = true;
    bool isDidForeground_ = false;

    /*
     * Window Layout
     */
    std::atomic_bool isDragTaskPostDone_ = true;
    void FlushLayoutSize(int32_t width, int32_t height) override;
    sptr<FutureCallback> layoutCallback_ = nullptr;
    void UpdateVirtualPixelRatio(const sptr<Display>& display);
    WMError GetVirtualPixelRatio(float& vpr);
    void SetCurrentTransform(const Transform& transform);
    Transform GetCurrentTransform() const;
    void NotifyAfterUIContentReady();
    void SetNeedRenotifyTransform(bool isNeedRenotify) { needRenotifyTransform_.store(isNeedRenotify); }
    bool IsNeedRenotifyTransform() const { return needRenotifyTransform_.load(); }
    mutable std::recursive_mutex transformMutex_;
    std::atomic<CrossAxisState> crossAxisState_ = CrossAxisState::STATE_INVALID;
    bool IsValidCrossState(int32_t state) const;

    /*
     * Window Immersive
     */
    std::map<AvoidAreaType, AvoidArea> lastAvoidAreaMap_;
    uint32_t GetStatusBarHeight() const override;
    WindowType rootHostWindowType_ = WindowType::APP_MAIN_WINDOW_BASE;

    /*
     * PC Fold Screen
     */
    bool supportEnterWaterfallMode_ { false };

    /*
     * Window Property
     */
    float lastSystemDensity_ = UNDEFINED_DENSITY;
    WSError NotifySystemDensityChange(float density);
    void RegisterWindowInspectorCallback();
    uint32_t GetTargetAPIVersionByApplicationInfo() const;

    /*
     * Window Input Event
     */
    bool GetWatchGestureConsumed() const;
    void SetWatchGestureConsumed(bool isWatchGestureConsumed);

    /*
     * Window Rotation
     */
    int16_t rotationAnimationCount_ { 0 };
    void NotifyRotationAnimationEnd();

private:
    //Trans between colorGamut and colorSpace
    static ColorSpace GetColorSpaceFromSurfaceGamut(GraphicColorGamut colorGamut);
    static GraphicColorGamut GetSurfaceGamutFromColorSpace(ColorSpace colorSpace);

    template<typename T> WMError RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> WMError UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> EnableIfSame<T, IWindowLifeCycle, std::vector<sptr<IWindowLifeCycle>>> GetListeners();
    template<typename T> EnableIfSame<T, IDisplayMoveListener, std::vector<sptr<IDisplayMoveListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowChangeListener, std::vector<sptr<IWindowChangeListener>>> GetListeners();
    template <typename T>
    EnableIfSame<T, IWindowCrossAxisListener, std::vector<sptr<IWindowCrossAxisListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IAvoidAreaChangedListener, std::vector<sptr<IAvoidAreaChangedListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IDialogDeathRecipientListener, std::vector<sptr<IDialogDeathRecipientListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IDialogTargetTouchListener, std::vector<sptr<IDialogTargetTouchListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IOccupiedAreaChangeListener, std::vector<sptr<IOccupiedAreaChangeListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IKeyboardDidShowListener, std::vector<sptr<IKeyboardDidShowListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IKeyboardDidHideListener, std::vector<sptr<IKeyboardDidHideListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IScreenshotListener, std::vector<sptr<IScreenshotListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, ITouchOutsideListener, std::vector<sptr<ITouchOutsideListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowVisibilityChangedListener, std::vector<IWindowVisibilityListenerSptr>> GetListeners();
    template<typename T>
    EnableIfSame<T, IDisplayIdChangeListener, std::vector<IDisplayIdChangeListenerSptr>> GetListeners();
    template<typename T>
    EnableIfSame<T, ISystemDensityChangeListener, std::vector<ISystemDensityChangeListenerSptr>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowNoInteractionListener, std::vector<IWindowNoInteractionListenerSptr>> GetListeners();
    template<typename T> void ClearUselessListeners(std::map<int32_t, T>& listeners, int32_t persistentId);
    template<typename T> void ClearUselessListeners(std::unordered_map<int32_t, T>& listeners, int32_t persistentId);
    RSSurfaceNode::SharedPtr CreateSurfaceNode(const std::string& name, WindowType type);
    template<typename T>
    EnableIfSame<T, IWindowStatusChangeListener, std::vector<sptr<IWindowStatusChangeListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowRectChangeListener, std::vector<sptr<IWindowRectChangeListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, ISwitchFreeMultiWindowListener, std::vector<sptr<ISwitchFreeMultiWindowListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowHighlightChangeListener, std::vector<sptr<IWindowHighlightChangeListener>>> GetListeners();
    void NotifyAfterFocused();
    void NotifyUIContentFocusStatus();
    void NotifyAfterUnfocused(bool needNotifyUiContent = true);
    void NotifyAfterResumed();
    void NotifyAfterPaused();
    void NotifyUIContentHighlightStatus(bool isHighlighted);

    /*
     * Window Decor listener
     */
    template<typename T>
    EnableIfSame<T, IWindowTitleButtonRectChangedListener,
        std::vector<sptr<IWindowTitleButtonRectChangedListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, ISubWindowCloseListener, sptr<ISubWindowCloseListener>> GetListeners();
    template<typename T>
    EnableIfSame<T, IMainWindowCloseListener, sptr<IMainWindowCloseListener>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowWillCloseListener, std::vector<sptr<IWindowWillCloseListener>>> GetListeners();

    WMError InitUIContent(const std::string& contentInfo, napi_env env, napi_value storage,
        WindowSetUIContentType setUIContentType, BackupAndRestoreType restoreType, AppExecFwk::Ability* ability,
        OHOS::Ace::UIContentErrorCode& aceRet);
    WMError SetUIContentInner(const std::string& contentInfo, napi_env env, napi_value storage,
        WindowSetUIContentType setUIContentType, BackupAndRestoreType restoreType, AppExecFwk::Ability* ability);
    std::shared_ptr<std::vector<uint8_t>> GetAbcContent(const std::string& abcPath);
    inline void DestroyExistUIContent();
    std::string GetRestoredRouterStack();

    bool CheckIfNeedCommitRsTransaction(WindowSizeChangeReason wmReason);
    void UpdateRectForRotation(const Rect& wmRect, const Rect& preRect, WindowSizeChangeReason wmReason,
        const SceneAnimationConfig& config, const std::map<AvoidAreaType, AvoidArea>& avoidAreas = {});
    void UpdateRectForOtherReason(const Rect& wmRect, const Rect& preRect, WindowSizeChangeReason wmReason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr,
        const std::map<AvoidAreaType, AvoidArea>& avoidAreas = {});
    void UpdateRectForOtherReasonTask(const Rect& wmRect, const Rect& preRect, WindowSizeChangeReason wmReason,
        const std::shared_ptr<RSTransaction>& rsTransaction,
        const std::map<AvoidAreaType, AvoidArea>& avoidAreas = {});
    void UpdateRectForResizeWithAnimation(const Rect& wmRect, const Rect& preRect, WindowSizeChangeReason wmReason,
        const RectAnimationConfig& rectAnimationConfig, const std::shared_ptr<RSTransaction>& rsTransaction = nullptr,
        const std::map<AvoidAreaType, AvoidArea>& avoidAreas = {});
    void SubmitNoInteractionMonitorTask(int32_t eventId, const IWindowNoInteractionListenerSptr& listener);
    bool IsUserOrientation(Orientation orientation) const;
    WMError GetAppForceLandscapeConfig(AppForceLandscapeConfig& config);
    void SetForceSplitEnable(bool isForceSplit, const std::string& homePage = "");
    void SetFrameLayoutCallbackEnable(bool enable);
    void UpdateFrameLayoutCallbackIfNeeded(WindowSizeChangeReason wmReason);
    bool IsNotifyInteractiveDuplicative(bool interactive);
    void SetUniqueVirtualPixelRatioForSub(bool useUniqueDensity, float virtualPixelRatio);

    /*
     * PC Window
     */
    void GetTitleButtonVisible(bool& hideMaximizeButton, bool& hideMinimizeButton, bool& hideSplitButton,
        bool& hideCloseButton);

    static std::recursive_mutex lifeCycleListenerMutex_;
    static std::recursive_mutex windowChangeListenerMutex_;
    static std::recursive_mutex windowCrossAxisListenerMutex_;
    static std::recursive_mutex avoidAreaChangeListenerMutex_;
    static std::recursive_mutex dialogDeathRecipientListenerMutex_;
    static std::recursive_mutex dialogTargetTouchListenerMutex_;
    static std::recursive_mutex occupiedAreaChangeListenerMutex_;
    static std::recursive_mutex keyboardDidShowListenerMutex_;
    static std::recursive_mutex keyboardDidHideListenerMutex_;
    static std::recursive_mutex screenshotListenerMutex_;
    static std::recursive_mutex touchOutsideListenerMutex_;
    static std::recursive_mutex windowVisibilityChangeListenerMutex_;
    static std::recursive_mutex windowNoInteractionListenerMutex_;
    static std::recursive_mutex windowStatusChangeListenerMutex_;
    static std::mutex displayMoveListenerMutex_;
    static std::mutex windowRectChangeListenerMutex_;
    static std::mutex switchFreeMultiWindowListenerMutex_;
    static std::mutex highlightChangeListenerMutex_;
    static std::map<int32_t, std::vector<sptr<IWindowLifeCycle>>> lifecycleListeners_;
    static std::map<int32_t, std::vector<sptr<IDisplayMoveListener>>> displayMoveListeners_;
    static std::map<int32_t, std::vector<sptr<IWindowChangeListener>>> windowChangeListeners_;
    static std::map<int32_t, std::vector<sptr<IWindowCrossAxisListener>>> windowCrossAxisListeners_;
    static std::map<int32_t, std::vector<sptr<IAvoidAreaChangedListener>>> avoidAreaChangeListeners_;
    static std::map<int32_t, std::vector<sptr<IDialogDeathRecipientListener>>> dialogDeathRecipientListeners_;
    static std::map<int32_t, std::vector<sptr<IDialogTargetTouchListener>>> dialogTargetTouchListener_;
    static std::map<int32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> occupiedAreaChangeListeners_;
    static std::map<int32_t, std::vector<sptr<IKeyboardDidShowListener>>> keyboardDidShowListeners_;
    static std::map<int32_t, std::vector<sptr<IKeyboardDidHideListener>>> keyboardDidHideListeners_;
    static std::map<int32_t, std::vector<sptr<IScreenshotListener>>> screenshotListeners_;
    static std::map<int32_t, std::vector<sptr<ITouchOutsideListener>>> touchOutsideListeners_;
    static std::map<int32_t, std::vector<IWindowVisibilityListenerSptr>> windowVisibilityChangeListeners_;
    static std::mutex displayIdChangeListenerMutex_;
    static std::map<int32_t, std::vector<IDisplayIdChangeListenerSptr>> displayIdChangeListeners_;
    static std::mutex systemDensityChangeListenerMutex_;
    static std::unordered_map<int32_t, std::vector<ISystemDensityChangeListenerSptr>> systemDensityChangeListeners_;
    static std::map<int32_t, std::vector<IWindowNoInteractionListenerSptr>> windowNoInteractionListeners_;
    static std::map<int32_t, std::vector<sptr<IWindowStatusChangeListener>>> windowStatusChangeListeners_;
    static std::map<int32_t, std::vector<sptr<IWindowRectChangeListener>>> windowRectChangeListeners_;
    static std::map<int32_t, std::vector<sptr<ISwitchFreeMultiWindowListener>>> switchFreeMultiWindowListeners_;
    static std::map<int32_t, std::vector<sptr<IWindowHighlightChangeListener>>> highlightChangeListeners_;

    // FA only
    sptr<IAceAbilityHandler> aceAbilityHandler_;

    std::atomic<int32_t> lastInteractionEventId_ { 0 };

    bool isMainHandlerAvailable_ = true;

    std::string subWindowTitle_ = { "" };
    std::string dialogTitle_ = { "" };
    WindowTitleVisibleFlags windowTitleVisibleFlags_;

    std::string restoredRouterStack_; // It was set and get in same thread, which is js thread.

    /*
     * Window Layout
     */
    WSRect layoutRect_;
    std::atomic_bool windowSizeChanged_ = false;
    std::atomic_bool enableFrameLayoutFinishCb_ = false;
    std::atomic_bool dragActivated_ = true;
    WindowSizeChangeReason lastSizeChangeReason_ = WindowSizeChangeReason::END;
    bool postTaskDone_ = false;
    Transform layoutTransform_;
    SingleHandTransform singleHandTransform_;
    mutable std::mutex currentTransformMutex_;
    Transform currentTransform_;
    std::atomic_bool needRenotifyTransform_ = false;
    
    /*
     * Window Decor
     */
    DecorButtonStyle decorButtonStyle_;
    int32_t decorHeight_ = 0;

    /*
     * Window Decor listener
     */
    static std::recursive_mutex windowTitleButtonRectChangeListenerMutex_;
    static std::map<int32_t, std::vector<sptr<IWindowTitleButtonRectChangedListener>>>
        windowTitleButtonRectChangeListeners_;
    static std::mutex subWindowCloseListenersMutex_;
    static std::map<int32_t, sptr<ISubWindowCloseListener>> subWindowCloseListeners_;
    static std::mutex mainWindowCloseListenersMutex_;
    static std::map<int32_t, sptr<IMainWindowCloseListener>> mainWindowCloseListeners_;
    static std::mutex windowWillCloseListenersMutex_;
    static std::unordered_map<int32_t, std::vector<sptr<IWindowWillCloseListener>>> windowWillCloseListeners_;

    /*
     * Multi Window
     */
    bool isSplitButtonVisible_ = true;

    /*
     * PC Window
     */
    uint32_t targetAPIVersion_ = 0;

    /*
     * Window Input Event
     */
    bool isWatchGestureConsumed_ = false;

    /*
     * PC Event Filter
     */
    std::mutex keyEventFilterMutex_;
    KeyEventFilterFunc keyEventFilter_;
    std::mutex mouseEventFilterMutex_;
    MouseEventFilterFunc mouseEventFilter_;
    std::mutex touchEventFilterMutex_;
    TouchEventFilterFunc touchEventFilter_;

    /*
     * Window Scene
     */
    WSError NotifyWindowAttachStateChange(bool isAttach) override { return WSError::WS_OK; }

    /*
     * keyboard
     */
    bool isKeyboardDidShowRegistered_ = false;
    bool isKeyboardDidHideRegistered_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_SESSION_IMPL_H

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
#include <optional>
#include <shared_mutex>
#include <ability_context.h>
#include <event_handler.h>
#include <i_input_event_consumer.h>
#include <refbase.h>
#include <ui_content.h>
#include <ui/rs_surface_node.h>
#include "display_manager.h"
#include "singleton_container.h"

#include "common/include/window_session_property.h"
#include "interfaces/include/ws_common_inner.h"
#include "session/container/include/zidl/session_stage_stub.h"
#include "session/host/include/zidl/session_interface.h"
#include "window.h"
#include "window_option.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
template<typename T1, typename T2, typename Ret>
using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;
}
class WindowSessionImpl : public Window, public virtual SessionStageStub {
public:
    explicit WindowSessionImpl(const sptr<WindowOption>& option);
    ~WindowSessionImpl();
    static sptr<Window> Find(const std::string& name);
    static std::vector<sptr<Window>> GetSubWindow(int parentId);
    // inherits from window
    virtual WMError Create(const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<Rosen::ISession>& iSession);
    WMError Show(uint32_t reason = 0, bool withAnimation = false) override;
    WMError Hide(uint32_t reason = 0, bool withAnimation = false, bool isFromInnerkits = true) override;
    WMError SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) override;
    WMError Destroy() override;
    virtual WMError Destroy(bool needNotifyServer, bool needClearListener = true);
    WMError NapiSetUIContent(const std::string& contentInfo, napi_env env,
        napi_value storage, bool isdistributed, sptr<IRemoteObject> token, AppExecFwk::Ability* ability) override;
    WMError SetUIContentByName(const std::string& contentInfo, napi_env env, napi_value storage,
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
    WMError SetResizeByDragEnabled(bool dragEnabled) override;
    WMError SetRaiseByClickEnabled(bool raiseEnabled) override;
    WMError HideNonSystemFloatingWindows(bool shouldHide) override;
    bool IsFloatingWindowAppType() const override;
    WMError SetWindowType(WindowType type) override;
    WMError SetBrightness(float brightness) override;
    virtual float GetBrightness() const override;
    void SetRequestedOrientation(Orientation orientation) override;
    bool GetTouchable() const override;
    uint32_t GetWindowId() const override;
    Rect GetRect() const override;
    bool GetFocusable() const override;
    std::string GetContentInfo() override;
    Ace::UIContent* GetUIContent() const override;
    void OnNewWant(const AAFwk::Want& want) override;
    WMError SetAPPWindowLabel(const std::string& label) override;
    WMError SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon) override;
    void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) override;
    int64_t GetVSyncPeriod() override;
    void FlushFrameRate(uint32_t rate) override;
    // inherits from session stage
    WSError SetActive(bool active) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    void UpdateDensity() override;
    WSError UpdateFocus(bool focus) override;
    bool IsFocused() const override;
    WMError RequestFocus() const override;
    WSError UpdateWindowMode(WindowMode mode) override;
    WSError HandleBackEvent() override { return WSError::WS_OK; }
    WMError SetWindowGravity(WindowGravity gravity, uint32_t percent) override;
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    KeyboardAnimationConfig GetKeyboardAnimationConfig() override;

    void NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    void NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed) override;
    void NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info) override;
    void NotifyForegroundInteractiveStatus(bool interactive) override;

    WMError RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    WMError UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    WMError RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    WMError UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    WMError RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    WMError UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    void RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    void UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    WMError RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    WMError UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    WMError RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    WMError UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    WMError RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    WMError UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) override;
    WMError RegisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    WMError UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    void SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler) override;
    void SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer) override;

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
    void NotifyForegroundFailed(WMError ret);
    void NotifyBackgroundFailed(WMError ret);
    WSError MarkProcessed(int32_t eventId) override;
    void UpdateWindowSizeLimits();
    void UpdateTitleButtonVisibility();
    WSError NotifyDestroy() override;
    WSError NotifyCloseExistPipWindow() override;
    void NotifyAvoidAreaChange(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    void NotifyTouchDialogTarget() override;
    void NotifyScreenshot() override;
    void DumpSessionElementInfo(const std::vector<std::string>& params) override;
    // colorspace, gamut
    virtual bool IsSupportWideGamut() override;
    virtual void SetColorSpace(ColorSpace colorSpace) override;
    virtual ColorSpace GetColorSpace() override;
    WSError NotifyTouchOutside() override;

    WindowState state_ { WindowState::STATE_INITIAL };
    WindowState requestState_ { WindowState::STATE_INITIAL };
    WSError UpdateMaximizeMode(MaximizeMode mode) override;
    void NotifySessionForeground(uint32_t reason, bool withAnimation) override;
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WSError UpdateTitleInTargetPos(bool isShow, int32_t height) override;

protected:
    WMError Connect();
    bool IsWindowSessionInvalid() const;
    void NotifyAfterActive();
    void NotifyAfterInactive();
    void NotifyBeforeDestroy(std::string windowName);
    void ClearListenersById(int32_t persistentId);
    WMError WindowSessionCreateCheck();
    void UpdateDecorEnable(bool needNotify = false);
    void NotifyModeChange(WindowMode mode, bool hasDeco = true);
    WMError UpdateProperty(WSPropertyChangeAction action);
    WMError SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;
    virtual WMError SetLayoutFullScreenByApiVersion(bool status);
    void UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void NotifySizeChange(Rect rect, WindowSizeChangeReason reason);

    sptr<ISession> hostSession_;
    std::unique_ptr<Ace::UIContent> uiContent_;
    std::shared_ptr<AbilityRuntime::Context> context_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;

    sptr<WindowSessionProperty> property_;
    SystemSessionConfig windowSystemConfig_;
    NotifyNativeWinDestroyFunc notifyNativeFunc_;

    std::recursive_mutex mutex_;
    static std::map<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>> windowSessionMap_;
    // protect windowSessionMap_
    static std::shared_mutex windowSessionMutex_;
    static std::map<int32_t, std::vector<sptr<WindowSessionImpl>>> subWindowSessionMap_;
    bool isSystembarPropertiesSet_ = false;
    bool isIgnoreSafeAreaNeedNotify_ = false;
    bool isIgnoreSafeArea_ = false;
    bool isFocused_ { false };
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer_;

private:
    //Trans between colorGamut and colorSpace
    static ColorSpace GetColorSpaceFromSurfaceGamut(GraphicColorGamut colorGamut);
    static GraphicColorGamut GetSurfaceGamutFromColorSpace(ColorSpace colorSpace);

    template<typename T> WMError RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> WMError UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> EnableIfSame<T, IWindowLifeCycle, std::vector<sptr<IWindowLifeCycle>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowChangeListener, std::vector<sptr<IWindowChangeListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IAvoidAreaChangedListener, std::vector<sptr<IAvoidAreaChangedListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IDialogDeathRecipientListener, std::vector<sptr<IDialogDeathRecipientListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IDialogTargetTouchListener, std::vector<sptr<IDialogTargetTouchListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IOccupiedAreaChangeListener, std::vector<sptr<IOccupiedAreaChangeListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IScreenshotListener, std::vector<sptr<IScreenshotListener>>> GetListeners();
    template<typename T>
    EnableIfSame<T, ITouchOutsideListener, std::vector<sptr<ITouchOutsideListener>>> GetListeners();
    template<typename T> void ClearUselessListeners(std::map<int32_t, T>& listeners, int32_t persistentId);
    RSSurfaceNode::SharedPtr CreateSurfaceNode(std::string name, WindowType type);
    void NotifyAfterFocused();
    void NotifyAfterUnfocused(bool needNotifyUiContent = true);
    void NotifyAfterResumed();
    void NotifyAfterPaused();

    WMError SetUIContentInner(const std::string& contentInfo, napi_env env, napi_value storage,
        bool isdistributed, bool isLoadedByName, AppExecFwk::Ability* ability);

    bool IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const;
    void UpdateRectForRotation(const Rect& wmRect, const Rect& preRect, WindowSizeChangeReason wmReason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);

    static std::recursive_mutex globalMutex_;
    static std::map<int32_t, std::vector<sptr<IWindowLifeCycle>>> lifecycleListeners_;
    static std::map<int32_t, std::vector<sptr<IWindowChangeListener>>> windowChangeListeners_;
    static std::map<int32_t, std::vector<sptr<IAvoidAreaChangedListener>>> avoidAreaChangeListeners_;
    static std::map<int32_t, std::vector<sptr<IDialogDeathRecipientListener>>> dialogDeathRecipientListeners_;
    static std::map<int32_t, std::vector<sptr<IDialogTargetTouchListener>>> dialogTargetTouchListener_;
    static std::map<int32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> occupiedAreaChangeListeners_;
    static std::map<int32_t, std::vector<sptr<IScreenshotListener>>> screenshotListeners_;
    static std::map<int32_t, std::vector<sptr<ITouchOutsideListener>>> touchOutsideListeners_;

    // FA only
    sptr<IAceAbilityHandler> aceAbilityHandler_;

    WindowSizeChangeReason lastSizeChangeReason_ = WindowSizeChangeReason::END;
    bool postTaskDone_ = false;
    int16_t rotationAnimationCount_ { 0 };
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_SESSION_IMPL_H

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

#ifndef OHOS_ROSEN_WINDOW_IMPL_H
#define OHOS_ROSEN_WINDOW_IMPL_H

#include <map>
#include <string>

#include <ability_context.h>
#include <i_input_event_consumer.h>
#include <key_event.h>
#include <refbase.h>
#include <ui_content.h>
#include <ui/rs_surface_node.h>
#include <struct_multimodal.h>

#include "prepare_terminate_callback_stub.h"
#include "input_transfer_station.h"
#include "vsync_station.h"
#include "window.h"
#include "window_property.h"
#include "window_transition_info.h"
#include "wm_common_inner.h"
#include "wm_common.h"

using OHOS::AppExecFwk::DisplayOrientation;

namespace OHOS {
namespace Rosen {
union ColorParam {
#if BIG_ENDIANNESS
    struct {
        uint8_t alpha;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } argb;
#else
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } argb;
#endif
    uint32_t value;
};

const std::map<OHOS::AppExecFwk::DisplayOrientation, Orientation> ABILITY_TO_WMS_ORIENTATION_MAP {
    {OHOS::AppExecFwk::DisplayOrientation::UNSPECIFIED, Orientation::UNSPECIFIED},
    {OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE, Orientation::HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::PORTRAIT, Orientation::VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::FOLLOWRECENT, Orientation::LOCKED},
    {OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE_INVERTED, Orientation::REVERSE_HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::PORTRAIT_INVERTED, Orientation::REVERSE_VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION, Orientation::SENSOR},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE, Orientation::SENSOR_HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT, Orientation::SENSOR_VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_RESTRICTED, Orientation::AUTO_ROTATION_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED,
        Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED,
        Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::LOCKED, Orientation::LOCKED},
};

class WindowImpl : public Window, AAFwk::PrepareTerminateCallbackStub {
#define CALL_LIFECYCLE_LISTENER(windowLifecycleCb, listeners) \
    do {                                                      \
        for (auto& listener : (listeners)) {                  \
            if (listener != nullptr) {            \
                listener->windowLifecycleCb();    \
            }                                                 \
        }                                                     \
    } while (0)

#define CALL_LIFECYCLE_LISTENER_WITH_PARAM(windowLifecycleCb, listeners, param) \
    do {                                                                        \
        for (auto& listener : (listeners)) {                                    \
            if (listener != nullptr) {                              \
                listener->windowLifecycleCb(param);                 \
            }                                                                   \
        }                                                                       \
    } while (0)

#define CALL_UI_CONTENT(uiContentCb)                              \
    do {                                                          \
        std::lock_guard<std::recursive_mutex> lock(mutex_);       \
        if (uiContent_ != nullptr) {                              \
            uiContent_->uiContentCb();                            \
        }                                                         \
    } while (0)

public:
    explicit WindowImpl(const sptr<WindowOption>& option);
    ~WindowImpl();

    static sptr<Window> Find(const std::string& id);
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    static std::vector<sptr<Window>> GetSubWindow(uint32_t parantId);
    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const override;
    virtual Rect GetRect() const override;
    virtual Rect GetRequestRect() const override;
    virtual WindowType GetType() const override;
    virtual WindowMode GetMode() const override;
    virtual float GetAlpha() const override;
    virtual WindowState GetWindowState() const override;
    virtual WMError SetFocusable(bool isFocusable) override;
    virtual bool GetFocusable() const override;
    virtual WMError SetTouchable(bool isTouchable) override;
    virtual bool GetTouchable() const override;
    virtual const std::string& GetWindowName() const override;
    virtual uint32_t GetWindowId() const override;
    virtual uint32_t GetWindowFlags() const override;
    uint32_t GetRequestModeSupportInfo() const override;
    bool IsMainHandlerAvailable() const override;
    inline NotifyNativeWinDestroyFunc GetNativeDestroyCallback()
    {
        return notifyNativefunc_;
    }
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) const override;
    virtual bool IsFullScreen() const override;
    virtual bool IsLayoutFullScreen() const override;
    virtual WMError SetWindowType(WindowType type) override;
    virtual WMError SetWindowMode(WindowMode mode) override;
    virtual WMError SetAlpha(float alpha) override;
    virtual WMError SetTransform(const Transform& trans) override;
    virtual WMError AddWindowFlag(WindowFlag flag) override;
    virtual WMError RemoveWindowFlag(WindowFlag flag) override;
    virtual WMError SetWindowFlags(uint32_t flags) override;
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    virtual WMError UpdateSystemBarProperty(bool status);
    virtual WMError SetLayoutFullScreen(bool status) override;
    virtual WMError SetFullScreen(bool status) override;
    virtual const Transform& GetTransform() const override;
    virtual const Transform& GetZoomTransform() const;
    virtual WMError UpdateSurfaceNodeAfterCustomAnimation(bool isAdd) override;
    inline void SetWindowState(WindowState state)
    {
        state_ = state;
    }
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) override;

    WMError Create(uint32_t parentId,
        const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    virtual WMError Destroy() override;
    virtual WMError Show(uint32_t reason = 0, bool withAnimation = false) override;
    virtual WMError Hide(uint32_t reason = 0, bool withAnimation = false, bool isFromInnerkits = true) override;
    virtual WMError MoveTo(int32_t x, int32_t y) override;
    virtual WMError Resize(uint32_t width, uint32_t height) override;
    virtual WMError SetWindowGravity(WindowGravity gravity, uint32_t percent) override;
    virtual WMError SetKeepScreenOn(bool keepScreenOn) override;
    virtual bool IsKeepScreenOn() const override;
    virtual WMError SetTurnScreenOn(bool turnScreenOn) override;
    virtual bool IsTurnScreenOn() const override;
    virtual WMError SetBackgroundColor(const std::string& color) override;
    virtual WMError SetTransparent(bool isTransparent) override;
    virtual bool IsTransparent() const override;
    virtual WMError SetBrightness(float brightness) override;
    virtual float GetBrightness() const override;
    virtual WMError SetCallingWindow(uint32_t windowId) override;
    virtual WMError SetPrivacyMode(bool isPrivacyMode) override;
    virtual bool IsPrivacyMode() const override;
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) override;
    virtual WMError DisableAppWindowDecor() override;
    virtual WMError BindDialogTarget(sptr<IRemoteObject> targetToken) override;
    WmErrorCode RaiseToAppTop() override;
    virtual WMError SetAspectRatio(float ratio) override;
    virtual WMError ResetAspectRatio() override;
    virtual WMError SetSnapshotSkip(bool isSkip) override;
    // window effect
    virtual WMError SetCornerRadius(float cornerRadius) override;
    virtual WMError SetShadowRadius(float radius) override;
    virtual WMError SetShadowColor(std::string color) override;
    virtual WMError SetShadowOffsetX(float offsetX) override;
    virtual WMError SetShadowOffsetY(float offsetY) override;
    virtual WMError SetBlur(float radius) override;
    virtual WMError SetBackdropBlur(float radius) override;
    virtual WMError SetBackdropBlurStyle(WindowBlurStyle blurStyle) override;

    virtual bool IsDecorEnable() const override;
    virtual WMError Maximize() override;
    virtual WMError MaximizeFloating() override;
    virtual WMError Minimize() override;
    virtual WMError Recover() override;
    virtual WMError Close() override;
    virtual void StartMove() override;
    virtual WMError SetGlobalMaximizeMode(MaximizeMode mode) override;
    virtual MaximizeMode GetGlobalMaximizeMode() const override;

    virtual WMError RequestFocus() const override;
    virtual void SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer) override;

    virtual WMError RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    virtual WMError RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    virtual WMError UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    virtual WMError UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    virtual WMError RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    virtual WMError UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) override;
    virtual WMError RegisterDragListener(const sptr<IWindowDragListener>& listener) override;
    virtual WMError UnregisterDragListener(const sptr<IWindowDragListener>& listener) override;
    virtual WMError RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    virtual WMError UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener) override;
    virtual void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) override;
    virtual WMError RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    virtual WMError UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener) override;
    virtual WMError RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    virtual WMError UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener) override;
    virtual WMError RegisterAnimationTransitionController(
        const sptr<IAnimationTransitionController>& listener) override;
    virtual WMError RegisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    virtual WMError UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener) override;
    virtual WMError RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    virtual WMError UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener) override;
    virtual void RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    virtual void UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener) override;
    virtual void SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler) override;
    virtual void SetRequestModeSupportInfo(uint32_t modeSupportInfo) override;
    void UpdateRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void UpdateMode(WindowMode mode);
    void UpdateModeSupportInfo(uint32_t modeSupportInfo);
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) override;
    virtual void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& inputEvent) override;
    virtual void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) override;
    virtual int64_t GetVSyncPeriod() override;
    void UpdateFocusStatus(bool focused);
    virtual bool IsFocused() const override;
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;
    void UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    void UpdateWindowState(WindowState state);
    WmErrorCode UpdateSubWindowStateAndNotify(uint32_t parentId);
    WmErrorCode UpdateWindowStateWhenShow();
    WmErrorCode UpdateWindowStateWhenHide();
    sptr<WindowProperty> GetWindowProperty();
    void UpdateDragEvent(const PointInfo& point, DragEvent event);
    void UpdateDisplayId(DisplayId from, DisplayId to);
    void UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void UpdateActiveStatus(bool isActive);
    void NotifyTouchOutside();
    void NotifyScreenshot();
    void NotifyTouchDialogTarget(int32_t posX = 0, int32_t posY = 0) override;
    void NotifyDestroy();
    void NotifyForeground();
    void NotifyBackground();
    void UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn);
    void PerformBack() override;
    void NotifyForegroundInteractiveStatus(bool interactive);

    virtual WMError NapiSetUIContent(const std::string& contentInfo, napi_env env,
        napi_value storage, bool isdistributed, sptr<IRemoteObject> token, AppExecFwk::Ability* ability) override;
    virtual WMError SetUIContentByName(const std::string& contentInfo, napi_env env, napi_value storage,
        AppExecFwk::Ability* ability) override;
    virtual WMError SetUIContentByAbc(const std::string& abcPath, napi_env env, napi_value storage,
        AppExecFwk::Ability* ability) override;
    virtual std::string GetContentInfo() override;
    virtual const std::shared_ptr<AbilityRuntime::Context> GetContext() const override;
    virtual Ace::UIContent* GetUIContent() const override;
    virtual void OnNewWant(const AAFwk::Want& want) override;
    virtual void SetRequestedOrientation(Orientation) override;
    virtual Orientation GetRequestedOrientation() override;
    virtual void SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel) override;
    virtual WMError SetTouchHotAreas(const std::vector<Rect>& rects) override;
    virtual void GetRequestedTouchHotAreas(std::vector<Rect>& rects) const override;
    virtual WMError SetAPPWindowLabel(const std::string& label) override;
    virtual WMError SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon) override;

    // colorspace, gamut
    virtual bool IsSupportWideGamut() override;
    virtual void SetColorSpace(ColorSpace colorSpace) override;
    virtual ColorSpace GetColorSpace() override;

    virtual void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) override;
    virtual std::shared_ptr<Media::PixelMap> Snapshot() override;
    virtual WMError NotifyMemoryLevel(int32_t level) override;
    virtual bool IsAllowHaveSystemSubWindow() override;
    virtual KeyboardAnimationConfig GetKeyboardAnimationConfig() override;
    void RestoreSplitWindowMode(uint32_t mode);

    virtual void SetNeedDefaultAnimation(bool needDefaultAnimation) override;

    virtual void DoPrepareTerminate() override;
    void PendingClose();

    WMError SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) override;
    virtual WMError SetSpecificBarProperty(WindowType type, const SystemBarProperty& property) override;
private:
    template<typename T1, typename T2, typename Ret>
    using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;
    template<typename T> WMError RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> WMError UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> void ClearUselessListeners(std::map<uint32_t, T>& listeners, uint32_t winId)
    {
        listeners.erase(winId);
    }
    template<typename T>
    inline EnableIfSame<T, IWindowLifeCycle, std::vector<sptr<IWindowLifeCycle>>> GetListeners()
    {
        std::vector<sptr<IWindowLifeCycle>> lifecycleListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : lifecycleListeners_[GetWindowId()]) {
                lifecycleListeners.push_back(listener);
            }
        }
        return lifecycleListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IWindowChangeListener, std::vector<sptr<IWindowChangeListener>>> GetListeners()
    {
        std::vector<sptr<IWindowChangeListener>> windowChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : windowChangeListeners_[GetWindowId()]) {
                windowChangeListeners.push_back(listener);
            }
        }
        return windowChangeListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IAvoidAreaChangedListener, std::vector<sptr<IAvoidAreaChangedListener>>> GetListeners()
    {
        std::vector<sptr<IAvoidAreaChangedListener>> avoidAreaChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : avoidAreaChangeListeners_[GetWindowId()]) {
                avoidAreaChangeListeners.push_back(listener);
            }
        }
        return avoidAreaChangeListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IDisplayMoveListener, std::vector<sptr<IDisplayMoveListener>>> GetListeners()
    {
        std::vector<sptr<IDisplayMoveListener>> displayMoveListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : displayMoveListeners_) {
                displayMoveListeners.push_back(listener);
            }
        }
        return displayMoveListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IScreenshotListener, std::vector<sptr<IScreenshotListener>>> GetListeners()
    {
        std::vector<sptr<IScreenshotListener>> screenshotListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : screenshotListeners_[GetWindowId()]) {
                screenshotListeners.push_back(listener);
            }
        }
        return screenshotListeners;
    }
    template<typename T>
    inline EnableIfSame<T, ITouchOutsideListener, std::vector<sptr<ITouchOutsideListener>>> GetListeners()
    {
        std::vector<sptr<ITouchOutsideListener>> touchOutsideListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : touchOutsideListeners_[GetWindowId()]) {
                touchOutsideListeners.push_back(listener);
            }
        }
        return touchOutsideListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IDialogTargetTouchListener, std::vector<sptr<IDialogTargetTouchListener>>> GetListeners()
    {
        std::vector<sptr<IDialogTargetTouchListener>> dialogTargetTouchListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : dialogTargetTouchListeners_[GetWindowId()]) {
                dialogTargetTouchListeners.push_back(listener);
            }
        }
        return dialogTargetTouchListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IWindowDragListener, std::vector<sptr<IWindowDragListener>>> GetListeners()
    {
        std::vector<sptr<IWindowDragListener>> windowDragListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : windowDragListeners_) {
                windowDragListeners.push_back(listener);
            }
        }
        return windowDragListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IOccupiedAreaChangeListener, std::vector<sptr<IOccupiedAreaChangeListener>>> GetListeners()
    {
        std::vector<sptr<IOccupiedAreaChangeListener>> occupiedAreaChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(globalMutex_);
            for (auto& listener : occupiedAreaChangeListeners_[GetWindowId()]) {
                occupiedAreaChangeListeners.push_back(listener);
            }
        }
        return occupiedAreaChangeListeners;
    }
    template<typename T>
    inline EnableIfSame<T, IDialogDeathRecipientListener, wptr<IDialogDeathRecipientListener>> GetListener()
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        return dialogDeathRecipientListener_[GetWindowId()];
    }
    inline void NotifyAfterForeground(bool needNotifyListeners = true, bool needNotifyUiContent = true)
    {
        if (needNotifyListeners) {
            auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
            CALL_LIFECYCLE_LISTENER(AfterForeground, lifecycleListeners);
        }
        if (needNotifyUiContent) {
            CALL_UI_CONTENT(Foreground);
        }
    }
    inline void NotifyAfterBackground(bool needNotifyListeners = true, bool needNotifyUiContent = true)
    {
        if (needNotifyListeners) {
            auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
            CALL_LIFECYCLE_LISTENER(AfterBackground, lifecycleListeners);
        }
        if (needNotifyUiContent) {
            CALL_UI_CONTENT(Background);
        }
    }
    inline void NotifyAfterFocused()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterFocused, lifecycleListeners);
        CALL_UI_CONTENT(Focus);
    }
    inline void NotifyAfterUnfocused(bool needNotifyUiContent = true)
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        // use needNotifyUinContent to separate ui content callbacks
        CALL_LIFECYCLE_LISTENER(AfterUnfocused, lifecycleListeners);
        if (needNotifyUiContent) {
            CALL_UI_CONTENT(UnFocus);
        }
    }
    inline void NotifyAfterResumed()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterResumed, lifecycleListeners);
    }
    inline void NotifyAfterPaused()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterPaused, lifecycleListeners);
    }
    inline void NotifyBeforeDestroy(std::string windowName)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (uiContent_ != nullptr) {
            auto uiContent = std::move(uiContent_);
            uiContent_ = nullptr;
            uiContent->Destroy();
        }
        if (notifyNativefunc_) {
            notifyNativefunc_(windowName);
        }
    }
    inline void NotifyBeforeSubWindowDestroy(sptr<WindowImpl> window)
    {
        auto uiContent = window->GetUIContent();
        if (uiContent != nullptr) {
            uiContent->Destroy();
        }
        if (window->GetNativeDestroyCallback()) {
            window->GetNativeDestroyCallback()(window->GetWindowName());
        }
    }
    inline void NotifyAfterActive()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterActive, lifecycleListeners);
    }
    inline void NotifyAfterInactive()
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterInactive, lifecycleListeners);
    }
    inline void NotifyForegroundFailed(WMError ret)
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER_WITH_PARAM(ForegroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
    }
    inline void NotifyBackgroundFailed(WMError ret)
    {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER_WITH_PARAM(BackgroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
    }
    inline bool IsStretchableReason(WindowSizeChangeReason reason)
    {
        return reason == WindowSizeChangeReason::DRAG || reason == WindowSizeChangeReason::DRAG_END ||
            reason == WindowSizeChangeReason::DRAG_START || reason == WindowSizeChangeReason::RECOVER ||
            reason == WindowSizeChangeReason::MOVE || reason == WindowSizeChangeReason::UNDEFINED;
    }
    void ClearListenersById(uint32_t winId);
    void NotifySizeChange(Rect rect, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void NotifyAvoidAreaChange(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    void NotifyDisplayMoveChange(DisplayId from, DisplayId to);
    void NotifyOccupiedAreaChange(const sptr<OccupiedAreaChangeInfo>& info,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void NotifyModeChange(WindowMode mode, bool hasDeco = true);
    void NotifyDragEvent(const PointInfo& point, DragEvent event);
    void DestroyDialogWindow();
    void DestroyFloatingWindow();
    void DestroySubWindow();
    void SetDefaultOption(); // for api7
    bool IsWindowValid() const;
    static sptr<Window> FindWindowById(uint32_t WinId);
    void TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void ConsumeMoveOrDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void ReadyToMoveOrDragWindow(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const MMI::PointerEvent::PointerItem& pointerItem);
    void EndMoveOrDragWindow(int32_t posX, int32_t posY, int32_t pointId, int32_t sourceType);
    void ResetMoveOrDragState();
    bool IsPointerEventConsumed();
    bool IsPointInDragHotZone(int32_t startPointPosX, int32_t startPointPosY, int32_t sourceType);
    void AdjustWindowAnimationFlag(bool withAnimation = false);
    void MapFloatingWindowToAppIfNeeded();
    void MapDialogWindowToAppIfNeeded();
    WMError UpdateProperty(PropertyChangeAction action);
    WMError Destroy(bool needNotifyServer, bool needClearListener = true);
    WMError SetBackgroundColor(uint32_t color);
    uint32_t GetBackgroundColor() const;
    void InitAbilityInfo();
    std::shared_ptr<AppExecFwk::AbilityInfo> GetOriginalAbilityInfo() const;
    void RecordLifeCycleExceptionEvent(LifeCycleEvent event, WMError errCode) const;
    std::string TransferLifeCycleEventToString(LifeCycleEvent type) const;
    Rect GetSystemAlarmWindowDefaultSize(Rect defaultRect);
    void HandleModeChangeHotZones(int32_t posX, int32_t posY);
    WMError NotifyWindowTransition(TransitionReason reason);
    void UpdatePointerEventForStretchableWindow(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void UpdateDragType(int32_t startPointPosX, int32_t startPointPosY);
    void HandleBackKeyPressedEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);
    bool CheckCameraFloatingWindowMultiCreated(WindowType type);
    void GetConfigurationFromAbilityInfo();
    void UpdateTitleButtonVisibility();
    void SetModeSupportInfo(uint32_t modeSupportInfo);
    uint32_t GetModeSupportInfo() const;
    WMError PreProcessShow(uint32_t reason, bool withAnimation);
    bool NeedToStopShowing();
    void CalculateStartRectExceptHotZone(float virtualPixelRatio);
    void SetSystemConfig();
    void TransformSurfaceNode(const Transform& trans);
    bool IsAppMainOrSunOrFloatingWindow();
    bool IsAppMainOrSubOrFloatingWindow();
    WMError WindowCreateCheck(uint32_t parentId);
    uint32_t CalculatePointerDirection(int32_t pointerX, int32_t pointerY);
    void HandlePointerStyle(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    RSSurfaceNode::SharedPtr CreateSurfaceNode(std::string name, WindowType type);
    void UpdateWindowStateUnfrozen();
    void UpdateViewportConfig(const Rect& rect, const sptr<class Display>& display, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void UpdateDecorEnable(bool needNotify = false);
    WMError SetFloatingMaximize(bool isEnter);
    WMError SetUIContentInner(const std::string& contentInfo, napi_env env, napi_value storage,
        WindowSetUIContentType type, AppExecFwk::Ability* ability);
    std::shared_ptr<std::vector<uint8_t>> GetAbcContent(const std::string& abcPath);

    // colorspace, gamut
    using ColorSpaceConvertMap = struct {
        ColorSpace colorSpace;
        GraphicColorGamut surfaceColorGamut;
    };
    static const ColorSpaceConvertMap colorSpaceConvertMap[];
    static ColorSpace GetColorSpaceFromSurfaceGamut(GraphicColorGamut colorGamut);
    static GraphicColorGamut GetSurfaceGamutFromColorSpace(ColorSpace colorSpace);
    void ChangePropertyByApiVersion();
    void PerfLauncherHotAreaIfNeed(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void SetDefaultDisplayIdIfNeed();

    static std::map<std::string, std::pair<uint32_t, sptr<Window>>> windowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> subWindowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> appFloatingWindowMap_;
    static std::map<uint32_t, std::vector<sptr<WindowImpl>>> appDialogWindowMap_;
    sptr<WindowProperty> property_;
    WindowState state_ { WindowState::STATE_INITIAL };
    WindowState subWindowState_ {WindowState::STATE_INITIAL};
    WindowTag windowTag_;
    sptr<IAceAbilityHandler> aceAbilityHandler_;
    static std::map<uint32_t, std::vector<sptr<IScreenshotListener>>> screenshotListeners_;
    static std::map<uint32_t, std::vector<sptr<ITouchOutsideListener>>> touchOutsideListeners_;
    static std::map<uint32_t, std::vector<sptr<IDialogTargetTouchListener>>> dialogTargetTouchListeners_;
    static std::map<uint32_t, std::vector<sptr<IWindowLifeCycle>>> lifecycleListeners_;
    static std::map<uint32_t, std::vector<sptr<IWindowChangeListener>>> windowChangeListeners_;
    static std::map<uint32_t, std::vector<sptr<IAvoidAreaChangedListener>>> avoidAreaChangeListeners_;
    std::vector<sptr<IWindowDragListener>> windowDragListeners_;
    std::vector<sptr<IDisplayMoveListener>> displayMoveListeners_;
    static std::map<uint32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> occupiedAreaChangeListeners_;
    static std::map<uint32_t, sptr<IDialogDeathRecipientListener>> dialogDeathRecipientListener_;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer_;
    sptr<IAnimationTransitionController> animationTransitionController_;
    NotifyNativeWinDestroyFunc notifyNativefunc_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::string name_;
    std::unique_ptr<Ace::UIContent> uiContent_;
    std::shared_ptr<AbilityRuntime::Context> context_;
    std::recursive_mutex mutex_;
    std::recursive_mutex windowStateMutex_;
    static std::recursive_mutex globalMutex_;
    const float SYSTEM_ALARM_WINDOW_WIDTH_RATIO = 0.8;
    const float SYSTEM_ALARM_WINDOW_HEIGHT_RATIO = 0.3;
    WindowSizeChangeReason lastSizeChangeReason_ = WindowSizeChangeReason::END;

    sptr<MoveDragProperty> moveDragProperty_;
    SystemConfig windowSystemConfig_;
    bool isOriginRectSet_ = false;
    bool needRemoveWindowInputChannel_ = false;
    bool isMainHandlerAvailable_ = true;
    bool isAppFloatingWindow_ = false;
    bool isFocused_ = false;
    uint32_t mouseStyleID_ = 0;
    bool isIgnoreSafeAreaNeedNotify_ = false;
    bool isIgnoreSafeArea_ = false;
    bool needDefaultAnimation_ = true;
    bool postTaskDone_ = false;
    const std::map<DragType, uint32_t> STYLEID_MAP = {
        {DragType::DRAG_UNDEFINED, MMI::MOUSE_ICON::DEFAULT},
        {DragType::DRAG_BOTTOM_OR_TOP, MMI::MOUSE_ICON::NORTH_SOUTH},
        {DragType::DRAG_LEFT_OR_RIGHT, MMI::MOUSE_ICON::WEST_EAST},
        {DragType::DRAG_LEFT_TOP_CORNER, MMI::MOUSE_ICON::NORTH_WEST_SOUTH_EAST},
        {DragType::DRAG_RIGHT_TOP_CORNER, MMI::MOUSE_ICON::NORTH_EAST_SOUTH_WEST}
    };
    static float ConvertRadiusToSigma(float radius)
    {
        constexpr float BlurSigmaScale = 0.57735f;
        return radius > 0.0f ? BlurSigmaScale * radius + SK_ScalarHalf : 0.0f;
    }
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    bool needNotifyFocusLater_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_IMPL_H

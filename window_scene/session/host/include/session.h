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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_H
#include <list>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include <event_handler.h>

#include "accessibility_element_info.h"
#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/host/include/zidl/session_stub.h"
#include "session/host/include/scene_persistence.h"
#include "wm_common.h"
#include "occupied_area_change_info.h"
#include "window_visibility_info.h"
#include "pattern_detach_callback_interface.h"

namespace OHOS::MMI {
class PointerEvent;
class KeyEvent;
class AxisEvent;
enum class WindowArea;
} // namespace OHOS::MMI

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {
class RSSurfaceNode;
class RSTransaction;
class RSSyncTransactionController;
using NotifyPendingSessionActivationFunc = std::function<void(SessionInfo& info)>;
using NotifyChangeSessionVisibilityWithStatusBarFunc = std::function<void(SessionInfo& info, const bool visible)>;
using NotifySessionStateChangeFunc = std::function<void(const SessionState& state)>;
using NotifyBufferAvailableChangeFunc = std::function<void(const bool isAvailable)>;
using NotifySessionStateChangeNotifyManagerFunc = std::function<void(int32_t persistentId, const SessionState& state)>;
using NotifyRequestFocusStatusNotifyManagerFunc =
    std::function<void(int32_t persistentId, const bool isFocused, const bool byForeground, FocusChangeReason reason)>;
using NotifyBackPressedFunc = std::function<void(const bool needMoveToBackground)>;
using NotifySessionFocusableChangeFunc = std::function<void(const bool isFocusable)>;
using NotifySessionTouchableChangeFunc = std::function<void(const bool touchable)>;
using NotifyClickFunc = std::function<void()>;
using NotifyTerminateSessionFunc = std::function<void(const SessionInfo& info)>;
using NotifyTerminateSessionFuncNew =
    std::function<void(const SessionInfo& info, bool needStartCaller, bool isFromBroker)>;
using NotifyTerminateSessionFuncTotal = std::function<void(const SessionInfo& info, TerminateType terminateType)>;
using NofitySessionLabelUpdatedFunc = std::function<void(const std::string &label)>;
using NofitySessionIconUpdatedFunc = std::function<void(const std::string &iconPath)>;
using NotifySessionExceptionFunc = std::function<void(const SessionInfo& info, bool needRemoveSession)>;
using NotifySessionSnapshotFunc = std::function<void(const int32_t& persistentId)>;
using NotifyPendingSessionToForegroundFunc = std::function<void(const SessionInfo& info)>;
using NotifyPendingSessionToBackgroundForDelegatorFunc = std::function<void(const SessionInfo& info)>;
using NotifyRaiseToTopForPointDownFunc = std::function<void()>;
using NotifyUIRequestFocusFunc = std::function<void()>;
using NotifyUILostFocusFunc = std::function<void()>;
using NotifySessionInfoLockedStateChangeFunc = std::function<void(const bool lockedState)>;
using GetStateFromManagerFunc = std::function<bool(const ManagerState key)>;
using NotifySystemSessionPointerEventFunc = std::function<void(std::shared_ptr<MMI::PointerEvent> pointerEvent)>;
using NotifySessionInfoChangeNotifyManagerFunc = std::function<void(int32_t persistentid)>;
using NotifySystemSessionKeyEventFunc = std::function<bool(std::shared_ptr<MMI::KeyEvent> keyEvent,
    bool isPreImeEvent)>;
using NotifyContextTransparentFunc = std::function<void()>;

class ILifecycleListener {
public:
    virtual void OnActivation() = 0;
    virtual void OnConnect() = 0;
    virtual void OnForeground() = 0;
    virtual void OnBackground() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnExtensionDied() = 0;
    virtual void OnExtensionTimeout(int32_t errorCode) = 0;
    virtual void OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) = 0;
    virtual void OnDrawingCompleted() {}
};

enum class LifeCycleTaskType : uint32_t {
    START,
    STOP
};

enum class DetectTaskState : uint32_t {
    NO_TASK,
    ATTACH_TASK,
    DETACH_TASK
};

struct DetectTaskInfo {
    WindowMode taskWindowMode = WindowMode::WINDOW_MODE_UNDEFINED;
    DetectTaskState taskState = DetectTaskState::NO_TASK;
};

class Session : public SessionStub {
public:
    friend class HidumpController;
    using Task = std::function<void()>;
    explicit Session(const SessionInfo& info);
    virtual ~Session() = default;
    bool isKeyboardPanelEnabled_ = false;
    void SetEventHandler(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<AppExecFwk::EventHandler>& exportHandler = nullptr);

    virtual WSError ConnectInner(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        int32_t pid = -1, int32_t uid = -1, const std::string& identityToken = "");
    WSError Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr, int32_t pid = -1, int32_t uid = -1);
    WSError Foreground(sptr<WindowSessionProperty> property, bool isFromClient = false) override;
    WSError Background(bool isFromClient = false) override;
    WSError Disconnect(bool isFromClient = false) override;
    WSError Show(sptr<WindowSessionProperty> property) override;
    WSError Hide() override;
    WSError DrawingCompleted() override;
    void ResetSessionConnectState();
    
    bool RegisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);
    bool UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);

    void NotifyActivation();
    void NotifyConnect();
    void NotifyForeground();
    void NotifyBackground();
    void NotifyDisconnect();
    void NotifyExtensionDied() override;
    void NotifyExtensionTimeout(int32_t errorCode) override;
    void NotifyTransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) override;

    virtual WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool needNotifyClient = true);
    virtual WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);

    virtual WSError NotifyClientToUpdateRect(std::shared_ptr<RSTransaction> rsTransaction) { return WSError::WS_OK; }
    WSError TransferBackPressedEventForConsumed(bool& isConsumed);
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
        bool isPreImeEvent = false);
    WSError TransferFocusActiveEvent(bool isFocusActive);
    WSError TransferFocusStateEvent(bool focusState);
    virtual WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) { return WSError::WS_OK; }

    int32_t GetPersistentId() const;
    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const;
    void SetLeashWinSurfaceNode(std::shared_ptr<RSSurfaceNode> leashWinSurfaceNode);
    std::shared_ptr<RSSurfaceNode> GetLeashWinSurfaceNode() const;
    std::shared_ptr<Media::PixelMap> GetSnapshot() const;
    std::shared_ptr<Media::PixelMap> Snapshot(const float scaleParam = 0.0f) const;
    void SaveSnapshot(bool useFfrt);
    SessionState GetSessionState() const;
    virtual void SetSessionState(SessionState state);
    void SetSessionInfoAncoSceneState(int32_t ancoSceneState);
    void SetSessionInfoTime(const std::string& time);
    void SetSessionInfoAbilityInfo(const std::shared_ptr<AppExecFwk::AbilityInfo>& abilityInfo);
    void SetSessionInfoWant(const std::shared_ptr<AAFwk::Want>& want);
    void SetSessionInfoPersistentId(int32_t persistentId);
    void SetSessionInfoCallerPersistentId(int32_t callerPersistentId);
    void SetSessionInfoContinueState(ContinueState state);
    void SetSessionInfoLockedState(bool lockedState);
    void SetSessionInfoIsClearSession(bool isClearSession);
    void SetSessionInfoAffinity(std::string affinity);
    void GetCloseAbilityWantAndClean(AAFwk::Want& outWant);
    void SetSessionInfo(const SessionInfo& info);
    const SessionInfo& GetSessionInfo() const;
    void SetScreenId(uint64_t screenId);
    WindowType GetWindowType() const;
    float GetAspectRatio() const;
    WSError SetAspectRatio(float ratio) override;
    void SetFocusedOnShow(bool focusedOnShow);
    bool IsFocusedOnShow() const;
    WSError SetSessionProperty(const sptr<WindowSessionProperty>& property);
    sptr<WindowSessionProperty> GetSessionProperty() const;
    void SetSessionRect(const WSRect& rect);
    WSRect GetSessionRect() const;
    void SetSessionRequestRect(const WSRect& rect);
    WSRect GetSessionRequestRect() const;
    std::string GetWindowName() const;
    void SetSessionLastRect(const WSRect& rect);
    WSRect GetSessionLastRect() const;

    virtual WSError SetActive(bool active);
    virtual WSError UpdateSizeChangeReason(SizeChangeReason reason);
    SizeChangeReason GetSizeChangeReason() const { return reason_; }
    virtual WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    WSError UpdateDensity();
    WSError UpdateOrientation();

    void SetShowRecent(bool showRecent);
    void SetSystemActive(bool systemActive);
    bool GetShowRecent() const;
    void SetOffset(float x, float y);
    float GetOffsetX() const;
    float GetOffsetY() const;
    void SetBounds(const WSRectF& bounds);
    WSRectF GetBounds();
    void SetRotation(Rotation rotation);
    Rotation GetRotation() const;
    void SetBufferAvailable(bool bufferAvailable);
    bool GetBufferAvailable() const;
    void SetNeedSnapshot(bool needSnapshot);
    virtual void SetExitSplitOnBackground(bool isExitSplitOnBackground);
    virtual bool IsExitSplitOnBackground() const;
    virtual bool NeedStartingWindowExitAnimation() const { return true; }

    void SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func);
    void SetChangeSessionVisibilityWithStatusBarEventListener(
        const NotifyChangeSessionVisibilityWithStatusBarFunc& func);
    void SetTerminateSessionListener(const NotifyTerminateSessionFunc& func);
    WSError TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller, bool isFromBroker);
    void SetTerminateSessionListenerNew(const NotifyTerminateSessionFuncNew& func);
    void SetSessionExceptionListener(const NotifySessionExceptionFunc& func, bool fromJsScene);
    void SetSessionSnapshotListener(const NotifySessionSnapshotFunc& func);
    WSError TerminateSessionTotal(const sptr<AAFwk::SessionInfo> info, TerminateType terminateType);
    void SetTerminateSessionListenerTotal(const NotifyTerminateSessionFuncTotal& func);
    WSError Clear();
    WSError SetSessionLabel(const std::string &label);
    void SetUpdateSessionLabelListener(const NofitySessionLabelUpdatedFunc& func);
    WSError SetSessionIcon(const std::shared_ptr<Media::PixelMap> &icon);
    void SetUpdateSessionIconListener(const NofitySessionIconUpdatedFunc& func);
    void SetSessionStateChangeListenser(const NotifySessionStateChangeFunc& func);
    void SetBufferAvailableChangeListener(const NotifyBufferAvailableChangeFunc& func);
    void UnregisterSessionChangeListeners();
    void SetSessionStateChangeNotifyManagerListener(const NotifySessionStateChangeNotifyManagerFunc& func);
    void SetSessionInfoChangeNotifyManagerListener(const NotifySessionInfoChangeNotifyManagerFunc& func);
    void SetRequestFocusStatusNotifyManagerListener(const NotifyRequestFocusStatusNotifyManagerFunc& func);
    void SetNotifyUIRequestFocusFunc(const NotifyUIRequestFocusFunc& func);
    void SetNotifyUILostFocusFunc(const NotifyUILostFocusFunc& func);
    void SetGetStateFromManagerListener(const GetStateFromManagerFunc& func);

    void SetSystemConfig(const SystemSessionConfig& systemConfig);
    void SetSnapshotScale(const float snapshotScale);
    void SetBackPressedListenser(const NotifyBackPressedFunc& func);
    virtual WSError ProcessBackEvent(); // send back event to session_stage
    WSError MarkProcessed(int32_t eventId) override;

    sptr<ScenePersistence> GetScenePersistence() const;
    void SetParentSession(const sptr<Session>& session);
    sptr<Session> GetParentSession() const;
    void BindDialogToParentSession(const sptr<Session>& session);
    void RemoveDialogToParentSession(const sptr<Session>& session);
    std::vector<sptr<Session>> GetDialogVector() const;
    void ClearDialogVector();
    WSError NotifyDestroy();
    WSError NotifyCloseExistPipWindow();

    void SetPendingSessionToForegroundListener(const NotifyPendingSessionToForegroundFunc& func);
    WSError PendingSessionToForeground();
    void SetPendingSessionToBackgroundForDelegatorListener(const NotifyPendingSessionToBackgroundForDelegatorFunc&
        func);
    WSError PendingSessionToBackgroundForDelegator();

    void SetSessionFocusableChangeListener(const NotifySessionFocusableChangeFunc& func);
    void SetSessionTouchableChangeListener(const NotifySessionTouchableChangeFunc& func);
    void SetClickListener(const NotifyClickFunc& func);
    void NotifySessionFocusableChange(bool isFocusable);
    void NotifySessionTouchableChange(bool touchable);
    void NotifyClick();
    void NotifyRequestFocusStatusNotifyManager(bool isFocused, bool byForeground = true,
        FocusChangeReason reason = FocusChangeReason::DEFAULT);
    void NotifyUIRequestFocus();
    virtual void NotifyUILostFocus();
    bool GetStateFromManager(const ManagerState key);
    virtual void PresentFoucusIfNeed(int32_t pointerAcrion);
    virtual WSError UpdateFocus(bool isFocused);
    WSError NotifyFocusStatus(bool isFocused);
    virtual WSError UpdateWindowMode(WindowMode mode);
    WSError SetCompatibleModeInPc(bool enable, bool isSupportDragInPcCompatibleMode);
    virtual WSError SetSystemSceneBlockingFocus(bool blocking);
    bool GetBlockingFocus() const;
    WSError SetFocusable(bool isFocusable);
    bool NeedNotify() const;
    void SetNeedNotify(bool needNotify);
    bool GetFocusable() const;
    bool IsFocused() const;
    WSError SetTouchable(bool touchable);
    bool GetTouchable() const;
    void SetForceTouchable(bool touchable);
    virtual void SetSystemTouchable(bool touchable);
    bool GetSystemTouchable() const;
    virtual WSError SetVisible(bool isVisible);
    bool GetVisible() const;
    bool GetFocused() const;
    WSError SetVisibilityState(WindowVisibilityState state);
    WindowVisibilityState GetVisibilityState() const;
    WSError SetDrawingContentState(bool isRSDrawing);
    bool GetDrawingContentState() const;
    WSError SetBrightness(float brightness);
    float GetBrightness() const;
    void NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info,
                                      const std::shared_ptr<RSTransaction>& rsTransaction = nullptr);
    void SetSessionInfoLockedStateChangeListener(const NotifySessionInfoLockedStateChangeFunc& func);
    void NotifySessionInfoLockedStateChange(bool lockedState);
    void SetContextTransparentFunc(const NotifyContextTransparentFunc& func);
    void NotifyContextTransparent();
    bool NeedCheckContextTransparent() const;

    bool IsSessionValid() const;
    bool IsActive() const;
    bool IsSystemActive() const;
    bool IsSystemSession() const;
    bool IsTerminated() const;
    bool IsSessionForeground() const;
    virtual bool IsAnco() const { return false; }
    virtual void SetBlankFlag(bool isAddBlank) {};
    virtual bool GetBlankFlag() const { return false; }
    virtual bool GetBufferAvailableCallbackEnable() const { return false; }

    sptr<IRemoteObject> dialogTargetToken_ = nullptr;
    int32_t GetWindowId() const;
    void SetAppIndex(const int32_t appIndex);
    int32_t GetAppIndex() const;
    void SetCallingPid(int32_t id);
    void SetCallingUid(int32_t id);
    int32_t GetCallingPid() const;
    int32_t GetCallingUid() const;
    void SetAbilityToken(sptr<IRemoteObject> token);
    sptr<IRemoteObject> GetAbilityToken() const;
    WindowMode GetWindowMode() const;
    virtual void SetZOrder(uint32_t zOrder);
    uint32_t GetZOrder() const;
    void SetUINodeId(uint32_t uiNodeId);
    uint32_t GetUINodeId() const;
    virtual void SetFloatingScale(float floatingScale);
    float GetFloatingScale() const;
    virtual void SetScale(float scaleX, float scaleY, float pivotX, float pivotY);
    float GetScaleX() const;
    float GetScaleY() const;
    float GetPivotX() const;
    float GetPivotY() const;
    void SetSCBKeepKeyboard(bool scbKeepKeyboardFlag);
    bool GetSCBKeepKeyboardFlag() const;

    void SetRaiseToAppTopForPointDownFunc(const NotifyRaiseToTopForPointDownFunc& func);
    void NotifyScreenshot();
    void RemoveLifeCycleTask(const LifeCycleTaskType &taskType);
    void PostLifeCycleTask(Task &&task, const std::string &name, const LifeCycleTaskType &taskType);
    WSError UpdateMaximizeMode(bool isMaximize);
    void NotifySessionForeground(uint32_t reason, bool withAnimation);
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits);
    void HandlePointDownDialog();
    bool CheckDialogOnForeground();
    virtual void PresentFocusIfPointDown();
    void ResetSnapshot();
    std::shared_ptr<Media::PixelMap> GetSnapshotPixelMap(const float oriScale = 1.0f, const float newScale = 1.0f);
    virtual std::vector<Rect> GetTouchHotAreas() const
    {
        return std::vector<Rect>();
    }

    virtual void SetTouchHotAreas(const std::vector<Rect>& touchHotAreas);

    void SetVpr(float vpr)
    {
        vpr_ = vpr;
    }

    bool operator==(const Session* session) const
    {
        if (session == nullptr) {
            return false;
        }
        return (persistentId_ == session->persistentId_ && callingPid_ == session->callingPid_);
    }

    bool operator!=(const Session* session) const
    {
        return !this->operator==(session);
    }

    virtual void HandleStyleEvent(MMI::WindowArea area) {};
    WSError SetPointerStyle(MMI::WindowArea area);
    const char* DumpPointerWindowArea(MMI::WindowArea area) const;
    WSRectF UpdateHotRect(const WSRect& rect);
    WSError RaiseToAppTopForPointDown();

    virtual void NotifyForegroundInteractiveStatus(bool interactive);
    WSError UpdateTitleInTargetPos(bool isShow, int32_t height);
    void SetNotifySystemSessionPointerEventFunc(const NotifySystemSessionPointerEventFunc& func);
    void SetNotifySystemSessionKeyEventFunc(const NotifySystemSessionKeyEventFunc& func);
    bool IsSystemInput();
    //ForegroundInteractiveStatus interface only for event use
    bool GetForegroundInteractiveStatus() const;
    virtual void SetForegroundInteractiveStatus(bool interactive);
    void SetAttachState(bool isAttach, WindowMode windowMode = WindowMode::WINDOW_MODE_UNDEFINED);
    bool GetAttachState() const;
    void RegisterDetachCallback(const sptr<IPatternDetachCallback>& callback);
    virtual int32_t GetCustomDecorHeight()
    {
        return 0;
    };
    WSError SwitchFreeMultiWindow(bool enable);
    SystemSessionConfig GetSystemConfig() const;
    void RectCheckProcess();
    virtual void RectCheck(uint32_t curWidth, uint32_t curHeight) {};
    void RectSizeCheckProcess(uint32_t curWidth, uint32_t curHeight, uint32_t minWidth,
        uint32_t minHeight, uint32_t maxFloatingWindowSize);
    DetectTaskInfo GetDetectTaskInfo() const;
    void SetDetectTaskInfo(const DetectTaskInfo& detectTaskInfo);
    WSError GetUIContentRemoteObj(sptr<IRemoteObject>& uiContentRemoteObj);
    void CreateWindowStateDetectTask(bool isAttach, WindowMode windowMode);
    void RegisterIsScreenLockedCallback(const std::function<bool()>& callback);
    std::string GetWindowDetectTaskName() const;
    void RemoveWindowDetectTask();
    virtual bool CheckGetAvoidAreaAvailable(AvoidAreaType type) { return true; }

protected:
    class SessionLifeCycleTask : public virtual RefBase {
    public:
        SessionLifeCycleTask(const Task &task, const std::string &name, const LifeCycleTaskType &type)
            : task(task), name(name), type(type) {}
        Task task;
        const std::string name;
        LifeCycleTaskType type;
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        bool running = false;
    };
    void StartLifeCycleTask(sptr<SessionLifeCycleTask> lifeCycleTask);
    void GeneratePersistentId(bool isExtension, int32_t persistentId);
    virtual void UpdateSessionState(SessionState state);
    void NotifySessionStateChange(const SessionState& state);
    void UpdateSessionTouchable(bool touchable);

    WSRectF UpdateTopBottomArea(const WSRectF& rect, MMI::WindowArea area);
    WSRectF UpdateLeftRightArea(const WSRectF& rect, MMI::WindowArea area);
    WSRectF UpdateInnerAngleArea(const WSRectF& rect, MMI::WindowArea area);
    virtual void UpdatePointerArea(const WSRect& rect);
    virtual bool CheckPointerEventDispatch(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const;
    bool IsTopDialog() const;
    void HandlePointDownDialog(int32_t pointAction);
    void NotifySessionInfoChange();

    void PostTask(Task&& task, const std::string& name = "sessionTask", int64_t delayTime = 0);
    void PostExportTask(Task&& task, const std::string& name = "sessionExportTask", int64_t delayTime = 0);
    template<typename SyncTask, typename Return = std::invoke_result_t<SyncTask>>
    Return PostSyncTask(SyncTask&& task, const std::string& name = "sessionTask")
    {
        Return ret;
        if (!handler_ || handler_->GetEventRunner()->IsCurrentRunnerThread()) {
            StartTraceForSyncTask(name);
            ret = task();
            FinishTraceForSyncTask();
            return ret;
        }
        auto syncTask = [&ret, &task, name]() {
            StartTraceForSyncTask(name);
            ret = task();
            FinishTraceForSyncTask();
        };
        handler_->PostSyncTask(std::move(syncTask), name, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        return ret;
    }

    static std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    int32_t persistentId_ = INVALID_SESSION_ID;
    std::atomic<SessionState> state_ = SessionState::STATE_DISCONNECT;
    SessionInfo sessionInfo_;
    std::recursive_mutex sessionInfoMutex_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<Media::PixelMap> snapshot_;
    sptr<ISessionStage> sessionStage_;
    std::mutex lifeCycleTaskQueueMutex_;
    std::list<sptr<SessionLifeCycleTask>> lifeCycleTaskQueue_;
    bool isActive_ = false;
    bool isSystemActive_ = false;
    WSRect winRect_;
    WSRect lastWinRect_;
    WSRectF bounds_;
    Rotation rotation_;
    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;
    std::atomic_bool isExitSplitOnBackground_ = false;
    bool isVisible_ = false;
    SizeChangeReason reason_ = SizeChangeReason::UNDEFINED;

    NotifyPendingSessionActivationFunc pendingSessionActivationFunc_;
    NotifyChangeSessionVisibilityWithStatusBarFunc changeSessionVisibilityWithStatusBarFunc_;
    NotifySessionStateChangeFunc sessionStateChangeFunc_;
    NotifyBufferAvailableChangeFunc bufferAvailableChangeFunc_;
    NotifySessionInfoChangeNotifyManagerFunc sessionInfoChangeNotifyManagerFunc_;
    NotifySessionStateChangeNotifyManagerFunc sessionStateChangeNotifyManagerFunc_;
    NotifyRequestFocusStatusNotifyManagerFunc requestFocusStatusNotifyManagerFunc_;
    NotifyUIRequestFocusFunc requestFocusFunc_;
    NotifyUILostFocusFunc lostFocusFunc_;
    GetStateFromManagerFunc getStateFromManagerFunc_;
    NotifyBackPressedFunc backPressedFunc_;
    NotifySessionFocusableChangeFunc sessionFocusableChangeFunc_;
    NotifySessionTouchableChangeFunc sessionTouchableChangeFunc_;
    NotifyClickFunc clickFunc_;
    NotifyTerminateSessionFunc terminateSessionFunc_;
    NotifyTerminateSessionFuncNew terminateSessionFuncNew_;
    NotifyTerminateSessionFuncTotal terminateSessionFuncTotal_;
    NofitySessionLabelUpdatedFunc updateSessionLabelFunc_;
    NofitySessionIconUpdatedFunc updateSessionIconFunc_;
    std::shared_ptr<NotifySessionExceptionFunc> sessionExceptionFunc_;
    std::shared_ptr<NotifySessionExceptionFunc> jsSceneSessionExceptionFunc_;
    NotifySessionSnapshotFunc notifySessionSnapshotFunc_;
    NotifyPendingSessionToForegroundFunc pendingSessionToForegroundFunc_;
    NotifyPendingSessionToBackgroundForDelegatorFunc pendingSessionToBackgroundForDelegatorFunc_;
    NotifyRaiseToTopForPointDownFunc raiseToTopForPointDownFunc_;
    NotifySessionInfoLockedStateChangeFunc sessionInfoLockedStateChangeFunc_;
    NotifySystemSessionPointerEventFunc systemSessionPointerEventFunc_;
    NotifySystemSessionKeyEventFunc systemSessionKeyEventFunc_;
    NotifyContextTransparentFunc contextTransparentFunc_;
    SystemSessionConfig systemConfig_;
    bool needSnapshot_ = false;
    float snapshotScale_ = 0.5;
    sptr<ScenePersistence> scenePersistence_ = nullptr;
    uint32_t zOrder_ = 0;
    uint32_t uiNodeId_ = 0;
    bool isFocused_ = false;
    bool blockingFocus_ {false};
    float aspectRatio_ = 0.0f;
    std::map<MMI::WindowArea, WSRectF> windowAreas_;
    bool isTerminating = false;
    float floatingScale_ = 1.0f;
    bool isDirty_ = false;
    std::recursive_mutex sizeChangeMutex_;
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    float pivotX_ = 0.0f;
    float pivotY_ = 0.0f;
    bool scbKeepKeyboardFlag_ = false;
    mutable std::shared_mutex dialogVecMutex_;
    std::vector<sptr<Session>> dialogVec_;
    mutable std::shared_mutex parentSessionMutex_;
    sptr<Session> parentSession_;
    sptr<IWindowEventChannel> windowEventChannel_;

    mutable std::mutex pointerEventMutex_;
    mutable std::shared_mutex keyEventMutex_;
    bool rectChangeListenerRegistered_ = false;

private:
    void HandleDialogForeground();
    void HandleDialogBackground();
    WSError HandleSubWindowClick(int32_t action);
    void SetWindowSessionProperty(const sptr<WindowSessionProperty>& property);

    template<typename T>
    bool RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener);
    template<typename T>
    bool UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener);
    bool IsStateMatch(bool isAttach) const;
    bool IsSupportDetectWindow(bool isAttach);
    bool ShouldCreateDetectTask(bool isAttach, WindowMode windowMode) const;
    bool ShouldCreateDetectTaskInRecent(bool newShowRecent, bool oldShowRecent, bool isAttach) const;
    void CreateDetectStateTask(bool isAttach, WindowMode windowMode);
    template<typename T1, typename T2, typename Ret>
    using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;
    template<typename T>
    inline EnableIfSame<T, ILifecycleListener, std::vector<std::weak_ptr<ILifecycleListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<ILifecycleListener>> lifecycleListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(lifecycleListenersMutex_);
            for (auto& listener : lifecycleListeners_) {
                lifecycleListeners.push_back(listener);
            }
        }
        return lifecycleListeners;
    }

    std::recursive_mutex lifecycleListenersMutex_;
    std::vector<std::shared_ptr<ILifecycleListener>> lifecycleListeners_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    std::shared_ptr<AppExecFwk::EventHandler> exportHandler_;
    std::function<bool()> isScreenLockedCallback_;

    mutable std::shared_mutex propertyMutex_;
    sptr<WindowSessionProperty> property_;

    mutable std::shared_mutex uiRequestFocusMutex_;
    mutable std::shared_mutex uiLostFocusMutex_;

    bool focusedOnShow_ = true;
    bool showRecent_ = false;
    bool bufferAvailable_ = false;

    WSRect preRect_;
    int32_t callingPid_ = -1;
    int32_t callingUid_ = -1;
    int32_t appIndex_ = { 0 };
    std::string callingBundleName_ { "unknow" };
    bool isRSVisible_ {false};
    WindowVisibilityState visibilityState_ { WINDOW_LAYER_STATE_MAX};
    bool needNotify_ {true};
    bool isRSDrawing_ {false};
    sptr<IRemoteObject> abilityToken_ = nullptr;
    float vpr_ { 1.5f };
    bool forceTouchable_ { true };
    bool systemTouchable_ { true };
    std::atomic_bool foregroundInteractiveStatus_ { true };
    std::atomic<bool> isAttach_{ false };

    std::shared_ptr<RSSurfaceNode> leashWinSurfaceNode_;
    mutable std::mutex leashWinSurfaceNodeMutex;
    sptr<IPatternDetachCallback> detachCallback_ = nullptr;
    DetectTaskInfo detectTaskInfo_;
    mutable std::shared_mutex detectTaskInfoMutex_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_H
diff --git a/window_window_manager-master/window_scene/session/host/include/session.h b/window_window_manager-master/window_scene/session/host/include/session.h
index 176722a..bd24d52 100644
--- a/window_window_manager-master/window_scene/session/host/include/session.h
+++ b/window_window_manager-master/window_scene/session/host/include/session.h
@@ -71,8 +71,8 @@ using NotifyPendingSessionToBackgroundForDelegatorFunc = std::function<void(cons
 using NotifyRaiseToTopForPointDownFunc = std::function<void()>;
 using NotifyUIRequestFocusFunc = std::function<void()>;
 using NotifyUILostFocusFunc = std::function<void()>;
-using GetStateFromManagerFunc = std::function<bool(const ManagerState key)>;
 using NotifySessionInfoLockedStateChangeFunc = std::function<void(const bool lockedState)>;
+using GetStateFromManagerFunc = std::function<bool(const ManagerState key)>;
 using NotifySystemSessionPointerEventFunc = std::function<void(std::shared_ptr<MMI::PointerEvent> pointerEvent)>;
 using NotifySessionInfoChangeNotifyManagerFunc = std::function<void(int32_t persistentid)>;
 using NotifySystemSessionKeyEventFunc = std::function<bool(std::shared_ptr<MMI::KeyEvent> keyEvent,
@@ -334,20 +334,20 @@ public:
     int32_t GetCallingUid() const;
     void SetAbilityToken(sptr<IRemoteObject> token);
     sptr<IRemoteObject> GetAbilityToken() const;
-    WindowMode GetWindowMode();
+    WindowMode GetWindowMode() const;
     virtual void SetZOrder(uint32_t zOrder);
     uint32_t GetZOrder() const;
     void SetUINodeId(uint32_t uiNodeId);
     uint32_t GetUINodeId() const;
     virtual void SetFloatingScale(float floatingScale);
     float GetFloatingScale() const;
-    void SetSCBKeepKeyboard(bool scbKeepKeyboardFlag);
-    bool GetSCBKeepKeyboardFlag() const;
     virtual void SetScale(float scaleX, float scaleY, float pivotX, float pivotY);
     float GetScaleX() const;
     float GetScaleY() const;
     float GetPivotX() const;
     float GetPivotY() const;
+    void SetSCBKeepKeyboard(bool scbKeepKeyboardFlag);
+    bool GetSCBKeepKeyboardFlag() const;
 
     void SetRaiseToAppTopForPointDownFunc(const NotifyRaiseToTopForPointDownFunc& func);
     void NotifyScreenshot();
@@ -397,11 +397,17 @@ public:
     void SetNotifySystemSessionPointerEventFunc(const NotifySystemSessionPointerEventFunc& func);
     void SetNotifySystemSessionKeyEventFunc(const NotifySystemSessionKeyEventFunc& func);
     bool IsSystemInput();
+    // ForegroundInteractiveStatus interface only for event use
     bool GetForegroundInteractiveStatus() const;
     virtual void SetForegroundInteractiveStatus(bool interactive);
     void SetAttachState(bool isAttach, WindowMode windowMode = WindowMode::WINDOW_MODE_UNDEFINED);
     bool GetAttachState() const;
     void RegisterDetachCallback(const sptr<IPatternDetachCallback>& callback);
+    virtual int32_t GetCustomDecorHeight()
+    {
+        return 0;
+    };
+    WSError SwitchFreeMultiWindow(bool enable);
     SystemSessionConfig GetSystemConfig() const;
     void RectCheckProcess();
     virtual void RectCheck(uint32_t curWidth, uint32_t curHeight) {};
@@ -414,11 +420,6 @@ public:
     void RegisterIsScreenLockedCallback(const std::function<bool()>& callback);
     std::string GetWindowDetectTaskName() const;
     void RemoveWindowDetectTask();
-    WSError SwitchFreeMultiWindow(bool enable);
-    virtual int32_t GetCustomDecorHeight()
-    {
-        return 0;
-    };
     virtual bool CheckGetAvoidAreaAvailable(AvoidAreaType type) { return true; }
 
 protected:
@@ -531,12 +532,13 @@ protected:
     std::map<MMI::WindowArea, WSRectF> windowAreas_;
     bool isTerminating = false;
     float floatingScale_ = 1.0f;
-    bool scbKeepKeyboardFlag_ = false;
     bool isDirty_ = false;
+    std::recursive_mutex sizeChangeMutex_;
     float scaleX_ = 1.0f;
     float scaleY_ = 1.0f;
     float pivotX_ = 0.0f;
     float pivotY_ = 0.0f;
+    bool scbKeepKeyboardFlag_ = false;
     mutable std::shared_mutex dialogVecMutex_;
     std::vector<sptr<Session>> dialogVec_;
     mutable std::shared_mutex parentSessionMutex_;
@@ -550,7 +552,6 @@ protected:
 private:
     void HandleDialogForeground();
     void HandleDialogBackground();
-    void NotifyPointerEventToRs(int32_t pointAction);
     WSError HandleSubWindowClick(int32_t action);
     void SetWindowSessionProperty(const sptr<WindowSessionProperty>& property);
 
@@ -593,6 +594,7 @@ private:
     bool focusedOnShow_ = true;
     bool showRecent_ = false;
     bool bufferAvailable_ = false;
+
     WSRect preRect_;
     int32_t callingPid_ = -1;
     int32_t callingUid_ = -1;
@@ -608,10 +610,10 @@ private:
     bool systemTouchable_ { true };
     std::atomic_bool foregroundInteractiveStatus_ { true };
     std::atomic<bool> isAttach_{ false };
-    sptr<IPatternDetachCallback> detachCallback_ = nullptr;
 
     std::shared_ptr<RSSurfaceNode> leashWinSurfaceNode_;
     mutable std::mutex leashWinSurfaceNodeMutex;
+    sptr<IPatternDetachCallback> detachCallback_ = nullptr;
     DetectTaskInfo detectTaskInfo_;
     mutable std::shared_mutex detectTaskInfoMutex_;
 };
diff --git a/window_window_manager-master/window_scene/session/host/src/session.cpp b/window_window_manager-master/window_scene/session/host/src/session.cpp
index 272311f..8a6f72e 100644
--- a/window_window_manager-master/window_scene/session/host/src/session.cpp
+++ b/window_window_manager-master/window_scene/session/host/src/session.cpp
@@ -81,7 +81,6 @@ Session::Session(const SessionInfo& info) : sessionInfo_(info)
         auto runner = AppExecFwk::EventRunner::GetMainEventRunner();
         mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
     }
-
     using type = std::underlying_type_t<MMI::WindowArea>;
     for (type area = static_cast<type>(MMI::WindowArea::FOCUS_ON_TOP);
         area <= static_cast<type>(MMI::WindowArea::FOCUS_ON_BOTTOM_RIGHT); ++area) {
@@ -1020,8 +1019,7 @@ WSError Session::Background(bool isFromClient)
     SessionState state = GetSessionState();
     TLOGI(WmsLogTag::WMS_LIFE, "Background session, id: %{public}d, state: %{public}" PRIu32, GetPersistentId(),
         static_cast<uint32_t>(state));
-    if ((state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND) &&
-        GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
+    if (state == SessionState::STATE_ACTIVE && GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
         UpdateSessionState(SessionState::STATE_INACTIVE);
         state = SessionState::STATE_INACTIVE;
         isActive_ = false;
@@ -1148,7 +1146,8 @@ void Session::SetAttachState(bool isAttach, WindowMode windowMode)
         }
         TLOGD(WmsLogTag::WMS_LIFE, "isAttach:%{public}d persistentId:%{public}d", isAttach,
             session->GetPersistentId());
-        if (isAttach && session->detachCallback_ != nullptr) {
+        
+        if (!isAttach && session->detachCallback_ != nullptr) {
             TLOGI(WmsLogTag::WMS_LIFE, "Session detach, persistentId:%{public}d", session->GetPersistentId());
             session->detachCallback_->OnPatternDetach(session->GetPersistentId());
             session->detachCallback_ = nullptr;
@@ -1476,8 +1475,10 @@ void Session::SetParentSession(const sptr<Session>& session)
         WLOGFW("Session is nullptr");
         return;
     }
-    std::unique_lock<std::shared_mutex> lock(parentSessionMutex_);
-    parentSession_ = session;
+    {
+        std::unique_lock<std::shared_mutex> lock(parentSessionMutex_);
+        parentSession_ = session;
+    }
     TLOGD(WmsLogTag::WMS_SUB, "[WMSDialog][WMSSub]Set parent success, parentId: %{public}d, id: %{public}d",
         session->GetPersistentId(), GetPersistentId());
 }
@@ -1568,6 +1569,7 @@ bool Session::IsTopDialog() const
         auto dialogSession = *iter;
         if (dialogSession && (dialogSession->GetSessionState() == SessionState::STATE_ACTIVE ||
             dialogSession->GetSessionState() == SessionState::STATE_FOREGROUND)) {
+            WLOGFI("Dialog id: %{public}d, current dialog id: %{public}d", dialogSession->GetPersistentId(), currentPersistentId);
             return dialogSession->GetPersistentId() == currentPersistentId;
         }
     }
@@ -1632,14 +1634,6 @@ void Session::HandlePointDownDialog()
     }
 }
 
-void Session::NotifyPointerEventToRs(int32_t pointAction)
-{
-    if ((pointAction == MMI::PointerEvent::POINTER_ACTION_UP) |
-        (pointAction == MMI::PointerEvent::POINTER_ACTION_DOWN)) {
-        // RSInterfaces::GetInstance().NotifyTouchEvent(pointAction);
-    }
-}
-
 WSError Session::HandleSubWindowClick(int32_t action)
 {
     auto parentSession = GetParentSession();
@@ -1670,7 +1664,6 @@ WSError Session::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>&
         return WSError::WS_ERROR_NULLPTR;
     }
     auto pointerAction = pointerEvent->GetPointerAction();
-    NotifyPointerEventToRs(pointerAction);
     bool isPointDown = (pointerAction == MMI::PointerEvent::POINTER_ACTION_DOWN) ||
         (pointerAction == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
     if (GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
@@ -1718,7 +1711,6 @@ WSError Session::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>&
     } else {
         pointerEvent->MarkProcessed();
     }
-
     if (pointerAction == MMI::PointerEvent::POINTER_ACTION_MOVE ||
         pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_MOVE) {
         WLOGFD("Session TransferPointEvent, eventId:%{public}d, action:%{public}s, persistentId:%{public}d, "
@@ -1896,7 +1888,7 @@ void Session::SetSessionStateChangeListenser(const NotifySessionStateChangeFunc&
             return;
         }
         session->NotifySessionStateChange(changedState);
-        TLOGI(WmsLogTag::DEFAULT, "id: %{public}d, state_: %{public}d, changedState: %{public}d",
+        TLOGI(WmsLogTag::WMS_LIFE, "id: %{public}d, state_: %{public}d, changedState: %{public}d",
             session->GetPersistentId(), session->GetSessionState(), changedState);
     };
     PostTask(task, "SetSessionStateChangeListenser");
@@ -2385,7 +2377,7 @@ WSError Session::MarkProcessed(int32_t eventId)
 
 void Session::GeneratePersistentId(bool isExtension, int32_t persistentId)
 {
-    if (persistentId != INVALID_SESSION_ID  && !g_persistentIdSet.count(g_persistentId)) {
+    if (persistentId != INVALID_SESSION_ID  && !g_persistentIdSet.count(persistentId)) {
         g_persistentIdSet.insert(persistentId);
         persistentId_ = persistentId;
         return;
@@ -2429,7 +2421,7 @@ void Session::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info,
     sessionStage_->NotifyOccupiedAreaChangeInfo(info, rsTransaction);
 }
 
-WindowMode Session::GetWindowMode()
+WindowMode Session::GetWindowMode() const
 {
     auto property = GetSessionProperty();
     if (property == nullptr) {
@@ -2756,6 +2748,18 @@ Rotation Session::GetRotation() const
     return rotation_;
 }
 
+WSError Session::UpdateTitleInTargetPos(bool isShow, int32_t height)
+{
+    WLOGFD("Session update title in target position, id: %{public}d, isShow: %{public}d, height: %{public}d",
+        GetPersistentId(), isShow, height);
+    if (!IsSessionValid()) {
+        TLOGW(WmsLogTag::WMS_MAIN, "Session is invalid, id: %{public}d state: %{public}u",
+            GetPersistentId(), GetSessionState());
+        return WSError::WS_ERROR_INVALID_SESSION;
+    }
+    return sessionStage_->UpdateTitleInTargetPos(isShow, height);
+}
+
 void Session::SetSessionInfoLockedStateChangeListener(const NotifySessionInfoLockedStateChangeFunc& func)
 {
     sessionInfoLockedStateChangeFunc_ = func;
@@ -2769,18 +2773,6 @@ void Session::NotifySessionInfoLockedStateChange(bool lockedState)
     }
 }
 
-WSError Session::UpdateTitleInTargetPos(bool isShow, int32_t height)
-{
-    WLOGFD("Session update title in target position, id: %{public}d, isShow: %{public}d, height: %{public}d",
-        GetPersistentId(), isShow, height);
-    if (!IsSessionValid()) {
-        TLOGW(WmsLogTag::WMS_MAIN, "Session is invalid, id: %{public}d state: %{public}u",
-            GetPersistentId(), GetSessionState());
-        return WSError::WS_ERROR_INVALID_SESSION;
-    }
-    return sessionStage_->UpdateTitleInTargetPos(isShow, height);
-}
-
 WSError Session::SwitchFreeMultiWindow(bool enable)
 {
     TLOGD(WmsLogTag::WMS_LAYOUT, "windowId:%{public}d enable: %{public}d", GetPersistentId(), enable);
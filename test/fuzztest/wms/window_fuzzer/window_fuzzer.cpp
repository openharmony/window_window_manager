/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "marshalling_helper.h"

#include <securec.h>
#include <iremote_broker.h>
#include <iservice_registry.h>

#include "ability.h"
#include "ability_context.h"
#include "ability_context_impl.h"
#include "js_runtime.h"
#include <want.h>
#include "window.h"
#include "window_accessibility_controller.h"
#include "window_impl.h"
#include "window_manager.h"
#include "window_extension_connection.h"
#include "window_adapter.h"
#include "wm_common.h"
#include "window_option.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}
class FocusChangedListener : public IFocusChangedListener {
public:
    virtual void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }

    virtual void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }
};

class SystemBarChangedListener : public ISystemBarChangedListener {
public:
    virtual void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override
    {
    }
};

class VisibilityChangedListener : public IVisibilityChangedListener {
public:
    virtual void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
    }
};

class WindowUpdateListener : public IWindowUpdateListener {
public:
    virtual void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
    {
    }
};

class ExtensionCallback : public IWindowExtensionCallback {
public:
    void OnWindowReady(const std::shared_ptr<Rosen::RSSurfaceNode>& rsSurfaceNode) override
    {
    }
    void OnExtensionDisconnected() override
    {
    }
    void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event) override
    {
    }
    void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event) override
    {
    }
    void OnBackPress() override
    {
    }
};

class OccupiedAreaChangeListener : public IOccupiedAreaChangeListener {
public:
    void OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override
    {
    }
};

class TouchOutsideListener : public ITouchOutsideListener {
public:
    void OnTouchOutside() const override
    {
    }
};

class AnimationTransitionController : public IAnimationTransitionController {
public:
    void AnimationForShown() override
    {
    }

    void AnimationForHidden() override
    {
    }
};

class ScreenshotListener : public IScreenshotListener {
public:
    void OnScreenshot() override
    {
    }
};

class DialogTargetTouchListener : public IDialogTargetTouchListener {
public:
    void OnDialogTargetTouch() const override
    {
    }
};

class DialogDeathRecipientListener : public IDialogDeathRecipientListener {
public:
    void OnDialogDeathRecipient() const override
    {
    }
};

class WindowDragListener : public IWindowDragListener {
public:
    void OnDrag(int32_t x, int32_t y, DragEvent event) override
    {
    }
};

class DisplayMoveListener : public IDisplayMoveListener {
public:
    void OnDisplayMove(DisplayId from, DisplayId to) override
    {
    }
};

class AceAbilityHandler : public IAceAbilityHandler {
public:
    void SetBackgroundColor(uint32_t color) override
    {
    }

    virtual uint32_t GetBackgroundColor() override
    {
        return 0xffffffff;
    }
};

class WindowLifeCycle : public IWindowLifeCycle {
public:
    void AfterForeground() override
    {
    }

    void AfterBackground() override
    {
    }

    void AfterFocused() override
    {
    }

    void AfterUnfocused() override
    {
    }

    void ForegroundFailed(int32_t ret) override
    {
    }

    void BackgroundFailed(int32_t ret) override
    {
    }

    void AfterActive() override
    {
    }

    void AfterInactive()  override
    {
    }
};

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

bool DoSomethingInterestingWithMyAPI1(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    std::string name = "WindowFuzzTest1";
    sptr<WindowOption> option = nullptr;
    sptr<Window> window = new Window();
    if (window == nullptr) {
        return false;
    }
    size_t startPos = 0;
    sptr<IOccupiedAreaChangeListener> iOccupiedAreaChangeListener = new IOccupiedAreaChangeListener();
    OHOS::Rosen::Rect rect_ = {0, 0, 0, 0};
    window->RegisterOccupiedAreaChangeListener(iOccupiedAreaChangeListener);
    int32_t safeHeight = 80;
    startPos += GetObject<int32_t>(safeHeight, data + startPos, size - startPos);
    sptr<OHOS::Rosen::OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(
        OccupiedAreaType::TYPE_INPUT, rect_, safeHeight);
    iOccupiedAreaChangeListener->OnSizeChange(info, nullptr);
    window->UnregisterOccupiedAreaChangeListener(iOccupiedAreaChangeListener);
    sptr<IVisibilityChangedListener> visibilityChangedListener = new VisibilityChangedListener();
    std::vector<sptr<WindowVisibilityInfo>> infos;
    visibilityChangedListener->OnWindowVisibilityChanged(infos);
    return true;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    std::string name = "WindowFuzzTest";
    sptr<WindowOption> option = nullptr;
    sptr<Window> window = new Window();
    if (window == nullptr) {
        return false;
    }
    std::string windowName = "window test";
    window->Find(windowName);
    size_t startPos = 0;
    std::shared_ptr<AbilityRuntime::Context> context = nullptr;
    startPos += GetObject(context, data + startPos, size - startPos);
    window->GetTopWindowWithContext(context);
    uint32_t mainWinId = 1;
    startPos += GetObject<uint32_t>(mainWinId, data + startPos, size - startPos);
    window->GetTopWindowWithId(mainWinId);
    uint32_t parentId = 1;
    startPos += GetObject<uint32_t>(parentId, data + startPos, size - startPos);
    window->GetSubWindow(parentId);
    std::shared_ptr<AppExecFwk::Configuration> configuration = nullptr;
    startPos += GetObject(configuration, data + startPos, size - startPos);
    window->UpdateConfigurationForAll(configuration);
    window->Show(0);
    window->SetRequestedOrientation(static_cast<Orientation>(data[0]));
    window->Hide(0);
    window->Destroy();
    sptr<IFocusChangedListener> focusChangedListener = new FocusChangedListener();
    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo();
    focusChangedListener->OnFocused(focusChangeInfo);
    focusChangedListener->OnUnfocused(focusChangeInfo);
    sptr<IDispatchInputEventListener> iDispatchInputEventListener = new IDispatchInputEventListener();
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    std::shared_ptr<MMI::AxisEvent> axisEvent = nullptr;
    iDispatchInputEventListener->OnDispatchPointerEvent(pointerEvent);
    iDispatchInputEventListener->OnDispatchKeyEvent(keyEvent);
    std::shared_ptr<IInputEventConsumer> iInputEventConsumer = std::make_shared<IInputEventConsumer>();
    iInputEventConsumer->OnInputEvent(keyEvent);
    iInputEventConsumer->OnInputEvent(pointerEvent);
    iInputEventConsumer->OnInputEvent(axisEvent);
    DoSomethingInterestingWithMyAPI1(data, size);
    return true;
}

void CheckWindowImplFunctionsPart1(sptr<Window> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    bool boolVal = false;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetFocusable(boolVal);
    window->IsFocused();
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetTouchable(boolVal);
    window->SetResizeByDragEnabled(boolVal);

    WindowType windowType;
    WindowMode windowMode;
    startPos += GetObject(windowType, data + startPos, size - startPos);
    startPos += GetObject(windowMode, data + startPos, size - startPos);
    window->SetWindowType(windowType);
    window->SetWindowMode(windowMode);

    float alpha;
    startPos += GetObject(alpha, data + startPos, size - startPos);
    window->SetAlpha(alpha);

    Transform transForm;
    startPos += GetObject(transForm, data + startPos, size - startPos);
    window->SetTransform(transForm);

    WindowFlag windowFlag;
    startPos += GetObject(windowFlag, data + startPos, size - startPos);
    window->AddWindowFlag(windowFlag);
    startPos += GetObject(windowFlag, data + startPos, size - startPos);
    window->RemoveWindowFlag(windowFlag);

    SystemBarProperty systemBarProperty;
    startPos += GetObject(windowType, data + startPos, size - startPos);
    startPos += GetObject(systemBarProperty, data + startPos, size - startPos);
    window->SetSystemBarProperty(windowType, systemBarProperty);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetLayoutFullScreen(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetFullScreen(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->UpdateSurfaceNodeAfterCustomAnimation(boolVal);
}

void CheckWindowImplFunctionsPart2(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    int32_t posX;
    int32_t posY;
    startPos += GetObject(posX, data + startPos, size - startPos);
    startPos += GetObject(posY, data + startPos, size - startPos);
    window->MoveTo(posX, posY);

    uint32_t width;
    uint32_t hight;
    startPos += GetObject(width, data + startPos, size - startPos);
    startPos += GetObject(hight, data + startPos, size - startPos);
    window->Resize(width, hight);

    bool boolVal = false;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetKeepScreenOn(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetTurnScreenOn(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetTransparent(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetBrightness(boolVal);

    int32_t windowId;
    startPos += GetObject(windowId, data + startPos, size - startPos);
    window->SetCallingWindow(windowId);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetPrivacyMode(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetSystemPrivacyMode(boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetSnapshotSkip(boolVal);
}

void CheckWindowImplFunctionsPart3(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    float floatVal;
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetCornerRadius(floatVal);

    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetShadowRadius(floatVal);

    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetShadowOffsetX(floatVal);
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetShadowOffsetY(floatVal);
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetBlur(floatVal);
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->SetBackdropBlur(floatVal);

    WindowBlurStyle blurStyle;
    startPos += GetObject(blurStyle, data + startPos, size - startPos);
    window->SetBackdropBlurStyle(blurStyle);

    bool boolVal;
    OHOS::Rosen::Rect rect;
    WindowSizeChangeReason reason;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    startPos += GetObject(rect, data + startPos, size - startPos);
    startPos += GetObject(reason, data + startPos, size - startPos);
    window->UpdateRect(rect, boolVal, reason);

    WindowMode mode;
    startPos += GetObject(mode, data + startPos, size - startPos);
    window->UpdateMode(mode);

    uint32_t modeSupportInfo;
    startPos += GetObject(modeSupportInfo, data + startPos, size - startPos);
    window->UpdateModeSupportInfo(modeSupportInfo);

    WindowState windowState;
    startPos += GetObject(windowState, data + startPos, size - startPos);
    window->UpdateWindowState(windowState);

    PointInfo point;
    DragEvent event;
    startPos += GetObject(point, data + startPos, size - startPos);
    startPos += GetObject(event, data + startPos, size - startPos);
    window->UpdateDragEvent(point, event);

    DisplayId displayId[2];
    startPos += GetObject(displayId[0], data + startPos, size - startPos);
    startPos += GetObject(displayId[1], data + startPos, size - startPos);
    window->UpdateDisplayId(displayId[0], displayId[1]);
}

void CheckWindowImplFunctionsPart4(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    bool boolVal;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->UpdateActiveStatus(boolVal);

    Transform trans;
    startPos += GetObject(trans, data + startPos, size - startPos);
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->UpdateZoomTransform(trans, boolVal);

    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->SetNeedRemoveWindowInputChannel(boolVal);

    std::vector<OHOS::Rosen::Rect> rectVector;
    OHOS::Rosen::Rect rect;
    startPos += GetObject(rect, data + startPos, size - startPos);
    rectVector.emplace_back(rect);
    window->SetTouchHotAreas(rectVector);
    window->GetRequestedTouchHotAreas(rectVector);
    rectVector.clear();

    ColorSpace colorSpace;
    startPos += GetObject(colorSpace, data + startPos, size - startPos);
    window->SetColorSpace(colorSpace);

    int32_t level;
    startPos += GetObject(level, data + startPos, size - startPos);
    window->NotifyMemoryLevel(level);

    uint32_t mode;
    startPos += GetObject(mode, data + startPos, size - startPos);
    window->RestoreSplitWindowMode(mode);
}

void CheckWindowImplFunctionsPart5(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    OHOS::Rosen::Rect rect;
    WindowSizeChangeReason reason;
    startPos += GetObject(rect, data + startPos, size - startPos);
    startPos += GetObject(reason, data + startPos, size - startPos);
    window->NotifySizeChange(rect, reason);

    DisplayId displayId[2];
    startPos += GetObject(displayId[0], data + startPos, size - startPos);
    startPos += GetObject(displayId[1], data + startPos, size - startPos);
    window->NotifyDisplayMoveChange(displayId[0], displayId[1]);

    WindowMode mode;
    startPos += GetObject(mode, data + startPos, size - startPos);
    window->NotifyModeChange(mode);

    PointInfo point;
    DragEvent dragEvent;
    startPos += GetObject(point, data + startPos, size - startPos);
    startPos += GetObject(dragEvent, data + startPos, size - startPos);
    window->NotifyDragEvent(point, dragEvent);

    int32_t posX;
    int32_t posY;
    int32_t pointId;
    int32_t sourceType;
    startPos += GetObject(posX, data + startPos, size - startPos);
    startPos += GetObject(posY, data + startPos, size - startPos);
    startPos += GetObject(pointId, data + startPos, size - startPos);
    startPos += GetObject(sourceType, data + startPos, size - startPos);
    window->EndMoveOrDragWindow(posX, posY, pointId, sourceType);

    bool boolVal;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    window->AdjustWindowAnimationFlag(boolVal);

    PropertyChangeAction action;
    startPos += GetObject(action, data + startPos, size - startPos);
    window->UpdateProperty(action);

    uint32_t uinte32Val;
    startPos += GetObject(uinte32Val, data + startPos, size - startPos);
    window->SetBackgroundColor(uinte32Val);

    LifeCycleEvent lifeCycleEvent;
    OHOS::Rosen::WMError errCode;
    startPos += GetObject(lifeCycleEvent, data + startPos, size - startPos);
    startPos += GetObject(errCode, data + startPos, size - startPos);
    window->RecordLifeCycleExceptionEvent(lifeCycleEvent, errCode);
    window->TransferLifeCycleEventToString(lifeCycleEvent);

    startPos += GetObject(rect, data + startPos, size - startPos);
    window->GetSystemAlarmWindowDefaultSize(rect);
    window->HandleModeChangeHotZones(posX, posY);
}

void CheckWindowImplFunctionsPart6(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;

    TransitionReason reason;
    startPos += GetObject(reason, data + startPos, size - startPos);
    window->NotifyWindowTransition(reason);

    WindowType type;
    startPos += GetObject(type, data + startPos, size - startPos);
    window->CheckCameraFloatingWindowMultiCreated(type);

    uint32_t uint32Val[2];
    startPos += GetObject(uint32Val[0], data + startPos, size - startPos);
    startPos += GetObject(uint32Val[1], data + startPos, size - startPos);
    window->SetModeSupportInfo(uint32Val[0]);

    float floatVal;
    startPos += GetObject(floatVal, data + startPos, size - startPos);
    window->CalculateStartRectExceptHotZone(floatVal);

    Transform transform;
    startPos += GetObject(transform, data + startPos, size - startPos);
    window->TransformSurfaceNode(transform);

    window->WindowCreateCheck(uint32Val[0]);
    window->CalculatePointerDirection(uint32Val[0], uint32Val[1]);
    sptr<IWindowChangeListener> iWindowChangeListener = new IWindowChangeListener();
    std::shared_ptr<RSTransaction> rstransaction;
    OHOS::Rosen::Rect rect_ = {0, 0, 0, 0};
    iWindowChangeListener->OnSizeChange(rect_, WindowSizeChangeReason::UNDEFINED, rstransaction);
    sptr<IWindowExtensionCallback> iWindowExtensionCallback = new ExtensionCallback();
    std::shared_ptr<Rosen::RSSurfaceNode> rsSurfaceNode = nullptr;
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    iWindowExtensionCallback->OnWindowReady(rsSurfaceNode);
    iWindowExtensionCallback->OnExtensionDisconnected();
    iWindowExtensionCallback->OnKeyEvent(keyEvent);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    iWindowExtensionCallback->OnPointerEvent(pointerEvent);
    iWindowExtensionCallback->OnBackPress();
}

void CheckWindowImplFunctionsPart7(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    NotifyNativeWinDestroyFunc func = [](std::string) {};
    window->RegisterWindowDestroyedListener(func);

    sptr<IOccupiedAreaChangeListener> occupiedAreaChangeListener = new OccupiedAreaChangeListener();
    window->RegisterOccupiedAreaChangeListener(occupiedAreaChangeListener);
    window->UnregisterOccupiedAreaChangeListener(occupiedAreaChangeListener);
    sptr<ITouchOutsideListener> touchOutsideListener = new TouchOutsideListener();
    window->RegisterTouchOutsideListener(touchOutsideListener);
    window->UnregisterTouchOutsideListener(touchOutsideListener);
    sptr<IAnimationTransitionController> animationTransitionController = new AnimationTransitionController();
    window->RegisterAnimationTransitionController(animationTransitionController);
    sptr<IScreenshotListener> screenshotListener = new IScreenshotListener();
    window->RegisterScreenshotListener(screenshotListener);
    window->UnregisterScreenshotListener(screenshotListener);
    sptr<IDialogTargetTouchListener> dialogTargetTouchListener = new DialogTargetTouchListener();
    window->RegisterDialogTargetTouchListener(dialogTargetTouchListener);
    window->UnregisterDialogTargetTouchListener(dialogTargetTouchListener);
    sptr<IDialogDeathRecipientListener> dialogDeathRecipientListener = new DialogDeathRecipientListener();
    window->RegisterDialogDeathRecipientListener(dialogDeathRecipientListener);
    window->UnregisterDialogDeathRecipientListener(dialogDeathRecipientListener);
    sptr<IAceAbilityHandler> aceAbilityHandler = new AceAbilityHandler();
    window->SetAceAbilityHandler(aceAbilityHandler);
    uint32_t modeSupportInfo;
    startPos += GetObject<uint32_t>(modeSupportInfo, data + startPos, size - startPos);
    window->SetRequestModeSupportInfo(modeSupportInfo);
    float ratio;
    startPos += GetObject<float>(ratio, data + startPos, size - startPos);
    window->SetAspectRatio(ratio);
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    AvoidArea avoidArea;
    startPos += GetObject<AvoidAreaType>(avoidAreaType, data + startPos, size - startPos);
    startPos += GetObject<AvoidArea>(avoidArea, data + startPos, size - startPos);
    window->GetAvoidAreaByType(avoidAreaType, avoidArea);
    WindowGravity gravity = WindowGravity::WINDOW_GRAVITY_FLOAT;
    uint32_t invalidGravityPercent = 0;
    startPos += GetObject<WindowGravity>(gravity, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(invalidGravityPercent, data + startPos, size - startPos);
    window->SetWindowGravity(gravity, invalidGravityPercent);
    sptr<IRemoteObject> targetToken;
    window->BindDialogTarget(targetToken);
    std::string color = "ff22ee44";
    startPos += GetObject<std::string>(color, data + startPos, size - startPos);
    window->SetShadowColor(color);
}

void CheckWindowImplFunctionsPart8(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    size_t startPos = 0;
    std::vector<std::string> params{"-h"};
    std::vector<std::string> info{""};
    window->DumpInfo(params, info);
    params.push_back("");
    window->DumpInfo(params, info);

    auto keyEvent = MMI::KeyEvent::Create();
    window->ConsumeKeyEvent(keyEvent);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    window->ConsumeKeyEvent(keyEvent);

    auto pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item;
    int32_t pointerId = 0;
    startPos += GetObject<int32_t>(pointerId, data + startPos, size - startPos);
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    window->ConsumePointerEvent(pointerEvent);
    item.SetPointerId(pointerId);
    item.SetDisplayX(15); // 15 : position x
    item.SetDisplayY(15); // 15 : position y
    pointerEvent->AddPointerItem(item);
    window->ConsumePointerEvent(pointerEvent);
    int32_t x = 5;
    int32_t y = 5;
    startPos += GetObject<int32_t>(x, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(y, data + startPos, size - startPos);
    item.SetDisplayX(x); // 5 : position x
    item.SetDisplayY(y); // 5 : position y
    pointerEvent->UpdatePointerItem(pointerId, item);
    window->ConsumePointerEvent(pointerEvent);
}

void CheckWindowImplFunctionsPart9(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    if (window == nullptr || data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    std::shared_ptr<IInputEventConsumer> iInputEventConsumer = std::make_shared<IInputEventConsumer>();
    window->SetInputEventConsumer(iInputEventConsumer);
    std::shared_ptr<VsyncCallback> callback;
    window->RequestVsync(callback);
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    window->UpdateConfiguration(configuration);
    sptr<IWindowLifeCycle> windowLifeCycleListener = new IWindowLifeCycle();
    window->RegisterLifeCycleListener(windowLifeCycleListener);
    window->UnregisterLifeCycleListener(windowLifeCycleListener);
    sptr<IWindowChangeListener> windowChangeListener = new IWindowChangeListener();
    window->RegisterWindowChangeListener(sptr<IWindowChangeListener>(windowChangeListener));
    window->UnregisterWindowChangeListener(sptr<IWindowChangeListener>(windowChangeListener));
    sptr<IAvoidAreaChangedListener> avoidAreaChangedListener = new IAvoidAreaChangedListener();
    window->RegisterAvoidAreaChangeListener(avoidAreaChangedListener);
    window->UnregisterAvoidAreaChangeListener(avoidAreaChangedListener);
    sptr<IWindowDragListener> windowDragListener = new WindowDragListener();
    window->RegisterDragListener(windowDragListener);
    window->UnregisterDragListener(windowDragListener);
    sptr<IDisplayMoveListener> displayMoveListener = new DisplayMoveListener();
    window->RegisterDisplayMoveListener(displayMoveListener);
    window->UnregisterDisplayMoveListener(displayMoveListener);
    AAFwk::Want want;
    window->OnNewWant(want);
    size_t startPos = 0;
    const std::string contentInfo = "WindowFuzzTest";
    NativeEngine* engine = nullptr;
    NativeValue* storage = nullptr;
    bool isDistributed = false;
    startPos += GetObject(isDistributed, data + startPos, size - startPos);
    AppExecFwk::Ability ability;
    window->SetUIContent(contentInfo, engine, storage, isDistributed, &ability);
}

void WindowImplFuzzTest(const uint8_t* data, size_t size)
{
    std::string name = "WindowFuzzTest";
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<OHOS::Rosen::WindowImpl> window = new(std::nothrow) OHOS::Rosen::WindowImpl(option);
    if (window == nullptr) {
        return;
    }
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->Create(option->GetParentId(), abilityContext);

    size_t startPos = 0;
    uint32_t reason = 0;
    bool withAnimation = false;
    startPos += GetObject(reason, data + startPos, size - startPos);
    startPos += GetObject(withAnimation, data + startPos, size - startPos);
    window->Show(reason, withAnimation);
    std::shared_ptr<AbilityRuntime::Context> context = nullptr;
    startPos += GetObject(context, data + startPos, size - startPos);
    window->GetTopWindowWithContext(context);

    OHOS::CheckWindowImplFunctionsPart1(window, data, size);
    OHOS::CheckWindowImplFunctionsPart2(window, data, size);
    OHOS::CheckWindowImplFunctionsPart3(window, data, size);
    OHOS::CheckWindowImplFunctionsPart4(window, data, size);
    OHOS::CheckWindowImplFunctionsPart5(window, data, size);
    OHOS::CheckWindowImplFunctionsPart6(window, data, size);
    OHOS::CheckWindowImplFunctionsPart7(window, data, size);
    OHOS::CheckWindowImplFunctionsPart8(window, data, size);
    OHOS::CheckWindowImplFunctionsPart9(window, data, size);

    window->Hide(reason, withAnimation);
    window->Destroy();
}

void WindowImplFuzzTest01(const uint8_t* data, size_t size)
{
    std::string name = "WindowFuzzTest01";
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    sptr<OHOS::Rosen::WindowImpl> window = new(std::nothrow) OHOS::Rosen::WindowImpl(option);
    if (window == nullptr) {
        return;
    }
    sptr<IWindowUpdateListener> windowUpdateListener = new WindowUpdateListener();
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityWindowInfo;
    WindowUpdateType type = Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED;
    windowUpdateListener->OnWindowUpdate(accessibilityWindowInfo, type);
    std::string windowName = "test1";
    window->Find(windowName);
    size_t startPos = 0;
    uint32_t mainWinId = 1;
    startPos += GetObject<uint32_t>(mainWinId, data + startPos, size - startPos);
    window->GetTopWindowWithId(mainWinId);
    uint32_t parentId = 1;
    startPos += GetObject<uint32_t>(parentId, data + startPos, size - startPos);
    window->GetSubWindow(parentId);
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    window->UpdateConfigurationForAll(configuration);
    int32_t x;
    int32_t y;
    float scale;
    startPos += GetObject<int32_t>(x, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(y, data + startPos, size - startPos);
    startPos += GetObject<float>(scale, data + startPos, size - startPos);
    WindowAccessibilityController::GetInstance().SetAnchorAndScale(x, y, scale);
    int32_t deltaX;
    int32_t deltaY;
    startPos += GetObject<int32_t>(deltaX, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(deltaY, data + startPos, size - startPos);
    WindowAccessibilityController::GetInstance().SetAnchorOffset(deltaX, deltaY);
    sptr<WindowExtensionConnection> connection = new(std::nothrow)WindowExtensionConnection();
    if (connection == nullptr) {
        return;
    }
    AppExecFwk::ElementName element;
    element.SetBundleName("com.test.windowextension");
    element.SetAbilityName("WindowExtAbility");
    Rosen::Rect rect {100, 100, 60, 60};
    uint32_t uid = 100;
    uint32_t windowId = INVALID_WINDOW_ID;
    startPos += GetObject<uint32_t>(uid, data + startPos, size - startPos);
    startPos += GetObject<uint32_t>(windowId, data + startPos, size - startPos);
    connection->ConnectExtension(element, rect, uid, windowId, nullptr);
    connection->SetBounds(rect);
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::WindowImplFuzzTest(data, size);
    OHOS::WindowImplFuzzTest01(data, size);
    return 0;
}
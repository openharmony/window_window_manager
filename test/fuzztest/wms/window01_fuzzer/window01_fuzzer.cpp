/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ability_context.h"
#include "ability_context_impl.h"
#include "js_runtime.h"
#include <want.h>
#include "window.h"
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
    constexpr size_t DATA_MAX_SIZE = 1024;
}
class FocusChangedListener : public IFocusChangedListener {
public:
    void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }

    void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }
};

class SystemBarChangedListener : public ISystemBarChangedListener {
public:
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override
    {
    }
};

class VisibilityChangedListener : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
    }
};

class WindowUpdateListener : public IWindowUpdateListener {
public:
    void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
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

class WindowVisibilityChangeListener : public IWindowVisibilityChangedListener {
public:
    void OnWindowVisibilityChangedCallback(const bool isVisisble) override
    {
    }
};

class WindowNoInteractionListener : public IWindowNoInteractionListener {
public:
    void OnWindowNoInteractionCallback() override
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

    void AfterResumed() override
    {
    }

    void AfterPaused()  override
    {
    }
};

template<class T>
size_t GetObject(T& object, const uint8_t* data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

/* 调用该接口后, 需要释放内存 */
char *CopyDataToString(const uint8_t* data, size_t size)
{
    if (size > DATA_MAX_SIZE) {
        return nullptr;
    }
    char *string = (char *)malloc(size);
    if (string == nullptr) {
        std::cout << "malloc failed." << std::endl;
        return nullptr;
    }

    if (memcpy_s(string, size, data, size) != EOK) {
        std::cout << "copy failed." << std::endl;
        free(string);
        return nullptr;
    }
    return string;
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
    std::string name = "Window01FuzzTest";
    sptr<WindowOption> option = nullptr;
    sptr<Window> window = new Window();
    if (window == nullptr) {
        return false;
    }
    std::string windowName = "window test";
    window->Find(windowName);
    size_t startPos = 0;
    std::shared_ptr<AbilityRuntime::Context> context;
    window->GetTopWindowWithContext(context);
    uint32_t mainWinId = 1;
    startPos += GetObject<uint32_t>(mainWinId, data + startPos, size - startPos);
    window->GetTopWindowWithId(mainWinId);
    uint32_t parentId = 1;
    startPos += GetObject<uint32_t>(parentId, data + startPos, size - startPos);
    window->GetSubWindow(parentId);
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();
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

void CheckWindowImplFunctionsPart3(sptr<WindowImpl> window, const uint8_t* data, size_t size)
{
    size_t startPos = 0;
    float floatVal;

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

    uint32_t windowModeSupportType;
    startPos += GetObject(windowModeSupportType, data + startPos, size - startPos);
    window->UpdateWindowModeSupportType(windowModeSupportType);

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

void WindowImplFuzzTest(const uint8_t* data, size_t size)
{
    std::string name = "Window01FuzzTest";
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
    std::shared_ptr<AbilityRuntime::Context> context;
    window->GetTopWindowWithContext(context);
    OHOS::CheckWindowImplFunctionsPart3(window, data, size);

    window->Hide(reason, withAnimation);
    window->Destroy();
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::WindowImplFuzzTest(data, size);
    return 0;
}
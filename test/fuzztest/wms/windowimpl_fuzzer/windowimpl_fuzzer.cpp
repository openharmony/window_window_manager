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

#include "windowimpl_fuzzer.h"
#include <parcel.h>
#include <securec.h>

#include "marshalling_helper.h"
#include "window_manager.h"
#include "window_impl.h"
#include <refbase.h>

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

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

bool WindowImplFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
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
    size_t startPos = 0;
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
    std::string color = "#FF22EE44";
    window->SetShadowColor(color);
    
    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::WindowImplFuzzTest(data, size);
    return 0;
}


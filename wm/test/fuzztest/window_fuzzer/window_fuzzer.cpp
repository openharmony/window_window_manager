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

#include "parcel.h"
#include "window.h"
#include "window_manager.h"

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
    virtual void OnWindowUpdate(const sptr<AccessibilityWindowInfo>& windowInfo, WindowUpdateType type) override
    {
    }
};

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    std::string name = "WindowFuzzTest";
    sptr<WindowOption> option = nullptr;
    sptr<Window> window = Window::Create(name, option);
    if (window == nullptr) {
        return false;
    }
    window->Show(0);
    Orientation orientation = static_cast<Orientation>(data[0]);
    window->SetRequestedOrientation(static_cast<Orientation>(data[0]));
    if (window->GetRequestedOrientation() != orientation) {
        return false;
    }
    window->Hide(0);
    window->Destroy();
    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}


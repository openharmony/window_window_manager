/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_WINDOW_ADAPTER_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_WINDOW_ADAPTER_H
#include <gmock/gmock.h>

#include "window_adapter.h"

namespace OHOS {
namespace Rosen {
class MockWindowAdapter : public WindowAdapter {
public:
    MOCK_METHOD4(CreateWindow, WMError(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
        std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId));
    MOCK_METHOD1(AddWindow, WMError(sptr<WindowProperty>& windowProperty));
    MOCK_METHOD1(RemoveWindow, WMError(uint32_t windowId));
    MOCK_METHOD0(ClearWindowAdapter, void());
    MOCK_METHOD1(DestroyWindow, WMError(uint32_t windowId));
    MOCK_METHOD2(SetWindowBackgroundBlur, WMError(uint32_t windowId, WindowBlurLevel level));
    MOCK_METHOD2(SetAlpha, WMError(uint32_t windowId, float alpha));
    MOCK_METHOD2(SaveAbilityToken, WMError(const sptr<IRemoteObject>& abilityToken, uint32_t windowId));
    MOCK_METHOD3(SetSystemBarProperty, WMError(uint32_t windowId, WindowType type, const SystemBarProperty& property));
    MOCK_METHOD1(IsSupportWideGamut, bool(uint32_t windowId));
    MOCK_METHOD2(SetColorSpace, void(uint32_t windowId, ColorSpace colorSpace));
    MOCK_METHOD1(GetColorSpace, ColorSpace(uint32_t windowId));
};
}
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_WINDOW_ADAPTER_H
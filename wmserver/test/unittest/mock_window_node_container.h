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

#ifndef FRAMEWORKS_WMS_TEST_UT_MOCK_WINDOW_ADAPTER_H
#define FRAMEWORKS_WMS_TEST_UT_MOCK_WINDOW_ADAPTER_H
#include <gmock/gmock.h>

#include "window_node_container.h"

namespace OHOS {
namespace Rosen {
class MockWindowNodeContainer : public WindowNodeContainer {
public:
    MOCK_METHOD2(AddWindowNodeOnWindowTree, WMError(sptr<WindowNode>& node, const sptr<WindowNode>& parentNode));
    MOCK_METHOD1(RemoveWindowNode, WMError(sptr<WindowNode>& node));
    MOCK_METHOD1(HandleRemoveWindow, WMError(sptr<WindowNode>& node));
};
}
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_WINDOW_ADAPTER_H
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

// gtest
#include <gtest/gtest.h>
#include "window_test_utils.h"
#include "window_accessibility_controller.h"
#include "window_impl.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowDisplayZoomTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo windowInfo_;
};

void WindowDisplayZoomTest::SetUpTestCase()
{
}

void WindowDisplayZoomTest::TearDownTestCase()
{
}

void WindowDisplayZoomTest::SetUp()
{
    windowInfo_ = {
        .name = "zoomWindow",
        .rect = {0, 0, 300, 100},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WindowDisplayZoomTest::TearDown()
{
}

namespace {
}
} // namespace Rosen
} // namespace OHOS
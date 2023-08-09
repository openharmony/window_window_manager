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
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowModeSupportInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo fullAppInfo_1_;
    Utils::TestWindowInfo fullAppInfo_2_;
private:
    static constexpr uint32_t WAIT_SYANC_US = 100000;
};

void WindowModeSupportInfoTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowModeSupportInfoTest::TearDownTestCase()
{
}

void WindowModeSupportInfoTest::SetUp()
{
    fullAppInfo_1_ = {
            .name = "FullWindow",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };
    fullAppInfo_2_ = {
            .name = "FullWindow2",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };
}

void WindowModeSupportInfoTest::TearDown()
{
}

namespace {
} // namespace
} // namespace Rosen
} // namespace OHOS
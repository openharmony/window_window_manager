/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <securec.h>
#include "common_test_utils.h"
#include "display_manager.h"
#include "mock_display_manager_adapter.h"
#include "pixel_map.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;
class ScreenshotTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};
void ScreenshotTest::SetUpTestCase()
{
}

void ScreenshotTest::TearDownTestCase()
{
}

void ScreenshotTest::SetUp()
{
}

void ScreenshotTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetScreenshot_default
 * @tc.desc: SetWindowRect/GetWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, GetScreenshot_default, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetDefaultDisplayInfo()).Times(1).WillOnce(Return(nullptr));
    EXPECT_EQ(DISPLAY_ID_INVALID, DisplayManager::GetInstance().GetDefaultDisplayId());

    EXPECT_CALL(m->Mock(), GetDisplaySnapshot(_, _, _, _)).Times(1).WillOnce(Return(nullptr));
    ASSERT_EQ(nullptr, DisplayManager::GetInstance().GetScreenshot(0));
}

HWTEST_F(ScreenshotTest, GetScreenshot_01, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetDefaultDisplayInfo()).Times(1).WillOnce(Return(nullptr));
    EXPECT_EQ(DISPLAY_ID_INVALID, DisplayManager::GetInstance().GetDefaultDisplayId());

    Media::InitializationOptions opt;
    opt.size.width = CommonTestUtils::TEST_IMAGE_WIDTH;
    opt.size.height = CommonTestUtils::TEST_IMAGE_HEIGHT;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opt);
    EXPECT_CALL(m->Mock(), GetDisplaySnapshot(_, _, _, _)).Times(1).WillOnce(Return(pixelMap));
    auto screenshot = DisplayManager::GetInstance().GetScreenshot(0);
    ASSERT_NE(nullptr, screenshot);

    uint32_t width = screenshot->GetWidth();
    uint32_t height = screenshot->GetHeight();
    EXPECT_EQ(width, CommonTestUtils::TEST_IMAGE_WIDTH);
    EXPECT_EQ(height, CommonTestUtils::TEST_IMAGE_HEIGHT);
}
}
} // namespace Rosen
} // namespace OHOS

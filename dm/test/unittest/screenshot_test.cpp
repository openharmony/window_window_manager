/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "screenshot_test.h"

#include <securec.h>

#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"
#include "common_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t TEST_IMAGE_HEIGHT = 1080;
constexpr int32_t TEST_IMAGE_WIDTH = 1920;
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;
void ScreenshotTest::SetUpTestCase()
{
    CommonTestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
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
static std::shared_ptr<Media::PixelMap> CreatePixelMap()
{
    // pixel_map testing code
    Media::InitializationOptions opt;
    opt.size.width = TEST_IMAGE_WIDTH;
    opt.size.height = TEST_IMAGE_HEIGHT;
    opt.pixelFormat = Media::PixelFormat::RGBA_8888;
    opt.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opt.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;
    opt.editable = false;
    opt.useSourceIfMatch = false;

    const int bitmapDepth = 8; // color depth
    const int bpp = 4; // bytes per pixel
    const int pixelValue = 125;

    const int voulumeSize = opt.size.width * opt.size.height * bpp;
    auto data = (uint32_t *)malloc(voulumeSize);
    if (data == nullptr) {
        return nullptr;
    }

    uint8_t *pic = (uint8_t *)data;
    if (memset_s(pic, voulumeSize, pixelValue, voulumeSize) != EOK) {
        free(data);
        return nullptr;
    }

    uint32_t colorLen = voulumeSize * bitmapDepth;
    auto pixelMap = Media::PixelMap::Create(data, colorLen, opt);
    free(data);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap_(pixelMap.release());
    return pixelMap_;
}

/**
 * @tc.name: GetScreenshot_default
 * @tc.desc: SetWindowRect/GetWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, GetScreenshot_default, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetDefaultDisplayInfo()).Times(1).WillOnce(Return(nullptr));

    DisplayManager::GetInstance().GetDefaultDisplayId();

    EXPECT_CALL(m->Mock(), GetDisplaySnapshot(_)).Times(1).WillOnce(Return(nullptr));

    ASSERT_EQ(nullptr, DisplayManager::GetInstance().GetScreenshot(0));
}

HWTEST_F(ScreenshotTest, GetScreenshot_01, Function | MediumTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetDefaultDisplayInfo()).Times(1).WillOnce(Return(nullptr));
    DisplayManager::GetInstance().GetDefaultDisplayId();

    EXPECT_CALL(m->Mock(), GetDisplaySnapshot(_)).Times(1).WillOnce(Return(CreatePixelMap()));
    auto screenshot = DisplayManager::GetInstance().GetScreenshot(0);
    ASSERT_NE(nullptr, screenshot);

    uint32_t width = screenshot->GetWidth();
    uint32_t height = screenshot->GetHeight();
    ASSERT_EQ(width, TEST_IMAGE_WIDTH);
    ASSERT_EQ(height, TEST_IMAGE_HEIGHT);
}
}
} // namespace Rosen
} // namespace OHOS

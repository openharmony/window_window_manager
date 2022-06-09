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

#include "snapshot_utils_test.h"
#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"
#include "snapshot_utils.h"
#include "test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;
void SnapshotUtilsTest::SetUpTestCase()
{
    TestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
}

void SnapshotUtilsTest::TearDownTestCase()
{
}

void SnapshotUtilsTest::SetUp()
{
}

void SnapshotUtilsTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Check01
 * @tc.desc: Check if default png is valid file names
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotUtilsTest, Check01, Function | SmallTest | Level3)
{
    ASSERT_EQ(true, SnapShotUtils::CheckFileNameValid(defaultFile_));
}

/**
 * @tc.name: Check02
 * @tc.desc: Check custom png is valid file names
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotUtilsTest, Check02, Function | SmallTest | Level3)
{
    std::string fileName = "/data/test.png";
    ASSERT_EQ(true, SnapShotUtils::CheckFileNameValid(fileName));
}

/**
 * @tc.name: Check03
 * @tc.desc: Check random path is invalid file names
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotUtilsTest, Check03, Function | SmallTest | Level3)
{
    std::string fileName = "/path/to/test/1.png";
    ASSERT_EQ(false, SnapShotUtils::CheckFileNameValid(fileName));
}

/**
 * @tc.name: Write01
 * @tc.desc: Write default png using valid file names and valid PixelMap
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotUtilsTest, Write01, Function | SmallTest | Level3)
{
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    std::shared_ptr<Media::PixelMap> pixelMap = DisplayManager::GetInstance().GetScreenshot(id);
    ASSERT_NE(nullptr, pixelMap);
    ASSERT_EQ(true, SnapShotUtils::WriteToPngWithPixelMap(defaultFile_, *pixelMap));
}

/**
 * @tc.name: Write02
 * @tc.desc: Write default png using valid file names and valid WriteToPngParam
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotUtilsTest, Write02, Function | SmallTest | Level3)
{
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    std::shared_ptr<Media::PixelMap> pixelMap = DisplayManager::GetInstance().GetScreenshot(id);
    ASSERT_NE(nullptr, pixelMap);
    WriteToPngParam param = {
        .width = pixelMap->GetWidth(),
        .height = pixelMap->GetHeight(),
        .data = pixelMap->GetPixels(),
        .stride = pixelMap->GetRowBytes(),
        .bitDepth = defaultBitDepth_
    };
    ASSERT_EQ(true, SnapShotUtils::WriteToPng(defaultFile_, param));
}

/**
 * @tc.name: Write03
 * @tc.desc: Write custom png using valid file names and valid WriteToPngParam
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotUtilsTest, Write03, Function | SmallTest | Level3)
{
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    std::shared_ptr<Media::PixelMap> pixelMap = DisplayManager::GetInstance().GetScreenshot(id);
    ASSERT_NE(nullptr, pixelMap);
    WriteToPngParam param = {
        .width = (pixelMap->GetWidth() / 2),
        .height = (pixelMap->GetWidth() / 2),
        .data = pixelMap->GetPixels(),
        .stride = pixelMap->GetRowBytes(),
        .bitDepth = defaultBitDepth_
    };
    ASSERT_EQ(true, SnapShotUtils::WriteToPng(defaultFile_, param));
}
}
} // namespace Rosen
} // namespace OHOS
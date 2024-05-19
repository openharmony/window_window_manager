/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "screen_snapshot_picker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSnapshotPickerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSnapshotPickerTest::SetUpTestCase()
{
}

void ScreenSnapshotPickerTest::TearDownTestCase()
{
}

void ScreenSnapshotPickerTest::SetUp()
{
}

void ScreenSnapshotPickerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: SnapshotPickerConnectExtension
 * @tc.desc: SnapshotPickerConnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerTest, SnapshotPickerConnectExtension, Function | SmallTest | Level1)
{
    auto ret = ScreenSnapshotPicker::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetScreenSnapshotRect
 * @tc.desc: GetScreenSnapshotRect func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerTest, GetScreenSnapshotRect, Function | SmallTest | Level1)
{
    Media::Rect rect{};
    ScreenId screenId = SCREEN_ID_INVALID;
    auto ret = ScreenSnapshotPicker::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
    ScreenSnapshotPicker::GetInstance().GetScreenSnapshotInfo(rect, screenId);
    EXPECT_EQ(rect.top, 0);
    EXPECT_EQ(rect.left, 0);
    EXPECT_EQ(rect.width, 0);
    EXPECT_EQ(rect.height, 0);
    ScreenSnapshotPicker::GetInstance().SnapshotPickerDisconnectExtension();
}

}
} // namespace Rosen
} // namespace OHOS
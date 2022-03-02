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

#include "window_snapshot_test.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
void WindowSnapshotTest::SetUpTestCase()
{
}

void WindowSnapshotTest::TearDownTestCase()
{
}

void WindowSnapshotTest::SetUp()
{
}

void WindowSnapshotTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot when parameter abilityToken is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSnapshotTest, GetSnapshot, Function | SmallTest | Level3)
{
    sptr<SnapshotController> snapshotController_ = new SnapshotController();
    AAFwk::Snapshot snapshot_;
    ASSERT_EQ(static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), snapshotController_->GetSnapshot(nullptr, snapshot_));
}
}
} // namespace Rosen
} // namespace OHOS

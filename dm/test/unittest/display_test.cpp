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

#include <gtest/gtest.h>
#include "display_manager.h"
#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;
class DisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<Display> defaultDisplay_;
    static DisplayId defaultDisplayId_;
};
sptr<Display> DisplayTest::defaultDisplay_ = nullptr;
DisplayId DisplayTest::defaultDisplayId_ = DISPLAY_ID_INVALID;

void DisplayTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = static_cast<DisplayId>(defaultDisplay_->GetId());
}

void DisplayTest::TearDownTestCase()
{
    defaultDisplay_ = nullptr;
}

void DisplayTest::SetUp()
{
}

void DisplayTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetCutoutInfo01
 * @tc.desc: GetCutoutInfo with valid defaultDisplayId and return success
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTest, GetCutoutInfo01, Function | SmallTest | Level1)
{
    auto cutoutInfo = defaultDisplay_->GetCutoutInfo();
    ASSERT_NE(nullptr, cutoutInfo);
}
}
} // namespace Rosen
} // namespace OHOS
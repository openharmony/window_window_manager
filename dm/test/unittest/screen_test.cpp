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

#include "screen_test.h"
#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;

sptr<Display> ScreenTest::defaultDisplay_ = nullptr;
ScreenId ScreenTest::defaultScreenId_ = SCREEN_ID_INVALID;

void ScreenTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultScreenId_ = static_cast<ScreenId>(defaultDisplay_->GetId());
}

void ScreenTest::TearDownTestCase()
{
}

void ScreenTest::SetUp()
{
}

void ScreenTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetScreenActiveMode01
 * @tc.desc: SetScreenActiveMode with valid modeId and return success
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenActiveMode01, Function | SmallTest | Level1)
{
    auto screen = ScreenManager::GetInstance().GetScreenById(defaultScreenId_);
    auto supportedModes = screen->GetSupportedModes();
    ASSERT_GT(supportedModes.size(), 0);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenActiveMode(_, _)).Times(1).WillOnce(Return(true));
    bool res = screen->SetScreenActiveMode(supportedModes.size() - 1);
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetScreenActiveMode02
 * @tc.desc: SetScreenActiveMode with valid modeId and return failed
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenActiveMode02, Function | SmallTest | Level1)
{
    auto screen = ScreenManager::GetInstance().GetScreenById(defaultScreenId_);
    auto supportedModes = screen->GetSupportedModes();
    ASSERT_GT(supportedModes.size(), 0);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenActiveMode(_, _)).Times(1).WillOnce(Return(false));
    bool res = screen->SetScreenActiveMode(supportedModes.size() - 1);
    ASSERT_EQ(false, res);
}
}
} // namespace Rosen
} // namespace OHOS
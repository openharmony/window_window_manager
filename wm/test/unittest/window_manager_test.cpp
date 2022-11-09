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

#include <gtest/gtest.h>
#include "window_manager.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class WindowManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void WindowManagerTest::SetUpTestCase()
{
}

void WindowManagerTest::TearDownTestCase()
{
}

void WindowManagerTest::SetUp()
{
}

void WindowManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetVisibilityWindowInfo01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<sptr<WindowVisibilityInfo>> infos;
    infos.clear();
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_WINDOW));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
}
}
} // namespace Rosen
} // namespace OHOS

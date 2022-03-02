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

#include "window_inner_manager_test.h"

#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
WindowInnerManager& WindowInnerManagerTest::innerManager_ = SingletonContainer::Get<WindowInnerManager>();

void WindowInnerManagerTest::SetUpTestCase()
{
    innerManager_.Init();
    ASSERT_TRUE(innerManager_.hasInitThread_);
    ASSERT_FALSE(innerManager_.needDestroyThread_);
}

void WindowInnerManagerTest::TearDownTestCase()
{
    innerManager_.SendMessage(INNER_WM_DESTROY_THREAD);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_FALSE(innerManager_.hasInitThread_);
    ASSERT_TRUE(innerManager_.needDestroyThread_);
}

void WindowInnerManagerTest::SetUp()
{
}

void WindowInnerManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SendMessage01
 * @tc.desc: Init thread, wait for message, test dividerMap
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, InitThread01, Function | SmallTest | Level3)
{
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) == nullptr);
}

/**
 * @tc.name: SendMessage01
 * @tc.desc: create and show divider, mock create window return nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, SendMessage01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    innerManager_.SendMessage(INNER_WM_CREATE_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) == nullptr);
}

/**
 * @tc.name: SendMessage02
 * @tc.desc: create and show divider, mock create window return new window, hide and destroy success
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, SendMessage02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    innerManager_.SendMessage(INNER_WM_CREATE_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) != nullptr);

    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    innerManager_.SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) == nullptr);
}

/**
 * @tc.name: SendMessage03
 * @tc.desc: create and show divider, mock hide return error
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, SendMessage03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    innerManager_.SendMessage(INNER_WM_CREATE_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) != nullptr);

    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    innerManager_.SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) != nullptr);

    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    innerManager_.SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
}

/**
 * @tc.name: SendMessage04
 * @tc.desc: create and show divider, mock destroy return error
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, SendMessage04, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    innerManager_.SendMessage(INNER_WM_CREATE_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) != nullptr);

    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    innerManager_.SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
    ASSERT_TRUE(innerManager_.GetDividerWindow(displayId_) != nullptr);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    innerManager_.SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
    sleep(SEND_MSG_SLEEP_S);
}
}
} // namespace Rosen
} // namespace OHOS
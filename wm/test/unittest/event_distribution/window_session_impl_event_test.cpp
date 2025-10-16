/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "window_session_impl.h"
#include "mock_session.h"
#include "mock_session_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowSessionImplEventTest::SetUpTestCase()
{
}

void WindowSessionImplEventTest::TearDownTestCase()
{
}

void WindowSessionImplEventTest::SetUp()
{
}

void WindowSessionImplEventTest::TearDown()
{
}

namespace {
/**
 * @tc.name: LockCursor
 * @tc.desc: LockCursor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplEventTest, LockCursor, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("LockCursor");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->hostSession_ = nullptr;
    auto ret = window->LockCursor(1, true);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);

    window->property_->persistentId_ = 10;
    window->state_ = WindowState::STATE_SHOWN;
    sptr<SessionStubMocker> session = sptr<SessionStubMocker>::MakeSptr();
    window->hostSession_ = session;
    ret = window->LockCursor(1, true);
    EXPECT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: LockCursor
 * @tc.desc: LockCursor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplEventTest, UnlockCursor, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UnlockCursor");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->hostSession_ = nullptr;
    auto ret = window->UnlockCursor(1);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);

    window->property_->persistentId_ = 10;
    window->state_ = WindowState::STATE_SHOWN;
    sptr<SessionStubMocker> session = sptr<SessionStubMocker>::MakeSptr();
    window->hostSession_ = session;
    ret = window->UnlockCursor(1);
    EXPECT_EQ(ret, WMError::WM_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

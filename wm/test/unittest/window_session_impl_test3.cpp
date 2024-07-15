/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "window_session_impl.h"

#include <gtest/gtest.h>

#include "ability_context_impl.h"
#include "display_info.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplTest3 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowSessionImpl> window_;
};

void WindowSessionImplTest3::SetUpTestCase()
{
}

void WindowSessionImplTest3::TearDownTestCase()
{
}

void WindowSessionImplTest3::SetUp()
{
}

void WindowSessionImplTest3::TearDown()
{
    if (window_ != nullptr) {
        window_->Destroy();
    }
}

namespace {
sptr<WindowSessionImpl> GetTestWindowImpl(const std::string& name)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    option->SetWindowName(name);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    if (window == nullptr) {
        return nullptr;
    }
    SessionInfo sessionInfo = {name, name, name};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    if (session == nullptr) {
        return nullptr;
    }
    window->hostSession_ = session;
    return window;
}

/**
 * @tc.name: SetDecorHeight
 * @tc.desc: SetDecorHeight01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetDecorHeight01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetDecorHeight01 start";
    window_ = GetTestWindowImpl("SetDecorHeight01");
    ASSERT_NE(window_, nullptr);
    WMError res = window_->SetDecorHeight(100);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetDecorHeight01 end";
}

/**
 * @tc.name: SetInputEventConsumer
 * @tc.desc: SetInputEventConsumer01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetInputEventConsumer01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetInputEventConsumer01 start";
    window_ = GetTestWindowImpl("SetInputEventConsumer01");
    ASSERT_NE(window_, nullptr);
    window_->inputEventConsumer_ = nullptr;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer = std::make_shared<MockInputEventConsumer>();
    window_->SetInputEventConsumer(inputEventConsumer);
    ASSERT_NE(window_->inputEventConsumer_, nullptr);
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetInputEventConsumer01 end";
}

/**
 * @tc.name: GetListeners
 * @tc.desc: GetListeners01 IDisplayMoveListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetListeners01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: GetListeners01 start";
    window_ = GetTestWindowImpl("GetListeners01");
    ASSERT_NE(window_, nullptr);
    window_->displayMoveListeners_.clear();
    window_->NotifyDisplayMove(0, 100);
    ASSERT_TRUE(window_->displayMoveListeners_[window_->GetPersistentId()].empty());

    sptr<IDisplayMoveListener> displayMoveListener = new (std::nothrow) MockIDisplayMoveListener();
    ASSERT_EQ(window_->RegisterDisplayMoveListener(displayMoveListener), WMError::WM_OK);
    window_->NotifyDisplayMove(0, 100);
    ASSERT_FALSE(window_->displayMoveListeners_[window_->GetPersistentId()].empty());
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: GetListeners01 end";
}

/**
 * @tc.name: RegisterWindowNoInteractionListener
 * @tc.desc: RegisterWindowNoInteractionListener01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, RegisterWindowNoInteractionListener01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: RegisterWindowNoInteractionListener01 start";
    window_ = GetTestWindowImpl("RegisterWindowNoInteractionListener01");
    ASSERT_NE(window_, nullptr);
    ASSERT_EQ(window_->RegisterWindowNoInteractionListener(nullptr), WMError::WM_ERROR_NULLPTR);
    ASSERT_EQ(window_->UnregisterWindowNoInteractionListener(nullptr), WMError::WM_ERROR_NULLPTR);

    sptr<IWindowNoInteractionListener> windowNoInteractionListenerSptr =
        new (std::nothrow) MockIWindowNoInteractionListener();
    ASSERT_EQ(window_->RegisterWindowNoInteractionListener(windowNoInteractionListenerSptr), WMError::WM_OK);
    ASSERT_EQ(window_->UnregisterWindowNoInteractionListener(windowNoInteractionListenerSptr), WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: RegisterWindowNoInteractionListener01 end";
}
}
} // namespace Rosen
} // namespace OHOS
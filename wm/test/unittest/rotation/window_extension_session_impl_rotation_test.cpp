/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <ability_context_impl.h>
#include <context_impl.h>
#include <int_wrapper.h>
#include <transaction/rs_transaction.h>
#include <want_params_wrapper.h>

#include "display_info.h"
#include "extension/extension_business_info.h"
#include "extension_data_handler.h"
#include "iremote_object_mocker.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "window_extension_session_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace std;
namespace OHOS {
namespace Rosen {
class WindowExtensionSessionImplRotationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<WindowExtensionSessionImpl> window_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void WindowExtensionSessionImplRotationTest::SetUpTestCase()
{
}

void WindowExtensionSessionImplRotationTest::TearDownTestCase()
{
}

void WindowExtensionSessionImplRotationTest::SetUp()
{
    sptr<WindowOption> option = new(std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("WindowExtensionSessionImplRotationTest");
    window_ = new(std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window_);
    if (!handler_) {
        auto runner = AppExecFwk::EventRunner::Create("WindowExtensionSessionImplRotationTest");
        handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }
    window_->handler_ = handler_;
}

void WindowExtensionSessionImplRotationTest::TearDown()
{
    window_ = nullptr;
}

namespace {
/**
 * @tc.name: UpdateRectForRotation
 * @tc.desc: UpdateRect Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplRotationTest, UpdateRectForRotation, TestSize.Level1)
{
    Rect rect;
    WindowSizeChangeReason wmReason = WindowSizeChangeReason{0};
    std::shared_ptr<RSTransaction> rsTransaction = std::make_shared<RSTransaction>();
    rsTransaction->syncId_ = 1;
    rsTransaction->isOpenSyncTransaction_ = true;
    ASSERT_NE(nullptr, window_);
    window_->UpdateRectForRotation(rect, rect, wmReason, rsTransaction);

    Rect preRect;
    window_->UpdateRectForRotation(rect, preRect, wmReason, rsTransaction);

    rsTransaction->isOpenSyncTransaction_ = false;
    window_->UpdateRectForRotation(rect, rect, wmReason, rsTransaction);

    rsTransaction->syncId_ = -1;
    window_->UpdateRectForRotation(rect, rect, wmReason, rsTransaction);

    rsTransaction = nullptr;
    window_->UpdateRectForRotation(rect, rect, wmReason, rsTransaction);
    window_->UpdateRectForOtherReason(rect, wmReason);

    window_->handler_ = nullptr;
    window_->UpdateRectForRotation(rect, rect, wmReason, rsTransaction);
    usleep(WAIT_SYNC_IN_NS);
}
}
} // namespace Rosen
} // namespace OHOS
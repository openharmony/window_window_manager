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
#include "iremote_object_mocker.h"
#include "mock/mock_session_stub.h"
#include "session/host/include/zidl/session_stub.h"
#include "accessibility_event_info_parcel.h"
#include "window_manager_hilog.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStubMockTest"};
}

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
class SessionStubMockTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStub> session_ = nullptr;
};

void SessionStubMockTest::SetUpTestCase()
{
}

void SessionStubMockTest::TearDownTestCase()
{
}

void SessionStubMockTest::SetUp()
{
    session_ = new (std::nothrow) SessionStubMocker();
    EXPECT_NE(nullptr, session_);
}

void SessionStubMockTest::TearDown()
{
    session_ = nullptr;
}

namespace {
/**
 * @tc.name: HandleTransferAccessibilityEvent
 * @tc.desc: sessionStub HandleTransferAccessibilityEvent
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubMockTest, HandleTransferAccessibilityEvent, Function | SmallTest | Level2)
{
    WLOGI("HandleTransferAccessibilityEvent begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    Accessibility::AccessibilityEventInfo info;
    Accessibility::AccessibilityEventInfoParcel infoParcel(info);
    data.WriteParcelable(&infoParcel);
    int32_t uiExtensionIdLevel = -1;
    data.WriteInt32(uiExtensionIdLevel);
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleTransferAccessibilityEvent(data, reply));
    WLOGI("HandleTransferAccessibilityEvent end");
}
}
} // namespace Rosen
} // namespace OHOS
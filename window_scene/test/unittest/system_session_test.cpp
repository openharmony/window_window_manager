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
#include "session/host/include/system_session.h"
#include "common/include/session_permission.h"
#include "session/host/include/session.h"
#include "window_helper.h"
#include "window_manager_hilog.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SystemSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr<SystemSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SystemSession> systemSession_;
};

void SystemSessionTest::SetUpTestCase()
{
}

void SystemSessionTest::TearDownTestCase()
{
}

void SystemSessionTest::SetUp()
{
    systemSession_ = new SystemSession(info, specificCallback);
}

void SystemSessionTest::TearDown()
{
    systemSession_ = nullptr;
}

namespace {
/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, Show, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();

    ASSERT_TRUE((systemSession_ != nullptr));
    ASSERT_EQ(WSError::WS_OK, systemSession_->Show(property));
}

}
}
}
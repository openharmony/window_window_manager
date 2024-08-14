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

#include "session/container/include/zidl/session_stage_stub.h"
#include "session/container/include/zidl/session_stage_ipc_interface_code.h"
#include <ipc_types.h>
#include <iremote_stub.h>

#include "iremote_object_mocker.h"
#include "mock/mock_session_stage.h"
#include <message_option.h>
#include <message_parcel.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager.h"
#include "window_manager_agent.h"
#include "ws_common.h"
#include "zidl/window_manager_agent_interface.h"
#include "window_manager_hilog.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStageStubLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SessionStageStub> sessionStageStub_ = new SessionStageMocker();
};

void SessionStageStubLifecycleTest::SetUpTestCase()
{
}

void SessionStageStubLifecycleTest::TearDownTestCase()
{
}

void SessionStageStubLifecycleTest::SetUp()
{
}

void SessionStageStubLifecycleTest::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleNotifyDestroy
 * @tc.desc: test function : HandleNotifyDestroy
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubLifecycleTest, HandleNotifyDestroy, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->HandleNotifyDestroy(data, reply));
}

/**
 * @tc.name: HandleNotifyForegroundInteractiveStatus
 * @tc.desc: test function : HandleNotifyForegroundInteractiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubLifecycleTest, HandleNotifyForegroundInteractiveStatus, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(true);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->HandleNotifyForegroundInteractiveStatus(data, reply));
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: test function : NotifySessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubLifecycleTest, HandleNotifySessionForeground, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(1);
    data.WriteBool(true);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->HandleNotifySessionForeground(data, reply));
}

/**
 * @tc.name: NotifySessionBackground
 * @tc.desc: test function : NotifySessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubLifecycleTest, HandleNotifySessionBackground, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(1);
    data.WriteBool(true);
    data.WriteBool(true);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->HandleNotifySessionBackground(data, reply));
}
}
}
}
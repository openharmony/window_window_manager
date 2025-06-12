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

#include "iremote_object_mocker.h"
#include "mock_message_parcel.h"
#include "session/container/include/zidl/session_stage_proxy.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStageProxyLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionStageProxy> sessionStage_ = sptr<SessionStageProxy>::MakeSptr(iRemoteObjectMocker);
};

void SessionStageProxyLayoutTest::SetUpTestCase() {}

void SessionStageProxyLayoutTest::TearDownTestCase() {}

void SessionStageProxyLayoutTest::SetUp() {}

void SessionStageProxyLayoutTest::TearDown() {}

namespace {
/**
 * @tc.name: NotifySingleHandTransformChange
 * @tc.desc: test function : NotifySingleHandTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLayoutTest, NotifySingleHandTransformChange, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: NotifySingleHandTransformChange start";
    SingleHandTransform singleHandTransform;
    sessionStage_->NotifySingleHandTransformChange(singleHandTransform);
    ASSERT_TRUE((sessionStage_ != nullptr));
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: NotifySingleHandTransformChange end";
}

/**
 * @tc.name: UpdateWindowLayoutById01
 * @tc.desc: test function : UpdateWindowLayoutById
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLayoutTest, UpdateWindowLayoutById01, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: UpdateWindowLayoutById01 start";
    sessionStage_->UpdateWindowLayoutById(0, 1);
    ASSERT_TRUE((sessionStage_ != nullptr));
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: UpdateWindowLayoutById01 end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
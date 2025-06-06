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
#include "pointer_event.h"
#include "session_proxy.h"
#include "ws_common.h"
#include "wm_common.h"

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionProxyAnimationTest : public testing::Test {
public:
    SessionProxyAnimationTest() {}
    ~SessionProxyAnimationTest() {}
};
namespace {
/**
 * @tc.name: SetWindowCornerRadius
 * @tc.desc: SetWindowCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyAnimationTest, SetWindowCornerRadius, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyAnimationTest: SetWindowCornerRadius start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WSError res = sProxy->SetWindowCornerRadius(1.0f);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyAnimationTest: SetWindowCornerRadius end";
}

/**
 * @tc.name: SetWindowShadows
 * @tc.desc: SetWindowShadows
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyAnimationTest, SetWindowShadows, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyAnimationTest: SetWindowShadows start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);

    ShadowsInfo shadowsInfo = { 20.0, "#FF0000", 0.0, 0.0, true, true, true, true };
    WSError res = sProxy->SetWindowShadows(shadowsInfo);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyAnimationTest: SetWindowShadows end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
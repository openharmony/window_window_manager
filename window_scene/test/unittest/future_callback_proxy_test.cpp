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

#include <gtest/gtest.h>
#include "future_callback_proxy.h"
#include "iremote_object_mocker.h"
#include "window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class FutureCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FutureCallbackProxyTest::SetUpTestCase()
{
}

void FutureCallbackProxyTest::TearDownTestCase()
{
}

void FutureCallbackProxyTest::SetUp()
{
}

void FutureCallbackProxyTest::TearDown()
{
}
namespace {
/**
 * @tc.name: OnUpdateSessionRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(FutureCallbackProxyTest, OnUpdateSessionRect, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "FutureCallbackProxyTest: OnUpdateSessionRect start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<FutureCallbackProxy> fProxy = sptr<FutureCallbackProxy>::MakeSptr(iRemoteObjectMocker);
    WSRect rect{.posX_ = 1, .posY_ = 1, .width_ = 100, .height_ = 100};
    WSError res = fProxy->OnUpdateSessionRect(rect);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "FutureCallbackProxyTest: OnUpdateSessionRect end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
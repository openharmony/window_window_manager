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

#include "pattern_detach_callback_proxy.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PatternDetachCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<PatternDetachCallbackProxy> proxy_;
};

void PatternDetachCallbackProxyTest::SetUpTestCase()
{
}

void PatternDetachCallbackProxyTest::TearDownTestCase()
{
}

void PatternDetachCallbackProxyTest::SetUp()
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    proxy_ = new PatternDetachCallbackProxy(iRemoteObjectMocker);
}

void PatternDetachCallbackProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnPatternDetach
 * @tc.desc: OnPatternDetach Test
 * @tc.type: FUNC
 */
HWTEST_F(PatternDetachCallbackProxyTest, OnPatternDetach, Function | SmallTest | Level2)
{
    int32_t persisitentId = 0;
    int resultValue = 0;
    std::function<void()> func = [this, &resultValue, persisitentId]() {
        proxy_->OnPatternDetach(persisitentId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

}
} // namespace Rosen
} // namespace OHOS
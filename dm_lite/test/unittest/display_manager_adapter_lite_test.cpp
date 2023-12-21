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
#include "display_manager_adapter_lite.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerAdapterLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAdapterLiteTest::SetUpTestCase()
{
}

void DisplayManagerAdapterLiteTest::TearDownTestCase()
{
}

void DisplayManagerAdapterLiteTest::SetUp()
{
}

void DisplayManagerAdapterLiteTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteDied
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, OnRemoteDied, Function | SmallTest | Level2)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipientLite(SingletonContainer::Get<DisplayManagerAdapterLite>());
    dmsDeath_->OnRemoteDied(nullptr);
    ASSERT_NE(dmsDeath_, nullptr);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, OnRemoteDied01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipientLite(SingletonContainer::Get<DisplayManagerAdapterLite>());
    SingletonContainer::Get<DisplayManagerAdapterLite>().InitDMSProxy();
    sptr<IRemoteObject> obj =
        SingletonContainer::Get<DisplayManagerAdapterLite>().displayManagerServiceProxy_->AsObject();
    wptr<IRemoteObject> wptrDeath = obj;
    dmsDeath_->OnRemoteDied(wptrDeath);
    ASSERT_NE(dmsDeath_, nullptr);
}

/**
 * @tc.name: Clear
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, Clear, Function | SmallTest | Level2)
{
    SingletonContainer::Get<DisplayManagerAdapterLite>().InitDMSProxy();
    SingletonContainer::Get<DisplayManagerAdapterLite>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<DisplayManagerAdapterLite>().isProxyValid_);
}

/**
 * @tc.name: Clear01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, Clear01, Function | SmallTest | Level2)
{
    SingletonContainer::Get<DisplayManagerAdapterLite>().InitDMSProxy();
    SingletonContainer::Get<DisplayManagerAdapterLite>().displayManagerServiceProxy_ = nullptr;
    SingletonContainer::Get<DisplayManagerAdapterLite>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<DisplayManagerAdapterLite>().isProxyValid_);
}
}
}
}

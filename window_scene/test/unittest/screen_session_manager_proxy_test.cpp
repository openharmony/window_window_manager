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
#include <message_option.h>
#include <message_parcel.h>
#include <common/rs_rect.h>
#include <transaction/rs_marshalling_helper.h>
#include <iremote_broker.h>
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager_agent.h"
#include "zidl/screen_session_manager_proxy.h"
#include "zidl/window_manager_agent_interface.h"
#include "display_manager_adapter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class ScreenSessionManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSessionManagerProxyTest::SetUpTestCase()
{
}

void ScreenSessionManagerProxyTest::TearDownTestCase()
{
}

void ScreenSessionManagerProxyTest::SetUp()
{
}

void ScreenSessionManagerProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetPrivacyStateByDisplayId
 * @tc.desc: SetPrivacyStateByDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetPrivacyStateByDisplayId, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    DisplayId id = 0;
    bool hasPrivate = false;
    
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        screenSessionManagerProxy->SetPrivacyStateByDisplayId(id, hasPrivate);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: SetScreenPrivacyWindowList
 * @tc.desc: SetScreenPrivacyWindowList
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenPrivacyWindowList, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    DisplayId id = 0;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        screenSessionManagerProxy->SetScreenPrivacyWindowList(id, privacyWindowList);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}
}
}
}
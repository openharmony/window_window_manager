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
#include "screen_session_manager_client.h"
#include "zidl/screen_session_manager_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSessionManagerClientTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    ScreenSessionManagerClient* screenSessionManagerClient_;
};

void ScreenSessionManagerClientTest::SetUp()
{
    screenSessionManagerClient_ = &ScreenSessionManagerClient::GetInstance();
}

void ScreenSessionManagerClientTest::TearDown()
{
    screenSessionManagerClient_ = nullptr;
}

/**
 * @tc.name: RegisterScreenConnectionListener
 * @tc.desc: RegisterScreenConnectionListener test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, RegisterScreenConnectionListener, Function | SmallTest | Level2)
{
    IScreenConnectionListener* listener = nullptr;
    screenSessionManagerClient_->RegisterScreenConnectionListener(listener);
    EXPECT_EQ(screenSessionManagerClient_->screenConnectionListener_, nullptr);
}

/**
 * @tc.name: GetScreenSession
 * @tc.desc: GetScreenSession test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetScreenSession, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession, nullptr);

    screenSession = new ScreenSession(0, ScreenProperty(), 0);
    screenSessionManagerClient_->screenSessionMap_.emplace(screenId, screenSession);

    auto screenSession2 = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession2, screenSession);

    screenSessionManagerClient_->screenSessionMap_.clear();
    screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: OnScreenConnectionChanged01
 * @tc.desc: OnScreenConnectionChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenConnectionChanged01, Function | SmallTest | Level2)
{
    EXPECT_EQ(screenSessionManagerClient_->screenSessionManager_, nullptr);
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    screenSessionManagerClient_->screenSessionManager_ = new ScreenSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ScreenId rsId = 0;
    std::string name;
    screenSessionManagerClient_->OnScreenConnectionChanged(screenId, screenEvent, rsId, name);
    sptr<ScreenSession> screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_NE(screenSession, nullptr);
}

/**
 * @tc.name: OnScreenConnectionChanged02
 * @tc.desc: OnScreenConnectionChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenConnectionChanged02, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenEvent screenEvent = ScreenEvent::DISCONNECTED;
    ScreenId rsId = 0;
    std::string name;
    screenSessionManagerClient_->OnScreenConnectionChanged(screenId, screenEvent, rsId, name);
    sptr<ScreenSession> screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession, nullptr);
}
} // namespace Rosen
} // namespace OHOS

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
#include "session_manager/include/extension_session_manager.h"
#include "session/host/include/extension_session.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ExtensionSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ExtensionSessionManagerTest::SetUpTestCase()
{

}

void ExtensionSessionManagerTest::TearDownTestCase()
{
}

void ExtensionSessionManagerTest::SetUp()
{
}

void ExtensionSessionManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RequestExtensionSession
 * @tc.desc: RequestExtensionSession Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionManagerTest, RequestExtensionSession, Function | MediumTest | Level2)
{
    SessionInfo sessionInfo;
    sessionInfo.abilityName_ = "RequestExtensionSession";
    ASSERT_NE(nullptr, ExtensionSessionManager::GetInstance().RequestExtensionSession(sessionInfo));
}

/**
 * @tc.name: RequestExtensionSessionActivation01
 * @tc.desc: RequestExtensionSessionActivation Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionManagerTest, RequestExtensionSessionActivation01, Function | MediumTest | Level2)
{
    SessionInfo info;
    sptr<ExtensionSession> extensionSession = new ExtensionSession(info);
    ASSERT_EQ(WSError::WS_OK,
                ExtensionSessionManager::GetInstance().RequestExtensionSessionActivation(
                    extensionSession, 1, nullptr));
}

/**
 * @tc.name: RequestExtensionSessionBackground01
 * @tc.desc: RequestExtensionSessionBackground Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionManagerTest, RequestExtensionSessionBackground01, Function | MediumTest | Level2)
{
    SessionInfo info;
    sptr<ExtensionSession> extensionSession = new ExtensionSession(info);
    ASSERT_EQ(WSError::WS_OK,
                ExtensionSessionManager::GetInstance().RequestExtensionSessionBackground(extensionSession, nullptr));
}


/**
 * @tc.name: RequestExtensionSessionDestruction01
 * @tc.desc: RequestExtensionSessionDestruction Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionManagerTest, RequestExtensionSessionDestruction01, Function | MediumTest | Level2)
{
    SessionInfo info;
    sptr<ExtensionSession> extensionSession = new ExtensionSession(info);
    ASSERT_EQ(WSError::WS_OK,
                ExtensionSessionManager::GetInstance().RequestExtensionSessionDestruction(extensionSession, nullptr));
}

/**
 * @tc.name: GetInstance
 * @tc.desc: ExtensionSessionManager get instance and init
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionManagerTest, GetInstance, Function | MediumTest | Level2)
{
    ExtensionSessionManager *instance = &ExtensionSessionManager::GetInstance();
    if (instance != nullptr) {
        instance->Init();
    }
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: SetAbilitySessionInfo
 * @tc.desc: ExtensionSessionManager set ability session info
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionManagerTest, SetAbilitySessionInfo, Function | MediumTest | Level2)
{
    AAFwk::Want want;
    SessionInfo infoInput;
    infoInput.want = std::make_shared<AAFwk::Want>(want);
    sptr<ExtensionSession> extSession = new ExtensionSession(infoInput);
    ExtensionSessionManager *instance = &ExtensionSessionManager::GetInstance();
    sptr<AAFwk::SessionInfo> result = instance->SetAbilitySessionInfo(extSession);
    int32_t persistentId = extSession->GetPersistentId();
    ASSERT_EQ(result->persistentId, persistentId);
    delete extSession;
    delete result;
}

/**
 * @tc.name: RequestExtensionSessionDestruction02
 * @tc.desc: RequestExtensionSessionDestruction Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionManagerTest, RequestExtensionSessionDestruction02, Function | MediumTest | Level2)
{
    AAFwk::Want want;
    SessionInfo infoInput;
    infoInput.want = std::make_shared<AAFwk::Want>(want);
    sptr<ExtensionSession> extSession = nullptr;
    ExtensionSessionManager *instance = &ExtensionSessionManager::GetInstance();
    WSError result01 = instance->RequestExtensionSessionDestruction(extSession, nullptr);
    EXPECT_EQ(result01, WSError::WS_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

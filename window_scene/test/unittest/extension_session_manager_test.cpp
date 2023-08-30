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
    ExtensionSessionManager extensionSessionManager;
    ASSERT_NE(nullptr, 
                extensionSessionManager.RequestExtensionSession(sessionInfo));
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
    ExtensionSessionManager extensionSessionManager;
    ASSERT_EQ(WSError::WS_OK, 
                extensionSessionManager.RequestExtensionSessionActivation(extensionSession, 1));
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
    ExtensionSessionManager extensionSessionManager;
    ASSERT_EQ(WSError::WS_OK, 
                extensionSessionManager.RequestExtensionSessionBackground(extensionSession));
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
    ExtensionSessionManager extensionSessionManager;
    ASSERT_EQ(WSError::WS_OK, 
                extensionSessionManager.RequestExtensionSessionDestruction(extensionSession));
}
} // namespace
} // namespace Rosen
} // namespace OHOS
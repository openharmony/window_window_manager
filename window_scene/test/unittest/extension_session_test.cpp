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

#include "extension_session.h"
#include <gtest/gtest.h>
#include "session_info.h"
#include "interfaces/include/ws_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ExtensionSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<ExtensionSession::ExtensionSessionEventCallback> extSessionEventCallback = nullptr;
};

void TransferAbilityResultTest(uint32_t resultCode, const AAFwk::Want& want)
{   
    resultCode--;
}

void TransferExtensionDataTest(const AAFwk::WantParams& wantParams)
{
    AAFwk::WantParams test(wantParams);
}

void NotifyRemoteReadyTest()
{
}

void ExtensionSessionTest::SetUpTestCase()
{
}

void ExtensionSessionTest::TearDownTestCase()
{
}

void ExtensionSessionTest::SetUp()
{
    extSessionEventCallback = new(std::nothrow) ExtensionSession::ExtensionSessionEventCallback();
}

void ExtensionSessionTest::TearDown()
{
    delete extSessionEventCallback;
}

namespace {
/**
 * @tc.name: RegisterExtensionSessionEventCallback
 * @tc.desc: test function : RegisterExtensionSessionEventCallback
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, RegisterExtensionSessionEventCallback, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    extensionSession_.RegisterExtensionSessionEventCallback(extSessionEventCallback);
    ASSERT_NE(nullptr, extensionSession_.GetExtensionSessionEventCallback());
}

/**
 * @tc.name: GetExtensionSessionEventCallback
 * @tc.desc: test function : GetExtensionSessionEventCallback
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, GetExtensionSessionEventCallback, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    ASSERT_NE(nullptr, extensionSession_.GetExtensionSessionEventCallback());
}

/**
 * @tc.name: TransferAbilityResult
 * @tc.desc: test function : TransferAbilityResult
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferAbilityResult, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    extSessionEventCallback->transferAbilityResultFunc_ = TransferAbilityResultTest;
    extensionSession_.RegisterExtensionSessionEventCallback(extSessionEventCallback);
    uint32_t test01 = 15;
    AAFwk::Want want;
    WSError result = extensionSession_.TransferAbilityResult(test01, want);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: TransferExtensionData
 * @tc.desc: test function : TransferExtensionData
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferExtensionData, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    extSessionEventCallback->transferExtensionDataFunc_= TransferExtensionDataTest;
    extensionSession_.RegisterExtensionSessionEventCallback(extSessionEventCallback);
    AAFwk::WantParams wantParams;
    WSError result = extensionSession_.TransferExtensionData(wantParams);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyRemoteReady
 * @tc.desc: test function : NotifyRemoteReady
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, NotifyRemoteReady, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    extSessionEventCallback->notifyRemoteReadyFunc_= NotifyRemoteReadyTest;
    extensionSession_.RegisterExtensionSessionEventCallback(extSessionEventCallback);
    extensionSession_.NotifyRemoteReady();
}

/**
 * @tc.name: TransferComponentData
 * @tc.desc: test function : TransferComponentData
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferComponentData, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    info.isSystem_ = true;
    ExtensionSession extensionSession_(info);
    AAFwk::WantParams wantParams;
    WSError result = extensionSession_.TransferComponentData(wantParams);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
}

}
}
}
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
#include "accessibility_event_info.h"
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

void NotifySyncOnTest()
{
}

void NotifyAsyncOnTest()
{
}

void NotifyBindModalTest()
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
    extSessionEventCallback = nullptr;
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
    ASSERT_TRUE(extensionSession_ != nullptr);
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

/**
 * @tc.name: NotifySyncOn
 * @tc.desc: test function : NotifySyncOn
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, NotifySyncOn, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    ASSERT_TRUE(extensionSession_ != nullptr);
    extSessionEventCallback->notifyAsyncOnFunc_ = NotifySyncOnTest;
    extensionSession_.RegisterExtensionSessionEventCallback(extSessionEventCallback);
    extensionSession_.NotifySyncOn();
}

/**
 * @tc.name: NotifyAsyncOn
 * @tc.desc: test function : NotifyAsyncOn
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, NotifyAsyncOn, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    ASSERT_TRUE(extensionSession_ != nullptr);
    extSessionEventCallback->notifyAsyncOnFunc_ = NotifyAsyncOnTest;
    extensionSession_.RegisterExtensionSessionEventCallback(extSessionEventCallback);
    extensionSession_.NotifyAsyncOn();
}

/**
 * @tc.name: TriggerBindModalUIExtension
 * @tc.desc: test function : TriggerBindModalUIExtension
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TriggerBindModalUIExtension, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    ASSERT_TRUE(extensionSession_ != nullptr);
    extSessionEventCallback->notifyBindModalFunc_ = NotifyBindModalTest;
    extensionSession_.RegisterExtensionSessionEventCallback(extSessionEventCallback);
    extensionSession_.TriggerBindModalUIExtension();
}

/**
 * @tc.name: TransferAccessibilityEvent
 * @tc.desc: test function : TransferAccessibilityEvent
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferAccessibilityEvent, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    ASSERT_TRUE(extensionSession_ != nullptr);
    OHOS::Accessibility::AccessibilityEventInfo info1;
    int64_t uiExtensionIdLevel = 6;
    WSError result = extensionSession_.TransferAccessibilityEvent(info1, uiExtensionIdLevel);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateAvoidArea
 * @tc.desc: test function : UpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, UpdateAvoidArea, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession_(info);
    ASSERT_TRUE(extensionSession_ != nullptr);
    sptr<AvoidArea> avoidArea = new AvoidArea();
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM;
    WSError res = extensionSession_.UpdateAvoidArea(avoidArea, type);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, res);
}

}
}
}
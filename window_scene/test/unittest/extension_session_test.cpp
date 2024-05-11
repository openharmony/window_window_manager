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
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"

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
 * @tc.name: NotifyDensityFollowHost01
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, NotifyDensityFollowHost01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyDensityFollowHost01";
    info.bundleName_ = "NotifyDensityFollowHost01";
    ExtensionSession extensionSession(info);
    extensionSession.state_ = SessionState::STATE_CONNECT;
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    extensionSession.sessionStage_ = mockSessionStage;

    bool isFollowHost = true;
    float densityValue = 1.0f;
    EXPECT_CALL(*mockSessionStage, NotifyDensityFollowHost(isFollowHost, densityValue));
    WSError res = extensionSession.NotifyDensityFollowHost(isFollowHost, densityValue);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyDensityFollowHost02
 * @tc.desc: session is invalid
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, NotifyDensityFollowHost02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyDensityFollowHost02";
    info.bundleName_ = "NotifyDensityFollowHost02";
    ExtensionSession extensionSession(info);

    bool isFollowHost = true;
    float densityValue = 1.0f;
    WSError res = extensionSession.NotifyDensityFollowHost(isFollowHost, densityValue);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, res);
}

/**
 * @tc.name: NotifyDensityFollowHost03
 * @tc.desc: sessionStage_ is invalid
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, NotifyDensityFollowHost03, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyDensityFollowHost03";
    info.bundleName_ = "NotifyDensityFollowHost03";
    ExtensionSession extensionSession(info);
    extensionSession.state_ = SessionState::STATE_CONNECT;
    extensionSession.sessionStage_ = nullptr;

    bool isFollowHost = true;
    float densityValue = 1.0f;
    WSError res = extensionSession.NotifyDensityFollowHost(isFollowHost, densityValue);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, res);
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
 * @tc.name: TransferKeyEventForConsumed01
 * @tc.desc: TransferKeyEventForConsumed not timeout
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferKeyEventForConsumed01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession(info);
    ASSERT_NE(extensionSession, nullptr);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    sptr<WindowEventChannelMocker> mockEventChannel = new (std::nothrow) WindowEventChannelMocker(mockSessionStage);
    extensionSession.windowEventChannel_ = mockEventChannel;
    extensionSession.channelListener_ = new WindowEventChannelListener();
    EXPECT_CALL(*mockEventChannel, TransferKeyEventForConsumedAsync)
        .WillOnce([](const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     bool isPreImeEvent,
                     const sptr<IRemoteObject> &listener) {
            auto channelListener = iface_cast<IWindowEventChannelListener>(listener);
            channelListener->OnTransferKeyEventForConsumed(true, WSError::WS_OK);
            return WSError::WS_OK;
        });

    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    bool isConsumed = false;
    bool isTimeout = false;
    bool isPreImeEvent = false;
    WSError result = extensionSession.TransferKeyEventForConsumed(keyEvent, isConsumed, isTimeout, isPreImeEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(isTimeout, false);
}

/**
 * @tc.name: TransferKeyEventForConsumed02
 * @tc.desc: TransferKeyEventForConsumed timeout
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferKeyEventForConsumed02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession(info);
    ASSERT_NE(extensionSession, nullptr);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    sptr<WindowEventChannelMocker> mockEventChannel = new (std::nothrow) WindowEventChannelMocker(mockSessionStage);
    extensionSession.windowEventChannel_ = mockEventChannel;
    EXPECT_CALL(*mockEventChannel, TransferKeyEventForConsumedAsync);
    extensionSession.channelListener_ = new WindowEventChannelListener();

    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    bool isConsumed = false;
    bool isTimeout = false;
    bool isPreImeEvent = false;
    WSError result = extensionSession.TransferKeyEventForConsumed(keyEvent, isConsumed, isTimeout, isPreImeEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(isTimeout, true);
}

/**
 * @tc.name: TransferKeyEventForConsumed03
 * @tc.desc: TransferKeyEventForConsumed windowEventChannel_ nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferKeyEventForConsumed03, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession(info);
    ASSERT_NE(extensionSession, nullptr);

    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    bool isConsumed = false;
    bool isTimeout = false;
    bool isPreImeEvent = false;
    WSError result = extensionSession.TransferKeyEventForConsumed(keyEvent, isConsumed, isTimeout, isPreImeEvent);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TransferKeyEventForConsumed04
 * @tc.desc: TransferKeyEventForConsumed keyEvent nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferKeyEventForConsumed04, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession(info);
    ASSERT_NE(extensionSession, nullptr);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    sptr<WindowEventChannelMocker> mockEventChannel = new (std::nothrow) WindowEventChannelMocker(mockSessionStage);
    extensionSession.windowEventChannel_ = mockEventChannel;

    auto keyEvent = nullptr;
    bool isConsumed = false;
    bool isTimeout = false;
    bool isPreImeEvent = false;
    WSError result = extensionSession.TransferKeyEventForConsumed(keyEvent, isConsumed, isTimeout, isPreImeEvent);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowEventChannelListenerOnRemoteRequest01
 * @tc.desc: WindowEventChannelListenerOnRemoteRequest01 test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, WindowEventChannelListenerOnRemoteRequest01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(WindowEventChannelListener::GetDescriptor());
    data.WriteBool(true);
    data.WriteInt32(0);
    uint32_t code = static_cast<uint32_t>(IWindowEventChannelListener::WindowEventChannelListenerMessage::
        TRANS_ID_ON_TRANSFER_KEY_EVENT_FOR_CONSUMED_ASYNC);
    WindowEventChannelListener listener;
    ASSERT_EQ(listener.OnRemoteRequest(code, data, reply, option), 0);
}

/**
 * @tc.name: WindowEventChannelListenerOnRemoteRequest02
 * @tc.desc: WindowEventChannelListenerOnRemoteRequest02 test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, WindowEventChannelListenerOnRemoteRequest02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(WindowEventChannelListener::GetDescriptor());
    data.WriteBool(true);
    data.WriteInt32(0);
    uint32_t code = static_cast<uint32_t>(10001);
    WindowEventChannelListener listener;
    ASSERT_EQ(listener.OnRemoteRequest(code, data, reply, option), IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: ChannelDeathRecipientOnRemoteDied01
 * @tc.desc: ChannelDeathRecipientOnRemoteDied01 test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, ChannelDeathRecipientOnRemoteDied01, Function | SmallTest | Level1)
{
    sptr<WindowEventChannelListener> listener = new (std::nothrow) WindowEventChannelListener();
    EXPECT_NE(nullptr, listener);
    sptr<IRemoteObject::DeathRecipient> deathRecipient = nullptr;
    deathRecipient = new (std::nothrow) ChannelDeathRecipient(listener);
    EXPECT_NE(nullptr, deathRecipient);
    sptr<IRemoteObject> wptrDeath = nullptr;
    wptrDeath = new (std::nothrow) WindowEventChannel(nullptr);
    deathRecipient->OnRemoteDied(wptrDeath);
    EXPECT_NE(nullptr, deathRecipient);
}

/**
 * @tc.name: ChannelDeathRecipientOnRemoteDied02
 * @tc.desc: ChannelDeathRecipientOnRemoteDied02 test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, ChannelDeathRecipientOnRemoteDied02, Function | SmallTest | Level1)
{
    sptr<WindowEventChannelListener> listener = new (std::nothrow) WindowEventChannelListener();
    EXPECT_NE(nullptr, listener);
    sptr<IRemoteObject::DeathRecipient> deathRecipient = nullptr;
    deathRecipient = new (std::nothrow) ChannelDeathRecipient(listener);
    EXPECT_NE(nullptr, deathRecipient);
    deathRecipient->OnRemoteDied(nullptr);
    EXPECT_NE(nullptr, deathRecipient);
}

/**
 * @tc.name: TransferKeyEventAsync
 * @tc.desc: TransferKeyEventAsync
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionSessionTest, TransferKeyEventAsync, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    ExtensionSession extensionSession(info);
    ASSERT_NE(extensionSession, nullptr);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    sptr<WindowEventChannelMocker> mockEventChannel = new (std::nothrow) WindowEventChannelMocker(mockSessionStage);
    extensionSession.windowEventChannel_ = mockEventChannel;
    extensionSession.channelListener_ = new WindowEventChannelListener();
    EXPECT_CALL(*mockEventChannel, TransferKeyEventForConsumedAsync)
        .WillOnce([](const std::shared_ptr<MMI::KeyEvent> &keyEvent,
                     bool isPreImeEvent,
                     const sptr<IRemoteObject> &listener) {
            auto channelListener = iface_cast<IWindowEventChannelListener>(listener);
            channelListener->OnTransferKeyEventForConsumed(true, WSError::WS_OK);
            return WSError::WS_OK;
        });

    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    bool isPreImeEvent = false;
    WSError result = extensionSession.TransferKeyEventAsync(keyEvent, isPreImeEvent);
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
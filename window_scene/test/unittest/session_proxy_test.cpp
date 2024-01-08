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
#include "session_proxy.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include "accessibility_event_info.h"
// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionProxyTest : public testing::Test {
  public:
    SessionProxyTest() {}
    ~SessionProxyTest() {}
};
namespace {
/**
 * @tc.name: Foreground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Foreground, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<WindowSessionProperty> property;
    WSError res = sProxy->Foreground(property);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground end";
}

/**
 * @tc.name: Foreground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Foreground1, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    WSError res = sProxy->Foreground(property);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground end";
}

/**
 * @tc.name: Background
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Background, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Background start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    WSError res = sProxy->Background();
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: Background end";
}

/**
 * @tc.name: Disconnect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Disconnect, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Disconnect start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    WSError res = sProxy->Disconnect();
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: Disconnect end";
}

/**
 * @tc.name: PendingSessionActivation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, PendingSessionActivation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: PendingSessionActivation start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    WSError res = sProxy->PendingSessionActivation(abilitySessionInfo);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);

    sptr<AAFwk::SessionInfo> abilitySessionInfo1 = new(std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(abilitySessionInfo1, nullptr);
    res = sProxy->PendingSessionActivation(abilitySessionInfo1);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: PendingSessionActivation end";
}

/**
 * @tc.name: WriteAbilitySessionInfoBasic
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, WriteAbilitySessionInfoBasic, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: WriteAbilitySessionInfoBasic start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    MessageParcel data;
    bool res = sProxy->WriteAbilitySessionInfoBasic(data, abilitySessionInfo);
    ASSERT_EQ(res, false);

    sptr<AAFwk::SessionInfo> abilitySessionInfo1 = new(std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(abilitySessionInfo1, nullptr);
    res = sProxy->WriteAbilitySessionInfoBasic(data, abilitySessionInfo1);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "SessionProxyTest: WriteAbilitySessionInfoBasic end";
}

/**
 * @tc.name: TerminateSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, TerminateSession, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: TerminateSession start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    WSError res = sProxy->TerminateSession(abilitySessionInfo);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);

    sptr<AAFwk::SessionInfo> abilitySessionInfo1 = new(std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(abilitySessionInfo1, nullptr);
    res = sProxy->TerminateSession(abilitySessionInfo1);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: TerminateSession end";
}

/**
 * @tc.name: NotifySessionException
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifySessionException, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifySessionException start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    WSError res = sProxy->NotifySessionException(abilitySessionInfo);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);

    sptr<AAFwk::SessionInfo> abilitySessionInfo1 = new(std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(abilitySessionInfo1, nullptr);
    res = sProxy->NotifySessionException(abilitySessionInfo1);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifySessionException end";
}

/**
 * @tc.name: UpdateActiveStatus
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateActiveStatus, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateActiveStatus start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new(std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(abilitySessionInfo, nullptr);

    WSError res = sProxy->UpdateActiveStatus(abilitySessionInfo);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateActiveStatus end";
}

/**
 * @tc.name: OnSessionEvent
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnSessionEvent, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnSessionEvent start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    SessionEvent event = SessionEvent::EVENT_MAXIMIZE;
    WSError res = sProxy->OnSessionEvent(event);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnSessionEvent end";
}

/**
 * @tc.name: UpdateSessionRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateSessionRect, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateSessionRect start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    WSRect rect{.posX_ = 1, .posY_ = 1, .width_ = 100, .height_ = 100};
    SizeChangeReason reason = SizeChangeReason::RECOVER;
    WSError res = sProxy->UpdateSessionRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateSessionRect end";
}

/**
 * @tc.name: RaiseToAppTop
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, RaiseToAppTop, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseToAppTop start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    WSError res = sProxy->RaiseToAppTop();
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseToAppTop end";
}

/**
 * @tc.name: OnNeedAvoid
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnNeedAvoid, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnNeedAvoid start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    bool status = false;
    WSError res = sProxy->OnNeedAvoid(status);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: OnNeedAvoid end";
}

/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetAvoidAreaByType, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetAvoidAreaByType start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    AvoidAreaType status = AvoidAreaType::TYPE_SYSTEM;
    AvoidArea res = sProxy->GetAvoidAreaByType(status);
    AvoidArea area;
    ASSERT_EQ(res, area);

    GTEST_LOG_(INFO) << "SessionProxyTest: GetAvoidAreaByType end";
}

/**
 * @tc.name: RequestSessionBack
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, RequestSessionBack, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: RequestSessionBack start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    bool needMoveToBackground = true;
    WSError res = sProxy->RequestSessionBack(needMoveToBackground);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: RequestSessionBack end";
}

/**
 * @tc.name: MarkProcessed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, MarkProcessed, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: MarkProcessed start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    int32_t eventId = 0;
    WSError res = sProxy->MarkProcessed(eventId);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: MarkProcessed end";
}

/**
 * @tc.name: SetGlobalMaximizeMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetGlobalMaximizeMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetGlobalMaximizeMode start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    MaximizeMode mode = MaximizeMode::MODE_AVOID_SYSTEM_BAR;
    WSError res = sProxy->SetGlobalMaximizeMode(mode);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: SetGlobalMaximizeMode end";
}

/**
 * @tc.name: GetGlobalMaximizeMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetGlobalMaximizeMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetGlobalMaximizeMode start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    MaximizeMode mode = MaximizeMode::MODE_AVOID_SYSTEM_BAR;
    WSError res = sProxy->GetGlobalMaximizeMode(mode);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: GetGlobalMaximizeMode end";
}

/**
 * @tc.name: SetSessionProperty
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetSessionProperty, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetSessionProperty start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    WSError res = sProxy->SetSessionProperty(property);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: SetSessionProperty end";
}

/**
 * @tc.name: SetAspectRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetAspectRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetAspectRatio start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    float ratio = 10;
    WSError res = sProxy->SetAspectRatio(ratio);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: SetAspectRatio end";
}

/**
 * @tc.name: UpdateWindowSceneAfterCustomAnimation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateWindowSceneAfterCustomAnimation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowSceneAfterCustomAnimation start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    bool isAdd = false;
    WSError res = sProxy->UpdateWindowSceneAfterCustomAnimation(isAdd);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowSceneAfterCustomAnimation end";
}

/**
 * @tc.name: TransferAbilityResult
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, TransferAbilityResult, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAbilityResult start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    uint32_t resultCode = 0;
    AAFwk::Want want;
    WSError res = sProxy->TransferAbilityResult(resultCode, want);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAbilityResult end";
}

/**
 * @tc.name: NotifyExtensionDied
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyExtensionDied, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyExtensionDied start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    sProxy->NotifyExtensionDied();

    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyExtensionDied end";
}

/**
 * @tc.name: UpdateWindowAnimationFlag
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateWindowAnimationFlag, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowAnimationFlag start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    bool needDefaultAnimationFlag = false;
    WSError res = sProxy->UpdateWindowAnimationFlag(needDefaultAnimationFlag);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowAnimationFlag end";
}

/**
 * @tc.name: TransferAccessibilityEvent
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, TransferAccessibilityEvent, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAccessibilityEvent start";
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    Accessibility::AccessibilityEventInfo info;
    int64_t uiExtensionIdLevel = 0;
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAccessibilityEvent end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS

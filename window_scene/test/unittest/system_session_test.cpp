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
#include "session/host/include/system_session.h"

#include "common/include/session_permission.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "session/host/include/session.h"
#include <ui/rs_surface_node.h>
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "pointer_event.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class TestWindowEventChannel : public IWindowEventChannel {
public:
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError TransferFocusActiveEvent(bool isFocusActive) override;
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
        bool isPreImeEvent = false) override;
    WSError TransferKeyEventForConsumedAsync(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isPreImeEvent,
        const sptr<IRemoteObject>& listener) override;
    WSError TransferFocusState(bool focusState) override;
    WSError TransferBackpressedEventForConsumed(bool& isConsumed) override;
    WSError TransferSearchElementInfo(int64_t elementId, int32_t mode, int64_t baseParent,
        std::list<Accessibility::AccessibilityElementInfo>& infos) override;
    WSError TransferSearchElementInfosByText(int64_t elementId, const std::string& text, int64_t baseParent,
        std::list<Accessibility::AccessibilityElementInfo>& infos) override;
    WSError TransferFindFocusedElementInfo(int64_t elementId, int32_t focusType, int64_t baseParent,
        Accessibility::AccessibilityElementInfo& info) override;
    WSError TransferFocusMoveSearch(int64_t elementId, int32_t direction, int64_t baseParent,
        Accessibility::AccessibilityElementInfo& info) override;
    WSError TransferExecuteAction(int64_t elementId, const std::map<std::string, std::string>& actionArguments,
        int32_t action, int64_t baseParent) override;
    WSError TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType, int32_t eventType,
        int64_t timeMs) override;

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    };
};

WSError TestWindowEventChannel::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFocusActiveEvent(bool isFocusActive)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferKeyEventForConsumed(
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed, bool isPreImeEvent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferKeyEventForConsumedAsync(const std::shared_ptr<MMI::KeyEvent>& keyEvent,
    bool isPreImeEvent, const sptr<IRemoteObject>& listener)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFocusState(bool foucsState)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferBackpressedEventForConsumed(bool& isConsumed)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferSearchElementInfo(int64_t elementId, int32_t mode, int64_t baseParent,
    std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferSearchElementInfosByText(int64_t elementId, const std::string& text,
    int64_t baseParent, std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFindFocusedElementInfo(int64_t elementId, int32_t focusType, int64_t baseParent,
    Accessibility::AccessibilityElementInfo& info)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFocusMoveSearch(int64_t elementId, int32_t direction, int64_t baseParent,
    Accessibility::AccessibilityElementInfo& info)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferExecuteAction(int64_t elementId,
    const std::map<std::string, std::string>& actionArguments, int32_t action, int64_t baseParent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
    int32_t eventType, int64_t timeMs)
{
    return WSError::WS_OK;
}

class SystemSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr<SystemSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SystemSession> systemSession_;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
};

void SystemSessionTest::SetUpTestCase()
{
}

void SystemSessionTest::TearDownTestCase()
{
}

void SystemSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSystemSession1";
    info.moduleName_ = "testSystemSession2";
    info.bundleName_ = "testSystemSession3";
    systemSession_ = new SystemSession(info, specificCallback);
    EXPECT_NE(nullptr, systemSession_);
}

void SystemSessionTest::TearDown()
{
    systemSession_ = nullptr;
}

RSSurfaceNode::SharedPtr SystemSessionTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, Show, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();

    ASSERT_TRUE((systemSession_ != nullptr));
    ASSERT_EQ(WSError::WS_OK, systemSession_->Show(property));
}

/**
 * @tc.name: Reconnect01
 * @tc.desc: check func Reconnect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, Reconnect01, Function | SmallTest | Level1)
{
    auto surfaceNode = CreateRSSurfaceNode();
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    sptr<TestWindowEventChannel> testWindowEventChannel = new (std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);

    auto result = systemSession_->Reconnect(nullptr, nullptr, nullptr, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(nullptr, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(mockSessionStage, nullptr, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, TransferKeyEvent01, Function | SmallTest | Level1)
{
    systemSession_->state_ = SessionState::STATE_END;

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, systemSession_->TransferKeyEvent(nullptr));
}

/**
 * @tc.name: TransferKeyEvent02
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, TransferKeyEvent02, Function | SmallTest | Level1)
{
    systemSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, systemSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: ProcessBackEvent01
 * @tc.desc: check func ProcessBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, ProcessBackEvent01, Function | SmallTest | Level1)
{
    systemSession_->state_ = SessionState::STATE_END;

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, systemSession_->ProcessBackEvent());
}

/**
 * @tc.name: NotifyClientToUpdateRect01
 * @tc.desc: check func NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, NotifyClientToUpdateRect01, Function | SmallTest | Level1)
{
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    systemSession_->sessionStage_ = mockSessionStage;
    auto ret = systemSession_->NotifyClientToUpdateRect(nullptr);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: CheckPointerEventDispatch
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, CheckPointerEventDispatch, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ =  MMI::PointerEvent::Create();
    SessionInfo info;
    info.abilityName_ = "CheckPointerEventDispatch";
    info.bundleName_ = "CheckPointerEventDispatchBundleName";
    info.windowType_ = 2122;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    sptr<SystemSession> sysSession =
        new (std::nothrow) SystemSession(info, specificCallback_);
    sysSession->SetSessionState(SessionState::STATE_FOREGROUND);
    bool ret1 = sysSession->CheckPointerEventDispatch(pointerEvent_);
    ASSERT_EQ(true, ret1);
}

/**
 * @tc.name: UpdatePointerArea
 * @tc.desc: check func UpdatePointerArea
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, UpdatePointerArea, Function | SmallTest | Level1)
{
    WSRect rect = { 1, 1, 1, 1 };
    SessionInfo info;
    info.abilityName_ = "UpdatePointerArea";
    info.bundleName_ = "UpdatePointerAreaBundleName";
    info.windowType_ = 2122;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    sptr<SystemSession> sysSession =
        new (std::nothrow) SystemSession(info, specificCallback_);
    sysSession->UpdatePointerArea(rect);
    ASSERT_NE(sysSession->preRect_, rect);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetDecorEnable(true);
    sysSession->property_ = property;
    sysSession->UpdatePointerArea(rect);
    ASSERT_EQ(sysSession->preRect_, rect);
}

/**
 * @tc.name: Hide
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, Hide, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);

    auto ret = systemSession_->Hide();
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: Disconnect
 * @tc.desc: test function : Disconnect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, Disconnect, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);

    bool isFromClient = true;
    auto ret = systemSession_->Disconnect(isFromClient);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: ProcessPointDownSession
 * @tc.desc: test function : ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, ProcessPointDownSession, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);

    int32_t posX = 2;
    int32_t posY = 3;
    auto ret = systemSession_->ProcessPointDownSession(posX, posY);
    ASSERT_EQ(WSError::WS_OK, ret);
}
}
}
}
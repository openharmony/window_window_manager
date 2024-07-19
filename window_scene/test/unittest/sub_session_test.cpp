/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "session/host/include/sub_session.h"

#include "common/include/session_permission.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "session/host/include/session.h"
#include "session/host/include/main_session.h"
#include "session/host/include/system_session.h"
#include <ui/rs_surface_node.h>
#include "window_event_channel_base.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_property.h"
#include "window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SubSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr<SubSession::SpecificSessionCallback> specificCallback = nullptr;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    sptr<SubSession> subSession_;
    SystemSessionConfig systemConfig_;
};

void SubSessionTest::SetUpTestCase()
{
}

void SubSessionTest::TearDownTestCase()
{
}

void SubSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testMainSession1";
    info.moduleName_ = "testMainSession2";
    info.bundleName_ = "testMainSession3";
    subSession_ = new SubSession(info, specificCallback);
    EXPECT_NE(nullptr, subSession_);
}

void SubSessionTest::TearDown()
{
    subSession_ = nullptr;
}

RSSurfaceNode::SharedPtr SubSessionTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: Reconnect01
 * @tc.desc: check func Reconnect
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Reconnect01, Function | SmallTest | Level1)
{
    auto surfaceNode = CreateRSSurfaceNode();
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    sptr<TestWindowEventChannel> testWindowEventChannel = new (std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);

    auto result = subSession_->Reconnect(nullptr, nullptr, nullptr, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = subSession_->Reconnect(nullptr, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = subSession_->Reconnect(mockSessionStage, nullptr, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = subSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = subSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
    
    property->SetWindowState(WindowState::STATE_INITIAL);
    result = subSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
 
    property->SetWindowState(WindowState::STATE_CREATED);
    result = subSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);
 
    property->SetWindowState(WindowState::STATE_SHOWN);
    result = subSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);
 
    property->SetWindowState(WindowState::STATE_HIDDEN);
    result = subSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, TransferKeyEvent01, Function | SmallTest | Level1)
{
    subSession_->state_ = SessionState::STATE_END;

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, subSession_->TransferKeyEvent(nullptr));
}

/**
 * @tc.name: TransferKeyEvent02
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, TransferKeyEvent02, Function | SmallTest | Level1)
{
    subSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, subSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferKeyEvent03
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, TransferKeyEvent03, Function | SmallTest | Level1)
{
    ASSERT_NE(subSession_, nullptr);
    subSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    subSession_->SetParentSession(nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, subSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: IsTopmost01
 * @tc.desc: check func IsTopmost
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, IsTopmost01, Function | SmallTest | Level1)
{
    subSession_->GetSessionProperty()->SetTopmost(false);
    ASSERT_EQ(false, subSession_->IsTopmost());

    subSession_->GetSessionProperty()->SetTopmost(true);
    ASSERT_EQ(true, subSession_->IsTopmost());
}

/**
 * @tc.name: Hide01
 * @tc.desc: check func Hide
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Hide01, Function | SmallTest | Level1)
{
    subSession_->Hide();
    subSession_->GetMissionId();

    WSRect rect;
    subSession_->UpdatePointerArea(rect);
    subSession_->RectCheck(50, 100);
    ASSERT_EQ(WSError::WS_OK, subSession_->ProcessPointDownSession(50, 100));
}

/**
 * @tc.name: Hide02
 * @tc.desc: check func Hide
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Hide02, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_TRUE(subSession_ != nullptr);
    subSession_->SetSessionProperty(property);
    auto result = subSession_->Hide();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Hide03
 * @tc.desc: check func Hide
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Hide03, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = nullptr;
    ASSERT_TRUE(subSession_ != nullptr);
    subSession_->SetSessionProperty(property);
    auto result = subSession_->Hide();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Hide04
 * @tc.desc: check func Hide
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Hide04, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sptr<WindowProperty> winPropSrc = new (std::nothrow) WindowProperty();
    ASSERT_NE(nullptr, winPropSrc);
    uint32_t animationFlag = 1;
    winPropSrc->SetAnimationFlag(animationFlag);
    uint32_t res = winPropSrc->GetAnimationFlag();
    ASSERT_NE(res, static_cast<uint32_t>(WindowAnimation::CUSTOM));
    ASSERT_TRUE(subSession_ != nullptr);
    auto result = subSession_->Hide();
    ASSERT_EQ(result, WSError::WS_OK);

    animationFlag = 3;
    winPropSrc->SetAnimationFlag(animationFlag);
    res = winPropSrc->GetAnimationFlag();
    ASSERT_EQ(res, static_cast<uint32_t>(WindowAnimation::CUSTOM));
    ASSERT_TRUE(subSession_ != nullptr);
    result = subSession_->Hide();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Show01
 * @tc.desc: check func Show
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Show01, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = nullptr;
    ASSERT_EQ(nullptr, property);
    
    ASSERT_TRUE(subSession_ != nullptr);
    ASSERT_EQ(WSError::WS_OK, subSession_->Show(property));
}

/**
 * @tc.name: Show02
 * @tc.desc: check func Show
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Show02, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    sptr<WindowProperty> winPropSrc = new (std::nothrow) WindowProperty();
    ASSERT_NE(nullptr, winPropSrc);
    uint32_t animationFlag = 1;
    winPropSrc->SetAnimationFlag(animationFlag);
    uint32_t res = winPropSrc->GetAnimationFlag();
    ASSERT_NE(res, static_cast<uint32_t>(WindowAnimation::CUSTOM));

    ASSERT_TRUE(subSession_ != nullptr);
    auto result = subSession_->Show(property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Show03
 * @tc.desc: check func Show
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Show03, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    sptr<WindowProperty> winPropSrc = new (std::nothrow) WindowProperty();
    ASSERT_NE(nullptr, winPropSrc);
    uint32_t animationFlag = 3;
    winPropSrc->SetAnimationFlag(animationFlag);
    uint32_t res = winPropSrc->GetAnimationFlag();
    ASSERT_EQ(res, static_cast<uint32_t>(WindowAnimation::CUSTOM));

    ASSERT_TRUE(subSession_ != nullptr);
    auto result = subSession_->Show(property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: CheckPointerEventDispatch01
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, CheckPointerEventDispatch01, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    systemConfig_.uiType_ = "phone";

    ASSERT_TRUE(subSession_ != nullptr);
    auto result = subSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckPointerEventDispatch02
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, CheckPointerEventDispatch02, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    systemConfig_.uiType_ = "pc";

    ASSERT_TRUE(subSession_ != nullptr);
    subSession_->SetSessionState(SessionState::STATE_FOREGROUND);
    auto result = subSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckPointerEventDispatch03
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, CheckPointerEventDispatch03, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    systemConfig_.uiType_ = "pc";

    ASSERT_TRUE(subSession_ != nullptr);
    subSession_->SetSessionState(SessionState::STATE_BACKGROUND);
    subSession_->UpdateSessionState(SessionState::STATE_ACTIVE);
    auto result = subSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckPointerEventDispatch04
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, CheckPointerEventDispatch04, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    systemConfig_.uiType_ = "pc";

    ASSERT_TRUE(subSession_ != nullptr);
    subSession_->SetSessionState(SessionState::STATE_BACKGROUND);
    subSession_->UpdateSessionState(SessionState::STATE_INACTIVE);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
    auto result = subSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckPointerEventDispatch05
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, CheckPointerEventDispatch05, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    systemConfig_.uiType_ = "pc";

    ASSERT_TRUE(subSession_ != nullptr);
    subSession_->SetSessionState(SessionState::STATE_BACKGROUND);
    subSession_->UpdateSessionState(SessionState::STATE_INACTIVE);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    auto result = subSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_TRUE(result);
}
}
}
}
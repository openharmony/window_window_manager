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
#include "window_helper.h"
#include "window_manager_hilog.h"

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

class SubSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr<SubSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SubSession> subSession_;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
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
 * @tc.name: Hide
 * @tc.desc: check func Reconnect
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionTest, Hide, Function | SmallTest | Level1)
{
    subSession_->Hide();
    subSession_->GetMissionId();

    WSRect rect;
    subSession_->UpdatePointerArea(rect);
    subSession_->RectCheck(50, 100);
    ASSERT_EQ(WSError::WS_OK, subSession_->ProcessPointDownSession(50, 100));
}
}
}
}
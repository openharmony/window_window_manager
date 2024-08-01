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
#include "session/host/include/scb_system_session.h"
#include "common/include/session_permission.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "session/host/include/session.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SCBSystemSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr<SCBSystemSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SCBSystemSession> scbSystemSession_;
};

void SCBSystemSessionTest::SetUpTestCase()
{
}

void SCBSystemSessionTest::TearDownTestCase()
{
}

void SCBSystemSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSCBSystemSession1";
    info.moduleName_ = "testSCBSystemSession2";
    info.bundleName_ = "testSCBSystemSession3";
    scbSystemSession_ = new (std::nothrow) SCBSystemSession(info, specificCallback);
    EXPECT_NE(nullptr, scbSystemSession_);
}

void SCBSystemSessionTest::TearDown()
{
    scbSystemSession_ = nullptr;
}

namespace {
/**
 * @tc.name: NotifyClientToUpdateRect01
 * @tc.desc: check func NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, NotifyClientToUpdateRect01, Function | SmallTest | Level1)
{
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    scbSystemSession_->sessionStage_ = mockSessionStage;
    auto ret = scbSystemSession_->NotifyClientToUpdateRect(nullptr);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, TransferKeyEvent01, Function | SmallTest | Level1)
{
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, scbSystemSession_->TransferKeyEvent(nullptr));
}

/**
 * @tc.name: TransferKeyEvent02
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, TransferKeyEvent02, Function | SmallTest | Level1)
{
    scbSystemSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    scbSystemSession_->windowEventChannel_ = nullptr;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, scbSystemSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: UpdateFocus01
 * @tc.desc: check func UpdateFocus
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, UpdateFocus01, Function | SmallTest | Level1)
{
    bool isFocused = scbSystemSession_->isFocused_;

    ASSERT_EQ(WSError::WS_DO_NOTHING, scbSystemSession_->UpdateFocus(isFocused));
}

/**
 * @tc.name: UpdateFocus02
 * @tc.desc: check func UpdateFocus
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, UpdateFocus02, Function | SmallTest | Level1)
{
    bool isFocused = scbSystemSession_->isFocused_;

    ASSERT_EQ(WSError::WS_OK, scbSystemSession_->UpdateFocus(!isFocused));
}

/**
 * @tc.name: UpdateWindowMode
 * @tc.desc: check func UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, UpdateWindowMode, Function | SmallTest | Level1)
{
    scbSystemSession_->PresentFocusIfPointDown();
    scbSystemSession_->PresentFoucusIfNeed(2);
    ASSERT_EQ(WSError::WS_OK, scbSystemSession_->SetSystemSceneBlockingFocus(true));
    WSRect rect = {0, 0, 0, 0};
    scbSystemSession_->UpdatePointerArea(rect);
    auto ret = scbSystemSession_->UpdateWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, ret);
}

/**
 * @tc.name: RegisterBufferAvailableCallback
 * @tc.desc: check func RegisterBufferAvailableCallback
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, RegisterBufferAvailableCallback, Function | SmallTest | Level3)
{
    ASSERT_NE(scbSystemSession_, nullptr);
    scbSystemSession_->RegisterBufferAvailableCallback([]() {});
}

/**
 * @tc.name: BindKeyboardSession01
 * @tc.desc: check func BindKeyboardSession
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, BindKeyboardSession01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, scbSystemSession_);
    scbSystemSession_->BindKeyboardSession(nullptr);
}

/**
 * @tc.name: BindKeyboardSession02
 * @tc.desc: check func BindKeyboardSession
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, BindKeyboardSession02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, scbSystemSession_);
    SessionInfo info;
    info.bundleName_ = "IntentionEventManager";
    info.moduleName_ = "InputEventListener";
    info.isSystem_ = true;
    sptr<SceneSession::SpecificSessionCallback> callback =
        new SceneSession::SpecificSessionCallback();
    sptr<SceneSession> session = new SceneSession(info, callback);
    scbSystemSession_->BindKeyboardSession(session);
}

/**
 * @tc.name: NotifyClientToUpdateRect
 * @tc.desc: check func NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, NotifyClientToUpdateRect, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, scbSystemSession_);
    scbSystemSession_->NotifyClientToUpdateRect(nullptr);
}

/**
 * @tc.name: SetSystemSceneBlockingFocus
 * @tc.desc: check func SetSystemSceneBlockingFocus
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, SetSystemSceneBlockingFocus01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, scbSystemSession_);
    WSError ret = scbSystemSession_->SetSystemSceneBlockingFocus(true);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: PresentFocusIfPointDown
 * @tc.desc: check func PresentFocusIfPointDown
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, PresentFocusIfPointDown01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, scbSystemSession_);
    scbSystemSession_->PresentFocusIfPointDown();
}

/**
 * @tc.name: GetKeyboardSession
 * @tc.desc: check func GetKeyboardSession
 * @tc.type: FUNC
 */
HWTEST_F(SCBSystemSessionTest, GetKeyboardSession01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, scbSystemSession_);
    scbSystemSession_->GetKeyboardSession();
}

} //namespace
} //namespace Rosen
} //namespace OHOS
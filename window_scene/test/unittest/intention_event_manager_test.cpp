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

#include "intention_event/include/intention_event_manager.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class IntentionEventManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::unique_ptr<Ace::UIContent> uIContent_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
    std::shared_ptr<IntentionEventManager::InputEventListener> inputEventListener_;
};

void IntentionEventManagerTest::SetUpTestCase()
{
}

void IntentionEventManagerTest::TearDownTestCase()
{
}

void IntentionEventManagerTest::SetUp()
{
    uIContent_ = Ace::UIContent::Create(nullptr);
    EXPECT_NE(nullptr, uIContent_);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(nullptr);
    EXPECT_NE(nullptr, eventHandler_);
    inputEventListener_ =
        std::make_shared<IntentionEventManager::InputEventListener>(uIContent_.get(), eventHandler_);
    EXPECT_NE(nullptr, inputEventListener_);
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();
}

void IntentionEventManagerTest::TearDown()
{
    uIContent_ = nullptr;
    eventHandler_ = nullptr;
    inputEventListener_ = nullptr;
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: EnableInputEventListener
 * @tc.desc: EnableInputEventListener Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, EnableInputEventListener, Function | MediumTest | Level2)
{
    bool enable = DelayedSingleton<IntentionEventManager>::GetInstance()->
        EnableInputEventListener(nullptr, nullptr);
    EXPECT_EQ(false, enable);
    enable = DelayedSingleton<IntentionEventManager>::GetInstance()->
        EnableInputEventListener(uIContent_.get(), nullptr);
    EXPECT_EQ(false, enable);
    enable = DelayedSingleton<IntentionEventManager>::GetInstance()->
        EnableInputEventListener(uIContent_.get(), eventHandler_);
    EXPECT_EQ(true, enable);
}

/**
 * @tc.name: OnInputEvent0
 * @tc.desc: OnInputEvent0 Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, OnInputEvent0, Function | MediumTest | Level2)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    inputEventListener_->OnInputEvent(pointerEvent);
    pointerEvent = MMI::PointerEvent::Create();
    EXPECT_NE(nullptr, pointerEvent);

    pointerEvent->SetTargetWindowId(0);
    inputEventListener_->OnInputEvent(pointerEvent);

    SessionInfo info;
    info.bundleName_ = "IntentionEventManager";
    info.moduleName_ = "InputEventListener";
    info.isSystem_ = true;
    sptr<SceneSession::SpecificSessionCallback> callback =
        new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, callback);
    sptr<SceneSession> sceneSession0 = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession0);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(std::make_pair(0, sceneSession0));

    pointerEvent->SetDispatchTimes(10);
    EXPECT_EQ(10, pointerEvent->GetDispatchTimes());
    pointerEvent->SetPointerId(10);
    EXPECT_EQ(10, pointerEvent->GetPointerId());

    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(10);
    EXPECT_EQ(10, item.GetPointerId());
    pointerEvent->AddPointerItem(item);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(MMI::PointerEvent::POINTER_ACTION_UP, pointerEvent->GetPointerAction());
    inputEventListener_->OnInputEvent(pointerEvent);

    info.isSystem_ = false;
    sptr<SceneSession> sceneSession1 = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession1);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(std::make_pair(1, sceneSession1));
    pointerEvent->SetTargetWindowId(1);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    EXPECT_NE(nullptr, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    sceneSession1->SetSessionProperty(property);
    inputEventListener_->OnInputEvent(pointerEvent);
}

/**
 * @tc.name: OnInputEvent1
 * @tc.desc: OnInputEvent1 Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, OnInputEvent1, Function | MediumTest | Level2)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    inputEventListener_->OnInputEvent(keyEvent);
    keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(nullptr, keyEvent);
    SceneSessionManager::GetInstance().SetEnableInputEvent(false);
    EXPECT_EQ(false, SceneSessionManager::GetInstance().IsInputEventEnabled());
    inputEventListener_->OnInputEvent(keyEvent);

    SceneSessionManager::GetInstance().SetEnableInputEvent(true);
    SceneSessionManager::GetInstance().SetFocusedSessionId(INVALID_SESSION_ID);
    EXPECT_EQ(INVALID_SESSION_ID, SceneSessionManager::GetInstance().GetFocusedSessionId());
    inputEventListener_->OnInputEvent(keyEvent);

    SceneSessionManager::GetInstance().SetFocusedSessionId(1);
    inputEventListener_->OnInputEvent(keyEvent);

    SessionInfo info;
    info.bundleName_ = "IntentionEventManager";
    info.moduleName_ = "InputEventListener";
    info.isSystem_ = true;
    sptr<SceneSession::SpecificSessionCallback> callback =
        new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, callback);
    sptr<SceneSession> sceneSession0 = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession0);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(std::make_pair(1, sceneSession0));
    EXPECT_EQ(1, SceneSessionManager::GetInstance().sceneSessionMap_.size());
    inputEventListener_->OnInputEvent(keyEvent);

    info.isSystem_ = false;
    sptr<SceneSession> sceneSession1 = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession1);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(std::make_pair(2, sceneSession1));
    SceneSessionManager::GetInstance().SetFocusedSessionId(2);
    EXPECT_EQ(2, SceneSessionManager::GetInstance().GetFocusedSessionId());
    inputEventListener_->OnInputEvent(keyEvent);
}

/**
 * @tc.name: OnInputEvent2
 * @tc.desc: OnInputEvent2 Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, OnInputEvent2, Function | MediumTest | Level2)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(nullptr, keyEvent);
    SceneSessionManager::GetInstance().SetEnableInputEvent(true);
    SceneSessionManager::GetInstance().SetFocusedSessionId(1);
    SessionInfo info;
    info.bundleName_ = "IntentionEventManager";
    info.moduleName_ = "InputEventListener";
    info.isSystem_ = true;
    sptr<SceneSession::SpecificSessionCallback> callback =
        new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, callback);
    sptr<SceneSession> sceneSession = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession);
    auto func = [](std::shared_ptr<MMI::KeyEvent> keyEvent, bool isPreImeEvent) {
        return true;
    };
    sceneSession->SetNotifySystemSessionKeyEventFunc(func);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(std::make_pair(1, sceneSession));
    EXPECT_EQ(1, SceneSessionManager::GetInstance().sceneSessionMap_.size());
    inputEventListener_->OnInputEvent(keyEvent);
    sceneSession->SetNotifySystemSessionKeyEventFunc(nullptr);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_FN);
    EXPECT_EQ(MMI::KeyEvent::KEYCODE_FN, keyEvent->GetKeyCode());
    inputEventListener_->OnInputEvent(keyEvent);
}

/**
 * @tc.name: OnInputEvent3
 * @tc.desc: OnInputEvent3 Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, OnInputEvent3, Function | MediumTest | Level2)
{
    std::shared_ptr<IntentionEventManager::InputEventListener> inputEventListener =
        std::make_shared<IntentionEventManager::InputEventListener>(nullptr, nullptr);
    EXPECT_NE(nullptr, inputEventListener);
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(nullptr, keyEvent);
    SceneSessionManager::GetInstance().SetEnableInputEvent(true);
    SceneSessionManager::GetInstance().SetFocusedSessionId(1);
    SessionInfo info;
    info.bundleName_ = "IntentionEventManager";
    info.moduleName_ = "InputEventListener";
    info.isSystem_ = true;
    sptr<SceneSession::SpecificSessionCallback> callback =
        new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, callback);
    sptr<SceneSession> sceneSession = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession);
    sceneSession->SetNotifySystemSessionKeyEventFunc(nullptr);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(std::make_pair(1, sceneSession));
    EXPECT_EQ(1, SceneSessionManager::GetInstance().sceneSessionMap_.size());
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_UNKNOWN);
    EXPECT_EQ(MMI::KeyEvent::KEYCODE_UNKNOWN, keyEvent->GetKeyCode());
    inputEventListener->OnInputEvent(keyEvent);
    inputEventListener_->OnInputEvent(keyEvent);
}

/**
 * @tc.name: OnInputEvent4
 * @tc.desc: OnInputEvent4 Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, OnInputEvent4, Function | MediumTest | Level2)
{
    std::shared_ptr<IntentionEventManager::InputEventListener> inputEventListener =
        std::make_shared<IntentionEventManager::InputEventListener>(nullptr, nullptr);;
    EXPECT_EQ(nullptr, inputEventListener->uiContent_);
    std::shared_ptr<MMI::AxisEvent> axisEvent = nullptr;
    inputEventListener->OnInputEvent(axisEvent);
    axisEvent = MMI::AxisEvent::Create();
    EXPECT_NE(nullptr, axisEvent);
    inputEventListener->OnInputEvent(axisEvent);
    inputEventListener_->OnInputEvent(axisEvent);
}

/**
 * @tc.name: IsKeyboardEvent
 * @tc.desc: IsKeyboardEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, IsKeyboardEvent, Function | MediumTest | Level2)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(nullptr, keyEvent);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    EXPECT_EQ(true, inputEventListener_->IsKeyboardEvent(keyEvent));
}

/**
 * @tc.name: DispatchKeyEventCallback
 * @tc.desc: DispatchKeyEventCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, DispatchKeyEventCallback, Function | MediumTest | Level2)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    inputEventListener_->DispatchKeyEventCallback(2024, keyEvent, true);
    keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(nullptr, keyEvent);
    inputEventListener_->DispatchKeyEventCallback(2024, keyEvent, true);
    inputEventListener_->DispatchKeyEventCallback(2024, keyEvent, false);
    SessionInfo info;
    info.bundleName_ = "IntentionEventManager";
    info.moduleName_ = "InputEventListener";
    sptr<SceneSession::SpecificSessionCallback> callback =
        new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, callback);
    sptr<SceneSession> sceneSession = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(std::make_pair(2024, sceneSession));
    std::shared_ptr<IntentionEventManager::InputEventListener> inputEventListener =
        std::make_shared<IntentionEventManager::InputEventListener>(nullptr, nullptr);
    inputEventListener->DispatchKeyEventCallback(2024, keyEvent, false);
    inputEventListener_->DispatchKeyEventCallback(2024, keyEvent, false);
}

/**
 * @tc.name: CheckPointerEvent
 * @tc.desc: CheckPointerEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, CheckPointerEvent, Function | MediumTest | Level2)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    std::shared_ptr<IntentionEventManager::InputEventListener> inputEventListener =
        std::make_shared<IntentionEventManager::InputEventListener>(nullptr, nullptr);
    EXPECT_NE(nullptr, inputEventListener);
    EXPECT_EQ(false, inputEventListener->CheckPointerEvent(pointerEvent));
    pointerEvent = MMI::PointerEvent::Create();
    EXPECT_EQ(false, inputEventListener->CheckPointerEvent(pointerEvent));
    SceneSessionManager::GetInstance().SetEnableInputEvent(false);
    EXPECT_EQ(false, inputEventListener_->CheckPointerEvent(pointerEvent));
    SceneSessionManager::GetInstance().SetEnableInputEvent(true);
    pointerEvent->SetDispatchTimes(10);
    EXPECT_EQ(10, pointerEvent->GetDispatchTimes());
    EXPECT_EQ(true, inputEventListener_->CheckPointerEvent(pointerEvent));
}

/**
 * @tc.name: UpdateLastMouseEvent
 * @tc.desc: UpdateLastMouseEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, UpdateLastMouseEvent, Function | MediumTest | Level2)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    inputEventListener_->UpdateLastMouseEvent(pointerEvent);
    pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(MMI::PointerEvent::SOURCE_TYPE_MOUSE, pointerEvent->GetSourceType());
    EXPECT_NE(MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW, pointerEvent->GetPointerAction());
    inputEventListener_->UpdateLastMouseEvent(pointerEvent);

    pointerEvent->SetSourceType(MMI::PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
    inputEventListener_->UpdateLastMouseEvent(pointerEvent);
}

/**
 * @tc.name: ProcessEnterLeaveEventAsync0
 * @tc.desc: ProcessEnterLeaveEventAsync0 Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, ProcessEnterLeaveEventAsync0, Function | MediumTest | Level2)
{
    std::shared_ptr<IntentionEventManager::InputEventListener> inputEventListener1 =
        std::make_shared<IntentionEventManager::InputEventListener>(nullptr, nullptr);
    EXPECT_NE(nullptr, inputEventListener1);
    inputEventListener1->ProcessEnterLeaveEventAsync();
}

/**
 * @tc.name: ProcessEnterLeaveEventAsync1
 * @tc.desc: ProcessEnterLeaveEventAsync1 Test
 * @tc.type: FUNC
 */
HWTEST_F(IntentionEventManagerTest, ProcessEnterLeaveEventAsync1, Function | MediumTest | Level2)
{
    inputEventListener_->ProcessEnterLeaveEventAsync();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    EXPECT_NE(nullptr, pointerEvent);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    inputEventListener_->UpdateLastMouseEvent(pointerEvent);

    SceneSession::ClearEnterWindow();
    EXPECT_EQ(nullptr, SceneSession::GetEnterWindow().promote());
    inputEventListener_->ProcessEnterLeaveEventAsync();

    SessionInfo info;
    info.bundleName_ = "IntentionEventManager";
    info.moduleName_ = "InputEventListener";
    info.isSystem_ = false;
    info.persistentId_ = -1;
    sptr<SceneSession::SpecificSessionCallback> callback =
        new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, callback);
    sptr<SceneSession> sceneSession = new SceneSession(info, callback);
    EXPECT_NE(nullptr, sceneSession);
    
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
    sceneSession->TransferPointerEvent(pointerEvent, true);
    EXPECT_NE(nullptr, SceneSession::GetEnterWindow().promote());
    inputEventListener_->ProcessEnterLeaveEventAsync();

    SceneSession::ClearEnterWindow();
    info.persistentId_ = 2024;
    sceneSession = new SceneSession(info, callback);
    sceneSession->TransferPointerEvent(pointerEvent, true);
    inputEventListener_->ProcessEnterLeaveEventAsync();
    std::shared_ptr<IntentionEventManager::InputEventListener> inputEventListener2 =
        std::make_shared<IntentionEventManager::InputEventListener>(nullptr, eventHandler_);
    inputEventListener2->ProcessEnterLeaveEventAsync();
}
}
}
}
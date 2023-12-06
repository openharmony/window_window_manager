/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "event_runner.h"
#include "pointer_event.h"
#include "window_inner_manager.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
using InnerTask = std::function<void()>;
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
using EventPriority = OHOS::AppExecFwk::EventQueue::Priority;

namespace {
    const std::string INNER_WM_THREAD_NAME = "TestInnerWindowManager";
    constexpr int32_t TEST_CUSTOM_ID = 100;
}

class WindowInnerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowInnerManager> wim_ = new WindowInnerManager();
};

void WindowInnerManagerTest::SetUpTestCase()
{
}

void WindowInnerManagerTest::TearDownTestCase()
{
}

void WindowInnerManagerTest::SetUp()
{
}

void WindowInnerManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Stop
 * @tc.desc: test WindowInnerManager Stop
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, Stop, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.state_ = InnerWMRunningState::STATE_RUNNING;
    windowInnerManager.eventLoop_ = nullptr;
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = nullptr;
    windowInnerManager.Stop();
    ASSERT_EQ(InnerWMRunningState::STATE_NOT_START, windowInnerManager.state_);

    windowInnerManager.eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    windowInnerManager.eventHandler_ = std::make_shared<EventHandler>(windowInnerManager.eventLoop_);
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.Stop();
    ASSERT_EQ(0, windowInnerManager.eventLoop_.use_count());
    ASSERT_EQ(0, windowInnerManager.eventHandler_.use_count());
}

/**
 * @tc.name: NotifyServerReadyToMoveOrDrag
 * @tc.desc: test WindowInnerManager NotifyServerReadyToMoveOrDrag
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, NotifyServerReadyToMoveOrDrag, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.moveDragController_->activeWindowId_ = 1;
    uint32_t windowId = 1;
    sptr<WindowProperty> property = new WindowProperty();
    ASSERT_NE(property, nullptr);
    sptr<MoveDragProperty> moveDragProperty = new MoveDragProperty();
    ASSERT_NE(moveDragProperty, nullptr);
    auto result = windowInnerManager.NotifyServerReadyToMoveOrDrag(windowId, property, moveDragProperty);
    ASSERT_EQ(false, result);

    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;
    result = windowInnerManager.NotifyServerReadyToMoveOrDrag(windowId, property, moveDragProperty);
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: NotifyWindowEndUpMovingOrDragging
 * @tc.desc: test WindowInnerManager NotifyWindowEndUpMovingOrDragging
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, NotifyWindowEndUpMovingOrDragging, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.moveDragController_->activeWindowId_ = TEST_CUSTOM_ID;

    uint32_t windowId = INVALID_WINDOW_ID;
    windowInnerManager.NotifyWindowEndUpMovingOrDragging(windowId);
    ASSERT_EQ(windowInnerManager.moveDragController_->activeWindowId_, TEST_CUSTOM_ID);

    windowId = TEST_CUSTOM_ID;
    windowInnerManager.NotifyWindowEndUpMovingOrDragging(windowId);
    ASSERT_EQ(windowInnerManager.moveDragController_->activeWindowId_, INVALID_WINDOW_ID);
}
/**
 * @tc.name: NotifyWindowRemovedOrDestroyed
 * @tc.desc: test WindowInnerManager NotifyWindowRemovedOrDestroyed
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, NotifyWindowRemovedOrDestroyed, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();
    windowInnerManager.moveDragController_->activeWindowId_ = TEST_CUSTOM_ID;

    uint32_t windowId = INVALID_WINDOW_ID;
    windowInnerManager.NotifyWindowRemovedOrDestroyed(windowId);
    ASSERT_EQ(windowInnerManager.moveDragController_->activeWindowId_, TEST_CUSTOM_ID);

    windowId = TEST_CUSTOM_ID;

    auto moveDragProperty = windowInnerManager.moveDragController_->moveDragProperty_;
    moveDragProperty->startDragFlag_ = true;
    moveDragProperty->startMoveFlag_ = true;
    windowInnerManager.NotifyWindowRemovedOrDestroyed(windowId);
    ASSERT_EQ(windowInnerManager.moveDragController_->activeWindowId_, INVALID_WINDOW_ID);
}
/**
 * @tc.name: ConsumePointerEvent
 * @tc.desc: test WindowInnerManager ConsumePointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, ConsumePointerEvent, Function | SmallTest | Level2)
{
    WindowInnerManager& windowInnerManager = WindowInnerManager::GetInstance();
    windowInnerManager.eventHandler_ = nullptr;
    windowInnerManager.moveDragController_ = new MoveDragController();

    auto pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->processedCallback_ = nullptr;
    pointerEvent->sourceType_ = MMI::PointerEvent::SOURCE_TYPE_MOUSE;
    pointerEvent->buttonId_ = MMI::PointerEvent::MOUSE_BUTTON_RIGHT;
    pointerEvent->pointerAction_ = MMI::PointerEvent::POINTER_ACTION_MOVE;

    pointerEvent->agentWindowId_ = 1;
    windowInnerManager.moveDragController_->activeWindowId_ = 1;
    windowInnerManager.ConsumePointerEvent(pointerEvent);
    ASSERT_NE(windowInnerManager.moveDragController_->moveEvent_, nullptr);
    ASSERT_EQ(windowInnerManager.moveDragController_->moveEvent_->agentWindowId_, pointerEvent->agentWindowId_);

    pointerEvent->agentWindowId_ = 1;
    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;
    windowInnerManager.ConsumePointerEvent(pointerEvent);
    ASSERT_EQ(windowInnerManager.moveDragController_->moveEvent_->agentWindowId_, pointerEvent->agentWindowId_);

    pointerEvent->agentWindowId_ = INVALID_WINDOW_ID;
    windowInnerManager.moveDragController_->activeWindowId_ = INVALID_WINDOW_ID;
    windowInnerManager.ConsumePointerEvent(pointerEvent);
    ASSERT_EQ(windowInnerManager.moveDragController_->moveEvent_->agentWindowId_, pointerEvent->agentWindowId_);
}

/**
 * @tc.name: Start
 * @tc.desc: test WindowInnerManager Start
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, Start, Function | SmallTest | Level2)
{
    wim_->state_ = InnerWMRunningState::STATE_RUNNING;
    bool enableRecentholder = false;
    wim_->Start(enableRecentholder);
    ASSERT_EQ(wim_->state_, InnerWMRunningState::STATE_RUNNING);
}

/**
 * @tc.name: CreateInnerWindow
 * @tc.desc: test WindowInnerManager CreateInnerWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, CreateInnerWindow, Function | SmallTest | Level2)
{
    wim_->isRecentHolderEnable_ = true;
    std::string name = "test";
    DisplayId displayId = 1;
    Rect rect = {0.0, 0.0, 100.0, 100.0};
    WindowType type = WindowType::WINDOW_TYPE_PLACEHOLDER;
    WindowMode mode = WindowMode::WINDOW_MODE_FLOATING;
    wim_->CreateInnerWindow(name, displayId, rect, type, mode);
    ASSERT_EQ(wim_->isRecentHolderEnable_, true);
}

/**
 * @tc.name: DestroyInnerWindow
 * @tc.desc: test WindowInnerManager DestroyInnerWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, DestroyInnerWindow, Function | SmallTest | Level2)
{
    wim_->isRecentHolderEnable_ = true;
    DisplayId displayId = 1;
    WindowType type = WindowType::WINDOW_TYPE_PLACEHOLDER;
    wim_->DestroyInnerWindow(displayId, type);
    ASSERT_EQ(wim_->isRecentHolderEnable_, true);
}

/**
 * @tc.name: UpdateInnerWindow
 * @tc.desc: test WindowInnerManager UpdateInnerWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, UpdateInnerWindow, Function | SmallTest | Level2)
{
    wim_->isRecentHolderEnable_ = true;
    DisplayId displayId = 1;
    WindowType type = WindowType::WINDOW_TYPE_PLACEHOLDER;
    uint32_t width = 200;
    uint32_t height = 200;
    wim_->UpdateInnerWindow(displayId, type, width, height);
    ASSERT_EQ(wim_->isRecentHolderEnable_, true);
}

/**
 * @tc.name: TerminateAbility
 * @tc.desc: test WindowInnerManager TerminateAbility
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, TerminateAbility, Function | SmallTest | Level2)
{
    wim_->isRecentHolderEnable_ = true;
    wptr<WindowNode> node1 = nullptr;
    wim_->TerminateAbility(node1);
    ASSERT_EQ(wim_->isRecentHolderEnable_, true);
}

/**
 * @tc.name: CloseAbility
 * @tc.desc: test WindowInnerManager CloseAbility
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, CloseAbility, Function | SmallTest | Level2)
{
    wim_->isRecentHolderEnable_ = true;
    wptr<WindowNode> node1 = nullptr;
    wim_->CloseAbility(node1);
    ASSERT_EQ(wim_->isRecentHolderEnable_, true);
}

/**
 * @tc.name: CompleteFirstFrameDrawing
 * @tc.desc: test WindowInnerManager CompleteFirstFrameDrawing
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, CompleteFirstFrameDrawing, Function | SmallTest | Level2)
{
    wim_->isRecentHolderEnable_ = true;
    wptr<WindowNode> node1 = nullptr;
    wim_->CompleteFirstFrameDrawing(node1);
    ASSERT_EQ(wim_->isRecentHolderEnable_, true);
}

/**
 * @tc.name: UpdateMissionSnapShot
 * @tc.desc: test WindowInnerManager UpdateMissionSnapShot
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, UpdateMissionSnapShot, Function | SmallTest | Level2)
{
    wim_->isRecentHolderEnable_ = true;
    wptr<WindowNode> node1 = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    wim_->UpdateMissionSnapShot(node1, pixelMap);
    ASSERT_EQ(wim_->isRecentHolderEnable_, true);
}

/**
 * @tc.name: GetPid
 * @tc.desc: test WindowInnerManager GetPid
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, GetPid, Function | SmallTest | Level2)
{
    ASSERT_EQ(wim_->GetPid(), INVALID_PID);
}

/**
 * @tc.name: SetInputEventConsumer
 * @tc.desc: test WindowInnerManager SetInputEventConsumer
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, SetInputEventConsumer, Function | SmallTest | Level2)
{
    wim_->moveDragController_ = nullptr;
    wim_->SetInputEventConsumer();
    ASSERT_EQ(wim_->moveDragController_, nullptr);
    wim_->moveDragController_ = new MoveDragController();
    wim_->SetInputEventConsumer();
    ASSERT_NE(wim_->moveDragController_, nullptr);
}

/**
 * @tc.name: StartWindowInfoReportLoop
 * @tc.desc: test WindowInnerManager StartWindowInfoReportLoop
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerManagerTest, StartWindowInfoReportLoop, Function | SmallTest | Level2)
{
    wim_->isReportTaskStart_ = true;
    wim_->eventHandler_ = nullptr;
    wim_->StartWindowInfoReportLoop();
    ASSERT_EQ(wim_->isReportTaskStart_, true);
    wim_->isReportTaskStart_ = false;
    wim_->eventHandler_ = std::make_shared<EventHandler>(EventRunner::Create());
    wim_->StartWindowInfoReportLoop();
    ASSERT_EQ(wim_->isReportTaskStart_, true);
}
}
}
}

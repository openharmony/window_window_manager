/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "display_manager.h"
#include "drag_controller.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DragControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<WindowRoot> windowRoot_;
private:
    static sptr<MoveDragController> moveDragController_;
    static std::shared_ptr<MMI::IInputEventConsumer> inputListener_;
};

sptr<MoveDragController> DragControllerTest::moveDragController_ = nullptr;
std::shared_ptr<MMI::IInputEventConsumer> DragControllerTest::inputListener_ = nullptr;
sptr<WindowRoot> DragControllerTest::windowRoot_ = nullptr;

void DragControllerTest::SetUpTestCase()
{
    WindowInnerManager::GetInstance().Init();
    moveDragController_ = WindowInnerManager::GetInstance().moveDragController_;

    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    ASSERT_TRUE((displayInfo != nullptr));
    ASSERT_TRUE((display != nullptr));
    ASSERT_TRUE((display->GetDisplayInfo() != nullptr));
    ASSERT_TRUE(moveDragController_);
    inputListener_ = moveDragController_->inputListener_;
    ASSERT_TRUE(inputListener_);
}

void DragControllerTest::TearDownTestCase()
{
    moveDragController_ = nullptr;
    inputListener_ = nullptr;
    WindowInnerManager::GetInstance().Stop();
}

void DragControllerTest::SetUp()
{
    windowRoot_ = new WindowRoot(nullptr);
}

void DragControllerTest::TearDown()
{
    windowRoot_ = nullptr;
}

namespace {
/**
 * @tc.name: OnInputEvent01
 * @tc.desc: OnInputEven01, keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnInputEvent01, Function | SmallTest | Level2)
{
    ASSERT_TRUE(inputListener_);
    std::shared_ptr<MMI::KeyEvent> keyEvent;
    inputListener_->OnInputEvent(keyEvent);
    keyEvent = MMI::KeyEvent::Create();
    inputListener_->OnInputEvent(keyEvent);
}

/**
 * @tc.name: OnInputEvent02
 * @tc.desc: OnInputEvent02, axisEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnInputEvent02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(inputListener_);
    std::shared_ptr<MMI::AxisEvent> axisEvent;
    inputListener_->OnInputEvent(axisEvent);
    axisEvent = MMI::AxisEvent::Create();
    inputListener_->OnInputEvent(axisEvent);
}

/**
 * @tc.name: OnInputEvent03
 * @tc.desc: OnInputEvent03, pointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnInputEvent03, Function | SmallTest | Level2)
{
    ASSERT_TRUE(inputListener_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    inputListener_->OnInputEvent(pointerEvent);
    pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(INVALID_WINDOW_ID);
    ASSERT_TRUE(moveDragController_);
    moveDragController_->SetActiveWindowId(1);
    inputListener_->OnInputEvent(pointerEvent);
}

/**
 * @tc.name: Stop02
 * @tc.desc: Stop02
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, Stop02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    moveDragController_->Init();
    moveDragController_->Stop();
    moveDragController_->Init();
}

/**
 * @tc.name: HandleEndUpMovingOrDragging
 * @tc.desc: HandleEndUpMovingOrDragging
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleEndUpMovingOrDragging, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    uint32_t windowId = 1; // windowId: 1
    moveDragController_->HandleEndUpMovingOrDragging(windowId);
    moveDragController_->activeWindowId_ = windowId;
    moveDragController_->HandleEndUpMovingOrDragging(windowId);
}

/**
 * @tc.name: HandleWindowRemovedOrDestroyed
 * @tc.desc: HandleWindowRemovedOrDestroyed
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleWindowRemovedOrDestroyed, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    uint32_t windowId = 5; // windowId: 5
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_->startMoveFlag_ = true;
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_->startMoveFlag_ = false;
    moveDragController_->moveDragProperty_->startDragFlag_ = true;
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_->startMoveFlag_ = true;
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: ConvertPointerPosToDisplayGroupPos
 * @tc.desc: ConvertPointerPosToDisplayGroupPos
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, ConvertPointerPosToDisplayGroupPos, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);

    auto displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(0);
    displayInfo->SetWidth(720);   // displayWidth: 720
    displayInfo->SetHeight(1280); // displayHeight: 1280
    displayInfo->SetOffsetX(0);
    displayInfo->SetOffsetY(0);
    DisplayGroupInfo::GetInstance().AddDisplayInfo(displayInfo);

    int32_t posX = 0;
    int32_t posY = 0;
    moveDragController_->ConvertPointerPosToDisplayGroupPos(0, posX, posY);

    moveDragController_->ConvertPointerPosToDisplayGroupPos(1, posX, posY);
    moveDragController_->ConvertPointerPosToDisplayGroupPos(0, posX, posY);
    DisplayGroupInfo::GetInstance().RemoveDisplayInfo(0);
}

/**
 * @tc.name: ConsumePointerEvent
 * @tc.desc: ConsumePointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, ConsumePointerEvent, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    moveDragController_->ConsumePointerEvent(pointerEvent);
    pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    moveDragController_->ConsumePointerEvent(pointerEvent);
}

/**
 * @tc.name: OnReceiveVsync
 * @tc.desc: OnReceiveVsync
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, OnReceiveVsync, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    moveDragController_->OnReceiveVsync(0);
    moveDragController_->moveEvent_ = pointerEvent;
    moveDragController_->OnReceiveVsync(0);
}

/**
 * @tc.name: GetHotZoneRect
 * @tc.desc: GetHotZoneRect
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, GetHotZoneRect, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);

    auto displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(0);
    displayInfo->SetWidth(720);   // displayWidth: 720
    displayInfo->SetHeight(1280); // displayHeight: 1280
    displayInfo->SetOffsetX(0);
    displayInfo->SetOffsetY(0);
    DisplayGroupInfo::GetInstance().AddDisplayInfo(displayInfo);
    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->moveDragProperty_->targetDisplayId_ = 0;
    moveDragController_->moveDragProperty_->startRectExceptCorner_ = { 0, 0, 40, 40 };

    moveDragController_->moveDragProperty_->startPointPosX_ = 20; // startPointPosX: 20
    moveDragController_->moveDragProperty_->startPointPosY_ = 20; // startPointPosY: 20
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = 20; // startPointPosY: 20
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 41; // startPointPosX: 41
    moveDragController_->moveDragProperty_->startPointPosY_ = 20; // startPointPosY: 20
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 20; // startPointPosX: 20
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 20; // startPointPosX: 20
    moveDragController_->moveDragProperty_->startPointPosY_ = 41; // startPointPosY: 41
    moveDragController_->GetHotZoneRect();

    moveDragController_->moveDragProperty_->startPointPosX_ = 41; // startPointPosX: 41
    moveDragController_->moveDragProperty_->startPointPosY_ = 41; // startPointPosY: 41
    moveDragController_->GetHotZoneRect();
    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandleDragEvent01
 * @tc.desc: HandleDragEvent01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleDragEvent01, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    int32_t posX = 0;
    int32_t posY = 0;
    int32_t pointId = 0;
    int32_t sourceType = 0;
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startDragFlag_ = true;
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    pointId = 1;
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    pointId = 0;
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    sourceType = 1;
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    sourceType = 0;
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandleDragEvent02
 * @tc.desc: HandleDragEvent02
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleDragEvent02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    int32_t posX = 0;
    int32_t posY = 0;
    int32_t pointId = 0;
    int32_t sourceType = 0;
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startDragFlag_ = true;
    moveDragController_->moveDragProperty_->targetDisplayId_ = 0;
    moveDragController_->moveDragProperty_->startRectExceptCorner_ = { 0, 0, 40, 40 }; // hotZone: 0, 0, 40, 40

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startPointPosX_ = 45; // startPointPosX: 45
    moveDragController_->moveDragProperty_->startPointPosY_ = -1; // startPointPosY: -1
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startPointPosX_ = -1; // startPointPosX: -1
    moveDragController_->moveDragProperty_->startPointPosY_ = 45; // startPointPosY: 45
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_->startPointPosX_ = 45; // startPointPosX: 45
    moveDragController_->moveDragProperty_->startPointPosY_ = 45; // startPointPosY: 45
    moveDragController_->HandleDragEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandleMoveEvent
 * @tc.desc: HandleMoveEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandleMoveEvent, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    int32_t posX = 0;
    int32_t posY = 0;
    int32_t pointId = 0;
    int32_t sourceType = 0;
    moveDragController_->HandleMoveEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandleMoveEvent(0, posX, posY, pointId, sourceType);

        moveDragController_->moveDragProperty_->startMoveFlag_ = true;
    moveDragController_->HandleMoveEvent(0, posX, posY, pointId, sourceType);

    pointId = 1;
    moveDragController_->HandleMoveEvent(0, posX, posY, pointId, sourceType);

    pointId = 0;
    moveDragController_->HandleMoveEvent(0, posX, posY, pointId, sourceType);

    sourceType = 1;
    moveDragController_->HandleMoveEvent(0, posX, posY, pointId, sourceType);

    sourceType = 0;
    moveDragController_->HandleMoveEvent(0, posX, posY, pointId, sourceType);

    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: HandlePointerEvent
 * @tc.desc: HandlePointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, HandlePointerEvent, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerItem.SetDisplayX(0);
    pointerItem.SetDisplayY(0);
    pointerEvent->AddPointerItem(pointerItem);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->windowProperty_ = new WindowProperty();
    moveDragController_->moveDragProperty_ = new MoveDragProperty();
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->moveDragProperty_->startPointerId_ = 1;
    moveDragController_->moveDragProperty_->sourceType_ = 2; // sourceType: 2
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->moveDragProperty_->startPointerId_ = 0;
    moveDragController_->moveDragProperty_->sourceType_ = 2; // sourceType: 2
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
    moveDragController_->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    moveDragController_->HandlePointerEvent(pointerEvent);

    moveDragController_->windowProperty_ = nullptr;
    moveDragController_->moveDragProperty_ = nullptr;
}

/**
 * @tc.name: UpdateDragInfo01
 * @tc.desc: UpdateDragInfo01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, UpdateDragInfo01, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    uint32_t windowId = 0;
    sptr<DragController> dragcontroller = new DragController(windowRoot_);
    dragcontroller->UpdateDragInfo(windowId);
}

/**
 * @tc.name: UpdateDragInfo02
 * @tc.desc: UpdateDragInfo02
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, UpdateDragInfo02, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    sptr<DragController> dragcontroller = new DragController(windowRoot_);
    dragcontroller->UpdateDragInfo(windowId);
    sptr<WindowNode> dragNode = windowRoot_->GetWindowNode(windowId);
    ASSERT_EQ(dragNode, nullptr); 
}

/**
 * @tc.name: StartDrag01
 * @tc.desc: StartDrag01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, StartDrag01, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    sptr<DragController> dragcontroller = new DragController(windowRoot_);
    dragcontroller->StartDrag(windowId);
    sptr<WindowNode> dragNode = windowRoot_->GetWindowNode(windowId);
    ASSERT_EQ(dragNode, nullptr); 
}

/**
 * @tc.name: FinishDrag01
 * @tc.desc: FinishDrag01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, FinishDrag01, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    sptr<DragController> dragcontroller = new DragController(windowRoot_);
    dragcontroller->FinishDrag(windowId);
    sptr<WindowNode> dragNode = windowRoot_->GetWindowNode(windowId);
    ASSERT_EQ(dragNode, nullptr); 
}

/**
 * @tc.name: GetHitWindow01
 * @tc.desc: GetHitWindow01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, GetHitWindow01, Function | SmallTest | Level2)
{
    DisplayId id = 0;
    PointInfo point;
    point.x = 1;
    point.y = 1;
    sptr<DragController> dragcontroller = new DragController(windowRoot_);
    ASSERT_EQ(nullptr, dragcontroller->GetHitWindow(id, point)); 
}

/**
 * @tc.name: GetHitWindow02
 * @tc.desc: GetHitWindow02
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, GetHitWindow02, Function | SmallTest | Level2)
{
    DisplayId id = DISPLAY_ID_INVALID;
    PointInfo point;
    point.x = 1;
    point.y = 2;
    sptr<DragController> dragcontroller = new DragController(windowRoot_);
    ASSERT_EQ(nullptr, dragcontroller->GetHitWindow(id, point)); 
}

/**
 * @tc.name: GetHitWindow03
 * @tc.desc: GetHitWindow03
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, GetHitWindow03, Function | SmallTest | Level2)
{
    DisplayId id = 1;
    PointInfo point;
    point.x = 1;
    point.y = 2;
    sptr<DragController> dragcontroller = new DragController(windowRoot_);
    sptr<WindowNodeContainer> container = windowRoot_->GetOrCreateWindowNodeContainer(id);
    ASSERT_EQ(nullptr, container);
    ASSERT_EQ(nullptr, dragcontroller->GetHitWindow(id, point)); 
}

/**
 * @tc.name: Init01
 * @tc.desc: Init01
 * @tc.type: FUNC
 */
HWTEST_F(DragControllerTest, Init02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(moveDragController_);
    auto ret = moveDragController_->Init();
    ASSERT_EQ(true, ret); 
}
}
} // namespace Rosen
} // namespace OHOS

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
#include <memory>
#include <pointer_event.h>
#include "session/host/include/move_drag_controller.h"
#include "session/host/include/session.h"
#include "ui/rs_surface_node.h"
#include "window_manager_hilog.h"
#include "session/host/include/scene_session.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MoveDragControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<MoveDragController> moveDragController;
    sptr<Session> session_;
};

void MoveDragControllerTest::SetUpTestCase()
{
}

void MoveDragControllerTest::TearDownTestCase()
{
}

void MoveDragControllerTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    session_ = new (std::nothrow) Session(info);
    moveDragController = new MoveDragController(session_->GetPersistentId());
}

void MoveDragControllerTest::TearDown()
{
    session_ = nullptr;
    moveDragController = nullptr;
}

namespace {
/**
 * @tc.name: SetStartMoveFlag
 * @tc.desc: test function : SetStartMoveFlag
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, SetStartMoveFlag, Function | SmallTest | Level1)
{
    int32_t res = 0;
    moveDragController->SetStartMoveFlag(true);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: GetStartMoveFlag
 * @tc.desc: test function : GetStartMoveFlag
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, GetStartMoveFlag, Function | SmallTest | Level1)
{
    auto preIsStartMove = moveDragController->isStartMove_;
    auto preHasPointDown = moveDragController->hasPointDown_;
    moveDragController->hasPointDown_ = false;
    moveDragController->SetStartMoveFlag(true);
    bool res = moveDragController->GetStartMoveFlag();
    ASSERT_EQ(preIsStartMove, res);
    moveDragController->SetStartMoveFlag(false);
    res = moveDragController->GetStartMoveFlag();
    ASSERT_EQ(false, res);
    moveDragController->hasPointDown_ = true;
    moveDragController->SetStartMoveFlag(true);
    res = moveDragController->GetStartMoveFlag();
    ASSERT_EQ(true, res);
    moveDragController->SetStartMoveFlag(false);
    res = moveDragController->GetStartMoveFlag();
    ASSERT_EQ(false, res);
    moveDragController->isStartMove_ = preIsStartMove;
    moveDragController->hasPointDown_ = preHasPointDown;
}

/**
 * @tc.name: GetStartDragFlag
 * @tc.desc: test function : GetStartDragFlag
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, GetStartDragFlag, Function | SmallTest | Level1)
{
    bool res = moveDragController->GetStartDragFlag();
    ASSERT_EQ(false, res);
}

/**
 * @tc.name: GetTargetRect
 * @tc.desc: test function : GetTargetRect
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, GetTargetRect, Function | SmallTest | Level1)
{
    uint32_t tmp = 0;
    int32_t pos = 0;
    moveDragController->InitMoveDragProperty();
    WSRect res = moveDragController->GetTargetRect();
    ASSERT_EQ(tmp, res.height_);
    ASSERT_EQ(tmp, res.width_);
    ASSERT_EQ(pos, res.posX_);
    ASSERT_EQ(pos, res.posY_);
}

/**
 * @tc.name: InitMoveDragProperty
 * @tc.desc: test function : InitMoveDragProperty
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, InitMoveDragProperty, Function | SmallTest | Level1)
{
    int32_t res = 0;
    moveDragController->InitMoveDragProperty();
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: SetOriginalValue
 * @tc.desc: test function : SetOriginalValue
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, SetOriginalValue, Function | SmallTest | Level1)
{
    int32_t res = 0;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t pointerType = pointerEvent->GetSourceType();
    int32_t pointerPosX = 10;
    int32_t pointerPosY = 30;
    WSRect winRect = { 100, 100, 1000, 1000 };
    moveDragController->SetOriginalValue(pointerId, pointerType, pointerPosX, pointerPosY, winRect);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: SetAspectRatio
 * @tc.desc: test function : SetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, SetAspectRatio, Function | SmallTest | Level1)
{
    int32_t res = 0;
    moveDragController->SetAspectRatio(0.5);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: UpdateGravityWhenDrag
 * @tc.desc: test function : UpdateGravityWhenDrag
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, UpdateGravityWhenDrag, Function | SmallTest | Level1)
{
    int32_t res = 0;
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    if (!surfaceNode || !pointerEvent) {
        return;
    }
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    auto tempPointerEvent = pointerEvent;
    pointerEvent = nullptr;
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    pointerEvent = tempPointerEvent;
    auto tempSurfaceNode = surfaceNode;
    surfaceNode = nullptr;
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    surfaceNode = tempSurfaceNode;
    moveDragController->type_ = AreaType::UNDEFINED;
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    moveDragController->type_ = AreaType::RIGHT;
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    ASSERT_EQ(0, res);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    ASSERT_EQ(0, res);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    ASSERT_EQ(0, res);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    ASSERT_EQ(0, res);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode, true);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: CalcMoveTargetRect
 * @tc.desc: test function : CalcMoveTargetRect
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcMoveTargetRect, Function | SmallTest | Level1)
{
    int32_t res = 0;
    moveDragController->InitMoveDragProperty();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    WSRect originalRect = { 100, 100, 1000, 1000 };

    moveDragController->CalcMoveTargetRect(pointerEvent, originalRect);
    ASSERT_EQ(0, res);

    pointerEvent = MMI::PointerEvent::Create();
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t pointerType = pointerEvent->GetSourceType();
    int32_t pointerPosX = 10;
    int32_t pointerPosY = 30;
    moveDragController->SetOriginalValue(pointerId, pointerType, pointerPosX, pointerPosY, originalRect);
    moveDragController->CalcMoveTargetRect(pointerEvent, originalRect);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: EventDownInit
 * @tc.desc: test function : EventDownInit
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, EventDownInit, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SystemSessionConfig sysConfig;
    moveDragController->InitMoveDragProperty();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    WSRect originalRect = { 100, 100, 1000, 1000 };

    pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);

    auto res = moveDragController->EventDownInit(pointerEvent, originalRect, property, sysConfig);
    ASSERT_EQ(false, res);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetTargetDisplayId(0);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    originalRect = { 10, 10, 10, 10 };
    pointerItem.SetWindowX(100000000);
    pointerItem.SetWindowY(100000000);
    pointerEvent->AddPointerItem(pointerItem);
    res = moveDragController->EventDownInit(pointerEvent, originalRect, property, sysConfig);
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: EventDownInit01
 * @tc.desc: test function : EventDownInit
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, EventDownInit01, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetDecorEnable(true);

    SystemSessionConfig sysConfig;
    moveDragController->InitMoveDragProperty();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    WSRect originalRect = { 100, 100, 1000, 1000 };
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(1);
    pointerItem.SetWindowX(1);
    pointerItem.SetWindowY(1);
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    auto res = moveDragController->EventDownInit(pointerEvent, originalRect, property, sysConfig);
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: CalcFreeformTargetRect
 * @tc.desc: test function : CalcFreeformTargetRect
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcFreeformTargetRect, Function | SmallTest | Level1)
{
    AreaType type = AreaType::RIGHT;
    WSRect originalRect = { 100, 100, 1000, 1000 };
    int32_t tranX = 10;
    int32_t tranY = 30;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    type = AreaType::LEFT;
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    type = AreaType::RIGHT;
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    type = AreaType::BOTTOM;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    originalRect = { 100, 100, 1000, 0 };
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    originalRect = { 100, 100, 500, 100 };
    moveDragController->limits_ = { 3, 3, 3, 3, 2.0, 2.0 };
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    type = AreaType::RIGHT;
    originalRect = { 100, 100, 500, 0 };
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    originalRect = { 100, 100, 100, 100 };
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    type = AreaType::UNDEFINED;
    originalRect = { 100, 100, 500, 100 };
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    type = AreaType::RIGHT;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
    moveDragController->limits_ = { 3, 3, 3, 3, 0.0001, 0.0001 };
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTargetRect(type, tranX, tranY, originalRect);
}

/**
 * @tc.name: CalcFixedAspectRatioTargetRect01
 * @tc.desc: test function : CalcFixedAspectRatioTargetRect01
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcFixedAspectRatioTargetRect01, Function | SmallTest | Level1)
{
    AreaType type = AreaType::RIGHT;
    float aspectRatio = 0.5;
    WSRect originalRect = { 100, 100, 1000, 1000 };
    int32_t tranX = 0;
    int32_t tranY = 0;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
}

/**
 * @tc.name: CalcFixedAspectRatioTargetRect02
 * @tc.desc: test function : CalcFixedAspectRatioTargetRect02
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcFixedAspectRatioTargetRect02, Function | SmallTest | Level1)
{
    AreaType type = AreaType::RIGHT;
    float aspectRatio = 0.5;
    WSRect originalRect = { 100, 100, 1000, 1000 };
    int32_t tranX = 20;
    int32_t tranY = 20;
    ASSERT_TRUE((moveDragController != nullptr));
    type = AreaType::UNDEFINED;
    moveDragController->mainMoveAxis_ = MoveDragController::AxisType::UNDEFINED;
    tranX = 0;
    tranY = 0;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::RIGHT;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    moveDragController->mainMoveAxis_ = MoveDragController::AxisType::X_AXIS;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::LEFT_TOP;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::RIGHT_TOP;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::RIGHT_BOTTOM;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::LEFT_BOTTOM;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::LEFT;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::TOP;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::RIGHT;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::BOTTOM;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::UNDEFINED;
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
}

/**
 * @tc.name: CalcFreeformTranslateLimits01
 * @tc.desc: test function : CalcFreeformTranslateLimits01
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcFreeformTranslateLimits01, Function | SmallTest | Level1)
{
    AreaType type = AreaType::RIGHT;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->CalcFreeformTranslateLimits(type);
    type = AreaType::BOTTOM;
    moveDragController->CalcFreeformTranslateLimits(type);
    type = AreaType::TOP;
    moveDragController->CalcFreeformTranslateLimits(type);
    type = AreaType::LEFT;
    moveDragController->CalcFreeformTranslateLimits(type);
}

/**
 * @tc.name: CalcFixedAspectRatioTranslateLimits01
 * @tc.desc: test function : CalcFixedAspectRatioTranslateLimits01
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcFixedAspectRatioTranslateLimits01, Function | SmallTest | Level1)
{
    moveDragController->limits_ = { 30, 60, 30, 60, 2.0, 2.0 };
    moveDragController->aspectRatio_ = 1.0f;
    MoveDragController::AxisType axis = MoveDragController::AxisType::X_AXIS;
    AreaType type = AreaType::RIGHT;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->isDecorEnable_ = true;
    moveDragController->CalcFixedAspectRatioTranslateLimits(type, axis);
    moveDragController->isDecorEnable_ = false;
    moveDragController->CalcFixedAspectRatioTranslateLimits(type, axis);
    moveDragController->limits_ = { 60, 60, 60, 60, 2.0, 2.0 };
    moveDragController->CalcFixedAspectRatioTranslateLimits(type, axis);
    type = AreaType::LEFT;
    moveDragController->CalcFixedAspectRatioTranslateLimits(type, axis);
    axis = MoveDragController::AxisType::Y_AXIS;
    type = AreaType::BOTTOM;
    moveDragController->CalcFixedAspectRatioTranslateLimits(type, axis);
    axis = MoveDragController::AxisType::X_AXIS;
    type = AreaType::TOP;
    moveDragController->CalcFixedAspectRatioTranslateLimits(type, axis);
}

/**
 * @tc.name: FixTranslateByLimits01
 * @tc.desc: test function : FixTranslateByLimits01
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, FixTranslateByLimits01, Function | SmallTest | Level1)
{
    int32_t tranX = 100;
    int32_t tranY = 100;
    moveDragController->maxTranX_ = 50;
    moveDragController->maxTranY_ = 50;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->FixTranslateByLimits(tranX, tranY);
    tranX = 10;
    moveDragController->FixTranslateByLimits(tranX, tranY);
    tranY = 10;
    moveDragController->FixTranslateByLimits(tranX, tranY);
}

/**
 * @tc.name: InitMainAxis01
 * @tc.desc: test function : InitMainAxis01
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, InitMainAxis01, Function | SmallTest | Level1)
{
    AreaType type = AreaType::LEFT;
    int32_t tranX = 100;
    int32_t tranY = 100;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->InitMainAxis(type, tranX, tranY);
    type = AreaType::RIGHT;
    moveDragController->InitMainAxis(type, tranX, tranY);
    type = AreaType::TOP;
    moveDragController->InitMainAxis(type, tranX, tranY);
    type = AreaType::BOTTOM;
    moveDragController->InitMainAxis(type, tranX, tranY);
    type = AreaType::UNDEFINED;
    tranX = 0;
    tranY = 0;
    moveDragController->InitMainAxis(type, tranX, tranY);
    tranY = 1;
    moveDragController->InitMainAxis(type, tranX, tranY);
}

/**
 * @tc.name: ConvertXYByAspectRatio01
 * @tc.desc: test function : ConvertXYByAspectRatio01
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, ConvertXYByAspectRatio01, Function | SmallTest | Level1)
{
    float aspectRatio = 1.0f;
    int32_t tx = 100;
    int32_t ty = 100;
    moveDragController->mainMoveAxis_ = MoveDragController::AxisType::X_AXIS;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->ConvertXYByAspectRatio(tx, ty, aspectRatio);
    moveDragController->mainMoveAxis_ = MoveDragController::AxisType::Y_AXIS;
    moveDragController->ConvertXYByAspectRatio(tx, ty, aspectRatio);
}

/**
 * @tc.name: InitDecorValue01
 * @tc.desc: test function : InitDecorValue01
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, InitDecorValue01, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SystemSessionConfig sysConfig;
    ASSERT_TRUE((moveDragController != nullptr));
    moveDragController->InitDecorValue(property, sysConfig);
}

/**
 * @tc.name: ConsumeMoveEvent
 * @tc.desc: test function : ConsumeMoveEvent
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, ConsumeMoveEvent, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    if (!pointerEvent) {
        return;
    }
    WSRect originalRect = { 100, 100, 1000, 1000 };
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(nullptr, originalRect));
    auto preStratDragFlag = moveDragController->GetStartDragFlag();
    moveDragController->isStartDrag_ = true;
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    moveDragController->isStartDrag_ = false;
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    pointerEvent->SetSourceType(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    moveDragController->SetStartMoveFlag(false);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    moveDragController->SetStartMoveFlag(true);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UNKNOWN);
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    moveDragController->moveDragProperty_.pointerId_ = -2;
    moveDragController->moveDragProperty_.pointerType_ = -2;
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
    moveDragController->isStartDrag_ = preStratDragFlag;
}


/**
 * @tc.name: ProcessWindowDragHotAreaFunc
 * @tc.desc: test function : ProcessWindowDragHotAreaFunc
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, ProcessWindowDragHotAreaFunc, Function | SmallTest | Level1)
{
    bool isSendHotAreaMessage = true;
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    moveDragController->ProcessWindowDragHotAreaFunc(isSendHotAreaMessage, reason);
    ASSERT_EQ(true, isSendHotAreaMessage);
    auto dragHotAreaFunc = [](int32_t type, const SizeChangeReason& reason) {
        type = 0;
    };
    auto preFunc = moveDragController->windowDragHotAreaFunc_;
    moveDragController->windowDragHotAreaFunc_ = dragHotAreaFunc;
    moveDragController->ProcessWindowDragHotAreaFunc(isSendHotAreaMessage, reason);
    ASSERT_EQ(true, isSendHotAreaMessage);
    moveDragController->windowDragHotAreaFunc_ = preFunc;
}

/**
 * @tc.name: ConsumeDragEvent
 * @tc.desc: test function : ConsumeDragEvent
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, ConsumeDragEvent, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    if (!pointerEvent) {
        return;
    }
    WSRect originalRect = { 100, 100, 1000, 1000 };
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    if (!property) {
        return;
    }
    SystemSessionConfig sysConfig;
    moveDragController->GetVirtualPixelRatio();
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(nullptr, originalRect, property, sysConfig));
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, nullptr, sysConfig));
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
    moveDragController->SetStartMoveFlag(true);
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
    moveDragController->SetStartMoveFlag(false);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_DOWN));
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerItem.SetWindowX(0);
    pointerItem.SetWindowY(0);
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_DOWN));
    ASSERT_EQ(true, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_MOVE));
    ASSERT_EQ(true, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_UP));
    ASSERT_EQ(true, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_UNKNOWN));
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, property, sysConfig));
}

/**
 * @tc.name: UpdateDragType01
 * @tc.desc: test function : UpdateDragType
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, UpdateDragType01, Function | SmallTest | Level1)
{
    moveDragController->rectExceptCorner_.posX_ = 2;
    moveDragController->rectExceptCorner_.width_ = 2;
    moveDragController->rectExceptCorner_.posY_ = 0;
    moveDragController->rectExceptCorner_.height_ = 0;
    moveDragController->UpdateDragType(3, 3);
    ASSERT_EQ(moveDragController->dragType_, MoveDragController::DragType::DRAG_BOTTOM_OR_TOP);
}

/**
 * @tc.name: UpdateDragType02
 * @tc.desc: test function : UpdateDragType
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, UpdateDragType02, Function | SmallTest | Level1)
{
    moveDragController->rectExceptCorner_.posX_ = 0;
    moveDragController->rectExceptCorner_.width_ = 0;
    moveDragController->rectExceptCorner_.posY_ = 2;
    moveDragController->rectExceptCorner_.height_ = 2;
    moveDragController->UpdateDragType(3, 3);
    ASSERT_EQ(moveDragController->dragType_, MoveDragController::DragType::DRAG_LEFT_OR_RIGHT);
}

/**
 * @tc.name: UpdateDragType03
 * @tc.desc: test function : UpdateDragType
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, UpdateDragType03, Function | SmallTest | Level1)
{
    moveDragController->rectExceptCorner_.posX_ = 1;
    moveDragController->rectExceptCorner_.width_ = 0;
    moveDragController->rectExceptCorner_.posY_ = 1;
    moveDragController->rectExceptCorner_.height_ = 0;
    moveDragController->UpdateDragType(1, 1);
    ASSERT_EQ(moveDragController->dragType_, MoveDragController::DragType::DRAG_LEFT_TOP_CORNER);
}

/**
 * @tc.name: IsPointInDragHotZone01
 * @tc.desc: test function : IsPointInDragHotZone
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, IsPointInDragHotZone01, Function | SmallTest | Level1)
{
    WSRect winRect = { 10, 10, 10, 10 };
    int32_t sourceType = MMI::PointerEvent::SOURCE_TYPE_MOUSE;
    int32_t startPointPosX = 1;
    int32_t startPointPosY = 1;
    bool res = moveDragController->IsPointInDragHotZone(startPointPosX, startPointPosY, sourceType, winRect);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: IsPointInDragHotZone02
 * @tc.desc: test function : IsPointInDragHotZone
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, IsPointInDragHotZone02, Function | SmallTest | Level1)
{
    WSRect winRect = { 5, 5, 0, 0 };
    int32_t startPointPosX = 1;
    int32_t startPointPosY = 1;
    bool res = moveDragController->IsPointInDragHotZone(startPointPosX, startPointPosY, 0, winRect);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CalculateStartRectExceptHotZone
 * @tc.desc: test function : CalculateStartRectExceptHotZone
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalculateStartRectExceptHotZone, Function | SmallTest | Level1)
{
    float vpr = 1.0f;
    WSRect winRect;
    winRect.posX_ = 100;
    winRect.posY_ = 100;
    winRect.width_ = 100;
    winRect.height_ = 100;
    moveDragController->CalculateStartRectExceptHotZone(vpr, winRect);

    EXPECT_EQ(moveDragController->rectExceptFrame_.posX_, 105);
    EXPECT_EQ(moveDragController->rectExceptFrame_.posY_, 105);
    EXPECT_EQ(moveDragController->rectExceptFrame_.width_, 90);
    EXPECT_EQ(moveDragController->rectExceptFrame_.height_, 90);

    EXPECT_EQ(moveDragController->rectExceptCorner_.posX_, 116);
    EXPECT_EQ(moveDragController->rectExceptCorner_.posY_, 116);
    EXPECT_EQ(moveDragController->rectExceptCorner_.width_, 68);
    EXPECT_EQ(moveDragController->rectExceptCorner_.height_, 68);
}

/**
 * @tc.name: CalcFirstMoveTargetRect
 * @tc.desc: test function : CalcFirstMoveTargetRect
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcFirstMoveTargetRect, Function | SmallTest | Level1)
{
    int res = 0;
    WSRect windowRect = { 0, 0, 0, 0 };
    moveDragController->CalcFirstMoveTargetRect(windowRect, false);
    res++;
    moveDragController->moveTempProperty_.pointerId_ = 0;
    moveDragController->CalcFirstMoveTargetRect(windowRect, false);
    ASSERT_EQ(res, 1);
    auto preIsStartMove = moveDragController->GetStartMoveFlag();
    auto preMoveTempProperty = moveDragController->moveTempProperty_;
    moveDragController->isStartMove_ = false;
    moveDragController->moveTempProperty_ = { -1, -1, -1, -1, -1, -1, -1, -1 };
    moveDragController->CalcFirstMoveTargetRect(windowRect, false);
    ASSERT_EQ(res, 1);
    moveDragController->moveTempProperty_ = { 1, 1, 1, 1, 1, 1, 1, 1 };
    moveDragController->CalcFirstMoveTargetRect(windowRect, false);
    ASSERT_EQ(res, 1);
    moveDragController->isStartMove_ = true;
    moveDragController->CalcFirstMoveTargetRect(windowRect, false);
    ASSERT_EQ(res, 1);
    moveDragController->moveTempProperty_ = { -1, -1, -1, -1, -1, -1, -1, -1 };
    moveDragController->CalcFirstMoveTargetRect(windowRect, false);
    ASSERT_EQ(res, 1);
    moveDragController->moveTempProperty_ = { 1, 1, 1, 1, 1, 1, 1, 1 };
    moveDragController->CalcFirstMoveTargetRect(windowRect, false);
    ASSERT_EQ(res, 1);
    moveDragController->CalcFirstMoveTargetRect(windowRect, true);
    ASSERT_EQ(res, 1);
    moveDragController->isStartMove_ = preIsStartMove;
    moveDragController->moveTempProperty_ = preMoveTempProperty;
}

/**
 * @tc.name: CalcFirstMoveTargetRect001
 * @tc.desc: test function : CalcFirstMoveTargetRect001
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CalcFirstMoveTargetRect001, Function | SmallTest | Level1)
{
    WSRect windowRect = { 1, 2, 3, 4 };
    moveDragController->InitMoveDragProperty();
    moveDragController->SetStartMoveFlag(true);
    moveDragController->CalcFirstMoveTargetRect(windowRect, true);
    WSRect targetRect = moveDragController->GetTargetRect();
    ASSERT_EQ(targetRect.posX_, 0);
}

/**
 * @tc.name: GetFullScreenToFloatingRect
 * @tc.desc: test function : GetFullScreenToFloatingRect
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, GetFullScreenToFloatingRect, Function | SmallTest | Level1)
{
    WSRect originalRect = { 1, 2, 0, 4 };
    WSRect windowRect = { 5, 6, 7, 8 };
    auto preMoveTempProperty = moveDragController->moveTempProperty_;
    moveDragController->moveTempProperty_ = { -1, -1, -1, -1, -1, -1, -1, -1 };
    WSRect rect = moveDragController->GetFullScreenToFloatingRect(originalRect, windowRect);
    ASSERT_EQ(originalRect.posX_, rect.posX_);
    moveDragController->moveTempProperty_ = { 1, 1, 1, 1, 1, 1, 1, 1 };
    rect = moveDragController->GetFullScreenToFloatingRect(originalRect, windowRect);
    ASSERT_EQ(windowRect.posX_, rect.posX_);
    originalRect = { 1, 2, 3, 4 };
    rect = moveDragController->GetFullScreenToFloatingRect(originalRect, windowRect);
    WSRect targetRect = { -1, 2, 7, 8, };
    ASSERT_EQ(targetRect.posX_, rect.posX_);
    moveDragController->moveTempProperty_ = preMoveTempProperty;
}

/**
 * @tc.name: CheckDragEventLegal
 * @tc.desc: test function : CheckDragEventLegal
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, CheckDragEventLegal, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    auto tempPointerEvent = pointerEvent;
    pointerEvent = nullptr;
    auto result = moveDragController->CheckDragEventLegal(pointerEvent, property);
    ASSERT_EQ(result, false);
    pointerEvent = tempPointerEvent;
    result = moveDragController->CheckDragEventLegal(pointerEvent, nullptr);
    ASSERT_EQ(result, false);
    moveDragController->isStartMove_ = true;
    result = moveDragController->CheckDragEventLegal(pointerEvent, property);
    ASSERT_EQ(result, false);
    moveDragController->isStartMove_ = false;
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_UP));
    result = moveDragController->CheckDragEventLegal(pointerEvent, property);
    ASSERT_EQ(result, false);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_DOWN));
    result = moveDragController->CheckDragEventLegal(pointerEvent, property);
    ASSERT_EQ(result, true);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN));
    result = moveDragController->CheckDragEventLegal(pointerEvent, property);
    ASSERT_EQ(result, true);
    auto preMoveDragProperty = moveDragController->moveDragProperty_;
    moveDragController->moveDragProperty_.pointerId_ = -1;
    result = moveDragController->CheckDragEventLegal(pointerEvent, property);
    ASSERT_EQ(result, true);
    moveDragController->moveDragProperty_ = preMoveDragProperty;
}

/**
 * @tc.name: UpdateMoveTempProperty
 * @tc.desc: test function : UpdateMoveTempProperty
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, UpdateMoveTempProperty, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    auto result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN));
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_DOWN));
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_MOVE));
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_UP));
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP));
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_CANCEL));
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_OK);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_UNKNOWN));
    result = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateHotAreaType
 * @tc.desc: UpdateHotAreaType
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, UpdateHotAreaType, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    moveDragController->UpdateHotAreaType(pointerEvent);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerId(0);
    moveDragController->UpdateHotAreaType(pointerEvent);
    auto preWindowDragHotAreaType = moveDragController->windowDragHotAreaType_;
    moveDragController->windowDragHotAreaType_ = WINDOW_HOT_AREA_TYPE_UNDEFINED;
    moveDragController->UpdateHotAreaType(pointerEvent);
    moveDragController->windowDragHotAreaType_ = preWindowDragHotAreaType;
}

/**
 * @tc.name: OnLostFocus
 * @tc.desc: OnLostFocus
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, OnLostFocus, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    moveDragController->isStartMove_ = true;
    moveDragController->isStartDrag_ = false;
    moveDragController->OnLostFocus();
    moveDragController->isStartMove_ = false;
    moveDragController->isStartDrag_ = true;
    int windowHotAreaTypeOther = 1;
    moveDragController->windowDragHotAreaType_ = windowHotAreaTypeOther;
    moveDragController->OnLostFocus();
    moveDragController->windowDragHotAreaType_ = WINDOW_HOT_AREA_TYPE_UNDEFINED;
    moveDragController->OnLostFocus();
}

/**
 * @tc.name: NotifyWindowInputPidChange
 * @tc.desc: NotifyWindowInputPidChange
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, NotifyWindowInputPidChange, Function | SmallTest | Level1)
{
    bool isServerPid = true;
    auto preCallback = moveDragController->pidChangeCallback_;
    moveDragController->NotifyWindowInputPidChange(isServerPid);
    isServerPid = false;
    moveDragController->NotifyWindowInputPidChange(isServerPid);
    moveDragController->SetNotifyWindowPidChangeCallback(nullptr);
    ASSERT_EQ(moveDragController->pidChangeCallback_, nullptr);
    isServerPid = true;
    moveDragController->NotifyWindowInputPidChange(isServerPid);
    isServerPid = false;
    moveDragController->NotifyWindowInputPidChange(isServerPid);
    moveDragController->SetNotifyWindowPidChangeCallback(preCallback);
}

/**
 * @tc.name: HasPointDown
 * @tc.desc: HasPointDown
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, HasPointDown, Function | SmallTest | Level1)
{
    bool preHasPointDown = moveDragController->hasPointDown_;
    moveDragController->hasPointDown_ = true;
    bool res = moveDragController->HasPointDown();
    ASSERT_EQ(res, true);
    moveDragController->hasPointDown_ = false;
    res = moveDragController->HasPointDown();
    ASSERT_EQ(res, false);
    moveDragController->hasPointDown_ = preHasPointDown;
}

/**
 * @tc.name: ProcessSessionRectChange
 * @tc.desc: ProcessSessionRectChange
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, ProcessSessionRectChange, Function | SmallTest | Level1)
{
    int32_t res = 0;
    auto preCallback = moveDragController->moveDragCallback_;
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    MoveDragCallback callBack = [](const SizeChangeReason& reason) {
            return;
        };
    moveDragController->moveDragCallback_ = callBack;
    moveDragController->ProcessSessionRectChange(reason);
    moveDragController->moveDragCallback_ = nullptr;
    moveDragController->ProcessSessionRectChange(reason);
    moveDragController->moveDragCallback_ = preCallback;
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: GetOriginalPointerPosX
 * @tc.desc: GetOriginalPointerPosX
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, GetOriginalPointerPosX, Function | SmallTest | Level1)
{
    int32_t posX = moveDragController->moveDragProperty_.originalPointerPosX_;
    int32_t res = moveDragController->GetOriginalPointerPosX();
    ASSERT_EQ(posX, res);
}

/**
 * @tc.name: GetOriginalPointerPosY
 * @tc.desc: GetOriginalPointerPosY
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, GetOriginalPointerPosY, Function | SmallTest | Level1)
{
    int32_t posY = moveDragController->moveDragProperty_.originalPointerPosY_;
    int32_t res = moveDragController->GetOriginalPointerPosY();
    ASSERT_EQ(posY, res);
}
}
}
}
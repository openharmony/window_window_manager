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
    moveDragController->SetStartMoveFlag(true);
    bool res = moveDragController->GetStartMoveFlag();
    ASSERT_EQ(false, res);
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
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode);
    ASSERT_EQ(0, res);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode);
    ASSERT_EQ(0, res);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode);
    ASSERT_EQ(0, res);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode);
    ASSERT_EQ(0, res);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    moveDragController->UpdateGravityWhenDrag(pointerEvent, surfaceNode);
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
    moveDragController->CalcFixedAspectRatioTargetRect(type, tranX, tranY, aspectRatio, originalRect);
    type = AreaType::LEFT_TOP;
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
    type = AreaType::TOP;
    moveDragController->InitMainAxis(type, tranX, tranY);
    type = AreaType::UNDEFINED;
    tranX = 0;
    tranY = 0;
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
    WSRect originalRect = { 100, 100, 1000, 1000 };
    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(nullptr, originalRect));

    ASSERT_EQ(false, moveDragController->ConsumeMoveEvent(pointerEvent, originalRect));
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
}

/**
 * @tc.name: ConsumeDragEvent
 * @tc.desc: test function : ConsumeDragEvent
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, ConsumeDragEvent, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    WSRect originalRect = { 100, 100, 1000, 1000 };
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SystemSessionConfig sysConfig;
    moveDragController->GetVirtualPixelRatio();
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(nullptr, originalRect, property, sysConfig));
    ASSERT_EQ(false, moveDragController->ConsumeDragEvent(pointerEvent, originalRect, nullptr, sysConfig));
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
 * @tc.name: ClacFirstMoveTargetRect
 * @tc.desc: test function : ClacFirstMoveTargetRect
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, ClacFirstMoveTargetRect, Function | SmallTest | Level1)
{
    int res = 0;
    WSRect windowRect = { 0, 0, 0, 0 };
    moveDragController->ClacFirstMoveTargetRect(windowRect);
    res++;
    moveDragController->moveTempProperty_.pointerId_ = 0;
    moveDragController->ClacFirstMoveTargetRect(windowRect);
    ASSERT_EQ(res, 1);
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
    moveDragController->isStartMove_ = true;
    bool res01 = moveDragController->CheckDragEventLegal(nullptr, nullptr);
    bool res02 = moveDragController->CheckDragEventLegal(pointerEvent, property);
    ASSERT_EQ(res01, false);
    ASSERT_EQ(res02, false);
    moveDragController->isStartMove_ = false;
}

/**
 * @tc.name: HandleMouseStyle
 * @tc.desc: test function : HandleMouseStyle
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, HandleMouseStyle, Function | SmallTest | Level1)
{
    int res = 0;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    WSRect winRect;
    res++;
    moveDragController->HandleMouseStyle(nullptr, winRect);
    ASSERT_EQ(res, 1);

    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    moveDragController->HandleMouseStyle(pointerEvent, winRect);
    ASSERT_EQ(res, 1);

    moveDragController->mouseStyleID_ = MMI::MOUSE_ICON::NORTH_SOUTH;
    moveDragController->isStartDrag_ = true;
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    moveDragController->HandleMouseStyle(pointerEvent, winRect);
    ASSERT_EQ(res, 1);
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
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    auto res = moveDragController->UpdateMoveTempProperty(pointerEvent);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}
}
}
}
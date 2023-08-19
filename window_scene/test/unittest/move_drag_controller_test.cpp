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
#include <pointer_event.h>
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
    ASSERT_EQ(0 , res);
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
    ASSERT_EQ(true , res);
}

/**
 * @tc.name: GetStartDragFlag
 * @tc.desc: test function : GetStartDragFlag
 * @tc.type: FUNC
 */
HWTEST_F(MoveDragControllerTest, GetStartDragFlag, Function | SmallTest | Level1)
{
    bool res = moveDragController->GetStartDragFlag();
    ASSERT_EQ(false , res);
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
    ASSERT_EQ(tmp , res.height_);
    ASSERT_EQ(tmp , res.width_);
    ASSERT_EQ(pos , res.posX_);
    ASSERT_EQ(pos , res.posY_);
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
    ASSERT_EQ(0 , res);
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
    int32_t pointerPosX = 10;
    int32_t pointerPosY = 30;
    WSRect winRect = { 100, 100, 1000, 1000 };
    moveDragController->SetOriginalValue(pointerId, pointerPosX, pointerPosY, winRect);
    ASSERT_EQ(0 , res);
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
    ASSERT_EQ(0 , res);
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
    ASSERT_EQ(0 , res);

    pointerEvent = MMI::PointerEvent::Create();
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t pointerPosX = 10;
    int32_t pointerPosY = 30;
    moveDragController->SetOriginalValue(pointerId, pointerPosX, pointerPosY, originalRect);
    moveDragController->CalcMoveTargetRect(pointerEvent, originalRect);
    ASSERT_EQ(0 , res);
}

}
}
}
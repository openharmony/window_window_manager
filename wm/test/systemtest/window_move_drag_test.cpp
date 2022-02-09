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

// gtest
#include <gtest/gtest.h>
#include "pointer_event.h"
#include "window_helper.h"
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowMoveDrag"};
    constexpr float POINT_HOTZONE_RATIO = 0.5;
    constexpr float DRAG_HOTZONE_RATIO = 0.6;
}
using utils = WindowTestUtils;
class WindowMoveDragTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

private:
    std::shared_ptr<MMI::PointerEvent> CreatePointerEvent(int32_t posX,
                                                          int32_t posY,
                                                          uint32_t pointerId,
                                                          int32_t pointerAction);
    void CalExpectRects();
    void DoMoveOrDrag();
    static inline std::vector<sptr<Window>> activeWindows_;
    static inline utils::TestWindowInfo winInfo_;
    static inline uint32_t pointerId_ = 0;
    static inline int32_t pointX_ = 0;
    static inline int32_t pointY_ = 0;
    static inline int32_t startPointX_ = 0;
    static inline int32_t startPointY_ = 0;
    static inline Rect startPointRect_  = {0, 0, 0, 0};
    static inline Rect expectRect_ = {0, 0, 0, 0};
    static inline sptr<Window> window_ = nullptr;
};

void WindowMoveDragTest::SetUpTestCase()
{
    startPointX_ = 0;
    startPointY_ = 0;
    pointX_ = 0;
    pointY_ = 0;
    startPointRect_    = {0, 0, 0, 0};
    expectRect_   = {0, 0, 0, 0};
    winInfo_.rect = {0, 0, 0, 0};
}

void WindowMoveDragTest::TearDownTestCase()
{
}

void WindowMoveDragTest::SetUp()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        WLOGFE("GetDefaultDisplay: failed!\n");
    } else {
        WLOGFI("GetDefaultDisplay: id %{public}llu, w %{public}d, h %{public}d, fps %{public}u\n",
               display->GetId(), display->GetWidth(), display->GetHeight(), display->GetFreshRate());
    }
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByDisplayRect(displayRect);

    winInfo_ = {
        .name = "Floating",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    window_ = utils::CreateTestWindow(winInfo_);
    ASSERT_TRUE((window_ != nullptr));
}

void WindowMoveDragTest::TearDown()
{
    ASSERT_EQ(WMError::WM_OK, window_->Destroy());
}

std::shared_ptr<MMI::PointerEvent> WindowMoveDragTest::CreatePointerEvent(int32_t posX,
                                                                          int32_t posY,
                                                                          uint32_t pointerId,
                                                                          int32_t pointerAction)
{
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(pointerId);
    pointerItem.SetGlobalX(posX);
    pointerItem.SetGlobalY(posY);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetPointerAction(pointerAction);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    return pointerEvent;
}

void WindowMoveDragTest::DoMoveOrDrag()
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent =
        CreatePointerEvent(startPointX_, startPointY_, pointerId_++, MMI::PointerEvent::POINTER_ACTION_DOWN);
    window_->ConsumePointerEvent(pointerEvent);
    ASSERT_TRUE(utils::RectEqualToRect(window_->GetRect(), startPointRect_));

    pointerEvent = CreatePointerEvent(pointX_, pointY_, pointerId_++, MMI::PointerEvent::POINTER_ACTION_MOVE);
    window_->ConsumePointerEvent(pointerEvent);
    CalExpectRects();
    ASSERT_TRUE(utils::RectEqualToRect(window_->GetRect(), expectRect_));

    pointerEvent = CreatePointerEvent(pointX_, pointY_, pointerId_++, MMI::PointerEvent::POINTER_ACTION_UP);
    window_->ConsumePointerEvent(pointerEvent);
    ASSERT_TRUE(utils::RectEqualToRect(window_->GetRect(), expectRect_));
}

void WindowMoveDragTest::CalExpectRects()
{
    bool isPointInWindow;
    if ((startPointX_ > startPointRect_.posX_) &&
        (startPointX_ < static_cast<int32_t>(startPointRect_.posX_ + startPointRect_.width_)) &&
        (startPointY_ > startPointRect_.posY_) &&
        (startPointY_ < static_cast<int32_t>(startPointRect_.posY_ + startPointRect_.height_))) {
        isPointInWindow = true;
    } else {
        isPointInWindow = false;
    }

    int32_t diffX = pointX_ - startPointX_;
    int32_t diffY = pointY_ - startPointY_;
    Rect oriRect = startPointRect_;
    if (!isPointInWindow) {
        if (startPointX_ <= startPointRect_.posX_) {
            oriRect.posX_ += diffX;
            oriRect.width_ -= diffX;
        } else if (startPointX_ >= static_cast<int32_t>(startPointRect_.posX_ + startPointRect_.width_)) {
            oriRect.width_ += diffX;
        }

        if (startPointY_ <= startPointRect_.posY_) {
            oriRect.posY_   += diffY;
            oriRect.height_ -= diffY;
        } else if (startPointY_ >= static_cast<int32_t>(startPointRect_.posY_ + startPointRect_.height_)) {
            oriRect.height_ += diffY;
        }
    }
    bool isVertical = (utils::displayRect_.width_ < utils::displayRect_.height_) ? true : false;
    expectRect_ = WindowHelper::GetFixedWindowRectByMinRect(oriRect, startPointRect_, isVertical);
}

namespace {
/**
 * @tc.name: DragWindow01
 * @tc.desc: drag left
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow01, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ - HOTZONE * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ + startPointRect_.height_ * POINT_HOTZONE_RATIO;
    pointX_ = startPointRect_.posX_ + HOTZONE * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ + HOTZONE * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow02
 * @tc.desc: drag left top
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow02, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ - HOTZONE * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ - HOTZONE * POINT_HOTZONE_RATIO;

    pointX_ = startPointRect_.posX_ + HOTZONE * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ + HOTZONE * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow03
 * @tc.desc: drag left bottom
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow03, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ - HOTZONE * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ + startPointRect_.height_ + HOTZONE * POINT_HOTZONE_RATIO;

    pointX_ = startPointRect_.posX_ + HOTZONE * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ + startPointRect_.height_ + HOTZONE * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow04
 * @tc.desc: drag right
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow04, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + startPointRect_.width_ + HOTZONE * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ + startPointRect_.height_ * POINT_HOTZONE_RATIO;

    pointX_ = startPointRect_.posX_ + startPointRect_.width_ + HOTZONE * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ + startPointRect_.height_ * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow05
 * @tc.desc: drag right top
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow05, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + startPointRect_.width_ + HOTZONE * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ - HOTZONE * POINT_HOTZONE_RATIO;

    pointX_ = startPointRect_.posX_ + startPointRect_.width_ + HOTZONE * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ + HOTZONE * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow06
 * @tc.desc: drag right bottom
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow06, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + startPointRect_.width_ + HOTZONE * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ + startPointRect_.height_ + HOTZONE * POINT_HOTZONE_RATIO;

    pointX_ = startPointRect_.posX_ + startPointRect_.width_ + HOTZONE * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ + startPointRect_.height_ + HOTZONE * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow07
 * @tc.desc: drag top
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow07, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + startPointRect_.width_ * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ - HOTZONE * POINT_HOTZONE_RATIO;

    pointX_ = startPointRect_.posX_ + startPointRect_.width_ * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ - HOTZONE * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow08
 * @tc.desc: drag bottom
 * @tc.type: FUNC
 * @tc.require: AR000GGTV8
 */
HWTEST_F(WindowMoveDragTest, DragWindow08, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + startPointRect_.width_ * POINT_HOTZONE_RATIO;
    startPointY_ = startPointRect_.posY_ + startPointRect_.height_ + HOTZONE * POINT_HOTZONE_RATIO;

    pointX_ = startPointRect_.posX_ + startPointRect_.width_ * DRAG_HOTZONE_RATIO;
    pointY_ = startPointRect_.posY_ + startPointRect_.height_ + HOTZONE * DRAG_HOTZONE_RATIO;
    DoMoveOrDrag();
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}
}
} // namespace Rosen
} // namespace OHOS

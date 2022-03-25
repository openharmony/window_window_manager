/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "pointer_event.h"
#include "window_helper.h"
#include "window_impl.h"
#include "window_test_utils.h"
#include "wm_common_inner.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowMoveDragTest"};
    constexpr float POINT_HOTZONE_RATIO = 0.5;
    constexpr float DRAG_HOTZONE_RATIO = 0.6;
    constexpr int WAIT_SYANC_MS = 100000;
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
    void DoMoveOrDrag(bool isMove, bool isDrag);
    static inline std::vector<sptr<Window>> activeWindows_;
    static inline uint32_t pointerId_ = 0;
    static inline int32_t pointX_ = 0;
    static inline int32_t pointY_ = 0;
    static inline int32_t startPointX_ = 0;
    static inline int32_t startPointY_ = 0;
    static inline bool hasStartMove_ = false;
    static inline Rect startPointRect_  = {0, 0, 0, 0};
    static inline Rect expectRect_ = {0, 0, 0, 0};
    static inline sptr<WindowImpl> window_ = nullptr;
    static inline float virtualPixelRatio_ = 0.0;
    static inline uint32_t hotZone_ = 0;
};

void WindowMoveDragTest::SetUpTestCase()
{
    startPointX_ = 0;
    startPointY_ = 0;
    pointX_ = 0;
    pointY_ = 0;
    startPointRect_ = {0, 0, 0, 0};
    expectRect_     = {0, 0, 0, 0};
    usleep(WAIT_SYANC_MS);
}

void WindowMoveDragTest::TearDownTestCase()
{
}

void WindowMoveDragTest::SetUp()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}llu, w %{public}d, h %{public}d, fps %{public}u\n",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByDisplayRect(displayRect);

    virtualPixelRatio_ = WindowTestUtils::GetVirtualPixelRatio(0);
    hotZone_ = static_cast<uint32_t>(HOTZONE * virtualPixelRatio_);

    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowMoveDragTest");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window_ = new WindowImpl(option);
    window_->Create("");
    usleep(WAIT_SYANC_MS);
    ASSERT_TRUE((window_ != nullptr));
}

void WindowMoveDragTest::TearDown()
{
    ASSERT_EQ(WMError::WM_OK, window_->Destroy());
    usleep(WAIT_SYANC_MS);
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

void WindowMoveDragTest::DoMoveOrDrag(bool isMove, bool isDrag)
{
    pointerId_++;
    std::shared_ptr<MMI::PointerEvent> pointerEvent =
        CreatePointerEvent(startPointX_, startPointY_, pointerId_, MMI::PointerEvent::POINTER_ACTION_DOWN);
    window_->ConsumePointerEvent(pointerEvent);
    ASSERT_TRUE(utils::RectEqualToRect(window_->GetRect(), startPointRect_));

    pointerEvent = CreatePointerEvent(pointX_, pointY_, pointerId_, MMI::PointerEvent::POINTER_ACTION_MOVE);
    window_->ConsumePointerEvent(pointerEvent);
    CalExpectRects();
    usleep(WAIT_SYANC_MS);
    ASSERT_TRUE(utils::RectEqualToRect(window_->GetRect(), expectRect_));
    ASSERT_EQ(isMove, window_->startMoveFlag_);
    ASSERT_EQ(isDrag, window_->startDragFlag_);

    pointerEvent = CreatePointerEvent(pointX_, pointY_, pointerId_, MMI::PointerEvent::POINTER_ACTION_UP);
    window_->ConsumePointerEvent(pointerEvent);
    ASSERT_TRUE(utils::RectEqualToRect(window_->GetRect(), expectRect_));
}

void WindowMoveDragTest::CalExpectRects()
{
    Rect startRectExceptFrame;
    // calculate window inner rect except frame
    startRectExceptFrame.posX_ = startPointRect_.posX_ +
        static_cast<int32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio_);
    startRectExceptFrame.posY_ = startPointRect_.posY_ +
        static_cast<int32_t>(WINDOW_FRAME_TOP_WIDTH * virtualPixelRatio_);
    startRectExceptFrame.width_ = startPointRect_.width_ -
        static_cast<uint32_t>((WINDOW_FRAME_WIDTH + WINDOW_FRAME_WIDTH) * virtualPixelRatio_);
    startRectExceptFrame.height_ = startPointRect_.height_ -
        static_cast<uint32_t>((WINDOW_FRAME_TOP_WIDTH + WINDOW_FRAME_WIDTH) * virtualPixelRatio_);

    bool isPointInWindow = WindowHelper::IsPointInTargetRect(startPointX_, startPointY_, startRectExceptFrame);
    int32_t diffX = pointX_ - startPointX_;
    int32_t diffY = pointY_ - startPointY_;
    Rect oriRect = startPointRect_;
    if (!isPointInWindow) {
        if (startPointX_ <= startRectExceptFrame.posX_) {
            oriRect.posX_ += diffX;
            oriRect.width_ -= diffX;
        } else if (startPointX_ >= startRectExceptFrame.posX_ + static_cast<int32_t>(startRectExceptFrame.width_)) {
            oriRect.width_ += diffX;
        }

        if (startPointY_ <= startRectExceptFrame.posY_) {
            oriRect.posY_   += diffY;
            oriRect.height_ -= diffY;
        } else if (startPointY_ >= startRectExceptFrame.posY_ + static_cast<int32_t>(startRectExceptFrame.height_)) {
            oriRect.height_ += diffY;
        }
    } else {
        if (hasStartMove_) {
            oriRect.posX_ += diffX;
            oriRect.posY_ += diffY;
            hasStartMove_ = false;
        }
    }
    bool isVertical = (utils::displayRect_.width_ < utils::displayRect_.height_) ? true : false;
    expectRect_ = WindowHelper::GetFixedWindowRectByLimitSize(oriRect, startPointRect_, isVertical, virtualPixelRatio_);
}

namespace {
/**
 * @tc.name: DragWindow01
 * @tc.desc: drag left
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow01, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ - static_cast<int32_t>(hotZone_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_ * POINT_HOTZONE_RATIO);
    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow02
 * @tc.desc: drag left top
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow02, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ - static_cast<int32_t>(hotZone_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ - static_cast<int32_t>(hotZone_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow03
 * @tc.desc: drag left bottom
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow03, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ - static_cast<int32_t>(hotZone_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ +
                   static_cast<int32_t>(startPointRect_.height_ + hotZone_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ +
              static_cast<int32_t>(startPointRect_.height_ + hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow04
 * @tc.desc: drag right
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow04, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ +
                   static_cast<int32_t>(startPointRect_.width_ + hotZone_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ + hotZone_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow05
 * @tc.desc: drag right top
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow05, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ +
                   static_cast<int32_t>(startPointRect_.width_ + hotZone_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ - static_cast<int32_t>(hotZone_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ + hotZone_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow06
 * @tc.desc: drag right bottom
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow06, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ +
                   static_cast<int32_t>(startPointRect_.width_ + hotZone_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ +
                   static_cast<int32_t>(startPointRect_.height_ + hotZone_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ +
              static_cast<int32_t>(startPointRect_.width_ + hotZone_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ +
              static_cast<int32_t>(startPointRect_.height_ + hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow07
 * @tc.desc: drag top
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow07, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ - static_cast<int32_t>(hotZone_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ - static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow08
 * @tc.desc: drag bottom
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow08, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ +
                   static_cast<int32_t>(startPointRect_.height_ + hotZone_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_ + hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, true);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow09
 * @tc.desc: point in decorZone, uiContent is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow09, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ +
        static_cast<int32_t>(WINDOW_TITLE_BAR_HEIGHT * POINT_HOTZONE_RATIO * virtualPixelRatio_);
    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_ + hotZone_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, false);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow10
 * @tc.desc: point in decorZone, start move
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow10, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ +
        static_cast<int32_t>(WINDOW_TITLE_BAR_HEIGHT * POINT_HOTZONE_RATIO * virtualPixelRatio_);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(hotZone_ * DRAG_HOTZONE_RATIO);
    window_->StartMove();
    hasStartMove_ = true;
    DoMoveOrDrag(true, false);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: DragWindow11
 * @tc.desc: drag inner
 * @tc.type: FUNC
 */
HWTEST_F(WindowMoveDragTest, DragWindow11, Function | MediumTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    usleep(WAIT_SYANC_MS);
    startPointRect_ = window_->GetRect();
    startPointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * POINT_HOTZONE_RATIO);
    startPointY_ = startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_ * POINT_HOTZONE_RATIO);

    pointX_ = startPointRect_.posX_ + static_cast<int32_t>(startPointRect_.width_ * DRAG_HOTZONE_RATIO);
    pointY_ = startPointRect_.posY_ + static_cast<int32_t>(startPointRect_.height_ * DRAG_HOTZONE_RATIO);
    DoMoveOrDrag(false, false);
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}
}
} // namespace Rosen
} // namespace OHOS

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
#include "window_test_utils.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
const int WAIT_CALLBACK_US = 1;  // 1s

class TestDragListener : public IWindowDragListener {
public:
    PointInfo point_ { 0, 0 };
    DragEvent event_ = DragEvent::DRAG_EVENT_END;
    void OnDrag(int32_t x, int32_t y, DragEvent event) override;
};

void TestDragListener::OnDrag(int32_t x, int32_t y, DragEvent event)
{
    event_ = event;
    point_.x = x;
    point_.y = y;
}

class WindowDragTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<TestDragListener> firstWindowDragListener_;
    static sptr<TestDragListener> secondWindowDragListener_;
    Utils::TestWindowInfo dragWindowInfo_;
    Utils::TestWindowInfo firstWindowInfo_;
    Utils::TestWindowInfo secondWindowInfo_;
    std::vector<sptr<Window>> activeWindows_;
};

sptr<TestDragListener> WindowDragTest::firstWindowDragListener_ =
    new TestDragListener();
sptr<TestDragListener> WindowDragTest::secondWindowDragListener_ =
    new TestDragListener();

void WindowDragTest::SetUpTestCase() {}

void WindowDragTest::TearDownTestCase() {}

void WindowDragTest::SetUp()
{
    dragWindowInfo_ = {
        .name = "dragWindow",
        .rect = {200, 200, 380, 380},
        .type = WindowType::WINDOW_TYPE_DRAGGING_EFFECT,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };

    firstWindowInfo_ = {
        .name = "firstWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };

    secondWindowInfo_ = {
        .name = "secondWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    activeWindows_.clear();
}

void WindowDragTest::TearDown() {
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
/**
 * @tc.name: DragIn
 * @tc.desc: Drag a window to another window
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragIn, Function | MediumTest | Level3) {
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    ASSERT_NE(firstWindow, nullptr);//
    activeWindows_.push_back(firstWindow);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    const sptr<Window> &dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    ASSERT_NE(dragWindow, nullptr);
    activeWindows_.push_back(dragWindow);
    dragWindow->Show();
    dragWindow->MoveTo(300, 300);
    sleep(WAIT_CALLBACK_US);

    ASSERT_EQ(300, firstWindowDragListener_->point_.x);
    ASSERT_EQ(300, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, firstWindowDragListener_->event_);

    firstWindow->UnregisterDragListener(firstWindowDragListener_);
}

/**
 * @tc.name: DragMove
 * @tc.desc: Window Move
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragMove, Function | MediumTest | Level3) {
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    if (firstWindow == nullptr) {
        return;
    } 
    ASSERT_NE(firstWindow, nullptr);
    activeWindows_.push_back(firstWindow);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    const sptr<Window> &dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    ASSERT_NE(dragWindow, nullptr);
    activeWindows_.push_back(dragWindow);
    dragWindow->Show();
    dragWindow->MoveTo(300, 300);

    sleep(WAIT_CALLBACK_US);
    ASSERT_EQ(300, firstWindowDragListener_->point_.x);
    ASSERT_EQ(300, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, firstWindowDragListener_->event_);

    dragWindow->MoveTo(400, 400);
    sleep(WAIT_CALLBACK_US);
    ASSERT_EQ(400, firstWindowDragListener_->point_.x);
    ASSERT_EQ(400, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_MOVE, firstWindowDragListener_->event_);

    firstWindow->UnregisterDragListener(firstWindowDragListener_);
}

/**
 * @tc.name: DragOut
 * @tc.desc: Drag the drag window out of the current window
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragOut, Function | MediumTest | Level3) {
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    if (firstWindow == nullptr) {
        return;
    } 
    ASSERT_NE(firstWindow, nullptr);
    activeWindows_.push_back(firstWindow);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    secondWindowInfo_.rect = {500, 500, 500, 500};
    const sptr<Window> &secondWindow = Utils::CreateTestWindow(secondWindowInfo_);
    ASSERT_NE(secondWindow, nullptr);
    activeWindows_.push_back(secondWindow);
    secondWindow->RegisterDragListener(secondWindowDragListener_);
    secondWindow->Show();

    const sptr<Window> &dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    ASSERT_NE(secondWindow, nullptr);
    activeWindows_.push_back(dragWindow);
    dragWindow->Show();
    dragWindow->MoveTo(300, 300);

    sleep(WAIT_CALLBACK_US);
    ASSERT_EQ(300, firstWindowDragListener_->point_.x);
    ASSERT_EQ(300, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, firstWindowDragListener_->event_);

    dragWindow->MoveTo(400, 400);
    sleep(WAIT_CALLBACK_US);
    ASSERT_EQ(400, firstWindowDragListener_->point_.x);
    ASSERT_EQ(400, firstWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_MOVE, firstWindowDragListener_->event_);

    dragWindow->MoveTo(600, 600);
    sleep(WAIT_CALLBACK_US);
    ASSERT_EQ(100, secondWindowDragListener_->point_.x);
    ASSERT_EQ(100, secondWindowDragListener_->point_.y);
    ASSERT_EQ(DragEvent::DRAG_EVENT_IN, secondWindowDragListener_->event_);
    ASSERT_EQ(DragEvent::DRAG_EVENT_OUT, firstWindowDragListener_->event_);

    firstWindow->UnregisterDragListener(firstWindowDragListener_);
    secondWindow->UnregisterDragListener(secondWindowDragListener_);
}

/**
 * @tc.name: DragEnd
 * @tc.desc: End window drag
 * @tc.type: FUNC
 */
HWTEST_F(WindowDragTest, DragEnd, Function | MediumTest | Level3) {
    const sptr<Window> firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    if (firstWindow == nullptr) {
        return;
    } 
    ASSERT_NE(nullptr, firstWindow);
    firstWindow->RegisterDragListener(firstWindowDragListener_);
    firstWindow->SetTurnScreenOn(true);
    firstWindow->Show();

    const sptr<Window> dragWindow = Utils::CreateTestWindow(dragWindowInfo_);
    ASSERT_NE(nullptr, dragWindow);
    dragWindow->Show();
    dragWindow->MoveTo(199, 199);

    sleep(WAIT_CALLBACK_US);
    dragWindow->Destroy();
    sleep(WAIT_CALLBACK_US);
    ASSERT_EQ(DragEvent::DRAG_EVENT_END, firstWindowDragListener_->event_);

    firstWindow->UnregisterDragListener(firstWindowDragListener_);
    firstWindow->Destroy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS

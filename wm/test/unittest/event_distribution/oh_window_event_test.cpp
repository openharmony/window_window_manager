/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "oh_window.h"

struct Input_TouchEvent {
    int32_t action;
    int32_t id;
    int32_t displayX;
    int32_t displayY;
    int64_t actionTime { -1 };
    int32_t windowId { -1 };
    int32_t displayId { -1 };
};

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHWindowEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void OHWindowEventTest::SetUpTestCase()
{
}

void OHWindowEventTest::TearDownTestCase()
{
}

void OHWindowEventTest::SetUp()
{
}

void OHWindowEventTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OH_WindowManager_InjectTouchEvent
 * @tc.desc: OH_WindowManager_InjectTouchEvent test
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowEventTest, OH_WindowManager_InjectTouchEvent, TestSize.Level0)
{
    int32_t windowId = -1;
    int32_t windowX = 0;
    int32_t windowY = 0;
    auto ret = OH_WindowManager_InjectTouchEvent(windowId, nullptr, windowX, windowY);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM), ret);
    Input_TouchEvent* touchEvent = OH_Input_CreateTouchEvent();
    ASSERT_NE(nullptr, touchEvent);
    touchEvent->actionTime = 100;
    touchEvent->id = 1;
    touchEvent->action = static_cast<Input_TouchEventAction>(10);
    touchEvent->displayX = 100;
    touchEvent->displayY = 200;
    touchEvent->windowId = -1;
    ret = OH_WindowManager_InjectTouchEvent(windowId, touchEvent, windowX, windowY);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM), ret);
    windowId = 1;
    ret = OH_WindowManager_InjectTouchEvent(windowId, touchEvent, windowX, windowY);
    EXPECT_EQ(OH_Input_GetTouchEventWindowId(touchEvent), windowId);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::OK), ret);
    OH_Input_SetTouchEventWindowId(touchEvent, 2);
    ret = OH_WindowManager_InjectTouchEvent(windowId, touchEvent, windowX, windowY);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM), ret);
    OH_Input_SetTouchEventWindowId(touchEvent, windowId);
    ret = OH_WindowManager_InjectTouchEvent(windowId, touchEvent, windowX, windowY);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::OK), ret);
    OH_Input_DestroyTouchEvent(&touchEvent);
}

/**
 * @tc.name: OH_WindowManager_LockCursor
 * @tc.desc: OH_WindowManager_LockCursor test
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowEventTest, OH_WindowManager_LockCursor, TestSize.Level0)
{
    int32_t windowId = 1;
    // Failed to obtain null window
    auto ret = OH_WindowManager_LockCursor(windowId, true);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL), ret);
}

/**
 * @tc.name: OH_WindowManager_UnlockCursor
 * @tc.desc: OH_WindowManager_UnlockCursor test
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowEventTest, OH_WindowManager_UnlockCursor, TestSize.Level0)
{
    int32_t windowId = 1;
    // Failed to obtain null window
    auto ret = OH_WindowManager_UnlockCursor(windowId);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL), ret);
}
}
} // namespace Rosen
} // namespace OHOS
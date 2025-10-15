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

#include <configuration.h>
#include <gtest/gtest.h>

// #include "ability_context_impl.h"
// #include "mock_static_call.h"
// #include "mock_session.h"
// #include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "oh_window.h"
// #include "singleton_mocker.h"
#include "window_impl.h"
#include "window_scene.h"
#include "window_session_impl.h"

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
using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
class OHWindowEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    // sptr<WindowScene> scene_ = nullptr;
    // std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};

void OHWindowTest::SetUpTestCase()
{
}

void OHWindowTest::TearDownTestCase()
{
#ifdef RS_ENABLE_VK
    RSModifiersDrawThread::Destroy();
#endif
}

void OHWindowTest::SetUp()
{
    // abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    // std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    // sptr<WindowOption> option = new WindowOption();
    // EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(option)));
    // DisplayId displayId = 0;
    // sptr<IWindowLifeCycle> listener = nullptr;
    // scene_ = sptr<WindowScene>::MakeSptr();
    // ASSERT_EQ(WMError::WM_OK, scene_->Init(displayId, abilityContext_, listener));
}

void OHWindowTest::TearDown()
{
    scene_->GoDestroy();
    scene_ = nullptr;
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: OH_WindowManager_InjectTouchEvent
 * @tc.desc: OH_WindowManager_InjectTouchEvent test
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowTest, OH_WindowManager_InjectTouchEvent, TestSize.Level0)
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
HWTEST_F(OHWindowTest, OH_WindowManager_LockCursor, TestSize.Level0)
{
    int32_t windowId = 1;
    // Failed to obtain null window
    auto ret = OH_WindowManager_LockCursor(windowId, true);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL), ret);

    // Failed to obtain eventHandler
    auto eventHandler = g_eventHandler;
    g_eventHandler = nullptr;
    ret = OH_WindowManager_LockCursor(windowId, true);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMAL), ret);
    g_eventHandler = eventHandler;

    // Window is not focused
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("OH_WindowManager_LockCursor");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(windowId);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->isFocused_ = false;
    WindowSessionImpl::windowSessionMap_.clear();
    WindowSessionImpl::windowSessionMap_.insert(std::make_pair(window->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ret = OH_WindowManager_LockCursor(windowId, true);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL), ret);

    // Normal
    window->isFocused_ = true;
    ret = OH_WindowManager_LockCursor(windowId, true);
    EXPECT_EQ(0, ret);
    WindowSessionImpl::windowSessionMap_.clear();
}
}
} // namespace Rosen
} // namespace OHOS
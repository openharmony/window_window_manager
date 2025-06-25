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

#include "ability_context_impl.h"
#include "mock_static_call.h"
#include "mock_session.h"
#include "oh_window.h"
#include "singleton_mocker.h"
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
class OHWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<WindowScene> scene_ = nullptr;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};

void OHWindowTest::SetUpTestCase()
{
}

void OHWindowTest::TearDownTestCase()
{
}

void OHWindowTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(option)));
    DisplayId displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    scene_ = sptr<WindowScene>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, scene_->Init(displayId, abilityContext_, listener));
}

void OHWindowTest::TearDown()
{
    scene_->GoDestroy();
    scene_ = nullptr;
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: ShowWindow01
 * @tc.desc: return OK when show window
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowTest, ShowWindow01, TestSize.Level1)
{
    ASSERT_NE(nullptr, scene_);
    ASSERT_NE(nullptr, scene_->GetMainWindow());
    auto ret = OH_WindowManager_ShowWindow(scene_->GetMainWindow()->GetWindowId());
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::OK), ret);
}

/**
 * @tc.name: IsWindowShown01
 * @tc.desc: return OK when window is shown
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowTest, IsWindowShowing01, TestSize.Level1)
{
    ASSERT_NE(nullptr, scene_);
    ASSERT_NE(nullptr, scene_->GetMainWindow());
    bool isShow;
    auto ret = OH_WindowManager_IsWindowShown(scene_->GetMainWindow()->GetWindowId(), &isShow);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::OK), ret);
}

/**
 * @tc.name: OH_WindowManager_GetAllWindowLayoutInfoList
 * @tc.desc: OH_WindowManager_GetAllWindowLayoutInfoList test
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowTest, OH_WindowManager_GetAllWindowLayoutInfoList, TestSize.Level0)
{
    int64_t displayId = -1;
    WindowManager_Rect** windowLayoutInfo = nullptr;
    size_t* windowLayoutInfoSize = nullptr;
    auto ret = OH_WindowManager_GetAllWindowLayoutInfoList(displayId, windowLayoutInfo, windowLayoutInfoSize);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM), ret);
    displayId = 0;
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM), ret);
    windowLayoutInfo = (WindowManager_Rect**)malloc(sizeof(WindowManager_Rect**));
    windowLayoutInfoSize = (size_t*)malloc(sizeof(size_t*));
    ret = OH_WindowManager_GetAllWindowLayoutInfoList(displayId, windowLayoutInfo, windowLayoutInfoSize);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WM_OK), ret);
    OH_WindowManager_ReleaseAllWindowLayoutInfoList(*windowLayoutInfo);
    *windowLayoutInfo = NULL;
    free(windowLayoutInfo);
    windowLayoutInfo = NULL;
    free(windowLayoutInfoSize);
    windowLayoutInfoSize = NULL;
}

/**
 * @tc.name: OH_WindowManager_InjectTouchEvent
 * @tc.desc: OH_WindowManager_InjectTouchEvent test
 * @tc.type: FUNC
 */
HWTEST_F(OHWindowTest, OH_WindowManager_InjectTouchEvent, TestSize.Level0)
{
    int32_t windowId = 1;
    auto ret = OH_WindowManager_InjectTouchEvent(windowId, nullptr);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM), ret);
    Input_TouchEvent touchEvent;
    touchEvent.actionTime = 100;
    touchEvent.id = 1;
    touchEvent.action = static_cast<Input_TouchEventAction>(10);
    touchEvent.displayX = 100;
    touchEvent.displayY = 200;
    touchEvent.windowId = 1;
    ret = OH_WindowManager_InjectTouchEvent(windowId, &touchEvent);
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("InjectTouchEvent");
    sptr<WindowImpl> window = sptr<WindowImpl>::MakeSptr(option);
    string winName = "test";
    WindowImpl::windowMap_.insert(std::make_pair(winName, std::pair<uint32_t, sptr<Window>>(windowId, window)));
    ret = OH_WindowManager_InjectTouchEvent(windowId, &touchEvent);
    touchEvent.action = Input_TouchEventAction::TOUCH_ACTION_DOWN;
    ret = OH_WindowManager_InjectTouchEvent(windowId, &touchEvent);
    EXPECT_EQ(static_cast<int32_t>(WindowManager_ErrorCode::OK), ret);
}
}
} // namespace Rosen
} // namespace OHOS
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

// gtest
#include <gtest/gtest.h>
#include "common_test_utils.h"
#include "scene_board_judgement.h"
#include "window.h"
#include "window_helper.h"
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

using Utils = WindowTestUtils;

class WindowAnimateToTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowAnimateToTest::SetUpTestCase() {}

void WindowAnimateToTest::TearDownTestCase() {}

void WindowAnimateToTest::SetUp()
{
    CommonTestUtils::GuaranteeFloatWindowPermission("wms_window_animation_transition_test");
}

void WindowAnimateToTest::TearDown() {}

namespace {
/**
 * @tc.name: AnimateTo
 * @tc.desc: Register AnimationController and hide with custom animation
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowAnimateToTest, AnimateTo, TestSize.Level1)
{
    WindowAnimationOption animationOption;
    WindowAnimationProperty animationProperty;
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT,
            WindowManager::GetInstance().AnimateTo(0, animationProperty, animationOption));
        return;
    }

    int32_t windowId = 0;
    FocusChangeInfo focusInfo;
    WindowManager::GetInstance().GetFocusWindowInfo(focusInfo);
    sleep(1);
    if (focusInfo.windowId_ == 0 || focusInfo.windowId_ == INVALID_WINDOW_ID) {
        // 功能特有测试，依赖存在前台获焦窗口，所以当没有获焦窗口ID时默认通过;
        ASSERT_EQ(windowId, 0);
        return;
    }
    animationOption.curve = WindowAnimationCurve::LINEAR;
    animationOption.duration = 100; // 100ms
    animationProperty.targetScale = 0.5f;
    if (WindowHelper::IsSystemWindow(focusInfo.windowType_)) {
        ASSERT_EQ(WMError::WM_DO_NOTHING,
            WindowManager::GetInstance().AnimateTo(focusInfo.windowId_, animationProperty, animationOption));
    } else {
        ASSERT_EQ(WMError::WM_OK,
            WindowManager::GetInstance().AnimateTo(focusInfo.windowId_, animationProperty, animationOption));
    }
}
} // namespace
} // namespace Rosen
} // namespace OHOS

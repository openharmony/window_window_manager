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

#include "session/host/include/scene_session.h"
#include "session/host/include/session.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class LayoutControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<LayoutController> layoutController_ = nullptr;
};

void LayoutControllerTest::SetUpTestCase() {}

void LayoutControllerTest::TearDownTestCase() {}

void LayoutControllerTest::SetUp()
{
    sptr<WindowSessionProperty> sessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    layoutController_ = sptr<LayoutController>::MakeSptr(sessionProperty);
}

void LayoutControllerTest::TearDown()
{
    layoutController_ = nullptr;
}

namespace {
/**
 * @tc.name: SetSessionGlobalRect
 * @tc.desc: SetSessionGlobalRect
 * @tc.type: FUNC
 */
HWTEST_F(LayoutControllerTest, SetSessionGlobalRect, TestSize.Level1)
{
    WSRect rect = { 500, 500, 800, 800 };
    layoutController_->globalRect_ = rect;
    EXPECT_EQ(layoutController_->SetSessionGlobalRect(rect), false);
    EXPECT_EQ(layoutController_->globalRect_, rect);

    layoutController_->globalRect_ = { 500, 500, 1000, 1000 };
    EXPECT_EQ(layoutController_->SetSessionGlobalRect(rect), true);
    EXPECT_EQ(layoutController_->globalRect_, rect);
}

/**
 * @tc.name: GetSessionGlobalRect
 * @tc.desc: GetSessionGlobalRect
 * @tc.type: FUNC
 */
HWTEST_F(LayoutControllerTest, GetSessionGlobalRect, TestSize.Level1)
{
    WSRect globalRect = { 500, 500, 800, 800 };
    WSRect winRect = { 600, 600, 800, 800 };
    layoutController_->globalRect_ = globalRect;
    layoutController_->winRect_ = winRect;

    Session::SetScbCoreEnabled(true);
    EXPECT_EQ(layoutController_->GetSessionGlobalRect(), globalRect);

    Session::SetScbCoreEnabled(false);
    EXPECT_EQ(layoutController_->GetSessionGlobalRect(), winRect);
}

/**
 * @tc.name: SetClientRect
 * @tc.desc: SetClientRect
 * @tc.type: FUNC
 */
HWTEST_F(LayoutControllerTest, SetClientRect, TestSize.Level1)
{
    WSRect clientRect = { 500, 500, 800, 800 };
    layoutController_->clientRect_ = clientRect;
    layoutController_->SetClientRect(clientRect);
    EXPECT_EQ(layoutController_->GetClientRect(), clientRect);

    WSRect clientRect2 = { 500, 500, 1000, 1000 };
    layoutController_->SetClientRect(clientRect2);
    EXPECT_NE(layoutController_->GetClientRect(), clientRect);
}

/**
 * @tc.name: ConvertRelativeRectToGlobal
 * @tc.desc: ConvertRelativeRectToGlobal
 * @tc.type: FUNC
 */
HWTEST_F(LayoutControllerTest, ConvertRelativeRectToGlobal, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "ConvertRelativeRectToGlobal";
    info.bundleName_ = "ConvertRelativeRectToGlobal";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    DisplayId defaultDisplayId = 0;
    DisplayId invalidDisplayId = -1ULL;
    WSRect relativeRect = { 500, 500, 800, 800 };
    sceneSession->GetSessionProperty()->SetDisplayId(invalidDisplayId);
    WSRect retRect = sceneSession->GetLayoutController()->ConvertRelativeRectToGlobal(relativeRect, invalidDisplayId);
    EXPECT_EQ(retRect, relativeRect);

    sceneSession->GetSessionProperty()->SetDisplayId(defaultDisplayId);
    retRect = sceneSession->GetLayoutController()->ConvertRelativeRectToGlobal(relativeRect, invalidDisplayId);
    EXPECT_EQ(retRect, relativeRect);
    retRect = sceneSession->GetLayoutController()->ConvertRelativeRectToGlobal(relativeRect, defaultDisplayId);
    EXPECT_EQ(retRect, relativeRect);
}

/**
 * @tc.name: ConvertGlobalRectToRelative
 * @tc.desc: ConvertGlobalRectToRelative
 * @tc.type: FUNC
 */
HWTEST_F(LayoutControllerTest, ConvertGlobalRectToRelative, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "ConvertGlobalRectToRelative";
    info.bundleName_ = "ConvertGlobalRectToRelative";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    DisplayId defaultDisplayId = 0;
    DisplayId invalidDisplayId = -1ULL;
    WSRect globalRect = { 500, 500, 800, 800 };
    sceneSession->GetSessionProperty()->SetDisplayId(invalidDisplayId);
    WSRect retRect = sceneSession->GetLayoutController()->ConvertGlobalRectToRelative(globalRect, invalidDisplayId);
    EXPECT_EQ(retRect, globalRect);

    sceneSession->GetSessionProperty()->SetDisplayId(defaultDisplayId);
    retRect = sceneSession->GetLayoutController()->ConvertGlobalRectToRelative(globalRect, invalidDisplayId);
    EXPECT_EQ(retRect, globalRect);
    retRect = sceneSession->GetLayoutController()->ConvertGlobalRectToRelative(globalRect, defaultDisplayId);
    EXPECT_EQ(retRect, globalRect);
}

/**
 * @tc.name: SetSystemConfigFunc
 * @tc.desc: SetSystemConfigFunc
 * @tc.type: FUNC
 */
HWTEST_F(LayoutControllerTest, SetSystemConfigFunc, TestSize.Level1)
{
    WSRect rect = { 500, 500, 800, 800 };
    layoutController_->SetSessionRect({ 1, 1, 1, 1 });
    layoutController_->SetSystemConfigFunc(nullptr);
    layoutController_->SetSystemConfigFunc([layoutController = this->layoutController_, rect] {
        layoutController->SetSessionRect(rect);
        SystemSessionConfig systemConfig;
        return systemConfig;
    });
    layoutController_->getSystemConfigFunc_();
    EXPECT_EQ(layoutController_->GetSessionRect(), rect);
}

// TODO: 补充 AdjustRectByAspectRatio 方法测试用例

// TODO: 补充 GetWindowLimits 方法测试用例
} // namespace
} // namespace Rosen
} // namespace OHOS

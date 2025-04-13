/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "ability_context_impl.h"
#include "accessibility_event_info.h"
#include "color_parser.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "window_helper.h"
#include "window_session_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 50000;
};

void WindowSessionImplLayoutTest::SetUpTestCase() {}

void WindowSessionImplLayoutTest::TearDownTestCase() {}

void WindowSessionImplLayoutTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSessionImplLayoutTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: UpdateRect01
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, UpdateRect01, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: UpdateRect01 start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateRect01");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);

    WSRect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.height_ = 50;
    rect.width_ = 50;

    Rect rectW; // GetRect().IsUninitializedRect is false
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 200; // rectW - rect > 50
    rectW.width_ = 200;  // rectW - rect > 50

    window->property_->SetWindowRect(rectW);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);

    rectW.height_ = 50;
    window->property_->SetWindowRect(rectW);
    res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);

    rectW.height_ = 200;
    rectW.width_ = 50;
    window->property_->SetWindowRect(rectW);
    res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
    Rect nowRect = window->property_->GetWindowRect();
    EXPECT_EQ(nowRect.posX_, rect.posX_);
    EXPECT_EQ(nowRect.posY_, rect.posY_);
    EXPECT_EQ(nowRect.width_, rect.width_);
    EXPECT_EQ(nowRect.height_, rect.height_);
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: UpdateRect01 end";
}

/**
 * @tc.name: UpdateRect02
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, UpdateRect02, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: UpdateRect02 start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateRect02");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);

    WSRect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.height_ = 0;
    rect.width_ = 0;

    Rect rectW; // GetRect().IsUninitializedRect is true
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 0; // rectW - rect > 50
    rectW.width_ = 0;  // rectW - rect > 50

    window->property_->SetWindowRect(rectW);
    SizeChangeReason reason = SizeChangeReason::ROTATION;
    WSError res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);

    rect.height_ = 50;
    rect.width_ = 50;
    rectW.height_ = 50;
    rectW.width_ = 50;
    window->property_->SetWindowRect(rectW);
    res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
    Rect nowRect = window->property_->GetWindowRect();
    EXPECT_EQ(nowRect.posX_, rect.posX_);
    EXPECT_EQ(nowRect.posY_, rect.posY_);
    EXPECT_EQ(nowRect.width_, rect.width_);
    EXPECT_EQ(nowRect.height_, rect.height_);
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: UpdateRect02 end";
}

/**
 * @tc.name: SetResizeByDragEnabled01
 * @tc.desc: SetResizeByDragEnabled and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, SetResizeByDragEnabled01, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetResizeByDragEnabled01");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    WMError retCode = window->SetResizeByDragEnabled(true);
    ASSERT_EQ(retCode, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: SetResizeByDragEnabled02
 * @tc.desc: SetResizeByDragEnabled and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, SetResizeByDragEnabled02, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetResizeByDragEnabled02");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    ASSERT_FALSE(window->IsWindowSessionInvalid());
    WMError retCode = window->SetResizeByDragEnabled(true);
    ASSERT_EQ(retCode, WMError::WM_OK);
    ASSERT_EQ(true, window->property_->GetDragEnabled());
}

/**
 * @tc.name: SetResizeByDragEnabled03
 * @tc.desc: SetResizeByDragEnabled and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, SetResizeByDragEnabled03, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetResizeByDragEnabled03");
    option->SetSubWindowDecorEnable(true);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    WMError retCode = window->SetResizeByDragEnabled(true);
    ASSERT_EQ(retCode, WMError::WM_OK);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    retCode = window->SetResizeByDragEnabled(true);
    ASSERT_EQ(retCode, WMError::WM_OK);

    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    retCode = window->SetResizeByDragEnabled(true);
    ASSERT_EQ(retCode, WMError::WM_ERROR_INVALID_TYPE);
}

/**
 * @tc.name: UpdateViewportConfig
 * @tc.desc: UpdateViewportConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, UpdateViewportConfig, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: UpdateViewportConfig start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);

    Rect rectW; // GetRect().IsUninitializedRect is true
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 0; // rectW - rect > 50
    rectW.width_ = 0;  // rectW - rect > 50

    window->virtualPixelRatio_ = -1.0;
    window->useUniqueDensity_ = true;
    WindowSizeChangeReason reason = WindowSizeChangeReason::UNDEFINED;
    window->UpdateViewportConfig(rectW, reason);
    ASSERT_EQ(window->virtualPixelRatio_, -1.0);

    window->virtualPixelRatio_ = -2.0;
    DisplayId displayId = 1;
    window->property_->SetDisplayId(displayId);
    window->UpdateViewportConfig(rectW, reason);
    ASSERT_EQ(window->virtualPixelRatio_, -2.0);

    displayId = 0;
    rectW.height_ = 500;
    rectW.width_ = 500;
    window->useUniqueDensity_ = false;
    window->property_->SetDisplayId(displayId);
    window->UpdateViewportConfig(rectW, reason);
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: UpdateViewportConfig end";
}

/**
 * @tc.name: UpdateViewportConfig01
 * @tc.desc: UpdateViewportConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, UpdateViewportConfig01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateViewportConfig01");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    Rect rectW;
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 0;
    rectW.width_ = 0;
    WindowSizeChangeReason reason = WindowSizeChangeReason::UNDEFINED;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    window->UpdateViewportConfig(rectW, reason, nullptr, displayInfo);
    rectW.width_ = 10;
    rectW.height_ = 0;
    window->UpdateViewportConfig(rectW, reason, nullptr, displayInfo);
    rectW.width_ = 10;
    rectW.height_ = 10;
    window->UpdateViewportConfig(rectW, reason, nullptr, displayInfo);
    ASSERT_NE(window, nullptr);
}

/**
 * @tc.name: NotifySingleHandTransformChange_TestUIContent
 * @tc.desc: NotifySingleHandTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, NotifySingleHandTransformChange_TestUIContent, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifySingleHandTransformChange_TestUIContent start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifySingleHandTransformChange_TestUIContent");
    option->SetIsUIExtFirstSubWindow(true);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(2025);
    std::string url = "";
    window->SetUIContentInner(
        url, nullptr, nullptr, WindowSetUIContentType::DEFAULT, BackupAndRestoreType::NONE, nullptr);
    SingleHandTransform testTransform;
    testTransform.posX = 100;
    window->NotifySingleHandTransformChange(testTransform);
    ASSERT_EQ(testTransform.posX, window->singleHandTransform_.posX);
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifySingleHandTransformChange_TestUIContent end";
}

/**
 * @tc.name: NotifyTransformChange_TestUIContent
 * @tc.desc: NotifyTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, NotifyTransformChange_TestUIContent, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifyTransformChange_TestUIContent start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifyTransformChange_TestUIContent");
    option->SetIsUIExtFirstSubWindow(true);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(2025);

    Transform testTransform;
    window->uiContent_ = nullptr;
    window->SetNeedRenotifyTransform(true);
    window->NotifyTransformChange(testTransform);
    ASSERT_EQ(true, window->IsNeedRenotifyTransform());

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->SetNeedRenotifyTransform(true);
    window->NotifyTransformChange(testTransform);
    ASSERT_EQ(false, window->IsNeedRenotifyTransform());
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifyTransformChange_TestUIContent end";
}

/**
 * @tc.name: NotifyAfterUIContentReady
 * @tc.desc: NotifyAfterUIContentReady
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, NotifyAfterUIContentReady, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifyAfterUIContentReady start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifyAfterUIContentReady");
    option->SetIsUIExtFirstSubWindow(true);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(2025);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->SetNeedRenotifyTransform(false);
    window->NotifyAfterUIContentReady();
    ASSERT_EQ(false, window->IsNeedRenotifyTransform());
    window->SetNeedRenotifyTransform(false);
    window->NotifyAfterUIContentReady();
    ASSERT_EQ(false, window->IsNeedRenotifyTransform());
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifyAfterUIContentReady end";
}
}
} // namespace Rosen
} // namespace OHOS

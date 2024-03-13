/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>
#include "picture_in_picture_controller.h"
#include "picture_in_picture_manager.h"
#include "window.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockWindow : public Window {
public:
    MockWindow() {};
    ~MockWindow() {};
    MOCK_METHOD0(Destroy, WMError());
};

class PictureInPictureControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PictureInPictureControllerTest::SetUpTestCase()
{
}

void PictureInPictureControllerTest::TearDownTestCase()
{
}

void PictureInPictureControllerTest::SetUp()
{
}

void PictureInPictureControllerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: ShowPictureInPictureWindow01
 * @tc.desc: ShowPictureInPictureWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, ShowPictureInPictureWindow01, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new PipOption();
    PictureInPictureController* pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
    pipControl->window_ = mw;
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
}

/**
 * @tc.name: StopPictureInPicture01
 * @tc.desc: StopPictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StopPictureInPicture01, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    ASSERT_EQ(PipWindowState::STATE_UNDEFINED, pipControl->GetControllerState());
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY,
        pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    pipControl->window_ = mw;
    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_ERROR_PIP_DESTROY_FAILED,
        pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    ASSERT_EQ(PipWindowState::STATE_UNDEFINED, pipControl->GetControllerState());
    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    ASSERT_EQ(PipWindowState::STATE_STOPPED, pipControl->GetControllerState());
}

/**
 * @tc.name: CreatePictureInPictureWindow
 * @tc.desc: CreatePictureInPictureWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, CreatePictureInPictureWindow, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    option = nullptr;
    sptr<WindowOption> windowOption = nullptr;

    EXPECT_EQ(nullptr, windowOption);
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow());
    ASSERT_NE(WMError::WM_OK, pipControl->CreatePictureInPictureWindow());
}

/**
 * @tc.name: StartPictureInPicture
 * @tc.desc: StartPictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartPictureInPicture, Function | SmallTest | Level2)
{
    StartPipType startType = StartPipType::AUTO_START;
    sptr<PipOption> pipOption_;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    pipOption_ = nullptr;
    sptr<Window> mainWindow_ = nullptr;

    EXPECT_EQ(true, pipControl->IsPullPiPAndHandleNavigation());
    ASSERT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
}

/**
 * @tc.name: StartPictureInPictureInner
 * @tc.desc: StartPictureInPictureInner
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartPictureInPictureInner, Function | SmallTest | Level2)
{
    StartPipType startType = StartPipType::AUTO_START;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    ASSERT_NE(WMError::WM_OK, pipControl->StartPictureInPictureInner(startType));
}

/**
 * @tc.name: GetPipWindow
 * @tc.desc: GetPipWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, GetPipWindow, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<Window> window_;
    uint32_t mainWindowId_ = 0;
    sptr<Window> window;
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    pipControl->SetPipWindow(window);
    auto ret = pipControl->GetPipWindow();
    ASSERT_EQ(window_, ret);
    auto ret1 = pipControl->GetMainWindowId();
    ASSERT_NE(mainWindowId_, ret1);
}

/**
 * @tc.name: SetAutoStartEnabled
 * @tc.desc: SetAutoStartEnabled
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, SetAutoStartEnabled, Function | SmallTest | Level2)
{
    bool enable = true;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    pipControl->SetAutoStartEnabled(enable);
    ASSERT_NE(WMError::WM_OK, pipControl->CreatePictureInPictureWindow());
}

/**
 * @tc.name: IsAutoStartEnabled
 * @tc.desc: IsAutoStartEnabled
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, IsAutoStartEnabled, Function | SmallTest | Level2)
{
    bool enable = true;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    pipControl->IsAutoStartEnabled(enable);
    auto ret = pipControl->GetControllerState();
    ASSERT_EQ(PipWindowState::STATE_UNDEFINED, ret);
}

/**
 * @tc.name: UpdateContentSize
 * @tc.desc: UpdateContentSize
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdateContentSize, Function | SmallTest | Level2)
{
    int32_t width = 0;
    int32_t height = 0;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    pipControl->UpdateContentSize(width, height);
    ASSERT_NE(WMError::WM_OK, pipControl->CreatePictureInPictureWindow());
}

/**
 * @tc.name: StartMove
 * @tc.desc: StartMove
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartMove, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    int32_t type = 0;
    std::string navigationId = " ";
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    GTEST_LOG_(INFO) << "TearDownCasecccccc";

    pipControl->StartMove();
    pipControl->DoScale();

    sptr<PictureInPictureController::PipMainWindowLifeCycleImpl> pipMainWindowLifeCycleImpl =
        new PictureInPictureController::PipMainWindowLifeCycleImpl(navigationId);
    GTEST_LOG_(INFO) << "TearDownCasecccccc3";

    pipMainWindowLifeCycleImpl->AfterBackground();
    pipMainWindowLifeCycleImpl->BackgroundFailed(type);

    ASSERT_NE(WMError::WM_OK, pipControl->CreatePictureInPictureWindow());
}

/**
 * @tc.name: DoActionEvent
 * @tc.desc: DoActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, DoActionEvent, Function | SmallTest | Level2)
{
    std::string actionName = " ";
    int32_t status = 0;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    pipControl->DoActionEvent(actionName, status);
    pipControl->RestorePictureInPictureWindow();
    GTEST_LOG_(INFO) << "TearDownCasecccccc5";
    pipControl->ResetExtController();
    ASSERT_NE(WMError::WM_OK, pipControl->CreatePictureInPictureWindow());
}

/**
 * @tc.name: SetXComponentController
 * @tc.desc: SetXComponentController
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, SetXComponentController, Function | SmallTest | Level2)
{
    sptr<IPiPLifeCycle> listener = nullptr;
    sptr<IPiPActionObserver> listener1 = nullptr;
    std::shared_ptr<XComponentController> xComponentController = nullptr;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    auto ret = pipControl->SetXComponentController(xComponentController);
    pipControl->SetPictureInPictureLifecycle(listener);
    pipControl->SetPictureInPictureActionObserver(listener1);
    pipControl->GetPictureInPictureLifecycle();
    pipControl->GetPictureInPictureActionObserver();
    pipControl->GetPiPNavigationId();
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);
    auto ret1 = pipControl->IsPullPiPAndHandleNavigation();
    ASSERT_EQ(true, ret1);
}
}
}
}
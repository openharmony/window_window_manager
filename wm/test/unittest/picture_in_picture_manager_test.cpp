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
#include "picture_in_picture_manager.h"
#include "singleton_container.h"
#include "window_scene_session_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PictureInPictureManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PictureInPictureManagerTest::SetUpTestCase()
{
}

void PictureInPictureManagerTest::TearDownTestCase()
{
}

void PictureInPictureManagerTest::SetUp()
{
}

void PictureInPictureManagerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: PiPWindowState
 * @tc.desc: PutPipControllerInfo/RemovePipControllerInfo/ReportPiPStartWindow/ReportPiPStopWindow/ReportPiPActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, PipControllerInfo, Function | SmallTest | Level2)
{
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipController = new PictureInPictureController(option, nullptr, 100, nullptr);
    PictureInPictureManager::PutPipControllerInfo(100, pipController);
    ASSERT_EQ(1, static_cast<int>(PictureInPictureManager::windowToControllerMap_.size()));
    PictureInPictureManager::RemovePipControllerInfo(100);
    ASSERT_EQ(0, static_cast<int>(PictureInPictureManager::windowToControllerMap_.size()));
    int32_t source = 0;
    std::string errorReason = "";
    SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(source, 1, 1, errorReason);
    SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(source, 1, 1, errorReason);
    source = 1;
    SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(source, 1, 1, errorReason);
    SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(source, 1, 1, errorReason);
    std::string actionEvent = "";
    SingletonContainer::Get<PiPReporter>().ReportPiPActionEvent(1, actionEvent);
    actionEvent = "nextVideo";
    SingletonContainer::Get<PiPReporter>().ReportPiPActionEvent(1, actionEvent);
}

/**
 * @tc.name: PictureInPictureController
 * @tc.desc: SetActiveController/IsActiveController/HasActiveController/RemoveActiveController
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, PictureInPictureController, Function | SmallTest | Level2)
{
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipController =
        new PictureInPictureController(option, nullptr, 100, nullptr);
    PictureInPictureManager::activeController_ = nullptr;
    ASSERT_FALSE(PictureInPictureManager::HasActiveController());
    PictureInPictureManager::RemoveActiveController(pipController);
    ASSERT_FALSE(PictureInPictureManager::HasActiveController());
    ASSERT_FALSE(PictureInPictureManager::IsActiveController(pipController));

    PictureInPictureManager::SetActiveController(pipController);
    ASSERT_TRUE(PictureInPictureManager::HasActiveController());
    ASSERT_TRUE(PictureInPictureManager::IsActiveController(pipController));
    ASSERT_TRUE(PictureInPictureManager::IsAttachedToSameWindow(100));
    ASSERT_FALSE(PictureInPictureManager::IsAttachedToSameWindow(1));
    PictureInPictureManager::RemoveActiveController(pipController);
    ASSERT_FALSE(PictureInPictureManager::HasActiveController());
    ASSERT_FALSE(PictureInPictureManager::IsActiveController(pipController));
}

/**
 * @tc.name: ShouldAbortPipStart
 * @tc.desc: ShouldAbortPipStart
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, ShouldAbortPipStart, Function | SmallTest | Level2)
{
    ASSERT_FALSE(PictureInPictureManager::ShouldAbortPipStart());

    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);
    pipController->curState_ = PiPWindowState::STATE_STARTING;

    ASSERT_TRUE(PictureInPictureManager::ShouldAbortPipStart());
}

/**
 * @tc.name: GetPipControllerInfo
 * @tc.desc: GetPipControllerInfo
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, GetPipControllerInfo, Function | SmallTest | Level2)
{
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);
    int32_t windowId = 1;
    ASSERT_EQ(nullptr, PictureInPictureManager::GetPipControllerInfo(windowId));

    sptr<PictureInPictureController> pipController1 =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController1, nullptr);
    PictureInPictureManager::windowToControllerMap_.insert(std::make_pair(windowId, pipController1));
    ASSERT_EQ(pipController1, PictureInPictureManager::GetPipControllerInfo(windowId));
}

/**
 * @tc.name: AttachAutoStartController
 * @tc.desc: AttachAutoStartController
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, AttachAutoStartController, Function | SmallTest | Level2)
{
    PictureInPictureManager::AttachAutoStartController(0, nullptr);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);
    wptr<PictureInPictureController> pipController1 =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController1, nullptr);

    PictureInPictureManager::autoStartController_ = nullptr;
    PictureInPictureManager::mainWindowLifeCycleImpl_ = nullptr;
    PictureInPictureManager::AttachAutoStartController(0, pipController1);
    PictureInPictureManager::autoStartController_ = pipController1;
    PictureInPictureManager::AttachAutoStartController(0, pipController1);
    sptr<IWindowLifeCycle> mainWindowLifeCycleImpl = new (std::nothrow) IWindowLifeCycle();
    ASSERT_NE(mainWindowLifeCycleImpl, nullptr);
    PictureInPictureManager::mainWindowLifeCycleImpl_ = mainWindowLifeCycleImpl;
    PictureInPictureManager::AttachAutoStartController(0, pipController1);
    auto option1 = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option1);
    auto windowSession = sptr<WindowSessionImpl>::MakeSptr(option1);
    ASSERT_NE(nullptr, windowSession);
    auto windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option1);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->GetMainWindowWithId(1);
    PictureInPictureManager::AttachAutoStartController(0, pipController1);
    pipController1->mainWindowId_ = 2;
    windowSceneSessionImpl->windowSessionMap_.insert(std::make_pair("window1", std::make_pair(2, windowSession)));
    windowSceneSessionImpl->GetMainWindowWithId(2);
    PictureInPictureManager::AttachAutoStartController(0, pipController1);
}

/**
 * @tc.name: DetachAutoStartController
 * @tc.desc: DetachAutoStartController
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, DetachAutoStartController, Function | SmallTest | Level2)
{
    int result = 0;
    PictureInPictureManager::DetachAutoStartController(0, nullptr);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);
    result++;
    wptr<PictureInPictureController> pipController1 =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController1, nullptr);
    PictureInPictureManager::autoStartController_ = pipController;
    PictureInPictureManager::DetachAutoStartController(0, pipController1);
    ASSERT_EQ(result, 1);
    PictureInPictureManager::autoStartController_ = pipController1;

    sptr<Window> mainWindow = nullptr;
    PictureInPictureManager::mainWindowLifeCycleImpl_ = nullptr;
    PictureInPictureManager::DetachAutoStartController(0, pipController1);
    ASSERT_EQ(result, 1);
    mainWindow = PictureInPictureManager::GetCurrentWindow();
    ASSERT_EQ(mainWindow, pipController->window_);
    PictureInPictureManager::DetachAutoStartController(0, pipController1);
    ASSERT_EQ(result, 1);
    sptr<IWindowLifeCycle> mainWindowLifeCycleImpl = new (std::nothrow) IWindowLifeCycle();
    ASSERT_NE(mainWindowLifeCycleImpl, nullptr);
    PictureInPictureManager::mainWindowLifeCycleImpl_ = mainWindowLifeCycleImpl;
    PictureInPictureManager::DetachAutoStartController(0, pipController1);
}

/**
 * @tc.name: IsAttachedToSameWindow
 * @tc.desc: IsAttachedToSameWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, IsAttachedToSameWindow, Function | SmallTest | Level2)
{
    bool res = PictureInPictureManager::IsAttachedToSameWindow(0);
    ASSERT_EQ(res, false);

    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);

    PictureInPictureManager::activeController_ = nullptr;
    ASSERT_FALSE(PictureInPictureManager::HasActiveController());
    bool res1 = PictureInPictureManager::IsAttachedToSameWindow(1);
    ASSERT_EQ(res1, false);

    PictureInPictureManager::SetActiveController(pipController);
    bool res2 = PictureInPictureManager::IsAttachedToSameWindow(100);
    ASSERT_EQ(res2, true);
    bool res3 = PictureInPictureManager::IsAttachedToSameWindow(1);
    ASSERT_EQ(res3, false);
}

/**
 * @tc.name: GetCurrentWindow
 * @tc.desc: GetCurrentWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, GetCurrentWindow, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, PictureInPictureManager::GetCurrentWindow());

    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);

    PictureInPictureManager::activeController_ = nullptr;
    ASSERT_FALSE(PictureInPictureManager::HasActiveController());
    ASSERT_EQ(nullptr, PictureInPictureManager::GetCurrentWindow());
    
    sptr<Window> window = nullptr;
    ASSERT_EQ(window, pipController->window_);
    PictureInPictureManager::SetActiveController(pipController);
    window = PictureInPictureManager::GetCurrentWindow();
    ASSERT_EQ(window, pipController->window_);
}

/**
 * @tc.name: DoRestore
 * @tc.desc: DoRestore/DoClose/DoActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, DoRestore, Function | SmallTest | Level2)
{
    int result = 0;
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::activeController_ = nullptr;
    PictureInPictureManager::DoPreRestore();
    PictureInPictureManager::DoRestore();
    PictureInPictureManager::DoClose(true, true);
    PictureInPictureManager::DoActionClose();
    PictureInPictureManager::DoLocateSource();
    std::string actionName = "test";
    PictureInPictureManager::DoActionEvent(actionName, 0);
    auto controlType = PiPControlType::VIDEO_PLAY_PAUSE;
    auto status = PiPControlStatus::PLAY;
    PictureInPictureManager::DoControlEvent(controlType, status);
    ASSERT_EQ(result, 0);
    
    PictureInPictureManager::SetActiveController(pipController);
    result++;

    PictureInPictureManager::DoPreRestore();
    PictureInPictureManager::DoRestore();
    PictureInPictureManager::DoClose(true, true);
    PictureInPictureManager::DoClose(true, false);
    PictureInPictureManager::DoActionClose();
    PictureInPictureManager::DoLocateSource();
    const std::string ACTION_CLOSE = "close";
    const std::string ACTION_PRE_RESTORE = "pre_restore";
    const std::string ACTION_RESTORE = "restore";
    const std::string ACTION_DESTROY = "destroy";
    PictureInPictureManager::DoActionEvent(ACTION_CLOSE, 0);
    PictureInPictureManager::DoActionEvent(ACTION_PRE_RESTORE, 0);
    PictureInPictureManager::DoActionEvent(ACTION_RESTORE, 0);
    PictureInPictureManager::DoActionEvent(ACTION_DESTROY, 0);
    ASSERT_EQ(result, 1);
}

/**
 * @tc.name: AutoStartPipWindow
 * @tc.desc: AutoStartPipWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, AutoStartPipWindow, Function | SmallTest | Level2)
{
    int result = 0;

    std::string navId = "";
    PictureInPictureManager::autoStartController_ = nullptr;
    PictureInPictureManager::AutoStartPipWindow(navId);
    ASSERT_EQ(result, 0);

    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    PictureInPictureManager::autoStartController_ = pipController;
    ASSERT_EQ(navId, "");
    option->SetTypeNodeEnabled(true);
    PictureInPictureManager::AutoStartPipWindow(navId);
    option->SetTypeNodeEnabled(false);
    PictureInPictureManager::AutoStartPipWindow(navId);
    ASSERT_EQ(result, 0);
    navId = "NavId";
    ASSERT_NE(navId, "");
    PictureInPictureManager::AutoStartPipWindow(navId);
    ASSERT_EQ(result, 0);
    SingletonContainer::Get<PiPReporter>().ReportPiPActionEvent(1, "close");
}

/**
 * @tc.name: DoDestroy
 * @tc.desc: DoDestroy
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, DoDestroy, Function | SmallTest | Level2)
{
    PictureInPictureManager::activeController_ = nullptr;
    ASSERT_FALSE(PictureInPictureManager::HasActiveController());
    PictureInPictureManager::DoDestroy();
    sptr<PipOption> option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipController =
        sptr<PictureInPictureController>::MakeSptr(option, nullptr, 100, nullptr);
    PictureInPictureManager::activeController_ = pipController;
    ASSERT_TRUE(PictureInPictureManager::HasActiveController());
    PictureInPictureManager::DoDestroy();
}
}
}
}
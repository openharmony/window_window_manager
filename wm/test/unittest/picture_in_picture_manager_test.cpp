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
 * @tc.desc: PutPipControllerInfo/RemovePipControllerInfo
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
}

/**
 * @tc.name: PictureInPictureController
 * @tc.desc: SetActiveController/IsActiveController/HasActiveController/RemoveActiveController
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureManagerTest, PictureInPictureController, Function | SmallTest | Level2)
{
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipController = new PictureInPictureController(option, nullptr, 100, nullptr);
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
    int result = 0;
    PictureInPictureManager::AttachAutoStartController(0, nullptr);

    sptr<PipOption> option = new (std::nothrow) PipOption();
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);
    result++;
    wptr<PictureInPictureController> pipController1 =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController1, nullptr);
    PictureInPictureManager::autoStartController_ = pipController1;
    sptr<IWindowLifeCycle> mainWindowLifeCycleImpl = new (std::nothrow) IWindowLifeCycle();
    ASSERT_NE(mainWindowLifeCycleImpl, nullptr);
    PictureInPictureManager::mainWindowLifeCycleImpl_ = mainWindowLifeCycleImpl;
    PictureInPictureManager::AttachAutoStartController(0, pipController1);
    ASSERT_EQ(result, 1);
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
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);
    result++;
    wptr<PictureInPictureController> pipController1 =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController1, nullptr);
    PictureInPictureManager::autoStartController_ = pipController1;
    PictureInPictureManager::DetachAutoStartController(0, pipController1);
    ASSERT_EQ(result, 1);
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
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);

    bool res1 = PictureInPictureManager::IsAttachedToSameWindow(100);
    ASSERT_EQ(res1, true);
    bool res2 = PictureInPictureManager::IsAttachedToSameWindow(1);
    ASSERT_EQ(res2, false);
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
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);

    sptr<Window> window = PictureInPictureManager::GetCurrentWindow();
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
    PictureInPictureManager::DoRestore();
    PictureInPictureManager::DoClose(true, true);
    std::string actionName = "test";
    PictureInPictureManager::DoActionEvent(actionName, 0);

    sptr<PipOption> option = new (std::nothrow) PipOption();
    sptr<PictureInPictureController> pipController =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    ASSERT_NE(pipController, nullptr);
    PictureInPictureManager::SetActiveController(pipController);
    result++;

    PictureInPictureManager::DoRestore();
    PictureInPictureManager::DoClose(true, true);
    PictureInPictureManager::DoClose(true, false);
    const std::string ACTION_CLOSE = "close";
    const std::string ACTION_RESTORE = "restore";
    PictureInPictureManager::DoActionEvent(ACTION_CLOSE, 0);
    PictureInPictureManager::DoActionEvent(ACTION_RESTORE, 0);
    ASSERT_EQ(result, 1);
}
}
}
}
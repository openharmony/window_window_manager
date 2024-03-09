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
}
}
}
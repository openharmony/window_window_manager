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
    MOCK_METHOD2(Show, WMError(uint32_t reason, bool withAnimation));
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
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
    pipControl->window_ = mw;
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
}

/**
 * @tc.name: StopPictureInPicture01
 * @tc.desc: StopPictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, GetPipPriority, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    uint32_t pipTemplateType;
    pipTemplateType = static_cast<uint32_t>(PipTemplateType::VIDEO_CALL);
    ASSERT_EQ(pipControl->GetPipPriority(pipTemplateType), 1);
    pipTemplateType = static_cast<uint32_t>(PipTemplateType::VIDEO_PLAY);
    ASSERT_EQ(pipControl->GetPipPriority(pipTemplateType), 0);
    pipTemplateType = static_cast<uint32_t>(PipTemplateType::VIDEO_MEETING);
    ASSERT_EQ(pipControl->GetPipPriority(pipTemplateType), 1);
    pipTemplateType = static_cast<uint32_t>(PipTemplateType::VIDEO_LIVE);
    ASSERT_EQ(pipControl->GetPipPriority(pipTemplateType), 0);
    pipTemplateType = static_cast<uint32_t>(PipTemplateType::END);
    ASSERT_NE(pipControl->GetPipPriority(pipTemplateType), 1);
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
        pipControl->StopPictureInPicture(true, false, StopPipType::NULL_STOP));
    pipControl->window_ = mw;
    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_ERROR_PIP_DESTROY_FAILED,
        pipControl->StopPictureInPicture(true, false, StopPipType::NULL_STOP));
    ASSERT_EQ(PipWindowState::STATE_UNDEFINED, pipControl->GetControllerState());
    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(true, false, StopPipType::NULL_STOP));
    ASSERT_EQ(PipWindowState::STATE_STOPPED, pipControl->GetControllerState());
}
}
}
}
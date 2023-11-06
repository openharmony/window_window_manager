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
class MockWindow : public Window
{
public:
    MockWindow() {};
    ~MockWindow() {};
    MOCK_METHOD5(NapiSetUIContent, WMError(const std::string& contentInfo, napi_env env, napi_value storage,
        bool isDistributed, AppExecFwk::Ability* ability));
    MOCK_METHOD2(Show, WMError(uint32_t reason, bool withAnimation));
    MOCK_METHOD0(Destroy, WMError());
};

class PictureInPictureControllerTest : public testing::Test
{
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
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, 100);
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->ShowPictureInPictureWindow());
    pipControl->window_ = mw;
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow());
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, pipControl->ShowPictureInPictureWindow());
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
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, 100);
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->StopPictureInPicture(false));
    pipControl->window_ = mw;
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STOPPING);
    ASSERT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPicture(false));
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STARTED);
    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    pipControl->StopPictureInPicture(false);
    ASSERT_EQ(PipWindowState::STATE_UNDEFINED, PictureInPictureManager::GetPipWindowState());
    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(false));
}
}
}
}
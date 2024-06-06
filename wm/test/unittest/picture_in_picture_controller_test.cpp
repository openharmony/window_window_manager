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

#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_helper.h"
#include <gmock/gmock.h>
#include "picture_in_picture_controller.h"
#include "picture_in_picture_manager.h"
#include "window.h"
#include "result_set.h"
#include "system_ability_definition.h"
#include "uri.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
    const std::string SETTING_COLUMN_VALUE = "VALUE";
    const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/"
        "settingsdata/SETTINGSDATA?Proxy=true";
    constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
}
class MockWindow : public Window {
public:
    MockWindow() {}
    ~MockWindow() {}
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
HWTEST_F(PictureInPictureControllerTest, StopPictureInPicture01, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    ASSERT_EQ(PiPWindowState::STATE_UNDEFINED, pipControl->GetControllerState());
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    pipControl->window_ = mw;
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(false, StopPipType::NULL_STOP));

    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    ASSERT_EQ(PiPWindowState::STATE_STARTED, pipControl->GetControllerState());
    pipControl->window_ = mw;
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    pipControl->handler_ = nullptr;
    pipControl->window_ = mw;
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    ASSERT_EQ(PiPWindowState::STATE_STARTED, pipControl->GetControllerState());
    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_ERROR_PIP_DESTROY_FAILED, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    EXPECT_CALL(*(mw), Destroy()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    ASSERT_EQ(PiPWindowState::STATE_STOPPED, pipControl->GetControllerState());
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
    ASSERT_EQ(PiPWindowState::STATE_UNDEFINED, ret);
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
 * @tc.name: getSettingsAutoStartStatus
 * @tc.desc: getSettingsAutoStartStatus
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, getSettingsAutoStartStatus01, Function | SmallTest | Level2)
{
    std::string key = "auto_start_pip_status";
    std::string value;
    sptr<MockWindow> mw = new MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    PictureInPictureController::remoteObj_ = nullptr;
    ASSERT_EQ(ERR_NO_INIT,  pipControl->getSettingsAutoStartStatus(key, value));
}

/**
 * @tc.name: getSettingsAutoStartStatus
 * @tc.desc: getSettingsAutoStartStatus
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, getSettingsAutoStartStatus02, Function | SmallTest | Level2)
{
    std::string key = " ";
    std::string value;
    sptr<MockWindow> mw = new MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    ASSERT_EQ(ERR_NAME_NOT_FOUND,  pipControl->getSettingsAutoStartStatus(key, value));
}

/**
 * @tc.name: getSettingsAutoStartStatus
 * @tc.desc: getSettingsAutoStartStatus
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, getSettingsAutoStartStatus03, Function | SmallTest | Level2)
{
    std::string key = "auto_start_pip_status";
    std::string value;
    sptr<MockWindow> mw = new MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    auto helper = DataShare::DataShareHelper::Creator(PictureInPictureController::remoteObj_, SETTING_URI_PROXY,
        SETTINGS_DATA_EXT_URI);
    std::vector<std::string> columns = {SETTING_COLUMN_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    Uri uri(SETTING_URI_PROXY + "&key=" + key);
    auto resultSet = helper->Query(uri, predicates, columns);
    int32_t count;
    resultSet->GetRowCount(count);
    int32_t INDEX = 0;
    int32_t ret = resultSet->GetString(INDEX, value);
    ASSERT_NE(NativeRdb::E_OK,  ret);
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
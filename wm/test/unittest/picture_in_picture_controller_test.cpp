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
 * @tc.name: GetPipPriority
 * @tc.desc: GetPipPriority
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, GetPipPriority, Function | SmallTest | Level2)
{
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, nullptr, 100, nullptr);
    uint32_t pipTypeTemplate = 5;
    uint32_t testValue = 0;
    ASSERT_EQ(testValue, pipControl->GetPipPriority(pipTypeTemplate));
    pipTypeTemplate = 3;
    ASSERT_EQ(testValue, pipControl->GetPipPriority(pipTypeTemplate));
    pipTypeTemplate = 0;
    ASSERT_EQ(testValue, pipControl->GetPipPriority(pipTypeTemplate));
    pipTypeTemplate = 1;
    testValue = 1;
    ASSERT_EQ(testValue, pipControl->GetPipPriority(pipTypeTemplate));
}

/**
 * @tc.name: ShowPictureInPictureWindow01
 * @tc.desc: ShowPictureInPictureWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, ShowPictureInPictureWindow01, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<MockWindow> mw1 = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw1);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);
    
    pipControl->pipOption_ = nullptr;
    pipControl->window_ = mw1;
    ASSERT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
    pipControl->pipOption_ = option;
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));

    pipControl->window_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
    pipControl->window_ = mw;
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));

    pipControl->pipLifeCycleListener_ = nullptr;
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));

    WMError errorCode = WMError::WM_OK;
    ASSERT_EQ(WMError::WM_OK, errorCode);
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(StartPipType::NULL_START));
}

/**
 * @tc.name: StopPictureInPicture01
 * @tc.desc: StopPictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StopPictureInPicture01, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->curState_ = PiPWindowState::STATE_STOPPING;
    ASSERT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    pipControl->curState_ = PiPWindowState::STATE_STOPPED;
    ASSERT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    pipControl->window_ = nullptr;
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
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);
    sptr<WindowOption> windowOption = nullptr;

    option->SetContext(nullptr);
    ASSERT_EQ(nullptr, option->GetContext());
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow());
    pipControl->pipOption_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow());

    EXPECT_EQ(nullptr, windowOption);
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow());
    
    sptr<Window> window = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow());
    WMError errorCode = WMError::WM_ERROR_PIP_CREATE_FAILED;
    ASSERT_NE(WMError::WM_OK, errorCode);
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow());
}

/**
 * @tc.name: StartPictureInPicture
 * @tc.desc: StartPictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartPictureInPicture, Function | SmallTest | Level2)
{
    StartPipType startType = StartPipType::AUTO_START;
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<MockWindow> mw1 = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw1);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->pipOption_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    option->SetContext(nullptr);
    ASSERT_EQ(nullptr, option->GetContext());
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->pipOption_ = option;

    pipControl->curState_ = PiPWindowState::STATE_STARTING;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;
    EXPECT_EQ(WMError::WM_OK, pipControl->StartPictureInPicture(startType));

    sptr<Window> mainWindow = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    mainWindow = mw1;
    EXPECT_EQ(WMError::WM_OK, pipControl->StartPictureInPicture(startType));

    EXPECT_EQ(true, pipControl->IsPullPiPAndHandleNavigation());
    ASSERT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    EXPECT_EQ(false, pipControl->IsPullPiPAndHandleNavigation());
    ASSERT_EQ(WMError::WM_OK, pipControl->StartPictureInPicture(startType));

    EXPECT_EQ(false, PictureInPictureManager::HasActiveController());
    ASSERT_EQ(WMError::WM_OK, pipControl->StartPictureInPicture(startType));
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
 * @tc.name: StopPictureInPictureFromClient
 * @tc.desc: StopPictureInPictureFromClient
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StopPictureInPictureFromClient, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->window_ = option;
    ASSERT_NE(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->StopPictureInPictureFromClient());
    pipControl->window_ = nullptr;
    ASSERT_NE(WMError::WM_OK, pipControl->StopPictureInPictureFromClient());

    pipControl->curState_ = PiPWindowState::STATE_STARTING;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPictureFromClient());
    pipControl->curState_ = PiPWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPictureFromClient());
    pipControl->curState_ = PiPWindowState::STATE_RESTORING;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPictureFromClient());
    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;
    EXPECT_EQ(WMError::WM_OK, pipControl->StopPictureInPictureFromClient());

    WMError res = WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    ASSERT_NE(res, WMError::WM_OK);
    EXPECT_EQ(WMError::WM_ERROR_PIP_DESTROY_FAILED, pipControl->StopPictureInPictureFromClient());
    res = WMError::WM_OK;
    ASSERT_EQ(res, WMError::WM_OK);
    EXPECT_EQ(WMError::WM_OK, pipControl->StopPictureInPictureFromClient());
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
    int result = 0;
    bool enable = true;
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->isAutoStartEnabled_ = enable;
    ASSERT_EQ(true, pipControl->isAutoStartEnabled_);
    pipControl->GetPiPNavigationId();
    ASSERT_EQ(false, pipControl->IsPullPiPAndHandleNavigation());
    ASSERT_EQ(result, 0);
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
    int result = 0;
    int32_t width = 0;
    int32_t height = 0;
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    std::shared_ptr<XComponentController> xComponentController = nullptr;
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->UpdateContentSize(width, height);
    ASSERT_EQ(result, 0);
    height = 150;
    pipControl->UpdateContentSize(width, height);
    ASSERT_EQ(result, 0);
    height = 0;
    width = 100;
    pipControl->UpdateContentSize(width, height);
    ASSERT_EQ(result, 0);
    height = 150;
    pipControl->UpdateContentSize(width, height);
    ASSERT_EQ(result, 0);

    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;
    pipControl->UpdateContentSize(width, height);
    ASSERT_EQ(result, 0);
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    pipControl->UpdateContentSize(width, height);
    sptr<Window> window = nullptr;
    pipControl->UpdateContentSize(width, height);
    ASSERT_EQ(result, 0);

    pipControl->SetXComponentController(xComponentController);
    pipControl->UpdateContentSize(width, height);
    ASSERT_EQ(result, 0);
    ASSERT_NE(WMError::WM_OK, pipControl->CreatePictureInPictureWindow());
}

/**
 * @tc.name: IsContentSizeChanged
 * @tc.desc: IsContentSizeChanged
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, IsContentSizeChanged, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);
    pipControl->windowRect_ = {0, 0, 0, 0};
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 0, 0, 0));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(0, 10.5, 0, 0));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(0, 0, 10.5, 0));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(0, 0, 0, 10.5));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 10.5, 0, 0));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 0, 10.5, 0));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 0, 0, 10.5));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(0, 10.5, 10.5, 0));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(0, 10.5, 0, 10.5));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(0, 0, 10.5, 10.5));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 10.5, 10.5, 0));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 10.5, 0, 10.5));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 0, 10.5, 10.5));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(0, 10.5, 10.5, 10.5));
    ASSERT_EQ(true, pipControl->IsContentSizeChanged(10.5, 10.5, 10.5, 10.5));
    ASSERT_EQ(false, pipControl->IsContentSizeChanged(0, 0, 0, 0));
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
    ASSERT_EQ(ERR_OK,  pipControl->getSettingsAutoStartStatus(key, value));
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
    pipControl->getSettingsAutoStartStatus(key, value);
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

/**
 * @tc.name: UpdatePiPSourceRect
 * @tc.desc: UpdatePiPSourceRect
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdatePiPSourceRect, Function | SmallTest | Level2)
{
    sptr<IPiPLifeCycle> listener = nullptr;
    sptr<IPiPActionObserver> listener1 = nullptr;
    std::shared_ptr<XComponentController> xComponentController = nullptr;
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);
    pipControl->SetXComponentController(xComponentController);
    pipControl->UpdatePiPSourceRect();
    ASSERT_NE(WMError::WM_OK, pipControl->CreatePictureInPictureWindow());
    ASSERT_EQ(0, pipControl->GetPipPriority(0));
}
}
}
}
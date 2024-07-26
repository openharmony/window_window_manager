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
#include "wm_common.h"
#include "xcomponent_controller.h"
#include "result_set.h"
#include "system_ability_definition.h"
#include "uri.h"
#include "ability_context_impl.h"

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
    MOCK_METHOD2(Show, WMError(uint32_t reason, bool withAnimation));
    MOCK_METHOD0(Destroy, WMError());
};

class MockXComponentController : public XComponentController {
public:
    MockXComponentController() {}
    ~MockXComponentController() {}
    MOCK_METHOD2(GetGlobalPosition, XComponentControllerErrorCode(float& offsetX, float& offsetY));
    MOCK_METHOD2(GetSize, XComponentControllerErrorCode(float& width, float& height));
    MOCK_METHOD1(SetExtController,
        XComponentControllerErrorCode(std::shared_ptr<XComponentController> xComponentController));
    MOCK_METHOD1(ResetExtController,
        XComponentControllerErrorCode(std::shared_ptr<XComponentController> xComponentController));
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
    StartPipType startType = StartPipType::NULL_START;
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<MockWindow> mw1 = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw1);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->pipOption_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->ShowPictureInPictureWindow(startType));
    pipControl->pipOption_ = option;

    pipControl->window_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->ShowPictureInPictureWindow(startType));
    pipControl->window_ = mw;

    auto listener = sptr<IPiPLifeCycle>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    pipControl->RegisterPiPLifecycle(listener);
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, pipControl->ShowPictureInPictureWindow(startType));
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
    pipControl->isStoppedFromClient_ = true;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    pipControl->isStoppedFromClient_ = false;
    ASSERT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    pipControl->curState_ = PiPWindowState::STATE_STOPPED;
    ASSERT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->StopPictureInPicture(false, StopPipType::NULL_STOP));

    pipControl->window_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    pipControl->window_ = mw;
    pipControl->isStoppedFromClient_ = true;
    pipControl->curState_ = PiPWindowState::STATE_STOPPING;
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(false, StopPipType::NULL_STOP));

    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(false, StopPipType::NULL_STOP));

    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    ASSERT_EQ(PiPWindowState::STATE_STARTED, pipControl->GetControllerState());
    pipControl->window_ = mw;
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));

    pipControl->handler_ = nullptr;
    pipControl->window_ = mw;
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    ASSERT_EQ(PiPWindowState::STATE_STARTED, pipControl->GetControllerState());
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPicture(true, StopPipType::NULL_STOP));
    ASSERT_NE(PiPWindowState::STATE_STARTED, pipControl->GetControllerState());
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

    pipControl->pipOption_ = nullptr;
    StartPipType startType = StartPipType::AUTO_START;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    pipControl->pipOption_ = option;
    option->SetContext(nullptr);
    ASSERT_EQ(nullptr, option->GetContext());
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));

    EXPECT_EQ(nullptr, windowOption);
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));

    sptr<Window> window = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    WMError errorCode = WMError::WM_ERROR_PIP_CREATE_FAILED;
    ASSERT_NE(WMError::WM_OK, errorCode);
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    option->SetNodeControllerRef(nullptr);
    ASSERT_EQ(nullptr, option->GetNodeControllerRef());
    ASSERT_EQ(nullptr, pipControl->GetCustomNodeController());
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
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->pipOption_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->pipOption_ = option;
    option->SetContext(nullptr);
    ASSERT_EQ(nullptr, option->GetContext());
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    void *contextPtr = static_cast<void*>(new AbilityRuntime::AbilityContextImpl());
    option->SetContext(contextPtr);

    pipControl->curState_ = PiPWindowState::STATE_STARTING;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StartPictureInPicture(startType));
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StartPictureInPicture(startType));
    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;

    pipControl->mainWindow_ = nullptr;
    pipControl->pipOption_->SetNavigationId("navId");
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->mainWindow_ = mw;

    pipControl->pipOption_->SetNavigationId("");
    ASSERT_EQ(true, pipControl->IsPullPiPAndHandleNavigation());
    PictureInPictureManager::SetActiveController(pipControl);
    ASSERT_TRUE(PictureInPictureManager::IsAttachedToSameWindow(100));
}

/**
 * @tc.name: StartPictureInPictureInner
 * @tc.desc: StartPictureInPictureInner
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartPictureInPictureInner, Function | SmallTest | Level2)
{
    StartPipType startType = StartPipType::AUTO_START;
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

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
    sptr<MockWindow> mw1 = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw1);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->window_ = mw1;
    ASSERT_NE(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->StopPictureInPictureFromClient());
    pipControl->window_ = nullptr;
    ASSERT_NE(WMError::WM_OK, pipControl->StopPictureInPictureFromClient());
    pipControl->window_ = mw1;
    pipControl->curState_ = PiPWindowState::STATE_STOPPING;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPictureFromClient());
    pipControl->curState_ = PiPWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPictureFromClient());
    pipControl->curState_ = PiPWindowState::STATE_RESTORING;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StopPictureInPictureFromClient());
    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;
    pipControl->window_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
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
    bool enable = true;
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->isAutoStartEnabled_ = enable;
    ASSERT_EQ(true, pipControl->isAutoStartEnabled_);
    pipControl->SetAutoStartEnabled(enable);
    pipControl->pipOption_->SetNavigationId("navId");
    pipControl->mainWindow_ = nullptr;
    pipControl->SetAutoStartEnabled(enable);

    enable = false;
    pipControl->isAutoStartEnabled_ = enable;
    ASSERT_EQ(false, pipControl->isAutoStartEnabled_);
    pipControl->pipOption_ = nullptr;
    pipControl->SetAutoStartEnabled(enable);
    pipControl->pipOption_ = option;

    pipControl->pipOption_->SetNavigationId("");
    pipControl->mainWindow_ = nullptr;
    pipControl->SetAutoStartEnabled(enable);
    pipControl->mainWindow_ = mw;
    pipControl->SetAutoStartEnabled(enable);
    pipControl->mainWindow_ = nullptr;
    pipControl->pipOption_->SetNavigationId("navId");
    pipControl->SetAutoStartEnabled(enable);
    pipControl->mainWindow_ = mw;
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
 * @tc.name: UpdateContentSize01
 * @tc.desc: UpdateContentSize
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdateContentSize01, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    int32_t width = 0;
    int32_t height = 0;
    pipControl->UpdateContentSize(width, height);
    height = 150;
    pipControl->UpdateContentSize(width, height);
    height = 0;
    width = 100;
    pipControl->UpdateContentSize(width, height);
    height = 150;
    pipControl->UpdateContentSize(width, height);

    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;
    pipControl->UpdateContentSize(width, height);
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    pipControl->UpdateContentSize(width, height);
}

/**
 * @tc.name: UpdateContentSize02
 * @tc.desc: UpdateContentSize
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdateContentSize02, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    pipControl->window_ = nullptr;
    int32_t width = 10;
    int32_t height = 20;
    pipControl->UpdateContentSize(width, height);
    pipControl->window_ = mw;

    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->UpdateContentSize(width, height);
    pipControl->mainWindowXComponentController_ = xComponentController;
    pipControl->windowRect_ = {0, 0, 0, 0};
    pipControl->IsContentSizeChanged(0, 0, 0, 0);
    pipControl->UpdateContentSize(width, height);
    pipControl->IsContentSizeChanged(10, 10, 10, 10);
    pipControl->UpdateContentSize(width, height);
}

/**
 * @tc.name: UpdatePiPControlStatus
 * @tc.desc: UpdatePiPControlStatus
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdatePiPControlStatus, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    auto controlType = PiPControlType::VIDEO_PLAY_PAUSE;
    auto status = PiPControlStatus::ENABLED;
    pipControl->UpdatePiPControlStatus(controlType, status);
    status = PiPControlStatus::PLAY;
    pipControl->UpdatePiPControlStatus(controlType, status);

    pipControl->window_ = nullptr;
    pipControl->UpdatePiPControlStatus(controlType, status);
    pipControl->window_ = mw;
    pipControl->UpdatePiPControlStatus(controlType, status);
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
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    sptr<IPiPActionObserver> listener = nullptr;

    pipControl->RegisterPiPActionObserver(listener);
    pipControl->DoActionEvent(actionName, status);
}

/**
 * @tc.name: DoControlEvent
 * @tc.desc: DoControlEvent
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, DoControlEvent, Function | SmallTest | Level2)
{
    auto controlType = PiPControlType::VIDEO_PLAY_PAUSE;
    auto status = PiPControlStatus::PLAY;
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    sptr<IPiPControlObserver> listener = nullptr;

    pipControl->pipOption_ = nullptr;
    pipControl->DoControlEvent(controlType, status);
    pipControl->pipOption_ = option;
    pipControl->DoControlEvent(controlType, status);
    pipControl->RegisterPiPControlObserver(listener);
    pipControl->DoControlEvent(controlType, status);
}

/**
 * @tc.name: PreRestorePictureInPicture
 * @tc.desc: PreRestorePictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, PreRestorePictureInPicture, Function | SmallTest | Level2)
{
    sptr<IPiPLifeCycle> listener = nullptr;
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->RegisterPiPLifecycle(listener);
    pipControl->PreRestorePictureInPicture();
}

/**
 * @tc.name: RestorePictureInPictureWindow
 * @tc.desc: RestorePictureInPictureWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, RestorePictureInPictureWindow, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->mainWindow_ = nullptr;
    pipControl->RestorePictureInPictureWindow();
    pipControl->mainWindow_ = mw;

    std::string navId = "navId";
    pipControl->RestorePictureInPictureWindow();
}

/**
 * @tc.name: UpdateXComponentPositionAndSize
 * @tc.desc: UpdateXComponentPositionAndSize
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdateXComponentPositionAndSize, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->UpdateXComponentPositionAndSize();
    pipControl->mainWindowXComponentController_ = xComponentController;

    pipControl->windowRect_.width_ = 10;
    pipControl->windowRect_.height_ = 10;
    pipControl->UpdateXComponentPositionAndSize();
    pipControl->windowRect_.width_ = 0;
    pipControl->UpdateXComponentPositionAndSize();
    pipControl->windowRect_.width_ = 10;
    pipControl->windowRect_.height_ = 0;
    pipControl->UpdateXComponentPositionAndSize();
    pipControl->windowRect_.width_ = 0;
    pipControl->UpdateXComponentPositionAndSize();
}

/**
 * @tc.name: IsPullPiPAndHandleNavigation
 * @tc.desc: IsPullPiPAndHandleNavigation
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, IsPullPiPAndHandleNavigation, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    pipControl->pipOption_->SetNavigationId("");
    ASSERT_EQ(true, pipControl->IsPullPiPAndHandleNavigation());
    pipControl->pipOption_->SetNavigationId("navId");
    pipControl->mainWindow_ = nullptr;
    ASSERT_EQ(false, pipControl->IsPullPiPAndHandleNavigation());
    pipControl->mainWindow_ = mw;
}

/**
 * @tc.name: ResetExtController
 * @tc.desc: ResetExtController
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, ResetExtController, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController);
    std::shared_ptr<MockXComponentController> xComponentController1 = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController1);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    
    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->ResetExtController();
    pipControl->pipXComponentController_ = nullptr;
    pipControl->ResetExtController();
    pipControl->mainWindowXComponentController_ = xComponentController1;
    pipControl->pipXComponentController_ = xComponentController;

    EXPECT_CALL(*(xComponentController1), ResetExtController(_)).Times(1)
        .WillOnce(Return(XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR));
    pipControl->ResetExtController();
}

/**
 * @tc.name: SetXComponentController
 * @tc.desc: SetXComponentController
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, SetXComponentController, Function | SmallTest | Level2)
{
    sptr<IPiPLifeCycle> listener = nullptr;
    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController);
    std::shared_ptr<MockXComponentController> xComponentController1 = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController1);
    sptr<MockWindow> mw = new MockWindow();
    sptr<PipOption> option = new PipOption();
    sptr<PictureInPictureController> pipControl = new PictureInPictureController(option, mw, 100, nullptr);

    pipControl->window_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->SetXComponentController(xComponentController));
    pipControl->window_ = mw;
    
    pipControl->mainWindowXComponentController_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->SetXComponentController(xComponentController));
    pipControl->pipXComponentController_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->SetXComponentController(xComponentController));
    pipControl->mainWindowXComponentController_ = xComponentController1;
    pipControl->pipXComponentController_ = xComponentController;
}

/**
 * @tc.name: UpdatePiPSourceRect
 * @tc.desc: UpdatePiPSourceRect
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdatePiPSourceRect, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    
    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->window_ = mw;
    pipControl->UpdatePiPSourceRect();
    pipControl->mainWindowXComponentController_ = xComponentController;
    pipControl->window_ = nullptr;
    pipControl->UpdatePiPSourceRect();
    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->UpdatePiPSourceRect();
    pipControl->mainWindowXComponentController_ = xComponentController;
    pipControl->window_ = mw;
    pipControl->UpdatePiPSourceRect();
}

/**
 * @tc.name: DestroyPictureInPictureWindow
 * @tc.desc: DestroyPictureInPictureWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, DestroyPictureInPictureWindow, Function | SmallTest | Level2)
{
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->window_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, pipControl->DestroyPictureInPictureWindow());

    sptr<MockWindow> window = sptr<MockWindow>::MakeSptr();
    pipControl->window_ = window;
    EXPECT_CALL(*(window), Destroy()).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_ERROR_PIP_DESTROY_FAILED, pipControl->DestroyPictureInPictureWindow());

    EXPECT_CALL(*(window), Destroy()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->DestroyPictureInPictureWindow());
}

/**
 * @tc.name: LocateSource
 * @tc.desc: LocateSource
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, LocateSource, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->window_ = mw;
    pipControl->mainWindow_ = nullptr;
    pipControl->LocateSource();
    pipControl->mainWindow_ = mw;
    pipControl->LocateSource();

    pipControl->pipOption_->SetNavigationId("");
    pipControl->LocateSource();
}

/**
 * @tc.name: StopPictureInPictureInner
 * @tc.desc: StopPictureInPictureInner
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StopPictureInPictureInner, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->window_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR,
        pipControl->StopPictureInPictureInner(StopPipType::NULL_STOP, true));
    pipControl->mainWindow_ = mw;
    ASSERT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR,
        pipControl->StopPictureInPictureInner(StopPipType::NULL_STOP, true));
    auto window = sptr<MockWindow>::MakeSptr();
    pipControl->window_ = window;
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPictureInner(StopPipType::NULL_STOP, true));
    ASSERT_EQ(WMError::WM_OK, pipControl->StopPictureInPictureInner(StopPipType::NULL_STOP, false));
}
}
}
}
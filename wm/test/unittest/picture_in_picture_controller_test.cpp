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
class MockWindow : public Window {
public:
    MockWindow() {};
    ~MockWindow() {};
    MOCK_METHOD2(Show, WMError(uint32_t reason, bool withAnimation));
    MOCK_METHOD0(Destroy, WMError());
    MOCK_METHOD0(NotifyPrepareClosePiPWindow, WMError());
    MOCK_CONST_METHOD0(GetWindowState, WindowState());
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
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
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
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(startType));
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    startType = StartPipType::AUTO_START;
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(startType));
    startType = StartPipType::NULL_START;
    pipControl->pipOption_->SetContentSize(10, 10);
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(startType));
    pipControl->pipOption_->SetContentSize(0, 10);
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(startType));
    pipControl->pipOption_->SetContentSize(10, 0);
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(startType));
    pipControl->pipOption_->SetContentSize(0, 0);
    EXPECT_CALL(*(mw), Show(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->ShowPictureInPictureWindow(startType));
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
HWTEST_F(PictureInPictureControllerTest, CreatePictureInPictureWindow01, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->pipOption_ = nullptr;
    StartPipType startType = StartPipType::AUTO_START;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    pipControl->pipOption_ = option;
    option->SetContext(nullptr);
    ASSERT_EQ(nullptr, option->GetContext());
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    AbilityRuntime::AbilityContextImpl* contextPtr = new AbilityRuntime::AbilityContextImpl();
    option->SetContext(contextPtr);
    pipControl->mainWindow_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    delete contextPtr;
}

/**
 * @tc.name: CreatePictureInPictureWindow
 * @tc.desc: CreatePictureInPictureWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, CreatePictureInPictureWindow02, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    pipControl->pipOption_ = option;
    StartPipType startType = StartPipType::AUTO_START;
    AbilityRuntime::AbilityContextImpl* contextPtr = new AbilityRuntime::AbilityContextImpl();
    option->SetContext(contextPtr);

    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController);
    pipControl->pipOption_->SetXComponentController(nullptr);
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->mainWindow_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    pipControl->pipOption_->SetXComponentController(xComponentController);
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    pipControl->pipOption_->SetXComponentController(nullptr);
    pipControl->mainWindow_ = mw;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    delete contextPtr;
}

/**
 * @tc.name: CreatePictureInPictureWindow
 * @tc.desc: CreatePictureInPictureWindow
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, CreatePictureInPictureWindow03, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    pipControl->pipOption_ = option;
    AbilityRuntime::AbilityContextImpl* contextPtr = new AbilityRuntime::AbilityContextImpl();
    option->SetContext(contextPtr);
    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    pipControl->pipOption_->SetXComponentController(xComponentController);
    pipControl->pipOption_->SetTypeNodeEnabled(true);
    pipControl->mainWindow_ = mw;
    StartPipType startType = StartPipType::NULL_START;
    EXPECT_CALL(*(mw), GetWindowState()).Times(2).WillOnce(Return(WindowState::STATE_CREATED));
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->CreatePictureInPictureWindow(startType));
    startType = StartPipType::AUTO_START;
    delete contextPtr;
}

/**
 * @tc.name: StartPictureInPicture
 * @tc.desc: StartPictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartPictureInPicture01, Function | SmallTest | Level2)
{
    StartPipType startType = StartPipType::AUTO_START;
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    
    pipControl->pipOption_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->pipOption_ = option;
    option->SetContext(nullptr);
    ASSERT_EQ(nullptr, option->GetContext());
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    AbilityRuntime::AbilityContextImpl* contextPtr = new AbilityRuntime::AbilityContextImpl();
    option->SetContext(contextPtr);
    pipControl->curState_ = PiPWindowState::STATE_STARTING;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StartPictureInPicture(startType));
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_PIP_REPEAT_OPERATION, pipControl->StartPictureInPicture(startType));
    delete contextPtr;
}

/**
 * @tc.name: StartPictureInPicture
 * @tc.desc: StartPictureInPicture
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartPictureInPicture01, Function | SmallTest | Level2)
{
    StartPipType startType = StartPipType::AUTO_START;
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    pipControl->pipOption_ = option;
    AbilityRuntime::AbilityContextImpl* contextPtr = new AbilityRuntime::AbilityContextImpl();
    option->SetContext(contextPtr);
    pipControl->curState_ = PiPWindowState::STATE_UNDEFINED;

    pipControl->pipOption_->SetNavigationId("navId");
    pipControl->mainWindow_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->pipOption_->SetNavigationId("");
    auto pipControl1 = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    auto pipControl2 = sptr<PictureInPictureController>::MakeSptr(option, mw, 200, nullptr);
    PictureInPictureManager::SetActiveController(pipControl1);
    PictureInPictureManager::IsActiveController(pipControl2);
    pipControl->mainWindowId_ = 100;
    PictureInPictureManager::IsAttachedToSameWindow(100);
    pipControl->window_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->window_ = mw;
    pipControl->pipOption_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_PIP_CREATE_FAILED, pipControl->StartPictureInPicture(startType));
    pipControl->pipOption_ = option;
    PictureInPictureManager::RemoveActiveController(pipControl1);
    PictureInPictureManager::IsActiveController(pipControl);
    pipControl->StartPictureInPicture(startType);
    delete contextPtr;
}

/**
 * @tc.name: StartPictureInPictureInner
 * @tc.desc: StartPictureInPictureInner
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, StartPictureInPictureInner, Function | SmallTest | Level2)
{
    StartPipType startType = StartPipType::USER_START;
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    pipControl->pipOption_->SetTypeNodeEnabled(true);
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
    EXPECT_CALL(*(mw1), NotifyPrepareClosePiPWindow()).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    EXPECT_EQ(WMError::WM_ERROR_PIP_DESTROY_FAILED, pipControl->StopPictureInPictureFromClient());
    EXPECT_CALL(*(mw1), NotifyPrepareClosePiPWindow()).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, pipControl->StopPictureInPictureFromClient());
}

/**
 * @tc.name: GetPipWindow
 * @tc.desc: GetPipWindow/SetPipWindow
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
    ASSERT_EQ(pipControl->window_, ret);
}

/**
 * @tc.name: SetAutoStartEnabled
 * @tc.desc: SetAutoStartEnabled
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, SetAutoStartEnabled01, Function | SmallTest | Level2)
{
    bool enable = true;
    sptr<MockWindow> mw = new (std::nothrow) MockWindow();
    ASSERT_NE(nullptr, mw);
    sptr<PipOption> option = new (std::nothrow) PipOption();
    ASSERT_NE(nullptr, option);
    sptr<PictureInPictureController> pipControl =
        new (std::nothrow) PictureInPictureController(option, mw, 100, nullptr);

    pipControl->mainWindow_ = nullptr;
    pipControl->SetAutoStartEnabled(enable);
    pipControl->mainWindow_ = mw;
    pipControl->isAutoStartEnabled_ = enable;
    ASSERT_EQ(true, pipControl->isAutoStartEnabled_);
    pipControl->pipOption_ = option;
    pipControl->pipOption_->SetTypeNodeEnabled(true);
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

    pipControl->mainWindow_ = mw;
    enable = false;
    pipControl->isAutoStartEnabled_ = enable;
    ASSERT_EQ(false, pipControl->isAutoStartEnabled_);
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->SetAutoStartEnabled(enable);
    pipControl->pipOption_->SetTypeNodeEnabled(true);
    pipControl->SetAutoStartEnabled(enable);
    pipControl->pipOption_ = nullptr;
    pipControl->SetAutoStartEnabled(enable);
    pipControl->pipOption_ = option;

    pipControl->pipOption_->SetNavigationId("");
    pipControl->SetAutoStartEnabled(enable);
    pipControl->pipOption_->SetNavigationId("navId");
    pipControl->SetAutoStartEnabled(enable);
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

    pipControl->pipOption_->SetTypeNodeEnabled(true);
    ASSERT_EQ(false, IsTypeNodeEnabled);
    pipControl->mainWindowXComponentController_ = xComponentController;
    pipControl->UpdateContentSize(width, height);
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->windowRect_ = {0, 0, 0, 0};
    pipControl->IsContentSizeChanged(0, 0, 0, 0);
    pipControl->UpdateContentSize(width, height);
    pipControl->IsContentSizeChanged(10, 10, 10, 10);
    pipControl->UpdateContentSize(width, height);
    pipControl->mainWindowXComponentController_ = nullptr;
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
    ASSERT_EQ(1, pipControl->option->GetControlEnable().size());
    ASSERT_EQ(controlType, pipControl->option->pipControlEnableInfoList_[0]);
    status = PiPControlStatus::PLAY;
    pipControl->UpdatePiPControlStatus(controlType, status);
    ASSERT_EQ(1, pipControl->option->GetControlStatus().size());
    ASSERT_EQ(status, pipControl->option->pipControlEnableInfoList_[0]);
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
 * @tc.name: DoActionEvent
 * @tc.desc: DoActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, DoActionEvent, Function | SmallTest | Level2)
{
    std::string actionName = "";
    int32_t status = 0;
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    sptr<IPiPActionObserver> listener = nullptr;

    pipControl->DoActionEvent(actionName, status);
    ASSERT_EQ(0, pipControl->option->GetControlStatus().size());
    actionName = "nextVideo";
    pipControl->DoActionEvent(actionName, status);
    ASSERT_EQ(1, pipControl->option->GetControlStatus().size());
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
    ASSERT_EQ(1, pipControl->option->GetControlStatus().size());
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
    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    pipControl->RegisterPiPLifecycle(listener);
    pipControl->PreRestorePictureInPicture();
    ASSERT_EQ(PiPWindowState::STATE_RESTORING, pipControl->curState_);
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

    pipControl->curState_ = PiPWindowState::STATE_STARTED;
    pipControl->RestorePictureInPictureWindow();
    ASSERT_EQ(PiPWindowState::STATE_STOPPING, pipControl->curState_);
}

/**
 * @tc.name: UpdateWinRectByComponent
 * @tc.desc: UpdateWinRectByComponent
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, UpdateWinRectByComponent, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    std::shared_ptr<MockXComponentController> xComponentController = std::make_shared<MockXComponentController>();
    ASSERT_NE(nullptr, xComponentController);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->pipOption_->SetTypeNodeEnabled(true);
    pipControl->UpdateWinRectByComponent();
    ASSERT_EQ(pipControl->windowRect_.width_, DEFAULT_ASPECT_RATIO[0]);
    ASSERT_EQ(pipControl->windowRect_.height_, DEFAULT_ASPECT_RATIO[1]);

    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->UpdateWinRectByComponent();
    pipControl->mainWindowXComponentController_ = xComponentController;

    pipControl->windowRect_.width_ = 0;
    pipControl->windowRect_.height_ = 10;
    pipControl->UpdateWinRectByComponent();
    ASSERT_EQ(pipControl->windowRect_.width_, 0);
    ASSERT_EQ(pipControl->windowRect_.height_, 0);
    pipControl->windowRect_.width_ = 10;
    pipControl->windowRect_.height_ = 0;
    ASSERT_EQ(pipControl->windowRect_.width_, 0);
    ASSERT_EQ(pipControl->windowRect_.height_, 0);
    pipControl->UpdateWinRectByComponent();
    pipControl->windowRect_.width_ = 0;
    pipControl->UpdateWinRectByComponent();
    ASSERT_EQ(pipControl->windowRect_.width_, 0);
    ASSERT_EQ(pipControl->windowRect_.height_, 0);
    pipControl->windowRect_.width_ = 10;
    pipControl->windowRect_.height_ = 10;
    pipControl->UpdateWinRectByComponent();
    ASSERT_EQ(pipControl->windowRect_.posX_, 0);
    ASSERT_EQ(pipControl->windowRect_.posY_, 0);
}

/**
 * @tc.name: RegisterPiPLifecycle
 * @tc.desc: RegisterPiPLifecycle/UnregisterPiPLifecycle
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, RegisterListener, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    auto listener = sptr<IPiPLifeCycle>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    auto listener1 = sptr<IPiPLifeCycle>::MakeSptr();
    ASSERT_NE(nullptr, listener1);
    pipControl->pipLifeCycleListeners_.push_back(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, pipControl->RegisterPiPLifecycle(nullptr));
    ASSERT_EQ(WMError::WM_OK, pipControl->RegisterPiPLifecycle(listener));
    ASSERT_EQ(WMError::WM_OK, pipControl->RegisterPiPLifecycle(listener1));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, pipControl->UnregisterPiPLifecycle(nullptr));
    ASSERT_EQ(WMError::WM_OK, pipControl->UnregisterPiPLifecycle(listener));
}

/**
 * @tc.name: RegisterPiPActionObserver
 * @tc.desc: RegisterPiPActionObserver/UnregisterPiPActionObserver
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, RegisterPiPActionObserver, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    auto listener = sptr<IPiPActionObserver>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    auto listener1 = sptr<IPiPActionObserver>::MakeSptr();
    ASSERT_NE(nullptr, listener1);
    pipControl->pipControlObservers_.push_back(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, pipControl->RegisterPiPActionObserver(nullptr));
    ASSERT_EQ(WMError::WM_OK, pipControl->RegisterPiPActionObserver(listener));
    ASSERT_EQ(WMError::WM_OK, pipControl->RegisterPiPActionObserver(listener1));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, pipControl->UnregisterPiPActionObserver(nullptr));
    ASSERT_EQ(WMError::WM_OK, pipControl->UnregisterPiPActionObserver(listener));
}

/**
 * @tc.name: RegisterPiPControlObserver
 * @tc.desc: RegisterPiPControlObserver/UnregisterPiPControlObserver
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, RegisterPiPControlObserver, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    auto listener = sptr<IPiPControlObserver>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    auto listener1 = sptr<IPiPControlObserver>::MakeSptr();
    ASSERT_NE(nullptr, listener1);
    pipControl->pipControlObservers_.push_back(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, pipControl->RegisterPiPControlObserver(nullptr));
    ASSERT_EQ(WMError::WM_OK, pipControl->RegisterPiPControlObserver(listener));
    ASSERT_EQ(WMError::WM_OK, pipControl->RegisterPiPControlObserver(listener1));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, pipControl->UnregisterPiPControlObserver(nullptr));
    ASSERT_EQ(WMError::WM_OK, pipControl->UnregisterPiPControlObserver(listener));
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
    pipControl->pipOption_->SetTypeNodeEnabled(true);
    ASSERT_EQ(true, pipControl->IsPullPiPAndHandleNavigation());
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->pipOption_->SetNavigationId("");
    ASSERT_EQ(true, pipControl->IsPullPiPAndHandleNavigation());
    pipControl->pipOption_->SetNavigationId("navId");
    pipControl->mainWindow_ = nullptr;
    ASSERT_EQ(false, pipControl->IsPullPiPAndHandleNavigation());
    pipControl->mainWindow_ = mw;
    auto ret = pipControl->pipOption_->GetNavigationId();
    NavigationController::GetNavigationController(nulllptr, ret);
    ASSERT_EQ(false, pipControl->IsPullPiPAndHandleNavigation());
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

    pipControl->pipOption_->SetTypeNodeEnabled(true);
    pipControl->ResetExtController();
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->ResetExtController();

    pipControl->pipXComponentController_ = nullptr;
    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->ResetExtController();
    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->pipXComponentController_ = xComponentController;
    pipControl->ResetExtController();
    pipControl->pipXComponentController_ = nullptr;
    pipControl->mainWindowXComponentController_ = xComponentController1;
    pipControl->ResetExtController();
    pipControl->pipXComponentController_ = xComponentController;
    EXPECT_CALL(*(xComponentController1), ResetExtController(_)).Times(1)
            .WillOnce(Return(XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_TYPE_ERROR));
    pipControl->ResetExtController();
    EXPECT_CALL(*(xComponentController1), ResetExtController(_)).Times(1)
        .WillOnce(Return(XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR));
    pipControl->ResetExtController();
}

/**
 * @tc.name: OnPictureInPictureStart
 * @tc.desc: OnPictureInPictureStart
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, OnPictureInPictureStart, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    pipControl->curState_ = PiPWindowState::STATE_STOPPED;
    pipControl->OnPictureInPictureStart();
    ASSERT_EQ(PiPWindowState::STATE_STARTED, pipControl->curState_);
}

/**
 * @tc.name: IsTypeNodeEnabled
 * @tc.desc: IsTypeNodeEnabled
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, IsTypeNodeEnabled, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    pipControl->pipOption_->SetTypeNodeEnabled(true);
    ASSERT_TRUE(pipControl->IsTypeNodeEnabled());
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    ASSERT_TRUE(!pipControl->IsTypeNodeEnabled());
    pipControl->pipOption_ = nullptr;
    ASSERT_TRUE(!pipControl->IsTypeNodeEnabled());
}

/**
 * @tc.name: GetTypeNode
 * @tc.desc: GetTypeNode
 * @tc.type: FUNC
 */
HWTEST_F(PictureInPictureControllerTest, GetTypeNode, Function | SmallTest | Level2)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);
    pipControl->pipOption_->SetTypeNodeRef(nullptr);
    ASSERT_EQ(nullptr, pipControl->GetTypeNode());
    pipControl->pipOption_ = nullptr;
    ASSERT_EQ(nullptr, pipControl->GetTypeNode());
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
    pipControl->pipOption_->SetTypeNodeEnabled(true);
    ASSERT_EQ(WMError::WM_OK, pipControl->SetXComponentController(xComponentController));
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->window_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->SetXComponentController(xComponentController));
    pipControl->window_ = mw;
    
    pipControl->pipXComponentController_ = nullptr;
    pipControl->mainWindowXComponentController_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->SetXComponentController(xComponentController));
    pipControl->mainWindowXComponentController_ = nullptr;
    pipControl->pipXComponentController_ = xComponentController;
    ASSERT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, pipControl->SetXComponentController(xComponentController));
    pipControl->mainWindowXComponentController_ = xComponentController1;
    EXPECT_CALL(*(xComponentController1), SetExtController(_)).Times(1)
        .WillOnce(Return(XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_TYPE_ERROR));
    ASSERT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, pipControl->SetXComponentController(xComponentController));
    EXPECT_CALL(*(xComponentController1), SetExtController(_)).Times(1)
        .WillOnce(Return(XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR));
    ASSERT_EQ(WMError::WM_OK, pipControl->SetXComponentController(xComponentController));
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

    pipControl->pipOption_->SetTypeNodeEnabled(true);
    pipControl->window_ = mw;
    Rect rect = {10, 0, 0, 0};
    pipControl->UpdatePiPSourceRect();
    ASSERT_EQ(0, pipControl->windowRect_.posX_);
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->UpdatePiPSourceRect();

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
    rect = {10, 10, 10, 10};
    pipControl->UpdatePiPSourceRect();
    ASSERT_EQ(10, pipControl->windowRect_.posX_);
    ASSERT_EQ(10, pipControl->windowRect_.posY_);
    ASSERT_EQ(10, pipControl->windowRect_.width_);
    ASSERT_EQ(10, pipControl->windowRect_.height_);
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
    pipControl->pipOption_ = nullptr;
    pipControl->mainWindow_ = nullptr;
    pipControl->window_ = window;
    ASSERT_EQ(WMError::WM_OK, pipControl->DestroyPictureInPictureWindow());
    pipControl->mainWindow_ = mw;
    pipControl->window_ = window;
    EXPECT_CALL(*(window), Destroy()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->DestroyPictureInPictureWindow());
    pipControl->pipOption_ = option;
    pipControl->pipOption_->SetNavigationId("navId");
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->mainWindow_ = nullptr;
    pipControl->window_ = window;
    EXPECT_CALL(*(window), Destroy()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, pipControl->DestroyPictureInPictureWindow());
    pipControl->pipOption_->SetNavigationId("");
    pipControl->mainWindow_ = mw;
    pipControl->window_ = window;
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
    auto mw1 = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw1);
    auto option = sptr<PipOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto pipControl = sptr<PictureInPictureController>::MakeSptr(option, mw, 100, nullptr);

    pipControl->window_ = mw;
    pipControl->mainWindow_ = nullptr;
    pipControl->LocateSource();
    pipControl->mainWindow_ = mw;
    pipControl->window_ = nullptr;
    pipControl->LocateSource();
    pipControl->mainWindow_ = nullptr;
    pipControl->LocateSource();
    pipControl->window_ = mw;
    pipControl->mainWindow_ = mw1;

    pipControl->pipOption_->SetNavigationId("");
    pipControl->pipOption_->SetTypeNodeEnabled(false);
    pipControl->LocateSource();
    pipControl->pipOption_->SetTypeNodeEnabled(true);
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

    pipControl->pipOption_ = nullptr;
    pipControl->StopPictureInPictureInner(StopPipType::NULL_STOP, true);
    pipControl->pipOption_ = option;
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
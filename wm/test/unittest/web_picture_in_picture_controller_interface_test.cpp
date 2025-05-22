/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <configuration.h>
#include <gtest/gtest.h>

#include "ability_context_impl.h"
#include "mock_static_call.h"
#include "mock_session.h"
#include "web_picture_in_picture_controller_interface.h"

#include "native_pip_window_listener.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WebPictureInPictureControllerInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WebPictureInPictureControllerInterface> controller = nullptr;
};

void WebPictureInPictureControllerInterfaceTest::SetUpTestCase()
{
}

void WebPictureInPictureControllerInterfaceTest::TearDownTestCase()
{
}

void WebPictureInPictureControllerInterfaceTest::SetUp()
{
    uint32_t mainWindowId = 100;
    PiPTemplateType pipTemplateType = PiPTemplateType::VIDEO_PLAY;
    uint32_t width = 100;
    uint32_t height = 150;
    std::vector<uint32_t> controlGroup = {101};
    int num = 0;
    napi_env env = reinterpret_cast<napi_env>(&num);
    controller = sptr<WebPictureInPictureControllerInterface>::MakeSptr();
    controller->Create();
    controller->SetMainWindowId(mainWindowId);
    controller->SetTemplateType(pipTemplateType);
    controller->SetRect(width, height);
    controller->SetControlGroup(controlGroup);
    controller->SetNapiEnv(env);
}

void WebPictureInPictureControllerInterfaceTest::TearDown()
{
    controller = nullptr;
}

void PipStartPipCallback(uint32_t controllerId, uint8_t requestId, uint64_t surfaceId)
{
}

void PipLifeCycleCallback(uint32_t controllerId, PiPState state, int32_t errorCode)
{
}

void PipControlEventCallback(uint32_t controllerId, PiPControlType controlType, PiPControlStatus status)
{
}

void PipResizeCallback(uint32_t controllerId, uint32_t width, uint32_t height, double scale)
{
}
namespace {
/**
 * @tc.name: Create
 * @tc.desc: Create
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, Create, TestSize.Level1)
{
    controller->isPipEnabled_ = false;
    WMError ret = controller->Create();
    controller->StartPip(0);
    controller->isPipEnabled_ = true;
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetMainWindowId
 * @tc.desc: SetMainWindowId
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, SetMainWindowId, TestSize.Level1)
{
    controller->Create();
    controller->isPipEnabled_ = false;
    WMError ret = controller->SetMainWindowId(0);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    controller->isPipEnabled_ = true;
    ret = controller->SetMainWindowId(0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = controller->SetMainWindowId(100);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetTemplateType
 * @tc.desc: SetTemplateType
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, SetTemplateType, TestSize.Level1)
{
    controller->Create();
    controller->isPipEnabled_ = false;
    PiPTemplateType pipTemplateType = PiPTemplateType::VIDEO_PLAY;
    WMError ret = controller->SetTemplateType(pipTemplateType);
    controller->isPipEnabled_ = true;
    ret = controller->SetTemplateType(pipTemplateType);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetRect
 * @tc.desc: SetRect
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, SetRect, TestSize.Level1)
{
    controller->Create();
    controller->isPipEnabled_ = false;
    WMError ret = controller->SetRect(100, 150);
    controller->isPipEnabled_ = true;
    ret = controller->SetRect(0, 150);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = controller->SetRect(100, 0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = controller->SetRect(100, 150);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: StopPip
 * @tc.desc: StopPip
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, StopPip, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->StopPip();
    controller->sptrWebPipController_ = nullptr;
    ret = controller->StopPip();
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: UpdateContentSize
 * @tc.desc: UpdateContentSize
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UpdateContentSize, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->UpdateContentSize(10, 20);
    EXPECT_EQ(WMError::WM_OK, ret);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UpdateContentSize(10, 20);
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: UpdatePiPControlStatus
 * @tc.desc: UpdatePiPControlStatus
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UpdatePiPControlStatus, TestSize.Level1)
{
    controller->StartPip(0);
    auto controlType = PiPControlType::VIDEO_PLAY_PAUSE;
    auto status = PiPControlStatus::PAUSE;
    controller->sptrWebPipController_->pipOption_->pipControlStatusInfoList_.clear();
    controller->UpdatePiPControlStatus(controlType, status);
    EXPECT_EQ(controller->sptrWebPipController_->pipOption_->pipControlStatusInfoList_[0].status,
        PiPControlStatus::PAUSE);
    controller->sptrWebPipController_ = nullptr;
    controller->UpdatePiPControlStatus(controlType, status);
    EXPECT_EQ(controller->sptrWebPipController_, nullptr);
}

/**
 * @tc.name: setPiPControlEnabled
 * @tc.desc: setPiPControlEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, setPiPControlEnabled, TestSize.Level1)
{
    controller->StartPip(0);
    auto controlType = PiPControlType::VIDEO_PREVIOUS;
    controller->setPiPControlEnabled(controlType, true);
    controller->setPiPControlEnabled(controlType, false);

    controller->sptrWebPipController_ = nullptr;
    controller->setPiPControlEnabled(controlType, true);
    EXPECT_EQ(controller->sptrWebPipController_, nullptr);
}

/**
 * @tc.name: RegisterStartPipListener
 * @tc.desc: RegisterStartPipListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, RegisterStartPipListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterStartPipListener(PipStartPipCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->RegisterStartPipListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = controller->RegisterStartPipListener(PipStartPipCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);
}

/**
 * @tc.name: UnregisterStartPipListener
 * @tc.desc: UnregisterStartPipListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterStartPipListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterStartPipListener(PipStartPipCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterStartPipListener(PipStartPipCallback);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = controller->UnregisterStartPipListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    ret = controller->UnregisterStartPipListener(PipStartPipCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);
}

/**
 * @tc.name: RegisterLifeCycleListener
 * @tc.desc: RegisterLifeCycleListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, RegisterLifeCycleListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterLifeCycleListener(PipLifeCycleCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->RegisterLifeCycleListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = controller->RegisterLifeCycleListener(PipLifeCycleCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);
}

/**
 * @tc.name: UnregisterLifeCycleListener
 * @tc.desc: UnregisterLifeCycleListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterLifeCycleListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterLifeCycleListener(PipLifeCycleCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterLifeCycleListener(PipLifeCycleCallback);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = controller->UnregisterLifeCycleListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    ret = controller->UnregisterLifeCycleListener(PipLifeCycleCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UnregisterLifeCycleListener(PipLifeCycleCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: RegisterControlEventListener
 * @tc.desc: RegisterControlEventListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, RegisterControlEventListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterControlEventListener(PipControlEventCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->RegisterControlEventListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = controller->RegisterControlEventListener(PipControlEventCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);
}

/**
 * @tc.name: UnregisterControlEventListener
 * @tc.desc: UnregisterControlEventListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterControlEventListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterControlEventListener(PipControlEventCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterControlEventListener(PipControlEventCallback);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = controller->UnregisterControlEventListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    ret = controller->UnregisterControlEventListener(PipControlEventCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UnregisterControlEventListener(PipControlEventCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: RegisterResizeListener
 * @tc.desc: RegisterResizeListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, RegisterResizeListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterResizeListener(PipResizeCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->RegisterResizeListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = controller->RegisterResizeListener(PipResizeCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);
}

/**
 * @tc.name: UnregisterResizeListener
 * @tc.desc: UnregisterResizeListener
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterResizeListener, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterResizeListener(PipResizeCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterResizeListener(PipResizeCallback);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = controller->UnregisterResizeListener(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    ret = controller->UnregisterResizeListener(PipResizeCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_STATE_ABNORMALLY, ret);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UnregisterResizeListener(PipResizeCallback);
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: UnregisterAllPiPStart
 * @tc.desc: UnregisterAllPiPStart
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterAllPiPStart, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterStartPipListener(PipStartPipCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterAllPiPStart();
    EXPECT_EQ(WMError::WM_OK, ret);
    EXPECT_EQ(controller->startPipCallbackSet_.size(), 0);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UnregisterAllPiPStart();
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: UnregisterAllPiPLifecycle
 * @tc.desc: UnregisterAllPiPLifecycle
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterAllPiPLifecycle, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterLifeCycleListener(PipLifeCycleCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterAllPiPLifecycle();
    EXPECT_EQ(WMError::WM_OK, ret);
    EXPECT_EQ(controller->lifeCycleCallbackSet_.size(), 0);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UnregisterAllPiPLifecycle();
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: UnregisterAllPiPControlObserver
 * @tc.desc: UnregisterAllPiPControlObserver
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterAllPiPControlObserver, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterControlEventListener(PipControlEventCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterAllPiPControlObserver();
    EXPECT_EQ(WMError::WM_OK, ret);
    EXPECT_EQ(controller->controlEventCallbackSet_.size(), 0);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UnregisterAllPiPControlObserver();
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}

/**
 * @tc.name: UnregisterAllPiPWindowSize
 * @tc.desc: UnregisterAllPiPWindowSize
 * @tc.type: FUNC
 */
HWTEST_F(WebPictureInPictureControllerInterfaceTest, UnregisterAllPiPWindowSize, TestSize.Level1)
{
    controller->StartPip(0);
    auto ret = controller->RegisterResizeListener(PipResizeCallback);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = controller->UnregisterAllPiPWindowSize();
    EXPECT_EQ(WMError::WM_OK, ret);
    EXPECT_EQ(controller->resizeCallbackSet_.size(), 0);

    controller->sptrWebPipController_ = nullptr;
    ret = controller->UnregisterAllPiPWindowSize();
    EXPECT_EQ(WMError::WM_ERROR_PIP_INTERNAL_ERROR, ret);
}
}
}
}
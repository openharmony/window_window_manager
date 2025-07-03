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

#include <gtest/gtest.h>
#include "ui_effect_controller_client.h"
#include "ui_effect_manager.h"
#include "mock/mock_accesstoken_kit.h"
#include "mock/mock_ui_effect_controller_client_stub.h"
#include "../../../test/common/mock/iremote_object_mocker.h"
#include "ui_effect_controller_client.h"
#include "ui_effect_controller.h"
#include "ui_effect_manager.h"

namespace OHOS::Rosen {

class UIEffectControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void UIEffectControllerTest::SetUpTestCase() {}

void UIEffectControllerTest::TearDownTestCase() {}

void UIEffectControllerTest::SetUp() {}

void UIEffectControllerTest::TearDown() {}

namespace {
using namespace testing;
using namespace testing::ext;
HWTEST_F(UIEffectControllerTest, UIEffectControllerClientAll, TestSize.Level1)
{
    UIEffectControllerClient client;
    EXPECT_EQ(client.id_, UIEFFECT_INVALID_ID);
    int32_t id = 23423;
    client.SetId(id);
    EXPECT_EQ(client.GetId(), id);
    sptr<UIEffectParams> param = sptr<UIEffectParams>::MakeSptr();
    client.SetParams(param);
    EXPECT_EQ(client.param_, param);
}

HWTEST_F(UIEffectControllerTest, UIEffectManagerTestCreateUIEffectController, TestSize.Level1)
{
    sptr<UIEffectControllerClientStubMocker> controllerClient = sptr<UIEffectControllerClientStubMocker>::MakeSptr();
    sptr<IUIEffectController> controller = nullptr;
    int32_t id = 1;
    EXPECT_EQ(UIEffectManager::GetInstance().CreateUIEffectController(controllerClient, controller, id),
        WMError::WM_ERROR_NULLPTR);
    bool setParamCalled = false;
    bool animateToCalled = false;
    UIEffectManager::GetInstance().RegisterUIEffectSetParamCallback([&setParamCalled](int32_t, sptr<UIEffectParams>) {
        setParamCalled = true;
    });
    EXPECT_EQ(UIEffectManager::GetInstance().CreateUIEffectController(controllerClient, controller, id),
        WMError::WM_ERROR_NULLPTR);
    UIEffectManager::GetInstance().RegisterUIEffectAnimateToCallback([&animateToCalled](int32_t,
        sptr<UIEffectParams>, sptr<WindowAnimationOption>, sptr<WindowAnimationOption>) {
        animateToCalled = true;
    });
    EXPECT_CALL(*controllerClient, AsObject()).WillOnce(Return(nullptr));
    EXPECT_EQ(UIEffectManager::GetInstance().CreateUIEffectController(controllerClient, controller, id),
        WMError::WM_ERROR_NULLPTR);
    sptr<RemoteObjectMocker> remoteObjectMocker = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*controllerClient, AsObject()).WillRepeatedly(
        [weakThis = wptr(remoteObjectMocker)]()-> sptr<RemoteObjectMocker> {
            auto mocker = weakThis.promote();
            if (mocker) {
                return mocker;
            } else {
                GTEST_LOG_(INFO) << "SceneSessionMocker:NULL";
                return nullptr;
            }
        });
    EXPECT_CALL(*remoteObjectMocker, AddDeathRecipient(_)).
        WillOnce([](const sptr<IRemoteObject::DeathRecipient>& recipient) {
            return false;
        });
    EXPECT_EQ(UIEffectManager::GetInstance().CreateUIEffectController(controllerClient, controller, id),
        WMError::WM_OK);
    EXPECT_CALL(*remoteObjectMocker, AddDeathRecipient(_)).
        WillOnce([](const sptr<IRemoteObject::DeathRecipient>& recipient) {
            return true;
        });
    EXPECT_EQ(UIEffectManager::GetInstance().CreateUIEffectController(controllerClient, controller, id),
        WMError::WM_OK);
}

HWTEST_F(UIEffectControllerTest, UIEffectManagerTestSetUIEffectControllerAliveState, TestSize.Level1)
{
    sptr<UIEffectControllerClient> controllerClient = sptr<UIEffectControllerClient>::MakeSptr();
    sptr<UIEffectController> controller = sptr<UIEffectController>::MakeSptr(0, nullptr, nullptr);
    controller->isAliveInUI_ = false;
    UIEffectManager::GetInstance().SetUIEffectControllerAliveState(10000, true);
    EXPECT_EQ(controller->isAliveInUI_, false);
    UIEffectManager::GetInstance().UIEffectControllerMap_[100] = std::make_tuple(nullptr, nullptr, nullptr);
    UIEffectManager::GetInstance().SetUIEffectControllerAliveState(100, true);
    EXPECT_EQ(controller->isAliveInUI_, false);
    UIEffectManager::GetInstance().UIEffectControllerMap_[200] = std::make_tuple(controller, nullptr, nullptr);
    UIEffectManager::GetInstance().SetUIEffectControllerAliveState(200, true);
    EXPECT_EQ(controller->isAliveInUI_, true);
    UIEffectManager::GetInstance().EraseUIEffectController(200);
}

// 需要mock_accesstoken_kit.h
HWTEST_F(UIEffectControllerTest, UIEffectControllerTest, TestSize.Level1)
{
    sptr<UIEffectController> controller = sptr<UIEffectController>::MakeSptr(0, nullptr, nullptr);
    sptr<UIEffectParams> param = sptr<UIEffectParams>::MakeSptr();
    sptr<WindowAnimationOption> option = sptr<WindowAnimationOption>::MakeSptr();
    MockAccesstokenKit::MockIsSystemApp(false);
    MockAccesstokenKit::MockIsSACalling(false);
    EXPECT_EQ(controller->SetParams(param), WMError::WM_ERROR_NOT_SYSTEM_APP);
    EXPECT_EQ(controller->AnimateTo(param, option, option), WMError::WM_ERROR_NOT_SYSTEM_APP);
    MockAccesstokenKit::MockIsSystemApp(true);
    MockAccesstokenKit::MockIsSACalling(true);
    bool setParamCalled = false;
    controller->setParamsCallback_ = nullptr;
    EXPECT_EQ(controller->SetParams(param), WMError::WM_ERROR_NULLPTR);
    controller->setParamsCallback_ = [&setParamCalled](int32_t, sptr<UIEffectParams>) {
        setParamCalled = true;
    };
    EXPECT_EQ(controller->SetParams(param), WMError::WM_OK);
    controller->isAliveInUI_ = false;
    EXPECT_EQ(controller->AnimateTo(param, option, option), WMError::WM_ERROR_UI_EFFECT_ERROR);
    controller->isAliveInUI_ = true;
    controller->animateToCallback_ = nullptr;
    EXPECT_EQ(controller->AnimateTo(param, option, option), WMError::WM_ERROR_NULLPTR);
    bool animateToCalled = false;
    controller->animateToCallback_ = [&animateToCalled](int32_t, sptr<UIEffectParams>, sptr<WindowAnimationOption>,
        sptr<WindowAnimationOption>) {
        animateToCalled = true;
    };
    EXPECT_EQ(controller->AnimateTo(param, option, option), WMError::WM_OK);
}
}
} // namespace OHOS::Rosen
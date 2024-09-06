/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "session_proxy.h"

#include <transaction/rs_transaction.h>
#include "ability_context_impl.h"
#include "mock_session.h"
#include "display_info.h"
#include "accessibility_event_info.h"
#include "window_manager_hilog.h"
#include "window_impl.h"
#include "native_engine.h"
#include "window_extension_session_impl.h"
#include "mock_uicontent.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Accessibility;
using namespace std;
namespace OHOS {
namespace Rosen {
class WindowExtensionSessionImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<WindowExtensionSessionImpl> window_ = nullptr;
};

void WindowExtensionSessionImplTest::SetUpTestCase()
{
}

void WindowExtensionSessionImplTest::TearDownTestCase()
{
}

void WindowExtensionSessionImplTest::SetUp()
{
    sptr<WindowOption> option = new(std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("WindowExtensionSessionImplTest");
    window_ = new(std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window_);
}

void WindowExtensionSessionImplTest::TearDown()
{
    window_ = nullptr;
}

namespace {
/**
 * @tc.name: Create01
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Create01, Function | SmallTest | Level3)
{
    auto abilityContext = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    ASSERT_NE(nullptr, abilityContext);
    SessionInfo sessionInfo;
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window_->Create(abilityContext, session));
    ASSERT_EQ(WMError::WM_OK, window_->Destroy(false));
}

/**
 * @tc.name: Create02
 * @tc.desc: context is nullptr, session is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Create02, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->Create(nullptr, nullptr));
}

/**
 * @tc.name: Create03
 * @tc.desc: context is not nullptr, session is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Create03, Function | SmallTest | Level3)
{
    auto abilityContext = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->Create(abilityContext, nullptr));
}

/**
 * @tc.name: Create04
 * @tc.desc: connet failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Create04, Function | SmallTest | Level3)
{
    auto abilityContext = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    ASSERT_NE(nullptr, abilityContext);
    SessionInfo sessionInfo;
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    EXPECT_CALL(*session, Connect).WillOnce(Return(WSError::WS_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_OK, window_->Create(abilityContext, session));
    ASSERT_EQ(WMError::WM_OK, window_->Destroy(false));
}

/**
 * @tc.name: Destroy01
 * @tc.desc: Destroy Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Destroy01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window_->hostSession_ = session;
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window_->Destroy(false, false));
}

/**
 * @tc.name: AddExtensionWindowStageToSCB
 * @tc.desc: AddExtensionWindowStageToSCB Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, AddExtensionWindowStageToSCB, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->AddExtensionWindowStageToSCB();
}

/**
 * @tc.name: RemoveExtensionWindowStageFromSCB
 * @tc.desc: RemoveExtensionWindowStageFromSCB Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RemoveExtensionWindowStageFromSCB, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->RemoveExtensionWindowStageFromSCB();
}

/**
 * @tc.name: UpdateConfiguration01
 * @tc.desc: UpdateConfiguration Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateConfiguration01, Function | SmallTest | Level3)
{
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    ASSERT_NE(nullptr, window_);
    window_->UpdateConfiguration(configuration);
}

/**
 * @tc.name: UpdateConfiguration02
 * @tc.desc: UpdateConfiguration Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateConfiguration02, Function | SmallTest | Level3)
{
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    window_->UpdateConfiguration(configuration);
}

/**
 * @tc.name: UpdateConfigurationForAll01
 * @tc.desc: UpdateConfigurationForAll01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateConfigurationForAll01, Function | SmallTest | Level3)
{
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();
    ASSERT_NE(nullptr, window_);
    window_->UpdateConfigurationForAll(configuration);
}

/**
 * @tc.name: UpdateConfigurationForAll02
 * @tc.desc: UpdateConfigurationForAll02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateConfigurationForAll02, Function | SmallTest | Level3)
{
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();
    ASSERT_NE(nullptr, window_);
    window_->windowExtensionSessionSet_.insert(window_);
    window_->UpdateConfigurationForAll(configuration);
    window_->windowExtensionSessionSet_.erase(window_);
}

/**
 * @tc.name: MoveTo01
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, MoveTo01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window_->MoveTo(0, 1));
}

/**
 * @tc.name: MoveTo02
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, MoveTo02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window_->MoveTo(0, 1));
}

/**
 * @tc.name: Resize01
 * @tc.desc: Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Resize01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window_->Resize(0, 1));
}

/**
 * @tc.name: Resize02
 * @tc.desc: Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Resize02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window_->Resize(0, 1));
}

/**
 * @tc.name: TransferAbilityResult01
 * @tc.desc: TransferAbilityResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferAbilityResult01, Function | SmallTest | Level3)
{
    AAFwk::Want want;
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window_->TransferAbilityResult(1, want));
}

/**
 * @tc.name: TransferAbilityResult02
 * @tc.desc: TransferAbilityResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferAbilityResult02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    AAFwk::Want want;
    ASSERT_EQ(WMError::WM_OK, window_->TransferAbilityResult(1, want));
}

/**
 * @tc.name: TransferExtensionData01
 * @tc.desc: TransferExtensionData
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferExtensionData01, Function | SmallTest | Level3)
{
    AAFwk::WantParams wantParams;
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window_->TransferExtensionData(wantParams));
}

/**
 * @tc.name: TransferExtensionData02
 * @tc.desc: TransferExtensionData
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferExtensionData02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    AAFwk::WantParams wantParams;
    ASSERT_EQ(WMError::WM_OK, window_->TransferExtensionData(wantParams));
}

/**
 * @tc.name: RegisterTransferComponentDataListener01
 * @tc.desc: RegisterTransferComponentDataListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataListener01, Function | SmallTest | Level3)
{
    NotifyTransferComponentDataFunc func;
    window_->RegisterTransferComponentDataListener(func);
    ASSERT_EQ(nullptr, window_->notifyTransferComponentDataFunc_);
}

/**
 * @tc.name: RegisterTransferComponentDataListener02
 * @tc.desc: RegisterTransferComponentDataListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataListener02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    NotifyTransferComponentDataFunc func;
    window_->RegisterTransferComponentDataListener(func);
}

/**
 * @tc.name: NotifyTransferComponentData01
 * @tc.desc: NotifyTransferComponentData Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyTransferComponentData01, Function | SmallTest | Level3)
{
    AAFwk::WantParams wantParams;
    ASSERT_EQ(WSError::WS_OK, window_->NotifyTransferComponentData(wantParams));
}

/**
 * @tc.name: NotifyTransferComponentData02
 * @tc.desc: NotifyTransferComponentData Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyTransferComponentData02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    NotifyTransferComponentDataFunc func = [](const AAFwk::WantParams &wantParams) -> AAFwk::WantParams {
        AAFwk::WantParams retWantParams;
        return retWantParams;
    };
    window_->RegisterTransferComponentDataListener(func);
    AAFwk::WantParams wantParams;
    ASSERT_EQ(WSError::WS_OK, window_->NotifyTransferComponentData(wantParams));
}

/**
 * @tc.name: NotifyTransferComponentDataSync01
 * @tc.desc: NotifyTransferComponentDataSync Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyTransferComponentDataSync01, Function | SmallTest | Level3)
{
    AAFwk::WantParams wantParams;
    AAFwk::WantParams reWantParams;
    ASSERT_EQ(WSErrorCode::WS_ERROR_NOT_REGISTER_SYNC_CALLBACK,
        window_->NotifyTransferComponentDataSync(wantParams, reWantParams));
}

/**
 * @tc.name: NotifyTransferComponentDataSync02
 * @tc.desc: NotifyTransferComponentDataSync Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyTransferComponentDataSync02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    NotifyTransferComponentDataForResultFunc func = [](const AAFwk::WantParams &wantParams) -> AAFwk::WantParams {
        AAFwk::WantParams retWantParams;
        return retWantParams;
    };
    window_->RegisterTransferComponentDataForResultListener(func);
    AAFwk::WantParams wantParams;
    AAFwk::WantParams reWantParams;
    ASSERT_EQ(WSErrorCode::WS_OK,
        window_->NotifyTransferComponentDataSync(wantParams, reWantParams));
}

/**
 * @tc.name: RegisterTransferComponentDataForResultListener01
 * @tc.desc: RegisterTransferComponentDataForResultListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataForResultListen01, Function | SmallTest | Level3)
{
    NotifyTransferComponentDataForResultFunc func;
    window_->RegisterTransferComponentDataForResultListener(func);
    ASSERT_EQ(nullptr, window_->notifyTransferComponentDataForResultFunc_);
}

/**
 * @tc.name: RegisterTransferComponentDataForResultListener02
 * @tc.desc: RegisterTransferComponentDataForResultListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataForResultListen02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    NotifyTransferComponentDataForResultFunc func;
    window_->RegisterTransferComponentDataForResultListener(func);
}

/**
* @tc.name: TriggerBindModalUIExtension01
* @tc.desc: TriggerBindModalUIExtension01 Test
* @tc.type: FUNC
*/
HWTEST_F(WindowExtensionSessionImplTest, TriggerBindModalUIExtension01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->TriggerBindModalUIExtension();
}

/**
* @tc.name: TriggerBindModalUIExtension02
* @tc.desc: TriggerBindModalUIExtension02 Test
* @tc.type: FUNC
*/
HWTEST_F(WindowExtensionSessionImplTest, TriggerBindModalUIExtension02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    window_->TriggerBindModalUIExtension();
}

/**
 * @tc.name: SetPrivacyMode01
 * @tc.desc: SetPrivacyMode Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetPrivacyMode01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->SetPrivacyMode(false));
}

/**
 * @tc.name: SetPrivacyMode02
 * @tc.desc: SetPrivacyMod
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetPrivacyMod02, Function | SmallTest | Level3)
{
    bool isPrivacyMode = true;
    window_->surfaceNode_ = nullptr;
    auto ret = window_->SetPrivacyMode(isPrivacyMode);

    struct RSSurfaceNodeConfig config;

    if (ret == WMError::WM_ERROR_NULLPTR) {
        window_->surfaceNode_ = RSSurfaceNode::Create(config);
    }
    ASSERT_NE(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetPrivacyMod03
 * @tc.desc: SetPrivacyMod03
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetPrivacyMod03, Function | SmallTest | Level3)
{
    struct RSSurfaceNodeConfig config;
    window_->surfaceNode_ = RSSurfaceNode::Create(config);
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(WMError::WM_OK, window_->SetPrivacyMode(true));
}

/**
 * @tc.name: SetPrivacyMod04
 * @tc.desc: SetPrivacyMod04
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetPrivacyMod04, Function | SmallTest | Level3)
{
    struct RSSurfaceNodeConfig config;
    window_->surfaceNode_ = RSSurfaceNode::Create(config);
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(WMError::WM_ERROR_INVALID_WINDOW, window_->SetPrivacyMode(false));
}

/**
 * @tc.name: SetPrivacyMod05
 * @tc.desc: SetPrivacyMod05
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetPrivacyMod05, Function | SmallTest | Level3)
{
    struct RSSurfaceNodeConfig config;
    window_->surfaceNode_ = RSSurfaceNode::Create(config);
    window_->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(WMError::WM_OK, window_->SetPrivacyMode(true));
}

/**
 * @tc.name: NotifyFocusStateEvent01
 * @tc.desc: NotifyFocusStateEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusStateEvent01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->NotifyFocusStateEvent(false);
}

/**
 * @tc.name: NotifyFocusStateEvent02
 * @tc.desc: NotifyFocusStateEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusStateEvent02, Function | SmallTest | Level3)
{
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    window_->NotifyFocusStateEvent(true);
}

/**
 * @tc.name: NotifyFocusActiveEvent01
 * @tc.desc: NotifyFocusActiveEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusActiveEvent01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->NotifyFocusActiveEvent(false);
}

/**
 * @tc.name: NotifyFocusActiveEvent02
 * @tc.desc: NotifyFocusActiveEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusActiveEvent02, Function | SmallTest | Level3)
{
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    window_->NotifyFocusActiveEvent(true);
}

/**
 * @tc.name: NotifyBackpressedEvent01
 * @tc.desc: NotifyFocusActiveEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyBackpressedEvent01, Function | SmallTest | Level3)
{
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    bool isConsumed = false;
    window_->NotifyBackpressedEvent(isConsumed);
}

/**
 * @tc.name: NotifyBackpressedEvent02
 * @tc.desc: NotifyFocusActiveEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyBackpressedEvent02, Function | SmallTest | Level3)
{
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    bool isConsumed = true;
    window_->NotifyBackpressedEvent(isConsumed);
}

/**
 * @tc.name: InputMethodKeyEventResultCallback01
 * @tc.desc: InputMethodKeyEventResultCallback01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, InputMethodKeyEventResultCallback01, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    bool consumed = false;
    auto isConsumedPromise = std::make_shared<std::promise<bool>>();
    auto isTimeout = std::make_shared<bool>(false);
    ASSERT_NE(nullptr, window_);
    window_->InputMethodKeyEventResultCallback(keyEvent, consumed, isConsumedPromise, isTimeout);
}

/**
 * @tc.name: InputMethodKeyEventResultCallback02
 * @tc.desc: InputMethodKeyEventResultCallback02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, InputMethodKeyEventResultCallback02, Function | SmallTest | Level3)
{
    bool consumed = false;
    auto isConsumedPromise = std::make_shared<std::promise<bool>>();
    auto isTimeout = std::make_shared<bool>(false);
    ASSERT_NE(nullptr, window_);
    window_->InputMethodKeyEventResultCallback(nullptr, consumed, isConsumedPromise, isTimeout);
}

/**
 * @tc.name: InputMethodKeyEventResultCallback03
 * @tc.desc: InputMethodKeyEventResultCallback03 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, InputMethodKeyEventResultCallback03, Function | SmallTest | Level3)
{
    bool consumed = false;
    auto isTimeout = std::make_shared<bool>(false);
    ASSERT_NE(nullptr, window_);
    window_->InputMethodKeyEventResultCallback(nullptr, consumed, nullptr, isTimeout);
}

/**
 * @tc.name: InputMethodKeyEventResultCallback04
 * @tc.desc: InputMethodKeyEventResultCallback04 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, InputMethodKeyEventResultCallback04, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    bool consumed = false;
    auto isTimeout = std::make_shared<bool>(false);
    ASSERT_NE(nullptr, window_);
    window_->InputMethodKeyEventResultCallback(keyEvent, consumed, nullptr, isTimeout);
}

/**
 * @tc.name: InputMethodKeyEventResultCallback05
 * @tc.desc: InputMethodKeyEventResultCallback05 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, InputMethodKeyEventResultCallback05, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    bool consumed = false;
    auto isConsumedPromise = std::make_shared<std::promise<bool>>();
    ASSERT_NE(nullptr, window_);
    window_->InputMethodKeyEventResultCallback(keyEvent, consumed, isConsumedPromise, nullptr);
}

/**
 * @tc.name: InputMethodKeyEventResultCallback06
 * @tc.desc: InputMethodKeyEventResultCallback06 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, InputMethodKeyEventResultCallback06, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    auto isConsumedPromise = std::make_shared<std::promise<bool>>();
    auto isTimeout = std::make_shared<bool>(false);
    ASSERT_NE(nullptr, window_);
    window_->InputMethodKeyEventResultCallback(keyEvent, true, isConsumedPromise, isTimeout);
}

/**
 * @tc.name: InputMethodKeyEventResultCallback07
 * @tc.desc: InputMethodKeyEventResultCallback07 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, InputMethodKeyEventResultCallback07, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    auto isConsumedPromise = std::make_shared<std::promise<bool>>();
    auto isTimeout = std::make_shared<bool>(true);
    ASSERT_NE(nullptr, window_);
    window_->InputMethodKeyEventResultCallback(keyEvent, true, isConsumedPromise, isTimeout);
}

/**
 * @tc.name: NotifyKeyEvent01
 * @tc.desc: NotifyKeyEvent01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyKeyEvent01, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_FN);
    bool consumed = false;
    bool notifyInputMethod = true;
    ASSERT_NE(nullptr, window_);
    window_->NotifyKeyEvent(keyEvent, consumed, notifyInputMethod);
}

/**
 * @tc.name: NotifyKeyEvent02
 * @tc.desc: NotifyKeyEvent02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyKeyEvent02, Function | SmallTest | Level3)
{
    bool consumed = false;
    bool notifyInputMethod = true;
    ASSERT_NE(nullptr, window_);
    window_->NotifyKeyEvent(nullptr, consumed, notifyInputMethod);
}

/**
 * @tc.name: NotifyKeyEvent03
 * @tc.desc: NotifyKeyEvent03 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyKeyEvent03, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    bool consumed = false;
    bool notifyInputMethod = true;
    ASSERT_NE(nullptr, window_);
    window_->NotifyKeyEvent(keyEvent, consumed, notifyInputMethod);
}

/**
 * @tc.name: NotifyKeyEvent04
 * @tc.desc: NotifyKeyEvent04 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyKeyEvent04, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_FN);
    bool consumed = false;
    bool notifyInputMethod = false;
    ASSERT_NE(nullptr, window_);
    window_->NotifyKeyEvent(keyEvent, consumed, notifyInputMethod);
}

/**
 * @tc.name: UpdateRect01
 * @tc.desc: UpdateRect Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateRect01, Function | SmallTest | Level2)
{
    WSRect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.height_ = 50;
    rect.width_ = 50;

    Rect preRect;
    preRect.posX_ = 0;
    preRect.posY_ = 0;
    preRect.height_ = 200;
    preRect.width_ = 200;

    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetWindowRect(preRect);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    ASSERT_EQ(WSError::WS_OK, window_->UpdateRect(rect, reason));

    preRect.height_ = 50;
    window_->property_->SetWindowRect(preRect);
    ASSERT_EQ(WSError::WS_OK, window_->UpdateRect(rect, reason));

    preRect.height_ = 200;
    preRect.width_ = 50;
    window_->property_->SetWindowRect(preRect);
    ASSERT_EQ(WSError::WS_OK, window_->UpdateRect(rect, reason));

    preRect.height_ = 50;
    preRect.width_ = 50;
    window_->property_->SetWindowRect(preRect);
    reason = SizeChangeReason::ROTATION;
    ASSERT_EQ(WSError::WS_OK, window_->UpdateRect(rect, reason));
}

/**
 * @tc.name: UpdateRectForRotation01
 * @tc.desc: UpdateRect Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateRectForRotation01, Function | SmallTest | Level2)
{
    Rect rect;
    WindowSizeChangeReason wmReason = WindowSizeChangeReason{0};
    std::shared_ptr<RSTransaction> rsTransaction;
    ASSERT_NE(nullptr, window_);
    window_->UpdateRectForRotation(rect, rect, wmReason, rsTransaction);
}

/**
 * @tc.name: NotifyAccessibilityHoverEvent01
 * @tc.desc: NotifyAccessibilityHoverEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyAccessibilityHoverEvent01, Function | SmallTest | Level3)
{
    float pointX = 0.0f;
    float pointY = 0.0f;
    int32_t sourceType = 0;
    int32_t eventType = 0;
    int64_t timeMs = 0;
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    ASSERT_EQ(WSError::WS_OK,
        window_->NotifyAccessibilityHoverEvent(pointX, pointY, sourceType, eventType, timeMs));
}

/**
 * @tc.name: NotifyAccessibilityHoverEvent02
 * @tc.desc: NotifyAccessibilityHoverEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyAccessibilityHoverEvent02, Function | SmallTest | Level3)
{
    float pointX = 0.0f;
    float pointY = 0.0f;
    int32_t sourceType = 0;
    int32_t eventType = 0;
    int64_t timeMs = 0;
    window_->uiContent_ = nullptr;
    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        window_->NotifyAccessibilityHoverEvent(pointX, pointY, sourceType, eventType, timeMs));
}

/**
 * @tc.name: NotifyAccessibilityChildTreeRegister01
 * @tc.desc: NotifyAccessibilityChildTreeRegister Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyAccessibilityChildTreeRegister01, Function | SmallTest | Level3)
{
    uint32_t windowId = 0;
    int32_t treeId = 0;
    int64_t accessibilityId = 0;
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    ASSERT_EQ(WSError::WS_OK, window_->NotifyAccessibilityChildTreeRegister(windowId, treeId, accessibilityId));
}

/**
 * @tc.name: NotifyAccessibilityChildTreeUnregister01
 * @tc.desc: NotifyAccessibilityChildTreeUnregister Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyAccessibilityChildTreeUnregister01, Function | SmallTest | Level3)
{
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    ASSERT_EQ(WSError::WS_OK, window_->NotifyAccessibilityChildTreeUnregister());
}

/**
 * @tc.name: NotifyAccessibilityDumpChildInfo01
 * @tc.desc: NotifyAccessibilityDumpChildInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyAccessibilityDumpChildInfo01, Function | SmallTest | Level3)
{
    std::vector<std::string> params;
    std::vector<std::string> info;
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    ASSERT_EQ(WSError::WS_OK, window_->NotifyAccessibilityDumpChildInfo(params, info));
}

/**
 * @tc.name: NotifyOccupiedAreaChangeInfo01
 * @tc.desc: NotifyExecuteAction Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyOccupiedAreaChangeInfo01, Function | SmallTest | Level3)
{
    sptr<OccupiedAreaChangeInfo> info = new(std::nothrow) OccupiedAreaChangeInfo();
    ASSERT_NE(nullptr, info);
    window_->NotifyOccupiedAreaChangeInfo(info);
}

/**
 * @tc.name: NotifyOccupiedAreaChangeInfo02
 * @tc.desc: NotifyExecuteAction Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyOccupiedAreaChangeInfo02, Function | SmallTest | Level3)
{
    sptr<IOccupiedAreaChangeListener> iOccupiedAreaChangeListener = new(std::nothrow) IOccupiedAreaChangeListener();
    ASSERT_NE(nullptr, iOccupiedAreaChangeListener);
    window_->RegisterOccupiedAreaChangeListener(iOccupiedAreaChangeListener);
    sptr<OccupiedAreaChangeInfo> info = new(std::nothrow) OccupiedAreaChangeInfo();
    ASSERT_NE(nullptr, info);
    window_->NotifyOccupiedAreaChangeInfo(info);
}

/**
 * @tc.name: GetAvoidAreaByType01
 * @tc.desc: NotifyExecuteAction Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetAvoidAreaByType01, Function | SmallTest | Level3)
{
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    AvoidArea avoidArea;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->GetAvoidAreaByType(avoidAreaType, avoidArea));
}

/**
 * @tc.name: GetAvoidAreaByType02
 * @tc.desc: NotifyExecuteAction Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetAvoidAreaByType02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    AvoidArea avoidArea;
    ASSERT_EQ(WMError::WM_OK, window_->GetAvoidAreaByType(avoidAreaType, avoidArea));
}

/**
 * @tc.name: RegisterAvoidAreaChangeListener
 * @tc.desc: RegisterAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterAvoidAreaChangeListener, Function | SmallTest | Level3)
{
    sptr<IAvoidAreaChangedListener> listener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->RegisterAvoidAreaChangeListener(listener));
}

/**
 * @tc.name: UnregisterAvoidAreaChangeListener
 * @tc.desc: UnregisterAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UnregisterAvoidAreaChangeListener, Function | SmallTest | Level3)
{
    sptr<IAvoidAreaChangedListener> listener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->UnregisterAvoidAreaChangeListener(listener));
}

/**
 * @tc.name: Show
 * @tc.desc: Show
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Show, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->persistentId_ = 12345;

    SessionInfo sessionInfo;
    sptr<SessionMocker> mockHostSession = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(mockHostSession, nullptr);
    window_->hostSession_ = mockHostSession;

    window_->property_->displayId_ = DISPLAY_ID_INVALID;
    EXPECT_CALL(*mockHostSession, Foreground).Times(0).WillOnce(Return(WSError::WS_OK));
    auto res = window_->Show();
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: Hide
 * @tc.desc: Hide
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Hide, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_->property_);

    SessionInfo sessionInfo;
    sptr<SessionMocker> mockHostSession = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(mockHostSession, nullptr);
    window_->hostSession_ = mockHostSession;

    window_->property_->persistentId_ = INVALID_SESSION_ID;
    auto res = window_->Hide(0, false, false);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);

    window_->property_->persistentId_ = 12345;
    window_->state_ = WindowState::STATE_HIDDEN;
    EXPECT_CALL(*mockHostSession, Background).Times(0);
    res = window_->Hide(0, false, false);
    ASSERT_EQ(res, WMError::WM_OK);

    window_->state_ = WindowState::STATE_CREATED;
    EXPECT_CALL(*mockHostSession, Background).Times(0);
    res = window_->Hide(0, false, false);
    ASSERT_EQ(res, WMError::WM_OK);

    window_->state_ = WindowState::STATE_SHOWN;
    EXPECT_CALL(*mockHostSession, Background).Times(1).WillOnce(Return(WSError::WS_OK));
    res = window_->Hide(0, false, false);
    ASSERT_EQ(res, WMError::WM_OK);
    ASSERT_EQ(window_->state_, WindowState::STATE_HIDDEN);

    window_->state_ = WindowState::STATE_SHOWN;
    EXPECT_CALL(*mockHostSession, Background).Times(1).WillOnce(Return(WSError::WS_DO_NOTHING));
    res = window_->Hide(0, false, false);
    ASSERT_EQ(res, WMError::WM_OK);
    ASSERT_EQ(window_->state_, WindowState::STATE_SHOWN);
}

/**
 * @tc.name: NotifyDensityFollowHost01
 * @tc.desc: test isFollowHost is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost01, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetDisplayId(displayId);

    auto isFollowHost = true;
    auto densityValue = 0.1f;

    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window_->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(Field(&Ace::ViewportConfig::density_, densityValue), _, _));

    ASSERT_EQ(window_->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: NotifyDensityFollowHost02
 * @tc.desc: test isFollowHost is true -> false
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost02, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetDisplayId(displayId);

    auto isFollowHost = false;
    auto densityValue = 0.1f;

    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(window_->property_->GetDisplayId());
    ASSERT_NE(display, nullptr);
    ASSERT_NE(display->GetDisplayInfo(), nullptr);
    auto vpr = display->GetDisplayInfo()->GetVirtualPixelRatio();

    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window_->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(Field(&Ace::ViewportConfig::density_, vpr), _, _));

    window_->isDensityFollowHost_ = true;
    ASSERT_EQ(window_->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: NotifyDensityFollowHost03
 * @tc.desc: test isFollowHost not change
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost03, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetDisplayId(displayId);

    auto isFollowHost = false;
    auto densityValue = 0.1f;
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window_->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(_, _, _)).Times(0);

    ASSERT_EQ(window_->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: NotifyDensityFollowHost04
 * @tc.desc: test densityValue invalid
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost04, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetDisplayId(displayId);

    auto isFollowHost = true;
    auto densityValue = 0.0f;
    ASSERT_EQ(window_->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_ERROR_INVALID_PARAM);
    densityValue = -0.1f;
    ASSERT_EQ(window_->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: NotifyDensityFollowHost05
 * @tc.desc: test densityValue not change
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost05, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetDisplayId(displayId);

    auto isFollowHost = true;
    auto densityValue = 0.1f;
    window_->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window_->uiContent_);
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window_->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(_, _, _)).Times(0);

    window_->hostDensityValue_ = densityValue;
    ASSERT_EQ(window_->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: GetVirtualPixelRatio01
 * @tc.desc: follow host density value
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetVirtualPixelRatio01, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetVirtualPixelRatio(3.25f);
    window_->isDensityFollowHost_ = true;
    window_->hostDensityValue_ = 2.0f;
    ASSERT_EQ(window_->hostDensityValue_, window_->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio02
 * @tc.desc: follow system density value
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetVirtualPixelRatio02, Function | SmallTest | Level2)
{
    auto systemDensity = 3.25;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetVirtualPixelRatio(systemDensity);
    window_->isDensityFollowHost_ = false;
    window_->hostDensityValue_ = 2.0f;
    ASSERT_EQ(systemDensity, window_->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio03
 * @tc.desc: hostDensityValue_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetVirtualPixelRatio03, Function | SmallTest | Level2)
{
    auto systemDensity = 3.25;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetVirtualPixelRatio(systemDensity);
    window_->isDensityFollowHost_ = true;
    ASSERT_EQ(systemDensity, window_->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio04
 * @tc.desc: GetVirtualPixelRatio04 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetVirtualPixelRatio04, Function | SmallTest | Level2)
{
    ASSERT_EQ(1.0f, window_->GetVirtualPixelRatio(nullptr));
}

/**
 * @tc.name: HideNonSecureWindows01
 * @tc.desc: HideNonSecureWindows Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->HideNonSecureWindows(false));
}
 
/**
 * @tc.name: HideNonSecureWindows02
 * @tc.desc: HideNonSecureWindows Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows02, Function | SmallTest | Level3)
{
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, window_->HideNonSecureWindows(false));
}
 
/**
 * @tc.name: HideNonSecureWindows03
 * @tc.desc: HideNonSecureWindows Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows03, Function | SmallTest | Level3)
{
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window_->HideNonSecureWindows(true));
}
 
/**
 * @tc.name: HideNonSecureWindows04
 * @tc.desc: HideNonSecureWindows Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows04, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->HideNonSecureWindows(true));
}

/**
 * @tc.name: HideNonSecureWindows06
 * @tc.desc: HideNonSecureWindows Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows06, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetUIExtensionUsage(UIExtensionUsage::MODAL);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window_->HideNonSecureWindows(false));
}

/**
 * @tc.name: SetWaterMarkFlag01
 * @tc.desc: SetWaterMarkFlag Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetWaterMarkFlag01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->SetWaterMarkFlag(false));
}
 
/**
 * @tc.name: SetWaterMarkFlag02
 * @tc.desc: SetWaterMarkFlag Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetWaterMarkFlag02, Function | SmallTest | Level3)
{
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, window_->SetWaterMarkFlag(false));
}

/**
 * @tc.name: SetWaterMarkFlag03
 * @tc.desc: SetWaterMarkFlag Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetWaterMarkFlag03, Function | SmallTest | Level3)
{
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window_->SetWaterMarkFlag(true));
}

/**
 * @tc.name: SetWaterMarkFlag04
 * @tc.desc: SetWaterMarkFlag Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetWaterMarkFlag04, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->SetWaterMarkFlag(true));
}

/**
 * @tc.name: CheckAndAddExtWindowFlags01
 * @tc.desc: CheckAndAddExtWindowFlags01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckAndAddExtWindowFlags01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->CheckAndAddExtWindowFlags();
}

/**
 * @tc.name: CheckAndAddExtWindowFlags02
 * @tc.desc: CheckAndAddExtWindowFlags02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckAndAddExtWindowFlags02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->extensionWindowFlags_.bitData = 1;
    window_->CheckAndAddExtWindowFlags();
}

/**
 * @tc.name: CheckAndRemoveExtWindowFlags01
 * @tc.desc: CheckAndRemoveExtWindowFlags01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckAndRemoveExtWindowFlags01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->CheckAndRemoveExtWindowFlags();
}

/**
 * @tc.name: CheckAndRemoveExtWindowFlags02
 * @tc.desc: CheckAndRemoveExtWindowFlags02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckAndRemoveExtWindowFlags02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, window_);
    window_->extensionWindowFlags_.bitData = 1;
    window_->CheckAndRemoveExtWindowFlags();
}

/**
 * @tc.name: UpdateExtWindowFlags01
 * @tc.desc: UpdateExtWindowFlags Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateExtWindowFlags01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window_->UpdateExtWindowFlags(ExtensionWindowFlags(),
        ExtensionWindowFlags()));
}

/**
 * @tc.name: UpdateExtWindowFlags02
 * @tc.desc: UpdateExtWindowFlags Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateExtWindowFlags02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->UpdateExtWindowFlags(ExtensionWindowFlags(), ExtensionWindowFlags()));
}

/**
 * @tc.name: GetHostWindowRect01
 * @tc.desc: GetHostWindowRect Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetHostWindowRect01, Function | SmallTest | Level3)
{
    Rect rect;
    ASSERT_EQ(rect, window_->GetHostWindowRect(-1));
}

/**
 * @tc.name: GetHostWindowRect02
 * @tc.desc: GetHostWindowRect Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetHostWindowRect02, Function | SmallTest | Level3)
{
    Rect rect;
    ASSERT_EQ(rect, window_->GetHostWindowRect(0));
}

/**
 * @tc.name: ConsumePointerEvent
 * @tc.desc: ConsumePointerEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, ConsumePointerEvent, Function | SmallTest | Level3)
{
    struct RSSurfaceNodeConfig config;
    window_->surfaceNode_ = RSSurfaceNode::Create(config);
    window_->state_ = WindowState::STATE_SHOWN;

    auto pointerEvent = MMI::PointerEvent::Create();
    window_->ConsumePointerEvent(nullptr);

    window_->ConsumePointerEvent(pointerEvent);

    SessionInfo sessionInfo;
    window_->hostSession_ = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    window_->ConsumePointerEvent(pointerEvent);

    MMI::PointerEvent::PointerItem item;
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    item.SetPointerId(0);
    item.SetDisplayX(15); // 15 : position x
    item.SetDisplayY(15); // 15 : position y
    item.SetWindowX(15); // 15 : position x
    item.SetWindowY(15); // 15 : position y
    pointerEvent->AddPointerItem(item);
    window_->ConsumePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_PULL_MOVE);
    pointerEvent->UpdatePointerItem(0, item);
    window_->ConsumePointerEvent(pointerEvent);
}

/**
 * @tc.name: PreNotifyKeyEvent
 * @tc.desc: PreNotifyKeyEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, PreNotifyKeyEvent, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(nullptr, keyEvent);
    ASSERT_NE(nullptr, window_->property_);
    window_->property_->SetUIExtensionUsage(UIExtensionUsage::MODAL);
    bool ret = window_->PreNotifyKeyEvent(keyEvent);
    ASSERT_EQ(ret, false);

    window_->property_->SetUIExtensionUsage(UIExtensionUsage::CONSTRAINED_EMBEDDED);
    ret = window_->PreNotifyKeyEvent(keyEvent);
    ASSERT_EQ(ret, true);

    window_->focusState_ = false;
    ret = window_->PreNotifyKeyEvent(keyEvent);
    ASSERT_EQ(ret, true);

    window_->focusState_ = true;
    ret = window_->PreNotifyKeyEvent(keyEvent);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckHideNonSecureWindowsPermission
 * @tc.desc: CheckHideNonSecureWindowsPermission Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckHideNonSecureWindowsPermission, Function | SmallTest | Level3)
{
    ASSERT_NE(window_->property_, nullptr);

    window_->property_->uiExtensionUsage_ = UIExtensionUsage::EMBEDDED;
    EXPECT_EQ(window_->CheckHideNonSecureWindowsPermission(true), WMError::WM_OK);
    EXPECT_EQ(window_->CheckHideNonSecureWindowsPermission(false), WMError::WM_OK);

    window_->property_->uiExtensionUsage_ = UIExtensionUsage::MODAL;
    EXPECT_EQ(window_->CheckHideNonSecureWindowsPermission(true), WMError::WM_OK);
    EXPECT_EQ(window_->CheckHideNonSecureWindowsPermission(false), WMError::WM_ERROR_INVALID_OPERATION);

    window_->property_->uiExtensionUsage_ = UIExtensionUsage::CONSTRAINED_EMBEDDED;
    window_->modalUIExtensionMayBeCovered_ = true;
    EXPECT_EQ(window_->CheckHideNonSecureWindowsPermission(true), WMError::WM_OK);
    EXPECT_EQ(window_->CheckHideNonSecureWindowsPermission(false), WMError::WM_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: NotifyModalUIExtensionMayBeCovered
 * @tc.desc: NotifyModalUIExtensionMayBeCovered Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyModalUIExtensionMayBeCovered, Function | SmallTest | Level3)
{
    ASSERT_NE(window_, nullptr);
    ASSERT_NE(window_->property_, nullptr);

    window_->property_->uiExtensionUsage_ = UIExtensionUsage::EMBEDDED;
    window_->NotifyModalUIExtensionMayBeCovered(true);

    window_->property_->uiExtensionUsage_ = UIExtensionUsage::MODAL;
    window_->extensionWindowFlags_.hideNonSecureWindowsFlag = true;
    window_->NotifyModalUIExtensionMayBeCovered(true);

    window_->property_->uiExtensionUsage_ = UIExtensionUsage::CONSTRAINED_EMBEDDED;
    window_->extensionWindowFlags_.hideNonSecureWindowsFlag = false;
    window_->NotifyModalUIExtensionMayBeCovered(false);
}

/**
 * @tc.name: ReportModalUIExtensionMayBeCovered
 * @tc.desc: ReportModalUIExtensionMayBeCovered Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, ReportModalUIExtensionMayBeCovered, Function | SmallTest | Level3)
{
    ASSERT_NE(window_, nullptr);
    window_->ReportModalUIExtensionMayBeCovered(true);
    window_->NotifyModalUIExtensionMayBeCovered(false);
}
}
} // namespace Rosen
} // namespace OHOS
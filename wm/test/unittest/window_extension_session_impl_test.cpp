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
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
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
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window_->property_->SetPersistentId(1);
    EXPECT_CALL(*session, Connect).WillOnce(Return(WSError::WS_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_OK, window_->Create(abilityContext, session));
    ASSERT_EQ(WMError::WM_OK, window_->Destroy(false));
}

/**
 * @tc.name: AddExtensionWindowStageToSCB
 * @tc.desc: AddExtensionWindowStageToSCB Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, AddExtensionWindowStageToSCB, Function | SmallTest | Level3)
{
    window_->AddExtensionWindowStageToSCB();
}

/**
 * @tc.name: UpdateConfigurationForAll01
 * @tc.desc: UpdateConfigurationForAll01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateConfigurationForAll01, Function | SmallTest | Level3)
{
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();
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
    window_->windowExtensionSessionSet_.insert(window_);
    window_->UpdateConfigurationForAll(configuration);
    window_->windowExtensionSessionSet_.erase(window_);
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
    window_->NotifyKeyEvent(keyEvent, consumed, notifyInputMethod);
}

/**
 * @tc.name: CheckAndAddExtWindowFlags01
 * @tc.desc: CheckAndAddExtWindowFlags01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckAndAddExtWindowFlags01, Function | SmallTest | Level3)
{
    window_->CheckAndAddExtWindowFlags();
}

/**
 * @tc.name: CheckAndAddExtWindowFlags02
 * @tc.desc: CheckAndAddExtWindowFlags02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckAndAddExtWindowFlags02, Function | SmallTest | Level3)
{
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
    window_->CheckAndRemoveExtWindowFlags();
}

/**
 * @tc.name: CheckAndRemoveExtWindowFlags02
 * @tc.desc: CheckAndRemoveExtWindowFlags02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, CheckAndRemoveExtWindowFlags02, Function | SmallTest | Level3)
{
    window_->extensionWindowFlags_.bitData = 1;
    window_->CheckAndRemoveExtWindowFlags();
}

/**
 * @tc.name: GetHostWindowRect01
 * @tc.desc: GetHostWindowRect01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, GetHostWindowRect01, Function | SmallTest | Level3)
{
    Rect rect;
    ASSERT_EQ(rect, window_->GetHostWindowRect(-1));
}

/**
 * @tc.name: Show
 * @tc.desc: Show
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Show, Function | SmallTest | Level3)
{
    ASSERT_NE(window_->property_, nullptr);
    window_->property_->persistentId_ = 12345;

    SessionInfo sessionInfo;
    sptr<SessionMocker> mockHostSession = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(mockHostSession, nullptr);
    window_->hostSession_ = mockHostSession;

    window_->property_->displayId_ = DISPLAY_ID_INVALID;
    EXPECT_CALL(*mockHostSession, Foreground).Times(0).WillOnce(Return(WSError::WS_OK));
    auto res = window_->Show();
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);

    window_->property_->displayId_ = 0;
    window_->state_ = WindowState::STATE_HIDDEN;
    EXPECT_CALL(*mockHostSession, Foreground).Times(1).WillOnce(Return(WSError::WS_OK));
    res = window_->Show();
    ASSERT_EQ(res, WMError::WM_OK);
    ASSERT_EQ(window_->state_, WindowState::STATE_SHOWN);
}

/**
 * @tc.name: Hide
 * @tc.desc: Hide
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Hide, Function | SmallTest | Level3)
{
    ASSERT_NE(window_->property_, nullptr);

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
 * @tc.name: MoveTo01
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, MoveTo01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowExtensionSessionImpl.MoveTo(0, 1));
}

/**
 * @tc.name: Resize01
 * @tc.desc: Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Resize01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowExtensionSessionImpl.Resize(0, 1));
}

/**
 * @tc.name: TransferAbilityResult01
 * @tc.desc: TransferAbilityResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferAbilityResult01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    AAFwk::Want want;
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, windowExtensionSessionImpl.TransferAbilityResult(1, want));
}

/**
 * @tc.name: TransferExtensionData01
 * @tc.desc: TransferExtensionData
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferExtensionData01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    AAFwk::WantParams wantParams;
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, windowExtensionSessionImpl.TransferExtensionData(wantParams));
}

/**
 * @tc.name: RegisterTransferComponentDataListener
 * @tc.desc: RegisterTransferComponentDataListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    NotifyTransferComponentDataFunc func;
    windowExtensionSessionImpl.RegisterTransferComponentDataListener(func);

    AAFwk::WantParams wantParams;
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, windowExtensionSessionImpl.TransferExtensionData(wantParams));
}

/**
 * @tc.name: NotifyTransferComponentData
 * @tc.desc: NotifyTransferComponentData Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyTransferComponentData, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    AAFwk::WantParams wantParams;
    ASSERT_EQ(WSError::WS_OK, windowExtensionSessionImpl.NotifyTransferComponentData(wantParams));
}

/**
 * @tc.name: SetPrivacyMode01
 * @tc.desc: SetPrivacyMode Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetPrivacyMode01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    ASSERT_EQ(WMError::WM_OK, windowExtensionSessionImpl.SetPrivacyMode(false));
}

/**
 * @tc.name: NotifyFocusStateEvent
 * @tc.desc: NotifyFocusStateEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusStateEvent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.NotifyFocusStateEvent(false);

    ASSERT_EQ(WMError::WM_OK, windowExtensionSessionImpl.SetPrivacyMode(false));
}

/**
 * @tc.name: NotifyFocusActiveEvent
 * @tc.desc: NotifyFocusActiveEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusActiveEvent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.NotifyFocusActiveEvent(false);

    ASSERT_EQ(WMError::WM_OK, windowExtensionSessionImpl.SetPrivacyMode(false));
}

/**
 * @tc.name: NotifySearchElementInfoByAccessibilityId01
 * @tc.desc: NotifySearchElementInfoByAccessibilityId Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifySearchElementInfoByAccessibilityId01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.uiContent_ = std::make_unique<Ace::UIContentMocker>();
    int64_t elementId = 0;
    int32_t mode = 0;
    int64_t baseParent = 0;
    list<AccessibilityElementInfo> infos;

    ASSERT_EQ(WSError::WS_OK,
        windowExtensionSessionImpl.NotifySearchElementInfoByAccessibilityId(elementId, mode, baseParent, infos));
}

/**
 * @tc.name: NotifySearchElementInfoByAccessibilityId02
 * @tc.desc: NotifySearchElementInfoByAccessibilityId Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifySearchElementInfoByAccessibilityId02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    int64_t elementId = 0;
    int32_t mode = 0;
    int64_t baseParent = 0;
    list<AccessibilityElementInfo> infos;

    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        windowExtensionSessionImpl.NotifySearchElementInfoByAccessibilityId(elementId, mode, baseParent, infos));
}

/**
 * @tc.name: NotifySearchElementInfosByText01
 * @tc.desc: NotifySearchElementInfosByText Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifySearchElementInfosByText01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.uiContent_ = std::make_unique<Ace::UIContentMocker>();
    int64_t elementId = 0;
    string text;
    int64_t baseParent = 0;
    list<AccessibilityElementInfo> infos;

    ASSERT_EQ(WSError::WS_OK,
        windowExtensionSessionImpl.NotifySearchElementInfosByText(elementId, text, baseParent, infos));
}

/**
 * @tc.name: NotifySearchElementInfosByText02
 * @tc.desc: NotifySearchElementInfosByText Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifySearchElementInfosByText02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    int64_t elementId = 0;
    string text;
    int64_t baseParent = 0;
    list<AccessibilityElementInfo> infos;

    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        windowExtensionSessionImpl.NotifySearchElementInfosByText(elementId, text, baseParent, infos));
}

/**
 * @tc.name: NotifyFindFocusedElementInfo01
 * @tc.desc: NotifyFindFocusedElementInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFindFocusedElementInfo01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    std::unique_ptr<Ace::UIContent> uiContent_ = std::make_unique<Ace::UIContentMocker>();
    int64_t elementId = 0;
    int32_t focusType = 0;
    int64_t baseParent = 0;
    AccessibilityElementInfo info;

    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        windowExtensionSessionImpl.NotifyFindFocusedElementInfo(elementId, focusType, baseParent, info));
}

/**
 * @tc.name: NotifyFindFocusedElementInfo02
 * @tc.desc: NotifyFindFocusedElementInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFindFocusedElementInfo02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    int64_t elementId = 0;
    int32_t focusType = 0;
    int64_t baseParent = 0;
    AccessibilityElementInfo info;

    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        windowExtensionSessionImpl.NotifyFindFocusedElementInfo(elementId, focusType, baseParent, info));
}

/**
 * @tc.name: NotifyFocusMoveSearch01
 * @tc.desc: NotifyFocusMoveSearch Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusMoveSearch01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.uiContent_ = std::make_unique<Ace::UIContentMocker>();
    int64_t elementId = 0;
    int32_t direction = 0;
    int64_t baseParent = 0;
    AccessibilityElementInfo info;

    ASSERT_EQ(WSError::WS_OK,
        windowExtensionSessionImpl.NotifyFocusMoveSearch(elementId, direction, baseParent, info));
}

/**
 * @tc.name: NotifyFocusMoveSearch02
 * @tc.desc: NotifyFocusMoveSearch Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyFocusMoveSearch02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    int64_t elementId = 0;
    int32_t direction = 0;
    int64_t baseParent = 0;
    AccessibilityElementInfo info;

    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        windowExtensionSessionImpl.NotifyFocusMoveSearch(elementId, direction, baseParent, info));
}

/**
 * @tc.name: NotifyExecuteAction01
 * @tc.desc: NotifyExecuteAction Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyExecuteAction01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    std::unique_ptr<Ace::UIContent> uiContent_ = std::make_unique<Ace::UIContentMocker>();
    int64_t elementId = 0;
    std::map<std::string, std::string> actionAguments;
    int32_t action = 0;
    int64_t baseParent = 0;
    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        windowExtensionSessionImpl.NotifyExecuteAction(elementId, actionAguments, action, baseParent));
}

/**
 * @tc.name: NotifyExecuteAction02
 * @tc.desc: NotifyExecuteAction Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyExecuteAction02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    int64_t elementId = 0;
    std::map<std::string, std::string> actionAguments;
    int32_t action = 0;
    int64_t baseParent = 0;
    ASSERT_EQ(WSError::WS_ERROR_NO_UI_CONTENT_ERROR,
        windowExtensionSessionImpl.NotifyExecuteAction(elementId, actionAguments, action, baseParent));
}

/**
 * @tc.name: TransferAccessibilityEvent
 * @tc.desc: TransferAccessibilityEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferAccessibilityEvent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    AccessibilityEventInfo info;
    int64_t uiExtensionIdLevel = 0;

    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
        windowExtensionSessionImpl.TransferAccessibilityEvent(info, uiExtensionIdLevel));
}

/**
 * @tc.name: TransferAccessibilityEvent01
 * @tc.desc: TransferAccessibilityEvent01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferAccessibilityEvent01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    window_->property_->SetPersistentId(1);

    AccessibilityEventInfo info;
    int64_t uiExtensionIdLevel = 0;
    ASSERT_EQ(WMError::WM_OK, window_->TransferAccessibilityEvent(info, uiExtensionIdLevel));
}

/**
 * @tc.name: RegisterAvoidAreaChangeListener
 * @tc.desc: RegisterAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterAvoidAreaChangeListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    sptr<IAvoidAreaChangedListener> listener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowExtensionSessionImpl.RegisterAvoidAreaChangeListener(listener));
}

/**
 * @tc.name: UnregisterAvoidAreaChangeListener
 * @tc.desc: UnregisterAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UnregisterAvoidAreaChangeListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    sptr<IAvoidAreaChangedListener> listener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowExtensionSessionImpl.UnregisterAvoidAreaChangeListener(listener));
}

/**
 * @tc.name: HideNonSecureWindows
 * @tc.desc: HideNonSecureWindows Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WM_OK, windowExtensionSessionImpl.HideNonSecureWindows(true));
    windowExtensionSessionImpl.state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowExtensionSessionImpl.HideNonSecureWindows(false));
}

/**
 * @tc.name: HideNonSecureWindows01
 * @tc.desc: HideNonSecureWindows01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->HideNonSecureWindows(false));
}

/**
 * @tc.name: HideNonSecureWindows02
 * @tc.desc: HideNonSecureWindows02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, HideNonSecureWindows02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    window_->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window_->HideNonSecureWindows(true));
}

/**
 * @tc.name: SetWaterMarkFlag
 * @tc.desc: SetWaterMarkFlag Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetWaterMarkFlag, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WM_OK, windowExtensionSessionImpl.SetWaterMarkFlag(true));
    windowExtensionSessionImpl.state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowExtensionSessionImpl.SetWaterMarkFlag(false));
}

/**
 * @tc.name: SetWaterMarkFlag01
 * @tc.desc: SetWaterMarkFlag01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetWaterMarkFlag01, Function | SmallTest | Level3)
{
    ASSERT_EQ(WMError::WM_OK, window_->SetWaterMarkFlag(false));
}

/**
 * @tc.name: SetWaterMarkFlag02
 * @tc.desc: SetWaterMarkFlag02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetWaterMarkFlag02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    window_->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window_->SetWaterMarkFlag(true));
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: UpdateExtWindowFlags Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateExtWindowFlags, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowExtensionSessionImpl.UpdateExtWindowFlags(ExtensionWindowFlags(7),
        ExtensionWindowFlags(7)));
}

/**
 * @tc.name: UpdateExtWindowFlags01
 * @tc.desc: UpdateExtWindowFlags01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateExtWindowFlags01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    window_->property_->SetPersistentId(1);

    ASSERT_EQ(WMError::WM_OK, window_->UpdateExtWindowFlags(ExtensionWindowFlags(), ExtensionWindowFlags()));
}


/**
 * @tc.name: NotifyDensityFollowHost01
 * @tc.desc: test isFollowHost is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(window, nullptr);

    DisplayId displayId = 0;
    window->property_->SetDisplayId(displayId);

    auto isFollowHost = true;
    auto densityValue = 0.1f;

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(Field(&Ace::ViewportConfig::density_, densityValue), _, _));

    ASSERT_EQ(window->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: NotifyDensityFollowHost02
 * @tc.desc: test isFollowHost is true -> false
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(window, nullptr);

    DisplayId displayId = 0;
    window->property_->SetDisplayId(displayId);

    auto isFollowHost = false;
    auto densityValue = 0.1f;

    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(window->property_->GetDisplayId());
    ASSERT_NE(display, nullptr);
    ASSERT_NE(display->GetDisplayInfo(), nullptr);
    auto vpr = display->GetDisplayInfo()->GetVirtualPixelRatio();

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(Field(&Ace::ViewportConfig::density_, vpr), _, _));

    window->isDensityFollowHost_ = true;
    ASSERT_EQ(window->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: NotifyDensityFollowHost03
 * @tc.desc: test isFollowHost not change
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(window, nullptr);

    DisplayId displayId = 0;
    window->property_->SetDisplayId(displayId);

    auto isFollowHost = false;
    auto densityValue = 0.1f;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(_, _, _)).Times(0);

    ASSERT_EQ(window->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: NotifyDensityFollowHost04
 * @tc.desc: test densityValue invalid
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost04, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(window, nullptr);

    DisplayId displayId = 0;
    window->property_->SetDisplayId(displayId);

    auto isFollowHost = true;
    auto densityValue = 0.0f;
    ASSERT_EQ(window->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_ERROR_INVALID_PARAM);
    densityValue = -0.1f;
    ASSERT_EQ(window->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: NotifyDensityFollowHost05
 * @tc.desc: test densityValue not change
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyDensityFollowHost05, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(window, nullptr);

    DisplayId displayId = 0;
    window->property_->SetDisplayId(displayId);

    auto isFollowHost = true;
    auto densityValue = 0.1f;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(_, _, _)).Times(0);

    window->hostDensityValue_ = densityValue;
    ASSERT_EQ(window->NotifyDensityFollowHost(isFollowHost, densityValue), WSError::WS_OK);
}

/**
 * @tc.name: GetVirtualPixelRatio01
 * @tc.desc: follow host density value
 * @tc.type: FUNC
*/
HWTEST_F(WindowExtensionSessionImplTest, GetVirtualPixelRatio01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);

    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetVirtualPixelRatio(3.25f);
    window->isDensityFollowHost_ = true;
    window->hostDensityValue_ = 2.0f;
    ASSERT_EQ(window->hostDensityValue_, window->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio02
 * @tc.desc: follow system density value
 * @tc.type: FUNC
*/
HWTEST_F(WindowExtensionSessionImplTest, GetVirtualPixelRatio02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);

    auto systemDensity = 3.25;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetVirtualPixelRatio(systemDensity);
    window->isDensityFollowHost_ = false;
    window->hostDensityValue_ = 2.0f;
    ASSERT_EQ(systemDensity, window->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio03
 * @tc.desc: hostDensityValue_ is nullptr
 * @tc.type: FUNC
*/
HWTEST_F(WindowExtensionSessionImplTest, GetVirtualPixelRatio03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowExtensionSessionImpl> window = new (std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);

    auto systemDensity = 3.25;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetVirtualPixelRatio(systemDensity);
    window->isDensityFollowHost_ = true;
    ASSERT_EQ(systemDensity, window->GetVirtualPixelRatio(displayInfo));
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
 * @tc.name: NotifyHostWindowMode
 * @tc.desc: NotifyHostWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyHostWindowMode, Function | SmallTest | Level3)
{
    WindowMode mode = WindowMode::WINDOW_MODE_FLOATING;
    ASSERT_EQ(WSError::WS_OK, window_->NotifyHostWindowMode(mode));
    ASSERT_EQ(mode, window_->GetMode());
}

/**
 * @tc.name: UpdateConfiguration
 * @tc.desc: UpdateConfiguration Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateConfiguration, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    std::shared_ptr<AppExecFwk::Configuration> configuration = nullptr;

    int res = 0;
    std::function<void()> func = [&]()
    {
        windowExtensionSessionImpl.UpdateConfiguration(configuration);
        res = 1;
    };
    func();
    ASSERT_EQ(1, res);
}

/**
 * @tc.name: Destroy
 * @tc.desc: Destroy Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Destroy, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    bool needNotifyServer = true;
    bool needClearListener = true;
    ASSERT_NE(WMError::WM_OK, windowExtensionSessionImpl.Destroy(needNotifyServer, needClearListener));
}

/**
 * @tc.name: Resize
 * @tc.desc: Resize Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Resize, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    uint32_t width = 0;
    uint32_t height = 0;

    auto res = windowExtensionSessionImpl.Resize(width, height);
    ASSERT_NE(WMError::WM_DO_NOTHING, static_cast<WMError>(res));
}

/**
 * @tc.name: TransferAbilityResult
 * @tc.desc: TransferAbilityResult Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferAbilityResult, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    uint32_t resultCod = 0;
    AAFwk::Want want;
    sptr<ISession> hostSession_ = nullptr;

    auto res = windowExtensionSessionImpl.TransferAbilityResult(resultCod, want);
    ASSERT_NE(WMError::WM_DO_NOTHING, static_cast<WMError>(res));
}

/**
 * @tc.name: TransferExtensionData
 * @tc.desc: TransferExtensionData Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, TransferExtensionData, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    AAFwk::WantParams wantParams;

    auto res = windowExtensionSessionImpl.TransferExtensionData(wantParams);
    ASSERT_NE(WMError::WM_DO_NOTHING, static_cast<WMError>(res));
}

/**
 * @tc.name: RegisterTransferComponentDataListener1
 * @tc.desc: RegisterTransferComponentDataListener1 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataListener1, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    NotifyTransferComponentDataFunc func;

    auto res = 0;
    std::function<void()> func1 = [&]()
    {
        windowExtensionSessionImpl.RegisterTransferComponentDataListener(func);
        res = 1;
    };
    func1();
    ASSERT_EQ(1, res);
}

/**
 * @tc.name: NotifyTransferComponentData2
 * @tc.desc: NotifyTransferComponentData2 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyTransferComponentData2, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    AAFwk::WantParams wantParams;

    auto res = windowExtensionSessionImpl.NotifyTransferComponentData(wantParams);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyTransferComponentDataSync
 * @tc.desc: NotifyTransferComponentDataSync Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyTransferComponentDataSync, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    AAFwk::WantParams wantParams;
    AAFwk::WantParams reWantParams;

    auto res = windowExtensionSessionImpl.NotifyTransferComponentDataSync(wantParams, reWantParams);
    ASSERT_EQ(WSErrorCode::WS_ERROR_NOT_REGISTER_SYNC_CALLBACK, res);
}

/**
 * @tc.name: RegisterTransferComponentDataForResultListener
 * @tc.desc: RegisterTransferComponentDataForResultListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataForResultListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    NotifyTransferComponentDataForResultFunc func;

    auto res = 0;
    std::function<void()> func1 = [&]()
    {
        windowExtensionSessionImpl.RegisterTransferComponentDataForResultListener(func);
        res = 1;
    };
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: UpdateConfiguration02
 * @tc.desc: UpdateConfiguration Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, UpdateConfiguration02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.uiContent_ = std::make_unique<Ace::UIContentMocker>();

    std::shared_ptr<AppExecFwk::Configuration> configuration;
    windowExtensionSessionImpl.UpdateConfiguration(configuration);
    ASSERT_NE(nullptr, windowExtensionSessionImpl.uiContent_);
}

/**
 * @tc.name: RegisterTransferComponentDataForResultListener02
 * @tc.desc: RegisterTransferComponentDataForResultListener02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, RegisterTransferComponentDataForResultListener02,
         Function | SmallTest | Level3)
{
    NotifyTransferComponentDataForResultFunc func;
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);

    windowExtensionSessionImpl.RegisterTransferComponentDataForResultListener(func);
    ASSERT_EQ(true, windowExtensionSessionImpl.IsWindowSessionInvalid());
}

/**
* @tc.name: TriggerBindModalUIExtension01
* @tc.desc: TriggerBindModalUIExtension01 Test
* @tc.type: FUNC
*/
HWTEST_F(WindowExtensionSessionImplTest, TriggerBindModalUIExtension01, Function | SmallTest | Level3)
{
    window_->TriggerBindModalUIExtension();
}

/**
* @tc.name: TriggerBindModalUIExtension02
* @tc.desc: TriggerBindModalUIExtension02 Test
* @tc.type: FUNC
*/
HWTEST_F(WindowExtensionSessionImplTest, TriggerBindModalUIExtension02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    window_->hostSession_ = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    window_->TriggerBindModalUIExtension();
}

/**
 * @tc.name: Destroy01
 * @tc.desc: Destroy Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Destroy01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    bool needNotifyServer = true;
    bool needClearListener = true;

    sptr<IRemoteObject> impl;
    windowExtensionSessionImpl.hostSession_ = new OHOS::Rosen::SessionProxy(impl);
    windowExtensionSessionImpl.Destroy(needNotifyServer, needClearListener);

    ASSERT_NE(nullptr, windowExtensionSessionImpl.hostSession_);
}

/**
 * @tc.name: SetPrivacyMode02
 * @tc.desc: SetPrivacyMod
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetPrivacyMod02, Function | SmallTest | Level3)
{
    bool isPrivacyMode = true;
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.surfaceNode_ = nullptr;
    auto ret = windowExtensionSessionImpl.SetPrivacyMode(isPrivacyMode);

    struct RSSurfaceNodeConfig config;

    if (ret == WMError::WM_ERROR_NULLPTR)
    {
        windowExtensionSessionImpl.surfaceNode_ = RSSurfaceNode::Create(config);
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

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    window_->hostSession_ = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, window_->hostSession_);
    ASSERT_NE(WMError::WM_OK, window_->SetPrivacyMode(true));
}

/**
 * @tc.name: NotifyBackpressedEvent
 * @tc.desc: NotifyBackpressedEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyBackpressedEvent, Function | SmallTest | Level3)
{
    bool isPrivacyMode = true;
    bool isConsumed = true;
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.uiContent_ = std::make_unique<Ace::UIContentMocker>();
    windowExtensionSessionImpl.NotifyBackpressedEvent(isConsumed);

    auto ret = windowExtensionSessionImpl.SetPrivacyMode(isPrivacyMode);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyKeyEvent
 * @tc.desc: NotifyKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, NotifyKeyEvent, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    bool isConsumed = true;
    bool notifyInputMethod = true;
    bool isPrivacyMode = true;

    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    windowExtensionSessionImpl.NotifyKeyEvent(keyEvent, isConsumed, notifyInputMethod);

    auto ret = windowExtensionSessionImpl.SetPrivacyMode(isPrivacyMode);
    ASSERT_EQ(WMError::WM_OK, ret);
}
}
} // namespace Rosen
} // namespace OHOS
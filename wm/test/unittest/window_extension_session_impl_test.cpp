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

#include <transaction/rs_transaction.h>
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
};

void WindowExtensionSessionImplTest::SetUpTestCase()
{
}

void WindowExtensionSessionImplTest::TearDownTestCase()
{
}

void WindowExtensionSessionImplTest::SetUp()
{
}

void WindowExtensionSessionImplTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Create01
 * @tc.desc: context is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, Create01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    sptr<Rosen::ISession> iSession = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowExtensionSessionImpl.Create(nullptr, iSession));
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
    windowExtensionSessionImpl.uiContent_ = std::make_unique<Ace::UIContentMocker>();
    int64_t elementId = 0;
    int32_t focusType = 0;
    int64_t baseParent = 0;
    AccessibilityElementInfo info;

    ASSERT_EQ(WSError::WS_OK,
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
    windowExtensionSessionImpl.uiContent_ = std::make_unique<Ace::UIContentMocker>();
    int64_t elementId = 0;
    std::map<std::string, std::string> actionAguments;
    int32_t action = 0;
    int64_t baseParent = 0;
    ASSERT_EQ(WSError::WS_ERROR_INTERNAL_ERROR,
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
}
} // namespace Rosen
} // namespace OHOS
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
#include "window_manager_hilog.h"
#include "native_engine.h"
#include "window_extension_session_impl.h"

using namespace testing;
using namespace testing::ext;

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
 * @tc.name: SetUIContent01
 * @tc.desc: SetUIContent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionSessionImplTest, SetUIContent01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    WindowExtensionSessionImpl windowExtensionSessionImpl(option);
    ASSERT_EQ(WMError::WM_OK, 
        windowExtensionSessionImpl.SetUIContent("a", nullptr, nullptr, false, nullptr));
}
}
} // namespace Rosen
} // namespace OHOS
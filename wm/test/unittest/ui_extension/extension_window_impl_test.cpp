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

#include "extension_window_impl.h"
#include "mock_window_extension_session_impl.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ExtensionWindowImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ExtensionWindowImplTest::SetUpTestCase() {}

void ExtensionWindowImplTest::TearDownTestCase()
{
#ifdef RS_ENABLE_VK
    RSModifiersDrawThread::Destroy();
#endif
}

void ExtensionWindowImplTest::SetUp() {}

void ExtensionWindowImplTest::TearDown() {}

namespace {
/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: GetAvoidAreaByType Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, GetAvoidAreaByType, TestSize.Level1)
{
    sptr<Window> window = new Window();
    ExtensionWindowImpl extensionWindowImpl(window);
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM;
    AvoidArea avoidArea;
    ASSERT_EQ(WMError::WM_OK, extensionWindowImpl.GetAvoidAreaByType(type, avoidArea));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetWindow
 * @tc.desc: GetWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, GetWindow, TestSize.Level1)
{
    sptr<Window> window = new (std::nothrow) Window();
    ASSERT_NE(nullptr, window);
    ExtensionWindowImpl extensionWindowImpl(window);

    EXPECT_EQ(window, extensionWindowImpl.GetWindow());
}

/**
 * @tc.name: HideNonSecureWindows
 * @tc.desc: HideNonSecureWindows Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, HideNonSecureWindows, TestSize.Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<MockWindowExtensionSessionImpl> window = new (std::nothrow) MockWindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ExtensionWindowImpl extensionWindowImpl(window);
   
    EXPECT_EQ(WMError::WM_OK, extensionWindowImpl.HideNonSecureWindows(true));
    EXPECT_TRUE(window->extensionWindowFlags_.hideNonSecureWindowsFlag);

    EXPECT_EQ(WMError::WM_OK, extensionWindowImpl.HideNonSecureWindows(false));
    EXPECT_FALSE(window->extensionWindowFlags_.hideNonSecureWindowsFlag);
}

/**
 * @tc.name: SetWaterMarkFlag
 * @tc.desc: SetWaterMarkFlag Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, SetWaterMarkFlag, TestSize.Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<MockWindowExtensionSessionImpl> window = new (std::nothrow) MockWindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ExtensionWindowImpl extensionWindowImpl(window);
   
    EXPECT_EQ(WMError::WM_OK, extensionWindowImpl.SetWaterMarkFlag(true));
    EXPECT_TRUE(window->extensionWindowFlags_.waterMarkFlag);

    EXPECT_EQ(WMError::WM_OK, extensionWindowImpl.SetWaterMarkFlag(false));
    EXPECT_FALSE(window->extensionWindowFlags_.waterMarkFlag);
}

/**
 * @tc.name: IsPcWindow
 * @tc.desc: IsPcWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, IsPcWindow, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<MockWindowExtensionSessionImpl> window = sptr<MockWindowExtensionSessionImpl>::MakeSptr(option);
    ExtensionWindowImpl extensionWindowImpl(window);
   
    EXPECT_EQ(extensionWindowImpl.IsPcWindow(), window->IsPcWindow());
}

/**
 * @tc.name: IsPcOrPadFreeMultiWindowMode
 * @tc.desc: IsPcOrPadFreeMultiWindowMode Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, IsPcOrPadFreeMultiWindowMode, TestSize.Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<MockWindowExtensionSessionImpl> window = new (std::nothrow) MockWindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ExtensionWindowImpl extensionWindowImpl(window);
   
    EXPECT_EQ(extensionWindowImpl.IsPcOrPadFreeMultiWindowMode(), window->IsPcOrPadFreeMultiWindowMode());
}

/**
 * @tc.name: HidePrivacyContentForHost
 * @tc.desc: HidePrivacyContentForHost Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, HidePrivacyContentForHost, TestSize.Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<MockWindowExtensionSessionImpl> window = new (std::nothrow) MockWindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ExtensionWindowImpl extensionWindowImpl(window);
   
    EXPECT_EQ(WMError::WM_OK, extensionWindowImpl.HidePrivacyContentForHost(true));
    EXPECT_EQ(WMError::WM_OK, extensionWindowImpl.HidePrivacyContentForHost(false));
}

/**
 * @tc.name: OccupyEvents
 * @tc.desc: OccupyEvents Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, OccupyEvents, TestSize.Level1)
{
    auto option = sptr<WindowOption>::MakeSptr();
    auto window = sptr<MockWindowExtensionSessionImpl>::MakeSptr(option);
    ExtensionWindowImpl extensionWindowImpl(window);
   
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, extensionWindowImpl.OccupyEvents(0));
    window->mockHandler_ = std::make_shared<MockDataHandler>();
    EXPECT_EQ(WMError::WM_OK, extensionWindowImpl.OccupyEvents(0));
}
} // namespace
} // namespace Rosen
} // namespace OHOS
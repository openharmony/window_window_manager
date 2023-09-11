/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "window_extension_proxy.h"
#include "window_extension_stub.h"
#include "window_extension_stub_impl.h"
#include "window_extension_client_interface.h"
#include "window_extension_client_stub_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowExtensionStubImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowExtensionStub> mockWindowExtensionStub_;
    sptr<WindowExtensionProxy> windowExtensionProxy_;
};

void WindowExtensionStubImplTest::SetUpTestCase()
{
}

void WindowExtensionStubImplTest::TearDownTestCase()
{
}

void WindowExtensionStubImplTest::SetUp()
{
}

void WindowExtensionStubImplTest::TearDown()
{
}

namespace {
/**
 * @tc.name: CreateWindow
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, CreateWindow, Function | SmallTest | Level2)
{
    Rect rect;
    uint32_t parentWindowId = 0;
    std::shared_ptr<AbilityRuntime::Context> context;
    sptr<IRemoteObject> iSession;
    sptr<WindowOption> option;
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    auto res = windowExtensionStubImpl.CreateWindow(rect, parentWindowId, context, iSession);
    ASSERT_EQ(nullptr, res);
    option = new(std::nothrow) WindowOption();
    res = windowExtensionStubImpl.CreateWindow(rect, parentWindowId, context, iSession);
    ASSERT_EQ(windowExtensionStubImpl.window_.promote(), res);
}

/**
 * @tc.name: CreateWindow
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, SetBounds, Function | SmallTest | Level2)
{
    Rect rect;
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    windowExtensionStubImpl.SetBounds(rect);
    ASSERT_EQ(windowExtensionStubImpl.window_.promote(), windowExtensionStubImpl.GetWindow());
}

/**
 * @tc.name: Hide
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, Hide, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    auto window = windowExtensionStubImpl.window_.promote();
    windowExtensionStubImpl.Hide();
    ASSERT_EQ(windowExtensionStubImpl.window_.promote(), windowExtensionStubImpl.GetWindow());
}

/**
 * @tc.name: Show
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, Show, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    auto window = windowExtensionStubImpl.window_.promote();
    windowExtensionStubImpl.Show();
    ASSERT_EQ(windowExtensionStubImpl.window_.promote(), windowExtensionStubImpl.GetWindow());
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, RequestFocus, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    auto window = windowExtensionStubImpl.window_.promote();
    windowExtensionStubImpl.RequestFocus();
    ASSERT_EQ(windowExtensionStubImpl.window_.promote(), windowExtensionStubImpl.GetWindow());
}

/**
 * @tc.name: GetExtensionWindow
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, GetExtensionWindow, Function | SmallTest | Level2)
{
    sptr<IWindowExtensionClient> token;
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    auto window = windowExtensionStubImpl.window_.promote();
    windowExtensionStubImpl.GetExtensionWindow(token);
    sptr<IWindowExtensionCallback> componentCallback;
    WindowExtensionClientStubImpl windowExtensionClientStubImpl(componentCallback);
    sptr<IWindowExtensionClient> clientToken(new WindowExtensionClientStubImpl(
        windowExtensionClientStubImpl.componentCallback_));
    windowExtensionStubImpl.GetExtensionWindow(clientToken);
    ASSERT_EQ(windowExtensionStubImpl.window_.promote(), windowExtensionStubImpl.GetWindow());
}

/**
 * @tc.name: GetWindow
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, GetWindow, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    auto window = windowExtensionStubImpl.window_.promote();
    windowExtensionStubImpl.GetWindow();
    ASSERT_EQ(windowExtensionStubImpl.window_.promote(), windowExtensionStubImpl.GetWindow());
}
}
}
}
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
#include "iremote_object_mocker.h"
#include "window_extension_session_impl.h"

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
    wptr<Window> window_ = nullptr;
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

    iSession = new IRemoteObjectMocker();
    ASSERT_NE(nullptr, iSession);
    option = new(std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    res = windowExtensionStubImpl.CreateWindow(rect, parentWindowId, context, iSession);
    ASSERT_EQ(nullptr, res);
}

/**
 * @tc.name: CreateWindow
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, SetBounds, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    Rect rect = { 150, 150, 400, 600 };
    windowExtensionStubImpl.SetBounds(rect);

    sptr<WindowOption> option = new(std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowRect(rect);
    sptr<WindowExtensionSessionImpl> window = new(std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetWindowRect(rect);
    windowExtensionStubImpl.window_ = window;
    windowExtensionStubImpl.SetBounds(rect);

    Rect rect2 = { 100, 100, 200, 300 };
    windowExtensionStubImpl.SetBounds(rect2);

    rect2 = { 100, 150, 200, 600 };
    windowExtensionStubImpl.SetBounds(rect2);

    rect2 = { 150, 100, 400, 300 };
    windowExtensionStubImpl.SetBounds(rect2);
}

/**
 * @tc.name: Hide
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, Hide, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    windowExtensionStubImpl.Hide();

    sptr<WindowOption> option = new(std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowExtensionSessionImpl> window = new(std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    windowExtensionStubImpl.window_ = window;
    windowExtensionStubImpl.Hide();
}

/**
 * @tc.name: Show
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, Show, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    windowExtensionStubImpl.Show();

    sptr<WindowOption> option = new(std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowExtensionSessionImpl> window = new(std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    windowExtensionStubImpl.window_ = window;
    windowExtensionStubImpl.Show();
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionStubImplTest, RequestFocus, Function | SmallTest | Level2)
{
    WindowExtensionStubImpl windowExtensionStubImpl("windowName");
    windowExtensionStubImpl.RequestFocus();

    sptr<WindowOption> option = new(std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowExtensionSessionImpl> window = new(std::nothrow) WindowExtensionSessionImpl(option);
    ASSERT_NE(nullptr, window);
    windowExtensionStubImpl.window_ = window;
    windowExtensionStubImpl.RequestFocus();
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
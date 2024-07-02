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
#include <gmock/gmock.h>

#include "window_extension_connection.h"
#include "window_extension_connection.cpp"
#include "wm_common.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class MockWindowExtensionProxy : public IRemoteProxy<IWindowExtension> {
public:
    explicit MockWindowExtensionProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IWindowExtension>(impl) {};
    ~MockWindowExtensionProxy() {};

    MOCK_METHOD(void, SetBounds, (const Rect& rect), (override));
    MOCK_METHOD(void, Hide, (), (override));
    MOCK_METHOD(void, Show, (), (override));
    MOCK_METHOD(void, RequestFocus, (), (override));
    MOCK_METHOD(void, GetExtensionWindow, (sptr<IWindowExtensionClient>& token), (override));
};

class ExtensionCallback : public Rosen::IWindowExtensionCallback {
public:
    ExtensionCallback() = default;
    ~ExtensionCallback()  = default;
    void OnWindowReady(const std::shared_ptr<Rosen::RSSurfaceNode>& rsSurfaceNode) override;
    void OnExtensionDisconnected() override;
    void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event) override;
    void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event) override;
    void OnBackPress() override;
    bool isWindowReady_ = false;
};

void ExtensionCallback::OnWindowReady(const std::shared_ptr<Rosen::RSSurfaceNode>& rsSurfaceNode)
{
    isWindowReady_ = true;
}

void ExtensionCallback::OnExtensionDisconnected()
{
}

void ExtensionCallback::OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event)
{
}

void ExtensionCallback::OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event)
{
}

void ExtensionCallback::OnBackPress()
{
}

class ExtensionConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
private:
    sptr<WindowExtensionConnection> connection_ = nullptr;
};

void ExtensionConnectionTest::SetUpTestCase()
{
}

void ExtensionConnectionTest::TearDownTestCase()
{
}

void ExtensionConnectionTest::SetUp()
{
    connection_ = new(std::nothrow)WindowExtensionConnection();
    ASSERT_NE(nullptr, connection_);
}

void ExtensionConnectionTest::TearDown()
{
    connection_ = nullptr;
}

namespace {
/**
 * @tc.name: WindowExtensionConnection01
 * @tc.desc: connect window extension
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, WindowExtensionConnection01, Function | SmallTest | Level2)
{
    AppExecFwk::ElementName element;
    element.SetBundleName("com.test.windowextension");
    element.SetAbilityName("WindowExtAbility");
    Rosen::Rect rect {100, 100, 60, 60};
    ASSERT_TRUE(connection_->ConnectExtension(element, rect, 100, INVALID_WINDOW_ID, nullptr) != ERR_OK);
    connection_->Show();
    connection_->RequestFocus();
    connection_->SetBounds(rect);
    connection_->Hide();
}

/**
 * @tc.name: Show
 * @tc.desc: Show Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, Show, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    auto mockProxy = new(std::nothrow) MockWindowExtensionProxy(remoteObject);
    ASSERT_NE(nullptr, mockProxy);
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->proxy_ = mockProxy;
    EXPECT_CALL(*mockProxy, Show());
    connection_->Show();
}

/**
 * @tc.name: Hide
 * @tc.desc: Hide Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, Hide, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    auto mockProxy = new(std::nothrow) MockWindowExtensionProxy(remoteObject);
    ASSERT_NE(nullptr, mockProxy);
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->proxy_ = mockProxy;
    EXPECT_CALL(*mockProxy, Hide());
    connection_->Hide();
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: RequestFocus Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, RequestFocus, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    auto mockProxy = new(std::nothrow) MockWindowExtensionProxy(remoteObject);
    ASSERT_NE(nullptr, mockProxy);
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->proxy_ = mockProxy;
    EXPECT_CALL(*mockProxy, RequestFocus());
    connection_->RequestFocus();
}

/**
 * @tc.name: SetBounds
 * @tc.desc: SetBounds Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, SetBounds, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    auto mockProxy = new(std::nothrow) MockWindowExtensionProxy(remoteObject);
    ASSERT_NE(nullptr, mockProxy);
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->proxy_ = mockProxy;
    Rect rect;
    EXPECT_CALL(*mockProxy, SetBounds(rect));
    connection_->SetBounds(rect);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: OnRemoteDied Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnRemoteDied01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = nullptr;
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->deathRecipient_ =
        new(std::nothrow) WindowExtensionConnection::Impl::WindowExtensionClientRecipient(nullptr);
    ASSERT_NE(nullptr, connection_->pImpl_->deathRecipient_);
    connection_->pImpl_->deathRecipient_->OnRemoteDied(remoteObject);
}

/**
 * @tc.name: OnRemoteDied02
 * @tc.desc: OnRemoteDied Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnRemoteDied02, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->deathRecipient_ =
        new(std::nothrow) WindowExtensionConnection::Impl::WindowExtensionClientRecipient(nullptr);
    ASSERT_NE(nullptr, connection_->pImpl_->deathRecipient_);
    connection_->pImpl_->deathRecipient_->OnRemoteDied(remoteObject);
}

/**
 * @tc.name: OnRemoteDied03
 * @tc.desc: OnRemoteDied Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnRemoteDied03, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->deathRecipient_ =
        new(std::nothrow) WindowExtensionConnection::Impl::WindowExtensionClientRecipient(nullptr);
    ASSERT_NE(nullptr, connection_->pImpl_->deathRecipient_);
    connection_->pImpl_->deathRecipient_->callback_ = new(std::nothrow) ExtensionCallback();
    ASSERT_NE(nullptr, connection_->pImpl_->deathRecipient_->callback_);
    connection_->pImpl_->deathRecipient_->OnRemoteDied(remoteObject);
}

/**
 * @tc.name: OnAbilityConnectDone01
 * @tc.desc: OnAbilityConnectDone Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnAbilityConnectDone01, Function | SmallTest | Level2)
{
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject = nullptr;
    int resultCode = 0;
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->OnAbilityConnectDone(element, remoteObject, resultCode);
}

/**
 * @tc.name: OnAbilityConnectDone02
 * @tc.desc: OnAbilityConnectDone Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnAbilityConnectDone02, Function | SmallTest | Level2)
{
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    int resultCode = 0;
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->OnAbilityConnectDone(element, remoteObject, resultCode);
}

/**
 * @tc.name: OnAbilityConnectDone03
 * @tc.desc: OnAbilityConnectDone Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnAbilityConnectDone03, Function | SmallTest | Level2)
{
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject = new(std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, remoteObject);
    auto mockProxy = new(std::nothrow) MockWindowExtensionProxy(remoteObject);
    ASSERT_NE(nullptr, mockProxy);
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->deathRecipient_ =
        new(std::nothrow) WindowExtensionConnection::Impl::WindowExtensionClientRecipient(nullptr);
    ASSERT_NE(nullptr, connection_->pImpl_->deathRecipient_);
    connection_->pImpl_->proxy_ = mockProxy;
    int resultCode = 0;
    connection_->pImpl_->OnAbilityConnectDone(element, remoteObject, resultCode);
}

/**
 * @tc.name: OnAbilityDisconnectDone01
 * @tc.desc: OnAbilityConnectDone Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnAbilityDisconnectDone01, Function | SmallTest | Level2)
{
    AppExecFwk::ElementName element;
    int resultCode = 0;
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->OnAbilityDisconnectDone(element, resultCode);
}

/**
 * @tc.name: OnAbilityDisconnectDone02
 * @tc.desc: OnAbilityConnectDone Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionConnectionTest, OnAbilityDisconnectDone02, Function | SmallTest | Level2)
{
    AppExecFwk::ElementName element;
    int resultCode = 0;
    ASSERT_NE(nullptr, connection_->pImpl_);
    connection_->pImpl_->componentCallback_ = new(std::nothrow) ExtensionCallback();
    ASSERT_NE(nullptr, connection_->pImpl_->componentCallback_);
    connection_->pImpl_->OnAbilityDisconnectDone(element, resultCode);
}
}
} // Rosen
} // OHOS
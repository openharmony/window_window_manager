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
#include "mock_window_adapter_lite.h"
#include "singleton_mocker.h"
#include "window_manager_lite.cpp"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
using Mocker = SingletonMocker<WindowAdapterLite, MockWindowAdapterLite>;

class TestCameraWindowChangedListener : public ICameraWindowChangedListener {
public:
    void OnCameraWindowChange(uint32_t accessTokenId, bool isShowing) override {};
};

class WindowManagerLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowManagerLiteTest::SetUpTestCase()
{
}

void WindowManagerLiteTest::TearDownTestCase()
{
}

void WindowManagerLiteTest::SetUp()
{
}

void WindowManagerLiteTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: using windowManagerLite to get focus info
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetFocusWindowInfo, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    FocusChangeInfo infosInput;
    FocusChangeInfo infosResult;
    infosResult.pid_ = 10;
    infosResult.uid_ = 11;
    infosResult.displayId_ = 12;
    infosResult.windowId_ = 13;
    EXPECT_CALL(m->Mock(), GetFocusWindowInfo(_)).Times(1).WillOnce(DoAll(SetArgReferee<0>(infosResult), Return()));
    WindowManagerLite::GetInstance().GetFocusWindowInfo(infosInput);
    ASSERT_EQ(infosInput.windowId_, infosResult.windowId_);
    ASSERT_EQ(infosInput.uid_, infosResult.uid_);
    ASSERT_EQ(infosInput.pid_, infosResult.pid_);
    ASSERT_EQ(infosInput.displayId_, infosResult.displayId_);
}

/**
 * @tc.name: UpdateCameraWindowStatus
 * @tc.desc: UpdateCameraWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UpdateCameraWindowStatus, Function | SmallTest | Level2)
{
    uint32_t accessTokenId = 0;
    bool isShowing = true;
    auto ret = 0;
    WindowManagerLite::GetInstance().UpdateCameraWindowStatus(accessTokenId, isShowing);
    ASSERT_EQ(0, ret);
}


/**
 * @tc.name: RegisterCameraWindowChangedListener01
 * @tc.desc: check RegisterCameraWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterCameraWindowChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManagerLite::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->cameraWindowChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->cameraWindowChangedListeners_;
    windowManager.pImpl_->cameraWindowChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->cameraWindowChangedListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterCameraWindowChangedListener(nullptr));

    sptr<TestCameraWindowChangedListener> listener = new TestCameraWindowChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterCameraWindowChangedListener(listener));

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterCameraWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->cameraWindowChangedListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterCameraWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->cameraWindowChangedListeners_.size());

    windowManager.pImpl_->cameraWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->cameraWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterCameraWindowChangedListener01
 * @tc.desc: check UnregisterCameraWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterCameraWindowChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManagerLite::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->cameraWindowChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->cameraWindowChangedListeners_;
    windowManager.pImpl_->cameraWindowChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->cameraWindowChangedListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterCameraWindowChangedListener(nullptr));

    sptr<TestCameraWindowChangedListener> listener1 = new TestCameraWindowChangedListener();
    sptr<TestCameraWindowChangedListener> listener2 = new TestCameraWindowChangedListener();
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraWindowChangedListener(listener1));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterCameraWindowChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterCameraWindowChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->cameraWindowChangedListeners_.size());
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraWindowChangedListener(listener1));

    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraWindowChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->cameraWindowChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->cameraWindowChangedListenerAgent_);

    windowManager.pImpl_->cameraWindowChangedListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraWindowChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->cameraWindowChangedListeners_.size());

    windowManager.pImpl_->cameraWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->cameraWindowChangedListeners_ = oldListeners;
}
}
}
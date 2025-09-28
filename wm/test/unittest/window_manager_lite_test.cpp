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
#include "window_manager_hilog.h"
#include "window_manager_lite.cpp"
#include "wm_common.h"
#include "session/host/include/scene_session.h"
#include "common/include/window_session_property.h"
#include "session_manager/include/scene_session_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
namespace {
    std::string g_errLog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
        const char *msg)
    {
        g_errLog = msg;
    }
using Mocker = SingletonMocker<WindowAdapterLite, MockWindowAdapterLite>;

class TestCameraWindowChangedListener : public ICameraWindowChangedListener {
public:
    void OnCameraWindowChange(uint32_t accessTokenId, bool isShowing) override {};
};

class TestWindowModeChangedListener : public IWindowModeChangedListener {
public:
    void OnWindowModeUpdate(WindowModeType mode) override {};
};

class TestWMSConnectionChangedListener : public IWMSConnectionChangedListener {
public:
    void OnConnected(int32_t userId, int32_t screenId) override {};
    void OnDisconnected(int32_t userId, int32_t screenId) override {};
};

class TestFocusChangedListener : public IFocusChangedListener {
public:
    void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override {};
    void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override {};
};

class TestDrawingContentChangedListener : public IDrawingContentChangedListener {
public:
    void OnWindowDrawingContentChanged(const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingInfo) override {
    };
};

class TestVisibilityChangedListener : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override {};
};

class IWMSConnectionChangedListenerSon : public IWMSConnectionChangedListener {
public:
    void OnConnected(int32_t userId, int32_t screenId) override {};
    void OnDisconnected(int32_t userId, int32_t screenId) override {};
};

class TestWindowStyleChangedListener : public IWindowStyleChangedListener {
public:
    void OnWindowStyleUpdate(WindowStyleType styleType)
    {
        TLOGI(WmsLogTag::DMS, "TestWindowStyleChangedListener");
    }
};

class TestPiPStateChangedListener : public IPiPStateChangedListener {
public:
    void OnPiPStateChanged(const std::string& bundleName, bool isForeground) override {}
};

class TestWindowUpdateListener : public IWindowUpdateListener {
public:
    void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
    {
        WLOGI("TestWindowUpdateListener");
    }
};

class TestIKeyboardCallingWindowDisplayChangedListener : public IKeyboardCallingWindowDisplayChangedListener {
public:
    void OnCallingWindowDisplayChanged(const CallingWindowInfo& callingWindowInfo)
    {
        info = callingWindowInfo;
        isNotified = true;
    }

private:
    CallingWindowInfo info = {0, -1, 0, 0};
    bool isNotified = false;
};

class TestWindowVisibilityStateListener : public IWindowInfoChangedListener {
public:
    void OnWindowInfoChanged(
        const std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>>& windowInfoList) override
    {
        WLOGI("TestWindowUpdateListener");
    };
};

class WindowManagerLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    // keyboard
    bool CheckCallingWindowInfo(const CallingWindowInfo& desiredInfo, const CallingWindowInfo& actualInfo);
};

void WindowManagerLiteTest::SetUpTestCase() {}

void WindowManagerLiteTest::TearDownTestCase() {}

void WindowManagerLiteTest::SetUp() {}

void WindowManagerLiteTest::TearDown() {}

bool WindowManagerLiteTest::CheckCallingWindowInfo(
    const CallingWindowInfo& desiredInfo, const CallingWindowInfo& actualInfo)
{
    return desiredInfo.windowId_ == actualInfo.windowId_ && desiredInfo.callingPid_ == actualInfo.callingPid_ &&
        desiredInfo.displayId_ == actualInfo.displayId_ && desiredInfo.userId_ == actualInfo.userId_;
}

namespace {
/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: using windowManagerLite to get focus info
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetFocusWindowInfo, TestSize.Level1)
{
    auto instance = WindowManagerLite::GetInstance(-1);
    FocusChangeInfo infosInput;
    FocusChangeInfo infosResult;
    infosResult.pid_ = 10;
    infosResult.uid_ = 11;
    infosResult.displayId_ = 12;
    infosResult.windowId_ = 13;

    ASSERT_NE(nullptr, instance);
    instance->GetFocusWindowInfo(infosInput);
}

/**
 * @tc.name: NotifyAccessibilityWindowInfo
 * @tc.desc: NotifyAccessibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyAccessibilityWindowInfo, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    WindowManagerLite lite;
    auto windowManager = WindowManagerLite::GetInstance(-1);
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    WindowUpdateType type = WindowUpdateType::WINDOW_UPDATE_ADDED;
    windowManager->NotifyAccessibilityWindowInfo(infos, type);

    lite.pImpl_->NotifyAccessibilityWindowInfo(infos, type);
    EXPECT_TRUE(g_errLog.find("infos is empty") != std::string::npos);

    sptr<AccessibilityWindowInfo> info = sptr<AccessibilityWindowInfo>::MakeSptr();
    infos.push_back(info);
    lite.pImpl_->NotifyAccessibilityWindowInfo(infos, type);

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    sptr<TestWindowUpdateListener> listener = sptr<TestWindowUpdateListener>::MakeSptr();
    windowManager->RegisterWindowUpdateListener(listener);
    lite.pImpl_->NotifyAccessibilityWindowInfo(infos, type);

    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: UpdateCameraWindowStatus01
 * @tc.desc: UpdateCameraWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UpdateCameraWindowStatus01, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 0;
    bool isShowing = true;
    WindowManagerLite::GetInstance().UpdateCameraWindowStatus(accessTokenId, isShowing);
    EXPECT_FALSE(g_errLog.find("Camera window, accessTokenId=%{public}u, isShowing=%{public}u") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: UpdateCameraWindowStatus02
 * @tc.desc: UpdateCameraWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UpdateCameraWindowStatus02, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    sptr<TestCameraWindowChangedListener> listener = sptr<TestCameraWindowChangedListener>::MakeSptr();
    windowManager->RegisterCameraWindowChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    uint32_t accessTokenId = 0;
    bool isShowing = true;
    WindowManagerLite::GetInstance().UpdateCameraWindowStatus(accessTokenId, isShowing);
}

/**
 * @tc.name: RegisterCameraWindowChangedListener01
 * @tc.desc: check RegisterCameraWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterCameraWindowChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->cameraWindowChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->cameraWindowChangedListeners_;
    windowManager->pImpl_->cameraWindowChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->cameraWindowChangedListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterCameraWindowChangedListener(nullptr));

    sptr<TestCameraWindowChangedListener> listener1 = sptr<TestCameraWindowChangedListener>::MakeSptr();
    sptr<TestCameraWindowChangedListener> listener2 = sptr<TestCameraWindowChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener1));

    windowManager->RegisterCameraWindowChangedListener(listener1);
    windowManager->RegisterCameraWindowChangedListener(listener2);

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->cameraWindowChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->cameraWindowChangedListenerAgent_);

    windowManager->pImpl_->cameraWindowChangedListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->cameraWindowChangedListeners_.size());

    windowManager->pImpl_->cameraWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->cameraWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterCameraWindowChangedListener01
 * @tc.desc: check UnregisterCameraWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterCameraWindowChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->cameraWindowChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->cameraWindowChangedListeners_;
    windowManager->pImpl_->cameraWindowChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->cameraWindowChangedListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterCameraWindowChangedListener(nullptr));

    sptr<TestCameraWindowChangedListener> listener1 = sptr<TestCameraWindowChangedListener>::MakeSptr();
    sptr<TestCameraWindowChangedListener> listener2 = sptr<TestCameraWindowChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener1));

    windowManager->RegisterCameraWindowChangedListener(listener1);
    windowManager->RegisterCameraWindowChangedListener(listener2);

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->cameraWindowChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->cameraWindowChangedListenerAgent_);

    windowManager->pImpl_->cameraWindowChangedListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraWindowChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->cameraWindowChangedListeners_.size());

    windowManager->pImpl_->cameraWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->cameraWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: Test01
 * @tc.desc: check UnregisterCameraWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, Test01, TestSize.Level1)
{
    sptr<IFocusChangedListener> listener = nullptr;
    auto ret1 = WindowManagerLite::GetInstance().RegisterFocusChangedListener(listener);
    auto ret2 = WindowManagerLite::GetInstance().UnregisterFocusChangedListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret1);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret2);
    sptr<IVisibilityChangedListener> listener1 = nullptr;
    auto ret3 = WindowManagerLite::GetInstance().RegisterVisibilityChangedListener(listener1);
    auto ret4 = WindowManagerLite::GetInstance().UnregisterVisibilityChangedListener(listener1);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret3);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret4);
    std::vector<sptr<WindowVisibilityInfo>> infos;
    ASSERT_EQ(WMError::WM_OK, WindowManagerLite::GetInstance().GetVisibilityWindowInfo(infos));
    sptr<IDrawingContentChangedListener> listener2 = nullptr;
    auto ret5 = WindowManagerLite::GetInstance().RegisterDrawingContentChangedListener(listener2);
    auto ret6 = WindowManagerLite::GetInstance().UnregisterDrawingContentChangedListener(listener2);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret5);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret6);
    sptr<IWindowModeChangedListener> listener3 = nullptr;
    auto ret7 = WindowManagerLite::GetInstance().RegisterWindowModeChangedListener(listener3);
    auto ret8 = WindowManagerLite::GetInstance().UnregisterWindowModeChangedListener(listener3);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret7);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret8);
    sptr<IWMSConnectionChangedListener> listener4 = nullptr;
    WMError res9 = WindowManagerLite::GetInstance().RegisterWMSConnectionChangedListener(listener4);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, res9);
    WMError res10 = WindowManagerLite::GetInstance().UnregisterWMSConnectionChangedListener();
    ASSERT_EQ(WMError::WM_OK, res10);
}

/**
 * @tc.name: Test04
 * @tc.desc: Test04
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, Test04, TestSize.Level1)
{
    WindowManagerLite lite;
    sptr<FocusChangeInfo> focusChangeInfo = nullptr;
    ASSERT_NE(lite.pImpl_, nullptr);
    lite.pImpl_->NotifyFocused(focusChangeInfo);
    lite.pImpl_->NotifyUnfocused(focusChangeInfo);
    focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr();
    lite.pImpl_->NotifyFocused(focusChangeInfo);
    lite.pImpl_->NotifyUnfocused(focusChangeInfo);
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    lite.pImpl_->NotifyWindowVisibilityInfoChanged(windowVisibilityInfos);
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos;
    lite.pImpl_->NotifyWindowDrawingContentInfoChanged(windowDrawingContentInfos);
    lite.pImpl_->NotifyWindowModeChange(WindowModeType::WINDOW_MODE_SPLIT);
}

/**
 * @tc.name: RegisterWindowModeChangedListener02
 * @tc.desc: check RegisterWindowModeChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterWindowModeChangedListener02, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowModeListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowModeListeners_;
    windowManager->pImpl_->windowModeListenerAgent_ = nullptr;
    windowManager->pImpl_->windowModeListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterWindowModeChangedListener(nullptr));

    sptr<TestWindowModeChangedListener> listener = sptr<TestWindowModeChangedListener>::MakeSptr();
    windowManager->RegisterWindowModeChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->windowModeListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowModeListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowModeChangedListener02
 * @tc.desc: check UnregisterWindowModeChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterWindowModeChangedListener02, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowModeListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowModeListeners_;
    windowManager->pImpl_->windowModeListenerAgent_ = nullptr;
    windowManager->pImpl_->windowModeListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterWindowModeChangedListener(nullptr));

    sptr<TestWindowModeChangedListener> listener1 = sptr<TestWindowModeChangedListener>::MakeSptr();
    sptr<TestWindowModeChangedListener> listener2 = sptr<TestWindowModeChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowModeChangedListener(listener1));

    windowManager->RegisterWindowModeChangedListener(listener1);
    windowManager->RegisterWindowModeChangedListener(listener2);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowModeChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowModeChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->windowModeListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowModeListenerAgent_);

    windowManager->pImpl_->windowModeListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowModeChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->windowModeListeners_.size());

    windowManager->pImpl_->windowModeListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowModeListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWMSConnectionChangedListener02
 * @tc.desc: check RegisterWMSConnectionChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterWMSConnectionChangedListener02, TestSize.Level1)
{
    auto& windowManager = WindowManagerLite::GetInstance();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWMSConnectionChangedListener(nullptr));

    sptr<TestWMSConnectionChangedListener> listener = sptr<TestWMSConnectionChangedListener>::MakeSptr();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWMSConnectionChangedListener(listener));

    windowManager.pImpl_->wmsConnectionChangedListener_ = nullptr;
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWMSConnectionChangedListener(listener));
}

/**
 * @tc.name: GetMainWindowInfos
 * @tc.desc: get top main window info
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetMainWindowInfos, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<MainWindowInfo> topNInfo;
    std::vector<MainWindowInfo> topNInfoResult;
    MainWindowInfo info1;
    info1.pid_ = 1900;
    info1.bundleName_ = "calendar";

    MainWindowInfo info2;
    info1.pid_ = 1901;
    info1.bundleName_ = "settings";

    MainWindowInfo info3;
    info1.pid_ = 1902;
    info1.bundleName_ = "photos";

    topNInfoResult.push_back(info1);
    topNInfoResult.push_back(info2);
    topNInfoResult.push_back(info3);

    int32_t topN = 3;

    auto instance = WindowManagerLite::GetInstance(-1);
    instance->GetMainWindowInfos(topN, topNInfo);
    ASSERT_NE(nullptr, instance);

    auto it1 = topNInfo.begin();
    auto it2 = topNInfoResult.begin();
    for (; it1 != topNInfo.end() && it2 != topNInfoResult.end(); it1++, it2++) {
        ASSERT_EQ(it1->pid_, it2->pid_);
        ASSERT_EQ(it1->bundleName_, it2->bundleName_);
    }
}

/**
 * @tc.name: UpdateAnimationSpeedWithPid
 * @tc.desc: Check UpdateAnimationSpeedWithPid
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UpdateAnimationSpeedWithPid, TestSize.Level1)
{
    auto& windowManager = WindowManagerLite::GetInstance();
    pid_t pid = 15234;
    float speed = 2.0f;
    WMError ret_1 = windowManager.UpdateAnimationSpeedWithPid(pid, speed);
    ASSERT_EQ(WMError::WM_OK, ret_1);
    speed = 1.0f;
    WMError ret_2 = windowManager.UpdateAnimationSpeedWithPid(pid, speed);
    ASSERT_EQ(WMError::WM_OK, ret_2);
}

/**
 * @tc.name: TestUpdateFocusChangeInfo
 * @tc.desc: TestUpdateFocusChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, TestUpdateFocusChangeInfo, TestSize.Level1)
{
    sptr<FocusChangeInfo> focusChangeInfo = nullptr;
    WindowManagerLite::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, true);
    sptr<FocusChangeInfo> focusChangeInfo1 = sptr<FocusChangeInfo>::MakeSptr();
    WindowManagerLite::GetInstance().UpdateFocusChangeInfo(focusChangeInfo1, true);
    WindowManagerLite::GetInstance().UpdateFocusChangeInfo(focusChangeInfo1, false);
    std::vector<sptr<WindowVisibilityInfo>> infos;
    WindowManagerLite::GetInstance().UpdateWindowVisibilityInfo(infos);
    std::vector<sptr<WindowDrawingContentInfo>> infos1;
    WindowManagerLite::GetInstance().UpdateWindowDrawingContentInfo(infos1);
    WindowManagerLite lite;
    lite.destroyed_ = true;
    lite.OnRemoteDied();
    lite.destroyed_ = false;
    lite.OnRemoteDied();
    ASSERT_EQ(nullptr, lite.pImpl_->focusChangedListenerAgent_);
    ASSERT_EQ(nullptr, lite.pImpl_->windowUpdateListenerAgent_);
    ASSERT_EQ(nullptr, lite.pImpl_->windowDrawingContentListenerAgent_);
}

/**
 * @tc.name: NotifyFocusedWithUn
 * @tc.desc: NotifyFocused With MotifyUnFocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyFocusedWithUn, TestSize.Level1)
{
    WindowManagerLite lite;
    sptr<FocusChangeInfo> focusChangeInfo = nullptr;
    lite.pImpl_->NotifyFocused(focusChangeInfo);
    lite.pImpl_->NotifyUnfocused(focusChangeInfo);
    ASSERT_EQ(focusChangeInfo, nullptr);
    focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr();
    lite.pImpl_->focusChangedListeners_.push_back(nullptr);
    sptr<TestFocusChangedListener> testFocusChangedListener = sptr<TestFocusChangedListener>::MakeSptr();
    lite.pImpl_->focusChangedListeners_.push_back(testFocusChangedListener);
    lite.pImpl_->NotifyFocused(focusChangeInfo);
    lite.pImpl_->NotifyUnfocused(focusChangeInfo);
    ASSERT_NE(focusChangeInfo, nullptr);
}

/**
 * @tc.name: NotifyWindowDrawingContentInfoChanged02
 * @tc.desc: NotifyWindowDrawingContentInfoChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWindowDrawingContentInfoChanged02, TestSize.Level1)
{
    WindowManagerLite lite;
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto listener = sptr<TestDrawingContentChangedListener>::MakeSptr();

    windowManager->RegisterDrawingContentChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos;
    lite.pImpl_->NotifyWindowDrawingContentInfoChanged(windowDrawingContentInfos);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterDrawingContentChangedListener(listener));
}

/**
 * @tc.name: NotifyWindowModeChange02
 * @tc.desc: NotifyWindowModeChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWindowModeChange02, TestSize.Level1)
{
    WindowManagerLite lite;
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto listener = sptr<TestWindowModeChangedListener>::MakeSptr();
    windowManager->RegisterWindowModeChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    lite.pImpl_->NotifyWindowModeChange(WindowModeType::WINDOW_MODE_SPLIT);
}

/**
 * @tc.name: NotifyWindowVisibilityInfoChanged02
 * @tc.desc: NotifyWindowVisibilityInfoChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWindowVisibilityInfoChanged02, TestSize.Level1)
{
    WindowManagerLite lite;
    auto& windowManager = WindowManagerLite::GetInstance();
    sptr<TestVisibilityChangedListener> listener = sptr<TestVisibilityChangedListener>::MakeSptr();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterVisibilityChangedListener(listener));

    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    lite.pImpl_->NotifyWindowVisibilityInfoChanged(windowVisibilityInfos);

    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterVisibilityChangedListener(listener));
}

/**
 * @tc.name: NotifyWindowVisibilityStateChanged
 * @tc.desc: NotifyWindowVisibilityStateChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWindowVisibilityStateChanged, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    WindowManagerLite lite;
    sptr<TestWindowVisibilityStateListener> listener;
    lite.pImpl_->windowVisibilityStateListeners_.push_back(listener);
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    sptr<WindowVisibilityInfo> info = sptr<WindowVisibilityInfo>::MakeSptr();
    windowVisibilityInfos.push_back(info);
    lite.pImpl_->NotifyWindowVisibilityStateChanged(windowVisibilityInfos);
    EXPECT_TRUE(g_errLog.find("listener is null") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: NotifyUnfocused01
 * @tc.desc: NotifyUnfocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyUnfocused01, TestSize.Level1)
{
    WindowManagerLite lite;
    auto windowManager = WindowManagerLite::GetInstance(-1);
    sptr<FocusChangeInfo> focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr();

    auto listener = sptr<TestFocusChangedListener>::MakeSptr();
    ASSERT_TRUE(listener != nullptr);
    windowManager->RegisterFocusChangedListener(listener);
    lite.pImpl_->NotifyUnfocused(focusChangeInfo);
    windowManager->RegisterFocusChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);
}

/**
 * @tc.name: GetAllMainWindowInfos001
 * @tc.desc: GetAllMainWindowInfos001
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetAllMainWindowInfos001, TestSize.Level1)
{
    std::vector<MainWindowInfo> infos;
    std::vector<MainWindowInfo> infosResult;
    MainWindowInfo info1;
    info1.pid_ = 1900;
    info1.bundleName_ = "calendar";

    MainWindowInfo info2;
    info1.pid_ = 1901;
    info1.bundleName_ = "settings";

    MainWindowInfo info3;
    info1.pid_ = 1902;
    info1.bundleName_ = "photos";

    infosResult.push_back(info1);
    infosResult.push_back(info2);
    infosResult.push_back(info3);

    auto instance = WindowManagerLite::GetInstance(-1);
    instance->GetAllMainWindowInfos(infos);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: GetAllMainWindowInfos002
 * @tc.desc: GetAllMainWindowInfos002
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetAllMainWindowInfos002, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<MainWindowInfo> infos;
    MainWindowInfo info1;
    info1.pid_ = 1900;
    info1.bundleName_ = "calendar";
    infos.push_back(info1);

    EXPECT_CALL(m->Mock(), GetAllMainWindowInfos(_)).Times(0).WillOnce(DoAll(Return(WMError::WM_OK)));

    auto errorCode = WindowManagerLite::GetInstance().GetAllMainWindowInfos(infos);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, errorCode);
}

/**
 * @tc.name: ClearMainSessions001
 * @tc.desc: ClearMainSessions001
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, ClearMainSessions001, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<int32_t> persistentIds;

    EXPECT_CALL(m->Mock(), ClearMainSessions(_)).Times(0).WillOnce(Return(WMError::WM_OK));

    auto errorCode = WindowManagerLite::GetInstance().ClearMainSessions(persistentIds);
    ASSERT_EQ(WMError::WM_OK, errorCode);
}

/**
 * @tc.name: ClearMainSessions002
 * @tc.desc: ClearMainSessions002
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, ClearMainSessions002, TestSize.Level1)
{
    std::vector<int32_t> persistentIds = { 1, 2 };
    auto instance = WindowManagerLite::GetInstance(-1);
    instance->ClearMainSessions(persistentIds);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: ClearMainSessions003
 * @tc.desc: ClearMainSessions003
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, ClearMainSessions003, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<int32_t> persistentIds;
    std::vector<int32_t> clearFailedIds;
    EXPECT_CALL(m->Mock(), ClearMainSessions(_, _)).Times(0).WillOnce(Return(WMError::WM_OK));

    auto errorCode = WindowManagerLite::GetInstance().ClearMainSessions(persistentIds, clearFailedIds);
    ASSERT_EQ(WMError::WM_OK, errorCode);
}

/**
 * @tc.name: ClearMainSessions004
 * @tc.desc: ClearMainSessions004
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, ClearMainSessions004, TestSize.Level1)
{
    std::vector<int32_t> persistentIds = { 1, 2 };
    std::vector<int32_t> clearFailedIds;
    auto instance = WindowManagerLite::GetInstance(-1);

    instance->ClearMainSessions(persistentIds, clearFailedIds);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: GetWindowModeType
 * @tc.desc: GetWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetWindowModeType, TestSize.Level1)
{
    WindowModeType windowModeType = WindowModeType::WINDOW_MODE_SPLIT_FLOATING;
    auto ret = WindowManagerLite::GetInstance().GetWindowModeType(windowModeType);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: RaiseWindowToTop
 * @tc.desc: RaiseWindowToTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RaiseWindowToTop, TestSize.Level1)
{
    auto ret = WindowManagerLite::GetInstance().RaiseWindowToTop(0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: NotifyWMSConnected01
 * @tc.desc: NotifyWMSConnected
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWMSConnected01, TestSize.Level1)
{
    WindowManagerLite::GetInstance().pImpl_->wmsConnectionChangedListener_ = nullptr;
    WindowManagerLite::GetInstance().pImpl_->NotifyWMSConnected(0, 0);
    WindowManagerLite::GetInstance().pImpl_->NotifyWMSDisconnected(0, 0);
    sptr<FocusChangeInfo> focusChangeInfo = nullptr;
    WindowManagerLite::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, true);
    focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr();
    ASSERT_NE(nullptr, focusChangeInfo);
    WindowManagerLite::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, true);
    WindowManagerLite::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, false);
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    WindowManagerLite::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos;
    WindowManagerLite::GetInstance().UpdateWindowDrawingContentInfo(windowDrawingContentInfos);
    WindowManagerLite::GetInstance().OnRemoteDied();
    WindowManagerLite::GetInstance().OnWMSConnectionChanged(0, 0, true);
    WindowManagerLite::GetInstance().OnWMSConnectionChanged(0, 0, false);
    WindowModeType windowModeType = WindowModeType::WINDOW_MODE_SPLIT_FLOATING;
    WindowManagerLite::GetInstance().UpdateWindowModeTypeInfo(windowModeType);
    auto ret = WindowManagerLite::GetInstance().GetWindowModeType(windowModeType);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: NotifyWMSConnected02
 * @tc.desc: NotifyWMSConnected
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWMSConnected02, TestSize.Level1)
{
    auto& windowManager = WindowManagerLite::GetInstance();
    sptr<TestWMSConnectionChangedListener> listener = sptr<TestWMSConnectionChangedListener>::MakeSptr();
    windowManager.pImpl_->wmsConnectionChangedListener_ = nullptr;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWMSConnectionChangedListener(listener));
    int32_t userId = 0;
    int32_t screenId = 0;
    WindowManagerLite::GetInstance().pImpl_->NotifyWMSConnected(userId, screenId);
    WindowManagerLite::GetInstance().pImpl_->NotifyWMSDisconnected(userId, screenId);
}

/**
 * @tc.name: NotifyWMSConnected03
 * @tc.desc: NotifyWMSConnected
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWMSConnected03, TestSize.Level1)
{
    WindowManagerLite::GetInstance().pImpl_->wmsConnectionChangedListener_ =
        sptr<IWMSConnectionChangedListenerSon>::MakeSptr();
    WindowManagerLite::GetInstance().pImpl_->NotifyWMSConnected(0, 0);
    EXPECT_NE(WindowManagerLite::GetInstance().pImpl_->wmsConnectionChangedListener_, nullptr);
    WindowManagerLite::GetInstance().pImpl_->NotifyWMSDisconnected(0, 0);
    EXPECT_NE(WindowManagerLite::GetInstance().pImpl_->wmsConnectionChangedListener_, nullptr);
}

/**
 * @tc.name: RegisterWindowStyleChangedListener
 * @tc.desc: check RegisterWindowStyleChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterWindowStyleChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowStyleListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowStyleListeners_;
    windowManager->pImpl_->windowStyleListenerAgent_ = nullptr;
    windowManager->pImpl_->windowStyleListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterWindowStyleChangedListener(nullptr));

    sptr<IWindowStyleChangedListener> listener = sptr<TestWindowStyleChangedListener>::MakeSptr();
    windowManager->RegisterWindowStyleChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    // to check that the same listner can not be registered twice
    windowManager->RegisterWindowStyleChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->windowStyleListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowStyleListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowStyleChangedListener
 * @tc.desc: check UnregisterWindowStyleChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterWindowStyleChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowStyleListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowStyleListeners_;
    windowManager->pImpl_->windowStyleListenerAgent_ = sptr<WindowManagerAgentLite>::MakeSptr();
    windowManager->pImpl_->windowStyleListeners_.clear();
    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterWindowStyleChangedListener(nullptr));

    sptr<TestWindowStyleChangedListener> listener1 = sptr<TestWindowStyleChangedListener>::MakeSptr();
    sptr<TestWindowStyleChangedListener> listener2 = sptr<TestWindowStyleChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowStyleChangedListener(listener1));

    windowManager->RegisterWindowStyleChangedListener(listener1);
    windowManager->RegisterWindowStyleChangedListener(listener2);

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowStyleChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowStyleChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->windowStyleListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowStyleListenerAgent_);

    windowManager->pImpl_->windowStyleListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowStyleChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->windowStyleListeners_.size());
    windowManager->pImpl_->windowStyleListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowStyleListeners_ = oldListeners;
}

/**
 * @tc.name: NotifyWindowStyleChange01
 * @tc.desc: check NotifyWindowStyleChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWindowStyleChange01, TestSize.Level1)
{
    WindowStyleType type = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;
    auto ret = WindowManagerLite::GetInstance().NotifyWindowStyleChange(type);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyWindowStyleChange02
 * @tc.desc: check NotifyWindowStyleChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyWindowStyleChange02, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    sptr<IWindowStyleChangedListener> listener = sptr<TestWindowStyleChangedListener>::MakeSptr();

    windowManager->RegisterWindowStyleChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    WindowStyleType type = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;
    auto ret = WindowManagerLite::GetInstance().NotifyWindowStyleChange(type);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetWindowStyleType
 * @tc.desc: check GetWindowStyleType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetWindowStyleType, TestSize.Level1)
{
    WindowStyleType type = WindowManagerLite::GetInstance().GetWindowStyleType();
    ASSERT_EQ(Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT, type);
}

/**
 * @tc.name: TerminateSessionByPersistentId001
 * @tc.desc: TerminateSessionByPersistentId001
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, TerminateSessionByPersistentId001, TestSize.Level1)
{
    int32_t persistentId = 1;
    auto instance = WindowManagerLite::GetInstance(-1);
    instance->TerminateSessionByPersistentId(persistentId);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: TerminateSessionByPersistentId002
 * @tc.desc: TerminateSessionByPersistentId002
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, TerminateSessionByPersistentId002, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    int32_t persistentId = 0;
    auto errorCode = WindowManagerLite::GetInstance().TerminateSessionByPersistentId(persistentId);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, errorCode);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: OnRemoteDied01
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, OnRemoteDied01, TestSize.Level1)
{
    WindowManagerLite::GetInstance().destroyed_ = true;
    WindowManagerLite::GetInstance().OnRemoteDied();
    ASSERT_EQ(WindowManagerLite::GetInstance().destroyed_, true);
}

/**
 * @tc.name: CloseTargetFloatWindow
 * @tc.desc: check CloseTargetFloatWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, CloseTargetFloatWindow, TestSize.Level1)
{
    std::string bundleName = "test";
    auto instance = WindowManagerLite::GetInstance(-1);

    instance->CloseTargetFloatWindow(bundleName);
    ASSERT_NE(nullptr, instance);

    std::string bundleName1 = "";
    instance->CloseTargetFloatWindow(bundleName1);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: RegisterPiPStateChangedListener
 * @tc.desc: check RegisterPiPStateChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterPiPStateChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->pipStateChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->pipStateChangedListeners_;
    windowManager->pImpl_->pipStateChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->pipStateChangedListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterPiPStateChangedListener(nullptr));

    sptr<IPiPStateChangedListener> listener = sptr<TestPiPStateChangedListener>::MakeSptr();
    windowManager->RegisterPiPStateChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->RegisterPiPStateChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->pipStateChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->pipStateChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterPiPStateChangedListener
 * @tc.desc: check UnregisterPiPStateChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterPiPStateChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->pipStateChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->pipStateChangedListeners_;
    windowManager->pImpl_->pipStateChangedListenerAgent_ = sptr<WindowManagerAgentLite>::MakeSptr();
    windowManager->pImpl_->pipStateChangedListeners_.clear();
    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterPiPStateChangedListener(nullptr));

    sptr<IPiPStateChangedListener> listener1 = sptr<TestPiPStateChangedListener>::MakeSptr();
    sptr<IPiPStateChangedListener> listener2 = sptr<TestPiPStateChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterPiPStateChangedListener(listener1));

    windowManager->RegisterPiPStateChangedListener(listener1);
    windowManager->RegisterPiPStateChangedListener(listener2);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterPiPStateChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterPiPStateChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->pipStateChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->pipStateChangedListenerAgent_);

    windowManager->pImpl_->pipStateChangedListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterPiPStateChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->pipStateChangedListeners_.size());
    windowManager->pImpl_->pipStateChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->pipStateChangedListeners_ = oldListeners;
}

/**
 * @tc.name: CloseTargetPiPWindow
 * @tc.desc: check CloseTargetPiPWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, CloseTargetPiPWindow, TestSize.Level1)
{
    std::string bundleName = "test";
    auto instance = WindowManagerLite::GetInstance(-1);

    instance->CloseTargetPiPWindow(bundleName);
    ASSERT_NE(nullptr, instance);

    std::string bundleName1 = "";
    auto ret = instance->CloseTargetPiPWindow(bundleName1);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: GetCurrentPiPWindowInfo01
 * @tc.desc: check GetCurrentPiPWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetCurrentPiPWindowInfo01, TestSize.Level1)
{
    std::string bundleName;
    auto instance = WindowManagerLite::GetInstance(-1);
    instance->GetCurrentPiPWindowInfo(bundleName);
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ("", bundleName);
}

/**
 * @tc.name: GetCurrentPiPWindowInfo02
 * @tc.desc: check GetCurrentPiPWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetCurrentPiPWindowInfo02, TestSize.Level1)
{
    std::string testBundleName = "test";
    std::string bundleName;
    auto instance = WindowManagerLite::GetInstance(-1);
    instance->GetCurrentPiPWindowInfo(bundleName);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: GetAccessibilityWindowInfo01
 * @tc.desc: check GetAccessibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetAccessibilityWindowInfo01, TestSize.Level1)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    infos.clear();
    auto instance = WindowManagerLite::GetInstance(-1);
    instance->GetAccessibilityWindowInfo(infos);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: RegisterWindowUpdateListener01
 * @tc.desc: check RegisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterWindowUpdateListener01, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowUpdateListeners_;
    windowManager->pImpl_->windowUpdateListenerAgent_ = nullptr;
    windowManager->pImpl_->windowUpdateListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterWindowUpdateListener(nullptr));

    sptr<TestWindowUpdateListener> listener = sptr<TestWindowUpdateListener>::MakeSptr();
    windowManager->RegisterWindowUpdateListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->windowUpdateListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowUpdateListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowUpdateListener01
 * @tc.desc: check UnregisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterWindowUpdateListener01, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowUpdateListeners_;
    windowManager->pImpl_->windowUpdateListenerAgent_ = nullptr;
    windowManager->pImpl_->windowUpdateListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterWindowUpdateListener(nullptr));

    sptr<TestWindowUpdateListener> listener1 = sptr<TestWindowUpdateListener>::MakeSptr();
    sptr<TestWindowUpdateListener> listener2 = sptr<TestWindowUpdateListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowUpdateListener(listener1));

    windowManager->RegisterWindowUpdateListener(listener1);
    windowManager->RegisterWindowUpdateListener(listener2);
    ASSERT_EQ(2, windowManager->pImpl_->windowUpdateListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowUpdateListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowUpdateListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->windowUpdateListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowUpdateListenerAgent_);

    windowManager->pImpl_->windowUpdateListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowUpdateListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->windowUpdateListeners_.size());

    windowManager->pImpl_->windowUpdateListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowUpdateListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterCallingWindowDisplayChangedListener1
 * @tc.desc: check RegisterCallingWindowDisplayChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterCallingWindowDisplayChangedListener1, Function | SmallTest | Level2)
{
    sptr<TestIKeyboardCallingWindowDisplayChangedListener> listener
        = sptr<TestIKeyboardCallingWindowDisplayChangedListener>::MakeSptr();
    auto& windowManager = WindowManagerLite::GetInstance();
    windowManager.pImpl_->callingDisplayChangedListeners_.clear();
    windowManager.pImpl_->callingDisplayListenerAgent_ = nullptr;
    WMError ret = windowManager.RegisterCallingWindowDisplayChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);

    ret = windowManager.RegisterCallingWindowDisplayChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowManager.pImpl_->callingDisplayListenerAgent_);
    ASSERT_EQ(1, static_cast<uint32_t>(windowManager.pImpl_->callingDisplayChangedListeners_.size()));

    // Register the same listener repeatedly.
    ret = windowManager.RegisterCallingWindowDisplayChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(1, static_cast<uint32_t>(windowManager.pImpl_->callingDisplayChangedListeners_.size()));
}

/**
 * @tc.name: RegisterCallingWindowDisplayChangedListener2
 * @tc.desc: check RegisterCallingWindowDisplayChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterCallingWindowDisplayChangedListener2, Function | SmallTest | Level2)
{
    sptr<TestIKeyboardCallingWindowDisplayChangedListener> listener1
        = sptr<TestIKeyboardCallingWindowDisplayChangedListener>::MakeSptr();
    sptr<TestIKeyboardCallingWindowDisplayChangedListener> listener2
        = sptr<TestIKeyboardCallingWindowDisplayChangedListener>::MakeSptr();
    
    auto& windowManager = WindowManagerLite::GetInstance();
    windowManager.pImpl_->callingDisplayChangedListeners_.clear();
    windowManager.pImpl_->callingDisplayListenerAgent_ = nullptr;
    WMError ret = windowManager.RegisterCallingWindowDisplayChangedListener(listener1);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowManager.pImpl_->callingDisplayListenerAgent_);
    ASSERT_EQ(1, static_cast<uint32_t>(windowManager.pImpl_->callingDisplayChangedListeners_.size()));

    ret = windowManager.RegisterCallingWindowDisplayChangedListener(listener2);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(2, static_cast<uint32_t>(windowManager.pImpl_->callingDisplayChangedListeners_.size()));

    ret = windowManager.UnregisterCallingWindowDisplayChangedListener(listener1);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(1, static_cast<uint32_t>(windowManager.pImpl_->callingDisplayChangedListeners_.size()));
    ASSERT_NE(nullptr, windowManager.pImpl_->callingDisplayListenerAgent_);

    // Unregister the same listener repeatedly.
    ret = windowManager.UnregisterCallingWindowDisplayChangedListener(listener1);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(1, static_cast<uint32_t>(windowManager.pImpl_->callingDisplayChangedListeners_.size()));

    ret = windowManager.UnregisterCallingWindowDisplayChangedListener(listener2);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(0, static_cast<uint32_t>(windowManager.pImpl_->callingDisplayChangedListeners_.size()));
    ASSERT_EQ(nullptr, windowManager.pImpl_->callingDisplayListenerAgent_);
}

/**
 * @tc.name: RegisterCallingWindowDisplayChangedListener3
 * @tc.desc: check RegisterCallingWindowDisplayChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterCallingWindowDisplayChangedListener3, Function | SmallTest | Level2)
{
    sptr<TestIKeyboardCallingWindowDisplayChangedListener> listener
        = sptr<TestIKeyboardCallingWindowDisplayChangedListener>::MakeSptr();
    
    auto& windowManager = WindowManagerLite::GetInstance();
    windowManager.pImpl_->callingDisplayChangedListeners_.clear();
    windowManager.pImpl_->callingDisplayListenerAgent_ = nullptr;
    WMError ret = windowManager.UnregisterCallingWindowDisplayChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    
    ret = windowManager.UnregisterCallingWindowDisplayChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyCallingWindowDisplayChanged1
 * @tc.desc: check NotifyCallingWindowDisplayChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyCallingWindowDisplayChanged1, Function | SmallTest | Level2)
{
    sptr<TestIKeyboardCallingWindowDisplayChangedListener> listener
        = sptr<TestIKeyboardCallingWindowDisplayChangedListener>::MakeSptr();
    
    auto& windowManager = WindowManagerLite::GetInstance();
    windowManager.pImpl_->callingDisplayChangedListeners_.clear();
    windowManager.pImpl_->callingDisplayListenerAgent_ = nullptr;

    const CallingWindowInfo& callingWindowInfo = {86, 57256, 12, 100};
    windowManager.NotifyCallingWindowDisplayChanged(callingWindowInfo);
    ASSERT_EQ(false, listener->isNotified);

    windowManager.pImpl_->callingDisplayChangedListeners_.emplace_back(nullptr);
    windowManager.NotifyCallingWindowDisplayChanged(callingWindowInfo);
    ASSERT_EQ(false, listener->isNotified);
}

/**
 * @tc.name: NotifyCallingWindowDisplayChanged2
 * @tc.desc: check NotifyCallingWindowDisplayChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, NotifyCallingWindowDisplayChanged2, Function | SmallTest | Level2)
{
    sptr<TestIKeyboardCallingWindowDisplayChangedListener> listener1
        = sptr<TestIKeyboardCallingWindowDisplayChangedListener>::MakeSptr();
    
    sptr<TestIKeyboardCallingWindowDisplayChangedListener> listener2
        = sptr<TestIKeyboardCallingWindowDisplayChangedListener>::MakeSptr();
    
    auto& windowManager = WindowManagerLite::GetInstance();
    windowManager.pImpl_->callingDisplayChangedListeners_.clear();
    windowManager.pImpl_->callingDisplayListenerAgent_ = nullptr;

    windowManager.pImpl_->callingDisplayChangedListeners_.emplace_back(listener1);
    windowManager.pImpl_->callingDisplayChangedListeners_.emplace_back(listener2);

    const CallingWindowInfo& callingWindowInfo = {86, 57256, 12, 100};
    windowManager.NotifyCallingWindowDisplayChanged(callingWindowInfo);
    ASSERT_EQ(true, listener1->isNotified);
    ASSERT_EQ(true, listener2->isNotified);
    ASSERT_EQ(true, CheckCallingWindowInfo(callingWindowInfo, listener1->info));
    ASSERT_EQ(true, CheckCallingWindowInfo(callingWindowInfo, listener2->info));
}

/**
 * @tc.name: UpdatePiPWindowStateChanged
 * @tc.desc: UpdatePiPWindowStateChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UpdatePiPWindowStateChanged, TestSize.Level1)
{
    WindowManagerLite lite;
    auto windowManager = WindowManagerLite::GetInstance(-1);
    sptr<TestPiPStateChangedListener> listener = nullptr;
    lite.pImpl_->pipStateChangedListeners_.push_back(listener);
    const std::string& bundleName = "test";
    windowManager->UpdatePiPWindowStateChanged(bundleName, true);
    lite.pImpl_->UpdatePiPWindowStateChanged(bundleName, true);

    lite.pImpl_->pipStateChangedListeners_.clear();
    listener = sptr<TestPiPStateChangedListener>::MakeSptr();
    lite.pImpl_->pipStateChangedListeners_.push_back(listener);
    windowManager->UpdatePiPWindowStateChanged(bundleName, true);
    lite.pImpl_->UpdatePiPWindowStateChanged(bundleName, true);

    lite.pImpl_->pipStateChangedListeners_.clear();
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterPiPStateChangedListener(listener));
}

/**
 * @tc.name: UnregisterFocusChangedListener
 * @tc.desc: UnregisterFocusChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterFocusChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    sptr<TestFocusChangedListener> listener = sptr<TestFocusChangedListener>::MakeSptr();
    ASSERT_TRUE(listener != nullptr);

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterFocusChangedListener(listener));
}

/**
 * @tc.name: UnregisterVisibilityChangedListener
 * @tc.desc: UnregisterVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterVisibilityChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    sptr<TestVisibilityChangedListener> listener = sptr<TestVisibilityChangedListener>::MakeSptr();
    ASSERT_TRUE(listener != nullptr);

    windowManager->RegisterVisibilityChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibilityChangedListener(listener));
}

/**
 * @tc.name: ProcessRegisterWindowInfoChangeCallback01
 * @tc.desc: Check ProcessRegisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, ProcessRegisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    WindowInfoKey observedInfo = WindowInfoKey::VISIBILITY_STATE;
    auto ret = WindowManagerLite::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ret = WindowManagerLite::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    observedInfo = WindowInfoKey::BUNDLE_NAME;
    ret = WindowManagerLite::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, listener);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: ProcessUnregisterWindowInfoChangeCallback01
 * @tc.desc: Check ProcessUnregisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, ProcessUnregisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    WindowInfoKey observedInfo = WindowInfoKey::VISIBILITY_STATE;
    auto ret = WindowManagerLite::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ret = WindowManagerLite::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    observedInfo = WindowInfoKey::BUNDLE_NAME;
    ret = WindowManagerLite::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, listener);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: RegisterWindowInfoChangeCallback01
 * @tc.desc: Check RegisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    auto interestInfoSizeOld = listener->GetInterestInfo().size();
    std::unordered_set<WindowInfoKey> observedInfo;
    observedInfo.insert(WindowInfoKey::VISIBILITY_STATE);
    auto ret = WindowManagerLite::GetInstance().RegisterWindowInfoChangeCallback(observedInfo, listener);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    ASSERT_EQ(interestInfoSizeOld + 1, listener->GetInterestInfo().size());
    std::unordered_set<WindowInfoKey> observedInfo1;
    observedInfo1.insert(WindowInfoKey::BUNDLE_NAME);
    ret = WindowManagerLite::GetInstance().RegisterWindowInfoChangeCallback(observedInfo1, listener);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: UnregisterWindowInfoChangeCallback01
 * @tc.desc: Check UnregisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    auto interestInfoSizeOld = listener->GetInterestInfo().size();
    std::unordered_set<WindowInfoKey> observedInfo;
    observedInfo.insert(WindowInfoKey::VISIBILITY_STATE);
    auto ret = WindowManagerLite::GetInstance().UnregisterWindowInfoChangeCallback(observedInfo, listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(interestInfoSizeOld + 1, listener->GetInterestInfo().size());
    std::unordered_set<WindowInfoKey> observedInfo1;
    observedInfo1.insert(WindowInfoKey::BUNDLE_NAME);
    ret = WindowManagerLite::GetInstance().UnregisterWindowInfoChangeCallback(observedInfo1, listener);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: RegisterVisibilityStateChangedListener01
 * @tc.desc: check RegisterVisibilityStateChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RegisterVisibilityStateChangedListener01, Function | SmallTest | Level2)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowVisibilityStateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowVisibilityStateListeners_;
    windowManager->pImpl_->windowVisibilityStateListenerAgent_ = nullptr;
    windowManager->pImpl_->windowVisibilityStateListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterVisibilityStateChangedListener(nullptr));

    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    windowManager->RegisterVisibilityStateChangedListener(listener);
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowVisibilityStateListenerAgent_);

    // to check that the same listner can not be registered twice
    windowManager->RegisterVisibilityStateChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->windowVisibilityStateListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowVisibilityStateListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterVisibilityStateChangedListener01
 * @tc.desc: check UnregisterVisibilityStateChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, UnregisterVisibilityStateChangedListener01, Function | SmallTest | Level2)
{
    auto windowManager = WindowManagerLite::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowVisibilityStateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowVisibilityStateListeners_;
    windowManager->pImpl_->windowVisibilityStateListenerAgent_ = sptr<WindowManagerAgentLite>::MakeSptr();
    windowManager->pImpl_->windowVisibilityStateListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterVisibilityStateChangedListener(nullptr));

    sptr<TestWindowVisibilityStateListener> listener1 = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    sptr<TestWindowVisibilityStateListener> listener2 = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowManager->UnregisterVisibilityStateChangedListener(listener1));

    windowManager->RegisterVisibilityStateChangedListener(listener1);
    windowManager->RegisterVisibilityStateChangedListener(listener2);

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibilityStateChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibilityStateChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->windowVisibilityStateListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowVisibilityStateListenerAgent_);

    windowManager->pImpl_->windowVisibilityStateListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibilityStateChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->windowVisibilityStateListeners_.size());

    windowManager->pImpl_->windowVisibilityStateListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowVisibilityStateListeners_ = oldListeners;
}

/**
 * @tc.name: SendPointerEventForHover
 * @tc.desc: SendPointerEventForHover
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, SendPointerEventForHover, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManagerLite::GetInstance();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    WMError ret = windowManager.SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    pointerEvent = MMI::PointerEvent::Create();
    ret = windowManager.SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);

    pointerEvent->pointerAction_ = MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER;
    pointerEvent->sourceType_ = MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN;
    ret = windowManager.SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: GetInstance
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, GetInstanceMulti, TestSize.Level1)
{
    sptr<WindowAdapterLite> instance = nullptr;
    int32_t userId = 101;
    instance = WindowAdapterLite::GetInstance(userId);
    ASSERT_NE(instance, nullptr);

    // branch overried
    instance = WindowAdapterLite::GetInstance(userId);
    ASSERT_NE(instance, nullptr);
}

/**
 * @tc.name: RemoveInstanceByUserId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerLiteTest, RemoveInstanceByUserId, TestSize.Level1)
{
    ASSERT_EQ(WMError::WM_OK, WindowManagerLite::RemoveInstanceByUserId(101));
}
}
} // namespace
} // namespace OHOS::Rosen
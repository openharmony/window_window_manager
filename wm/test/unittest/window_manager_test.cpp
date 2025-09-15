/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "iremote_object_mocker.h"
#include "mock_window_adapter.h"
#include "scene_board_judgement.h"
#include "scene_session_manager.h"
#include "singleton_mocker.h"
#include "window_manager.h"

#include "window_manager.cpp"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    std::string g_errLog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
        const char *msg)
    {
        g_errLog = msg;
    }
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class TestCameraFloatWindowChangedListener : public ICameraFloatWindowChangedListener {
public:
    void OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing) override
    {
        WLOGI("TestCameraFloatWindowChangedListener [%{public}u, %{public}u]", accessTokenId, isShowing);
    };
};

class TestVisibilityChangedListener : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
        WLOGI("TestVisibilityChangedListener");
    };
};

class TestSystemBarChangedListener : public ISystemBarChangedListener {
public:
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override
    {
        WLOGI("TestSystemBarChangedListener");
    };
};

class TestWindowSystemBarPropertyChangedListener : public IWindowSystemBarPropertyChangedListener {
public:
    int32_t count_ = 0;
    void OnWindowSystemBarPropertyChanged(WindowType type, const SystemBarProperty& systemBarProperty) override
    {
        count_ = 1;
        TLOGI(WmsLogTag::WMS_IMMS, "TestSystemBarChangedListener");
    }
};

class TestWindowUpdateListener : public IWindowUpdateListener {
public:
    void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
    {
        WLOGI("TestWindowUpdateListener");
    };
};

class TestWindowVisibilityStateListener : public IWindowInfoChangedListener {
public:
    void OnWindowInfoChanged(
        const std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>>& windowInfoList) override
    {
        WLOGI("TestWindowUpdateListener");
    };
};

class TestWindowDisplayIdChangeListener : public IWindowInfoChangedListener {
public:
    void OnWindowInfoChanged(
        const std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>>& windowInfoList) override
    {
        TLOGI(WmsLogTag::WMS_ATTRIBUTE, "TestWindowDisplayIdChangeListener");
    };
};

class TestWindowRectChangedListener : public IWindowInfoChangedListener {
public:
    void OnWindowInfoChanged(
        const std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>>& windowInfoList) override
    {
        TLOGI(WmsLogTag::WMS_ATTRIBUTE, "TestWindowRectChangedListener");
    };
};

class TestWindowModeChangedListenerForPropertyChange : public IWindowInfoChangedListener {
public:
    int32_t count_ = 0;

    void OnWindowInfoChanged(
        const std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>>& windowInfoList) override
    {
        TLOGI(WmsLogTag::WMS_ATTRIBUTE, "TestWindowModeChangedListenerForPropertyChange");
        ++count_;
    };
};

class TestFloatingScaleChangedListener : public IWindowInfoChangedListener {
public:
    int32_t count_ = 0;

    void OnWindowInfoChanged(
        const std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>>& windowInfoList) override
    {
        TLOGI(WmsLogTag::WMS_ATTRIBUTE, "TestFloatingScaleChangedListener");
        ++count_;
    };
};

class TestWindowModeChangedListener : public IWindowModeChangedListener {
public:
    void OnWindowModeUpdate(WindowModeType mode) override
    {
        WLOGI("TestWindowModeChangedListener");
    };
};

class TestWaterMarkFlagChangeListener : public IWaterMarkFlagChangedListener {
public:
    void OnWaterMarkFlagUpdate(bool showWaterMark) override
    {
        WLOGI("TestWaterMarkFlagChangeListener");
    };
};

class TestGestureNavigationEnabledChangedListener : public IGestureNavigationEnabledChangedListener {
public:
    void OnGestureNavigationEnabledUpdate(bool enable) override
    {
        WLOGI("TestGestureNavigationEnabledChangedListener");
    };
};

class TestDisplayInfoChangedListener : public IDisplayInfoChangedListener {
public:
    void OnDisplayInfoChange(const sptr<IRemoteObject>& token,
                             DisplayId displayId,
                             float density,
                             DisplayOrientation orientation) override
    {
        TLOGI(WmsLogTag::DMS, "TestDisplayInfoChangedListener");
    }
};

class TestVisibleWindowNumChangedListener : public IVisibleWindowNumChangedListener {
public:
    void OnVisibleWindowNumChange(const std::vector<VisibleWindowNumInfo>& visibleWindowNumInfo) override
    {
        for (const auto& num : visibleWindowNumInfo) {
            GTEST_LOG_(INFO) << "displayId " << num.displayId << ", visibleWindowNum " << num.visibleWindowNum;
        }
    };
};

class TestDrawingContentChangedListener : public IDrawingContentChangedListener {
public:
    void OnWindowDrawingContentChanged(const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingInfo)
    {
        TLOGI(WmsLogTag::DMS, "TestDrawingContentChangedListener");
    }
};

class TestFocusChangedListener : public IFocusChangedListener {
public:
    void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo)
    {
        TLOGI(WmsLogTag::DMS, "TestFocusChangedListener OnFocused()");
    }
    void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
    {
        TLOGI(WmsLogTag::DMS, "TestFocusChangedListener OnUnfocused()");
    }
};

class TestWindowStyleChangedListener : public IWindowStyleChangedListener {
public:
    void OnWindowStyleUpdate(WindowStyleType styleType)
    {
        TLOGI(WmsLogTag::DMS, "TestWindowStyleChangedListener");
    }
};

class TestWindowPidVisibilityChangedListener : public IWindowPidVisibilityChangedListener {
public:
    void NotifyWindowPidVisibilityChanged(const sptr<WindowPidVisibilityInfo>& info)
    {
        TLOGI(WmsLogTag::DMS, "TestWindowPidVisibilityChangedListener");
    }
};

class TestIWindowLifeCycleListener : public IWindowLifeCycleListener {
public:
    void OnWindowDestroyed(const WindowLifeCycleInfo& lifeCycleInfo, void* jsWindowNapiValue) override
    {
        listenerLifeCycleInfo.windowId = lifeCycleInfo.windowId;
        listenerLifeCycleInfo.windowType = lifeCycleInfo.windowType;
        listenerLifeCycleInfo.windowName = lifeCycleInfo.windowName;
    }

    TestIWindowLifeCycleListener()
    {
        listenerLifeCycleInfo.windowId = 0;
        listenerLifeCycleInfo.windowType = WindowType::SYSTEM_WINDOW_END;
        listenerLifeCycleInfo.windowName = "";
    }

    WindowLifeCycleInfo listenerLifeCycleInfo;
};

class WindowManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class TestWMSListener : public IWMSConnectionChangedListener {
public:
    void OnConnected(int32_t userId, int32_t screenId) override {}
    void OnDisconnected(int32_t userId, int32_t screenId) override {}
};

void WindowManagerTest::SetUpTestCase() {}

void WindowManagerTest::TearDownTestCase() {}

void WindowManagerTest::SetUp() {}

void WindowManagerTest::TearDown() {}

namespace {
/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetVisibilityWindowInfo01, TestSize.Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<sptr<WindowVisibilityInfo>> infos;
    infos.clear();
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_WINDOW));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
}

/**
 * @tc.name: ToggleShownStateForAllAppWindows
 * @tc.desc: ToggleShownStateForAllAppWindows ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, ToggleShownStateForAllAppWindows, TestSize.Level1)
{
    auto instance = WindowManager::GetInstance(-1);
    ASSERT_NE(nullptr, instance);
    instance->ToggleShownStateForAllAppWindows();
}

/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetAccessibilityWindowInfo01, TestSize.Level1)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    infos.clear();
    auto instance = WindowManager::GetInstance(-1);
    ASSERT_NE(nullptr, instance);
    instance->GetAccessibilityWindowInfo(infos);
}

/**
 * @tc.name: GetUnreliableWindowInfo
 * @tc.desc: GetUnreliableWindowInfo ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetUnreliableWindowInfo, TestSize.Level1)
{
    std::unique_ptr<Mocker> mocker = std::make_unique<Mocker>();
    int32_t windowId = 0;
    std::vector<sptr<UnreliableWindowInfo>> infos;
    EXPECT_CALL(mocker->Mock(), GetUnreliableWindowInfo(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().GetUnreliableWindowInfo(windowId, infos));
}

/**
 * @tc.name: GetSnapshotByWindowId01
 * @tc.desc: Check GetSnapshotByWindowId01
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetSnapshotByWindowId01, TestSize.Level1)
{
    auto& windowManager = WindowManager::GetInstance();
    int32_t windowId = -1;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    WMError ret = windowManager.GetSnapshotByWindowId(windowId, pixelMap);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    } else {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    }
}

/**
 * @tc.name: NotifyScreenshotEvent01
 * @tc.desc: Check NotifyScreenshotEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyScreenshotEvent01, TestSize.Level1)
{
    ScreenshotEventType type = ScreenshotEventType::SCROLL_SHOT_START;
    auto& windowManager = WindowManager::GetInstance();
    WMError ret = windowManager.NotifyScreenshotEvent(type);
    EXPECT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: RegisterCameraFloatWindowChangedListener01
 * @tc.desc: check RegisterCameraFloatWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterCameraFloatWindowChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->cameraFloatWindowChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->cameraFloatWindowChangedListeners_;
    windowManager->pImpl_->cameraFloatWindowChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->cameraFloatWindowChangedListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterCameraFloatWindowChangedListener(nullptr));

    sptr<TestCameraFloatWindowChangedListener> listener = sptr<TestCameraFloatWindowChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterCameraFloatWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager->pImpl_->cameraFloatWindowChangedListeners_.size());

    // to check that the same listner can not be registered twice
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterCameraFloatWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager->pImpl_->cameraFloatWindowChangedListeners_.size());

    windowManager->pImpl_->cameraFloatWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->cameraFloatWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterCameraFloatWindowChangedListener01
 * @tc.desc: check UnregisterCameraFloatWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterCameraFloatWindowChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->cameraFloatWindowChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->cameraFloatWindowChangedListeners_;
    windowManager->pImpl_->cameraFloatWindowChangedListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager->pImpl_->cameraFloatWindowChangedListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterCameraFloatWindowChangedListener(nullptr));

    sptr<TestCameraFloatWindowChangedListener> listener1 = sptr<TestCameraFloatWindowChangedListener>::MakeSptr();
    sptr<TestCameraFloatWindowChangedListener> listener2 = sptr<TestCameraFloatWindowChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraFloatWindowChangedListener(listener1));

    windowManager->RegisterCameraFloatWindowChangedListener(listener1);
    windowManager->RegisterCameraFloatWindowChangedListener(listener2);
    ASSERT_EQ(2, windowManager->pImpl_->cameraFloatWindowChangedListeners_.size());
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraFloatWindowChangedListener(listener1));

    windowManager->UnregisterCameraFloatWindowChangedListener(listener2);
    ASSERT_EQ(0, windowManager->pImpl_->cameraFloatWindowChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->cameraFloatWindowChangedListenerAgent_);

    windowManager->pImpl_->cameraFloatWindowChangedListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterCameraFloatWindowChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->cameraFloatWindowChangedListeners_.size());

    windowManager->pImpl_->cameraFloatWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->cameraFloatWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterVisibilityChangedListener01
 * @tc.desc: check RegisterVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterVisibilityChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowVisibilityListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowVisibilityListeners_;
    windowManager->pImpl_->windowVisibilityListenerAgent_ = nullptr;
    windowManager->pImpl_->windowVisibilityListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterVisibilityChangedListener(nullptr));

    sptr<TestVisibilityChangedListener> listener = sptr<TestVisibilityChangedListener>::MakeSptr();
    windowManager->RegisterVisibilityChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    // to check that the same listner can not be registered twice
    windowManager->RegisterVisibilityChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->windowVisibilityListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowVisibilityListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterVisibilityChangedListener01
 * @tc.desc: check UnregisterVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterVisibilityChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowVisibilityListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowVisibilityListeners_;
    windowManager->pImpl_->windowVisibilityListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager->pImpl_->windowVisibilityListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterVisibilityChangedListener(nullptr));

    sptr<TestVisibilityChangedListener> listener1 = sptr<TestVisibilityChangedListener>::MakeSptr();
    sptr<TestVisibilityChangedListener> listener2 = sptr<TestVisibilityChangedListener>::MakeSptr();

    windowManager->RegisterVisibilityChangedListener(listener1);
    windowManager->RegisterVisibilityChangedListener(listener2);

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibilityChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibilityChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->windowVisibilityListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowVisibilityListenerAgent_);

    windowManager->pImpl_->windowVisibilityListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibilityChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->windowVisibilityListeners_.size());

    windowManager->pImpl_->windowVisibilityListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowVisibilityListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWindowUpdateListener01
 * @tc.desc: check RegisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowUpdateListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowUpdateListeners_;
    windowManager->pImpl_->windowUpdateListenerAgent_ = nullptr;
    windowManager->pImpl_->windowUpdateListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterWindowUpdateListener(nullptr));

    sptr<TestWindowUpdateListener> listener = sptr<TestWindowUpdateListener>::MakeSptr();
    windowManager->RegisterWindowUpdateListener(listener);
    ASSERT_NE(nullptr, windowManager->pImpl_->windowUpdateListenerAgent_);

    // to check that the same listner can not be registered twice
    windowManager->RegisterWindowUpdateListener(listener);
    ASSERT_EQ(1, windowManager->pImpl_->windowUpdateListeners_.size());

    windowManager->pImpl_->windowUpdateListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowUpdateListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowUpdateListener01
 * @tc.desc: check UnregisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowUpdateListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager->pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowUpdateListeners_;
    windowManager->pImpl_->windowUpdateListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
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
 * @tc.name: RegisterWindowModeChangedListener01
 * @tc.desc: check RegisterWindowModeChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowModeChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowModeListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowModeListeners_;
    windowManager->pImpl_->windowModeListenerAgent_ = nullptr;
    windowManager->pImpl_->windowModeListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterWindowModeChangedListener(nullptr));

    sptr<TestWindowModeChangedListener> listener = sptr<TestWindowModeChangedListener>::MakeSptr();
    windowManager->RegisterWindowModeChangedListener(listener);
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowModeListenerAgent_);

    windowManager->pImpl_->windowModeListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowModeListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowModeChangedListener01
 * @tc.desc: check UnregisterWindowModeChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowModeChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowModeListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowModeListeners_;
    windowManager->pImpl_->windowModeListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
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
    ASSERT_EQ(nullptr, windowManager->pImpl_->windowModeListenerAgent_);

    windowManager->pImpl_->windowModeListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWindowModeChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->windowModeListeners_.size());

    windowManager->pImpl_->windowModeListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowModeListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterSystemBarChangedListener01
 * @tc.desc: check RegisterSystemBarChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterSystemBarChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->systemBarChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->systemBarChangedListeners_;
    windowManager->pImpl_->systemBarChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->systemBarChangedListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterSystemBarChangedListener(nullptr));

    sptr<ISystemBarChangedListener> listener = sptr<TestSystemBarChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(1, windowManager->pImpl_->systemBarChangedListeners_.size());

    // to check that the same listner can not be registered twice
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(1, windowManager->pImpl_->systemBarChangedListeners_.size());

    windowManager->pImpl_->systemBarChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->systemBarChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterSystemBarChangedListener01
 * @tc.desc: check UnregisterSystemBarChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterSystemBarChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->systemBarChangedListenerAgent_;
    auto oldListeners = windowManager->pImpl_->systemBarChangedListeners_;
    windowManager->pImpl_->systemBarChangedListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager->pImpl_->systemBarChangedListeners_.clear();
    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterSystemBarChangedListener(nullptr));

    sptr<TestSystemBarChangedListener> listener1 = sptr<TestSystemBarChangedListener>::MakeSptr();
    sptr<TestSystemBarChangedListener> listener2 = sptr<TestSystemBarChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterSystemBarChangedListener(listener1));

    windowManager->RegisterSystemBarChangedListener(listener1);
    windowManager->RegisterSystemBarChangedListener(listener2);
    ASSERT_EQ(2, windowManager->pImpl_->systemBarChangedListeners_.size());

    windowManager->UnregisterSystemBarChangedListener(listener1);
    windowManager->UnregisterSystemBarChangedListener(listener2);
    ASSERT_EQ(0, windowManager->pImpl_->systemBarChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->systemBarChangedListenerAgent_);

    windowManager->pImpl_->systemBarChangedListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterSystemBarChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->systemBarChangedListeners_.size());

    windowManager->pImpl_->systemBarChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->systemBarChangedListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWaterMarkListener01
 * @tc.desc: check RegisterWaterMarkListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWaterMarkFlagChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);

    windowManager->pImpl_->waterMarkFlagChangeAgent_ = nullptr;
    windowManager->pImpl_->waterMarkFlagChangeListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterWaterMarkFlagChangedListener(nullptr));

    auto listener = sptr<TestWaterMarkFlagChangeListener>::MakeSptr();
    windowManager->RegisterWaterMarkFlagChangedListener(listener);
    ASSERT_EQ(1, windowManager->pImpl_->waterMarkFlagChangeListeners_.size());
    ASSERT_NE(nullptr, windowManager->pImpl_->waterMarkFlagChangeAgent_);

    // to check that the same listner can not be registered twice
    windowManager->RegisterWaterMarkFlagChangedListener(listener);
    ASSERT_EQ(1, windowManager->pImpl_->waterMarkFlagChangeListeners_.size());
}

/**
 * @tc.name: UnregisterWaterMarkFlagChangedListener01
 * @tc.desc: check UnregisterWaterMarkFlagChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWaterMarkFlagChangedListener01, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    windowManager->pImpl_->waterMarkFlagChangeAgent_ = nullptr;
    windowManager->pImpl_->waterMarkFlagChangeListeners_.clear();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterWaterMarkFlagChangedListener(nullptr));

    sptr<TestWaterMarkFlagChangeListener> listener1 = sptr<TestWaterMarkFlagChangeListener>::MakeSptr();
    sptr<TestWaterMarkFlagChangeListener> listener2 = sptr<TestWaterMarkFlagChangeListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWaterMarkFlagChangedListener(listener1));

    windowManager->RegisterWaterMarkFlagChangedListener(listener1);
    windowManager->RegisterWaterMarkFlagChangedListener(listener2);
    ASSERT_EQ(2, windowManager->pImpl_->waterMarkFlagChangeListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWaterMarkFlagChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWaterMarkFlagChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->waterMarkFlagChangeListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->waterMarkFlagChangeAgent_);

    // if agent == nullptr, it can not be crashed.
    windowManager->pImpl_->waterMarkFlagChangeListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterWaterMarkFlagChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->waterMarkFlagChangeListeners_.size());
}

/**
 * @tc.name: RegisterGestureNavigationEnabledChangedListener
 * @tc.desc: check RegisterGestureNavigationEnabledChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterGestureNavigationEnabledChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);

    windowManager->pImpl_->gestureNavigationEnabledAgent_ = nullptr;
    windowManager->pImpl_->gestureNavigationEnabledListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterGestureNavigationEnabledChangedListener(nullptr));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<TestGestureNavigationEnabledChangedListener> listener =
        sptr<TestGestureNavigationEnabledChangedListener>::MakeSptr();

    windowManager->RegisterGestureNavigationEnabledChangedListener(listener);
    ASSERT_EQ(1, windowManager->pImpl_->gestureNavigationEnabledListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->gestureNavigationEnabledAgent_);

    // to check that the same listner can not be registered twice
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterGestureNavigationEnabledChangedListener(listener));
    ASSERT_EQ(1, windowManager->pImpl_->gestureNavigationEnabledListeners_.size());
}

/**
 * @tc.name: UnregisterGestureNavigationEnabledChangedListener
 * @tc.desc: check UnregisterGestureNavigationEnabledChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterGestureNavigationEnabledChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    windowManager->pImpl_->gestureNavigationEnabledAgent_ = nullptr;
    windowManager->pImpl_->gestureNavigationEnabledListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterGestureNavigationEnabledChangedListener(nullptr));

    sptr<TestGestureNavigationEnabledChangedListener> listener1 =
        sptr<TestGestureNavigationEnabledChangedListener>::MakeSptr();
    sptr<TestGestureNavigationEnabledChangedListener> listener2 =
        sptr<TestGestureNavigationEnabledChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM,
              windowManager->UnregisterGestureNavigationEnabledChangedListener(listener1));

    windowManager->RegisterGestureNavigationEnabledChangedListener(listener1);
    windowManager->RegisterGestureNavigationEnabledChangedListener(listener2);
    ASSERT_EQ(2, windowManager->pImpl_->gestureNavigationEnabledListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterGestureNavigationEnabledChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterGestureNavigationEnabledChangedListener(listener2));
    ASSERT_EQ(0, windowManager->pImpl_->gestureNavigationEnabledListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->gestureNavigationEnabledAgent_);

    // if agent == nullptr, it can not be crashed.
    windowManager->pImpl_->gestureNavigationEnabledListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterGestureNavigationEnabledChangedListener(listener1));
    ASSERT_EQ(0, windowManager->pImpl_->gestureNavigationEnabledListeners_.size());
}

/**
 * @tc.name: GetUIContentRemoteObj
 * @tc.desc: GetUIContentRemoteObj
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetUIContentRemoteObj, TestSize.Level1)
{
    sptr<IRemoteObject> remoteObj;
    WMError res = WindowManager::GetInstance().GetUIContentRemoteObj(1, remoteObj);
    ASSERT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: window GetFocusWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetFocusWindowInfo, TestSize.Level1)
{
    FocusChangeInfo focusInfo;
    DisplayId displayId = 0;
    WindowManager::GetInstance().GetFocusWindowInfo(focusInfo, displayId);
    WindowAdapter windowAdapter;

    windowAdapter.GetFocusWindowInfo(focusInfo, displayId);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: MinimizeAllAppWindows
 * @tc.desc: window MinimizeAllAppWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, MinimizeAllAppWindows, TestSize.Level1)
{
    DisplayId displayId = 0;
    WMError ret = WindowManager::GetInstance().MinimizeAllAppWindows(displayId);
    ASSERT_NE(ret, WMError::WM_OK);
}

/**
 * @tc.name: SetWindowLayoutMode
 * @tc.desc: window SetWindowLayoutMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetWindowLayoutMode, TestSize.Level1)
{
    WMError ret = WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::BASE);
    ASSERT_EQ(ret, WMError::WM_OK);

    ret = WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    ASSERT_EQ(ret, WMError::WM_OK);

    ret = WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::END);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: SkipSnapshotForAppProcess
 * @tc.desc: check SkipSnapshotForAppProcess
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SkipSnapshotForAppProcess, TestSize.Level1)
{
    int32_t pid = 1000;
    bool skip = true;
    auto ret = WindowManager::GetInstance().SkipSnapshotForAppProcess(pid, skip);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus
 * @tc.desc: UpdateCameraFloatWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UpdateCameraFloatWindowStatus, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 0;
    bool isShowing = true;
    WindowManager::GetInstance().UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    EXPECT_FALSE(g_errLog.find("Camera float window, accessTokenId=%{private}u, isShowing=%{public}u")
        != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: NotifyWaterMarkFlagChangedResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWaterMarkFlagChangedResult, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    bool showwatermark = true;
    WindowManager::GetInstance().NotifyWaterMarkFlagChangedResult(showwatermark);
    EXPECT_FALSE(g_errLog.find("Camera float window, accessTokenId=%{private}u, isShowing=%{public}u")
        != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: NotifyGestureNavigationEnabledResult
 * @tc.desc: NotifyGestureNavigationEnabledResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyGestureNavigationEnabledResult, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    bool enable = true;
    WindowManager::GetInstance().NotifyGestureNavigationEnabledResult(enable);
    EXPECT_FALSE(g_errLog.find("Notify gesture navigation enable result, enable=%{public}d")
        != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: DumpSessionAll
 * @tc.desc: DumpSessionAll
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, DumpSessionAll, TestSize.Level1)
{
    std::vector<std::string> infos;
    infos.push_back("DumpSessionWithId");
    WMError res = WindowManager::GetInstance().DumpSessionAll(infos);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: DumpSessionWithId
 * @tc.desc: DumpSessionWithId
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, DumpSessionWithId, TestSize.Level1)
{
    std::vector<std::string> infos;
    infos.push_back("DumpSessionWithId");
    int32_t persistentId = 0;
    WMError res = WindowManager::GetInstance().DumpSessionWithId(persistentId, infos);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: GetWindowModeType01
 * @tc.desc: GetWindowModeType01
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetWindowModeType01, TestSize.Level1)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    infos.clear();
    WindowModeType windowModeType;
    auto instance = WindowManager::GetInstance(-1);
    ASSERT_NE(nullptr, instance);
    instance->GetWindowModeType(windowModeType);
}

/**
 * @tc.name: RegisterVisibleWindowNumChangedListener
 * @tc.desc: check RegisterVisibleWindowNumChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterVisibleWindowNumChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);

    windowManager->pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->visibleWindowNumChangedListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterVisibleWindowNumChangedListener(nullptr));

    sptr<TestVisibleWindowNumChangedListener> listener = sptr<TestVisibleWindowNumChangedListener>::MakeSptr();
    windowManager->RegisterVisibleWindowNumChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);
}

/**
 * @tc.name: UnregisterVisibleWindowNumChangedListener
 * @tc.desc: check UnregisterVisibleWindowNumChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterVisibleWindowNumChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    windowManager->pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->visibleWindowNumChangedListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterVisibleWindowNumChangedListener(nullptr));

    sptr<TestVisibleWindowNumChangedListener> listener1 = sptr<TestVisibleWindowNumChangedListener>::MakeSptr();
    sptr<TestVisibleWindowNumChangedListener> listener2 = sptr<TestVisibleWindowNumChangedListener>::MakeSptr();

    windowManager->RegisterVisibleWindowNumChangedListener(listener1);
    windowManager->RegisterVisibleWindowNumChangedListener(listener2);

    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibleWindowNumChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibleWindowNumChangedListener(listener2));

    // if agent == nullptr, it can not be crashed.
    windowManager->pImpl_->visibleWindowNumChangedListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager->UnregisterVisibleWindowNumChangedListener(listener1));
}

/**
 * @tc.name: RegisterAndOnVisibleWindowNumChanged
 * @tc.desc: check RegisterAndOnVisibleWindowNumChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterAndOnVisibleWindowNumChanged, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    windowManager->pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
    windowManager->pImpl_->visibleWindowNumChangedListeners_.clear();

    sptr<TestVisibleWindowNumChangedListener> listener = sptr<TestVisibleWindowNumChangedListener>::MakeSptr();
    windowManager->RegisterVisibleWindowNumChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    VisibleWindowNumInfo newInfo;
    newInfo.displayId = 0;
    newInfo.visibleWindowNum = 2;
    visibleWindowNumInfo.push_back(newInfo);
    windowManager->UpdateVisibleWindowNum(visibleWindowNumInfo);
}

/**
 * @tc.name: Test01
 * @tc.desc: Test01
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, Test01, TestSize.Level1)
{
    sptr<IWMSConnectionChangedListener> listener = nullptr;
    WMError res = WindowManager::GetInstance().RegisterWMSConnectionChangedListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, res);
    WMError res1 = WindowManager::GetInstance().UnregisterWMSConnectionChangedListener();
    ASSERT_EQ(WMError::WM_OK, res1);
    WindowManager::GetInstance().RaiseWindowToTop(5);
    WMError res3 = WindowManager::GetInstance().NotifyWindowExtensionVisibilityChange(5, 5, true);
    ASSERT_EQ(WMError::WM_OK, res3);
    WindowManager::GetInstance().ShiftAppWindowFocus(0, 1);
}

/**
 * @tc.name: RegisterDisplayInfoChangedListener
 * @tc.desc: check RegisterDisplayInfoChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterDisplayInfoChangedListener, TestSize.Level1)
{
    auto windowManager = WindowManager::GetInstance(-1);
    windowManager->pImpl_->displayInfoChangedListeners_.clear();

    sptr<IRemoteObject> targetToken = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterDisplayInfoChangedListener(targetToken, nullptr));

    targetToken = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterDisplayInfoChangedListener(targetToken, nullptr));

    sptr<IDisplayInfoChangedListener> listener = sptr<TestDisplayInfoChangedListener>::MakeSptr();
    sptr<IDisplayInfoChangedListener> listener2 = sptr<TestDisplayInfoChangedListener>::MakeSptr();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterDisplayInfoChangedListener(nullptr, listener));
    ASSERT_EQ(0, windowManager->pImpl_->displayInfoChangedListeners_.size());
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterDisplayInfoChangedListener(targetToken, listener));
    ASSERT_EQ(1, windowManager->pImpl_->displayInfoChangedListeners_.size());

    // to check that the same listner can not be registered twice
    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterDisplayInfoChangedListener(targetToken, listener));
    ASSERT_EQ(1, windowManager->pImpl_->displayInfoChangedListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager->RegisterDisplayInfoChangedListener(targetToken, listener2));
    ASSERT_EQ(1, windowManager->pImpl_->displayInfoChangedListeners_.size());
    auto iter = windowManager->pImpl_->displayInfoChangedListeners_.find(targetToken);
    ASSERT_NE(windowManager->pImpl_->displayInfoChangedListeners_.end(), iter);
    ASSERT_EQ(2, iter->second.size());
}

/**
 * @tc.name: UnregisterDisplayInfoChangedListener
 * @tc.desc: check UnregisterDisplayInfoChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterDisplayInfoChangedListener, TestSize.Level1)
{
    auto& windowManager = WindowManager::GetInstance();
    windowManager.pImpl_->displayInfoChangedListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterDisplayInfoChangedListener(nullptr, nullptr));

    sptr<IRemoteObject> targetToken = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterDisplayInfoChangedListener(targetToken, nullptr));

    sptr<IDisplayInfoChangedListener> listener = sptr<TestDisplayInfoChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterDisplayInfoChangedListener(nullptr, listener));

    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterDisplayInfoChangedListener(targetToken, listener));

    sptr<IRemoteObject> targetToken2 = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IDisplayInfoChangedListener> listener2 = sptr<TestDisplayInfoChangedListener>::MakeSptr();

    // the same token can have multiple listeners
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterDisplayInfoChangedListener(targetToken, listener));
    ASSERT_EQ(1, windowManager.pImpl_->displayInfoChangedListeners_.size());
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterDisplayInfoChangedListener(targetToken, listener2));
    ASSERT_EQ(1, windowManager.pImpl_->displayInfoChangedListeners_.size());
    auto iter = windowManager.pImpl_->displayInfoChangedListeners_.find(targetToken);
    ASSERT_NE(windowManager.pImpl_->displayInfoChangedListeners_.end(), iter);
    ASSERT_EQ(2, iter->second.size());

    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterDisplayInfoChangedListener(targetToken2, listener));
    ASSERT_EQ(2, windowManager.pImpl_->displayInfoChangedListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterDisplayInfoChangedListener(targetToken, listener));
    ASSERT_EQ(1, iter->second.size());
    ASSERT_EQ(2, windowManager.pImpl_->displayInfoChangedListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterDisplayInfoChangedListener(targetToken, listener2));
    ASSERT_EQ(1, windowManager.pImpl_->displayInfoChangedListeners_.size());
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterDisplayInfoChangedListener(targetToken2, listener));
    ASSERT_EQ(0, windowManager.pImpl_->displayInfoChangedListeners_.size());
}

/**
 * @tc.name: NotifyDisplayInfoChanged
 * @tc.desc: check NotifyDisplayInfoChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyDisplayInfoChanged, TestSize.Level1)
{
    sptr<IRemoteObject> targetToken = sptr<IRemoteObjectMocker>::MakeSptr();
    DisplayId displayId = 0;
    float density = 0.2f;
    DisplayOrientation orientation = DisplayOrientation::UNKNOWN;

    auto& windowManager = WindowManager::GetInstance();
    windowManager.pImpl_->displayInfoChangedListeners_.clear();
    windowManager.pImpl_->NotifyDisplayInfoChanged(targetToken, displayId, density, orientation);

    sptr<IRemoteObject> targetToken2 = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IDisplayInfoChangedListener> listener = sptr<TestDisplayInfoChangedListener>::MakeSptr();

    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterDisplayInfoChangedListener(targetToken, listener));
    ASSERT_EQ(1, windowManager.pImpl_->displayInfoChangedListeners_.size());
    windowManager.pImpl_->NotifyDisplayInfoChanged(targetToken, displayId, density, orientation);

    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterDisplayInfoChangedListener(targetToken2, listener));
    ASSERT_EQ(2, windowManager.pImpl_->displayInfoChangedListeners_.size());
    windowManager.pImpl_->NotifyDisplayInfoChanged(targetToken, displayId, density, orientation);
    windowManager.pImpl_->NotifyDisplayInfoChanged(targetToken2, displayId, density, orientation);
    // no repeated notification is sent if parameters do not change
    windowManager.pImpl_->NotifyDisplayInfoChanged(targetToken, displayId, density, orientation);
}

/**
 * @tc.name: RegisterWindowStyleChangedListener
 * @tc.desc: check RegisterWindowStyleChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowStyleChangedListener, TestSize.Level1)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowStyleListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowStyleListeners_;
    windowManager.pImpl_->windowStyleListenerAgent_ = nullptr;
    windowManager.pImpl_->windowStyleListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWindowStyleChangedListener(nullptr));

    sptr<IWindowStyleChangedListener> listener = sptr<TestWindowStyleChangedListener>::MakeSptr();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
}

/**
 * @tc.name: UnregisterWindowStyleChangedListener
 * @tc.desc: check UnregisterWindowStyleChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowStyleChangedListener, TestSize.Level1)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowStyleListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowStyleListeners_;
    windowManager.pImpl_->windowStyleListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager.pImpl_->windowStyleListeners_.clear();
    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterWindowStyleChangedListener(nullptr));

    sptr<TestWindowStyleChangedListener> listener1 = sptr<TestWindowStyleChangedListener>::MakeSptr();
    sptr<TestWindowStyleChangedListener> listener2 = sptr<TestWindowStyleChangedListener>::MakeSptr();
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowStyleChangedListener(listener1));
}

/**
 * @tc.name: NotifyWindowStyleChange
 * @tc.desc: check NotifyWindowStyleChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWindowStyleChange, TestSize.Level1)
{
    WindowStyleType type = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;
    auto ret = WindowManager::GetInstance().NotifyWindowStyleChange(type);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetWindowStyleType
 * @tc.desc: check GetWindowStyleType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetWindowStyleType, TestSize.Level1)
{
    WindowStyleType type;
    type = WindowManager::GetInstance().GetWindowStyleType();
    ASSERT_EQ(Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT, type);
}

/**
 * @tc.name: ShiftAppWindowFocus01
 * @tc.desc: check ShiftAppWindowFocus
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, ShiftAppWindowFocus01, TestSize.Level1)
{
    WMError ret = WindowManager::GetInstance().ShiftAppWindowFocus(0, 1);
    ASSERT_NE(WMError::WM_OK, ret);
}

/**
 * @tc.name: RegisterVisibleWindowNumChangedListener01
 * @tc.desc: check RegisterVisibleWindowNumChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterVisibleWindowNumChangedListener01, TestSize.Level1)
{
    WMError ret;
    sptr<IVisibleWindowNumChangedListener> listener = sptr<TestVisibleWindowNumChangedListener>::MakeSptr();
    ret = WindowManager::GetInstance().RegisterVisibleWindowNumChangedListener(listener);
    ASSERT_NE(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().RegisterVisibleWindowNumChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UnregisterVisibleWindowNumChangedListener01
 * @tc.desc: check UnregisterVisibleWindowNumChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterVisibleWindowNumChangedListener01, TestSize.Level1)
{
    WMError ret;
    sptr<IVisibleWindowNumChangedListener> listener = sptr<TestVisibleWindowNumChangedListener>::MakeSptr();
    ret = WindowManager::GetInstance().UnregisterVisibleWindowNumChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().UnregisterVisibleWindowNumChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: RegisterDrawingContentChangedListener01
 * @tc.desc: check RegisterDrawingContentChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterDrawingContentChangedListener01, TestSize.Level1)
{
    WMError ret;
    sptr<IDrawingContentChangedListener> listener = sptr<TestDrawingContentChangedListener>::MakeSptr();
    ret = WindowManager::GetInstance().RegisterDrawingContentChangedListener(listener);
    ASSERT_NE(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().RegisterDrawingContentChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UnregisterDrawingContentChangedListener01
 * @tc.desc: check UnregisterDrawingContentChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterDrawingContentChangedListener01, TestSize.Level1)
{
    WMError ret;
    sptr<IDrawingContentChangedListener> listener = sptr<TestDrawingContentChangedListener>::MakeSptr();
    ret = WindowManager::GetInstance().UnregisterDrawingContentChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().UnregisterDrawingContentChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: RegisterFocusChangedListener01
 * @tc.desc: check RegisterFocusChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterFocusChangedListener01, TestSize.Level1)
{
    WMError ret;
    sptr<IFocusChangedListener> listener = sptr<TestFocusChangedListener>::MakeSptr();
    ret = WindowManager::GetInstance().RegisterFocusChangedListener(listener);
    ASSERT_NE(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().RegisterFocusChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UnregisterFocusChangedListener01
 * @tc.desc: check UnregisterFocusChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterFocusChangedListener01, TestSize.Level1)
{
    WMError ret;
    sptr<IFocusChangedListener> listener = sptr<TestFocusChangedListener>::MakeSptr();
    ret = WindowManager::GetInstance().UnregisterFocusChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().UnregisterFocusChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: SetProcessWatermark
 * @tc.desc: check SetProcessWatermark
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetProcessWatermark, TestSize.Level1)
{
    int32_t pid = 1000;
    const std::string watermarkName = "SetProcessWatermarkName";
    bool isEnabled = true;
    auto ret = WindowManager::GetInstance().SetProcessWatermark(pid, watermarkName, isEnabled);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyDisplayInfoChange01
 * @tc.desc: check NotifyDisplayInfoChange, Token is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyDisplayInfoChange01, TestSize.Level1)
{
    WMError ret = WindowManager::GetInstance().NotifyDisplayInfoChange(nullptr, 1, 2, DisplayOrientation::PORTRAIT);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: NotifyDisplayInfoChange02
 * @tc.desc: check NotifyDisplayInfoChange, Token is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyDisplayInfoChange02, TestSize.Level1)
{
    sptr<IRemoteObject> Token = sptr<IRemoteObjectMocker>::MakeSptr();
    WMError ret = WindowManager::GetInstance().NotifyDisplayInfoChange(Token, 1, 2, DisplayOrientation::PORTRAIT);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyWMSDisconnected01
 * @tc.desc: check NotifyWMSDisconnected
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWMSDisconnected01, TestSize.Level1)
{
    WMError ret = WindowManager::GetInstance().ShiftAppWindowFocus(0, 1);
    ASSERT_NE(WMError::WM_OK, ret);
    WindowManager::GetInstance().pImpl_->NotifyWMSDisconnected(1, 2);
}

/**
 * @tc.name: NotifyFocused01
 * @tc.desc: check NotifyFocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyFocused01, TestSize.Level1)
{
    sptr<FocusChangeInfo> focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr();
    ASSERT_NE(focusChangeInfo, nullptr);

    WindowManager::GetInstance().pImpl_->NotifyFocused(focusChangeInfo);
}

/**
 * @tc.name: NotifyUnfocused01
 * @tc.desc: check NotifyUnfocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyUnfocused01, TestSize.Level1)
{
    sptr<FocusChangeInfo> focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr();
    ASSERT_NE(focusChangeInfo, nullptr);

    WindowManager::GetInstance().pImpl_->NotifyUnfocused(focusChangeInfo);
}

/**
 * @tc.name: NotifyAccessibilityWindowInfo01
 * @tc.desc: check NotifyAccessibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyAccessibilityWindowInfo01, TestSize.Level1)
{
    WMError ret = WindowManager::GetInstance().ShiftAppWindowFocus(0, 1);
    ASSERT_NE(WMError::WM_OK, ret);
    sptr<AccessibilityWindowInfo> info = sptr<AccessibilityWindowInfo>::MakeSptr();
    ASSERT_NE(info, nullptr);

    std::vector<sptr<AccessibilityWindowInfo>> infos;
    infos.push_back(info);
    WindowManager::GetInstance().pImpl_->NotifyAccessibilityWindowInfo(infos, WindowUpdateType::WINDOW_UPDATE_ACTIVE);

    infos.clear();
    infos.push_back(nullptr);
    WindowManager::GetInstance().pImpl_->NotifyAccessibilityWindowInfo(infos, WindowUpdateType::WINDOW_UPDATE_ACTIVE);
}

/**
 * @tc.name: NotifyVisibleWindowNumChanged01
 * @tc.desc: check NotifyVisibleWindowNumChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyVisibleWindowNumChanged01, TestSize.Level1)
{
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    WindowManager::GetInstance().pImpl_->visibleWindowNumChangedListeners_.clear();
    WindowManager::GetInstance().pImpl_->visibleWindowNumChangedListeners_.push_back(nullptr);
    WindowManager::GetInstance().pImpl_->NotifyVisibleWindowNumChanged(visibleWindowNumInfo);

    sptr<IVisibleWindowNumChangedListener> listener = sptr<TestVisibleWindowNumChangedListener>::MakeSptr();
    ASSERT_NE(listener, nullptr);
    WindowManager::GetInstance().pImpl_->visibleWindowNumChangedListeners_.clear();
    WindowManager::GetInstance().pImpl_->visibleWindowNumChangedListeners_.push_back(listener);
    WindowManager::GetInstance().pImpl_->NotifyVisibleWindowNumChanged(visibleWindowNumInfo);
}

/**
 * @tc.name: RegisterWindowPidVisibilityChangedListener
 * @tc.desc: check RegisterWindowPidVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowPidVisibilityChangedListener, TestSize.Level1)
{
    WMError ret;
    sptr<IWindowPidVisibilityChangedListener> listener = sptr<TestWindowPidVisibilityChangedListener>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    ret = WindowManager::GetInstance().RegisterWindowPidVisibilityChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().RegisterWindowPidVisibilityChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UnregisterWindowPidVisibilityChangedListener
 * @tc.desc: check UnregisterWindowPidVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowPidVisibilityChangedListener, TestSize.Level1)
{
    WMError ret;
    sptr<IWindowPidVisibilityChangedListener> listener = sptr<TestWindowPidVisibilityChangedListener>::MakeSptr();
    ret = WindowManager::GetInstance().UnregisterWindowPidVisibilityChangedListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().UnregisterWindowPidVisibilityChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: NotifyWindowPidVisibilityChanged
 * @tc.desc: NotifyWindowPidVisibilityChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWindowPidVisibilityChanged, TestSize.Level1)
{
    sptr<WindowPidVisibilityInfo> info = sptr<WindowPidVisibilityInfo>::MakeSptr();
    WindowManager::GetInstance().NotifyWindowPidVisibilityChanged(info);
    ASSERT_NE(info, nullptr);
}

/**
 * @tc.name: UpdateScreenLockStatusForApp
 * @tc.desc: check UpdateScreenLockStatusForApp
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UpdateScreenLockStatusForApp, TestSize.Level1)
{
    auto ret = WindowManager::GetInstance().UpdateScreenLockStatusForApp("", true);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: GetDisplayIdByWindowId
 * @tc.desc: check GetDisplayIdByWindowId
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetDisplayIdByWindowId, TestSize.Level1)
{
    const std::vector<uint64_t> windowIds = { 1, 2 };
    std::unordered_map<uint64_t, DisplayId> windowDisplayIdMap;
    auto ret = WindowManager::GetInstance().GetDisplayIdByWindowId(windowIds, windowDisplayIdMap);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetGlobalDragResizeType
 * @tc.desc: check SetGlobalDragResizeType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetGlobalDragResizeType, TestSize.Level1)
{
    DragResizeType dragResizeType = DragResizeType::RESIZE_EACH_FRAME;
    auto ret = WindowManager::GetInstance().SetGlobalDragResizeType(dragResizeType);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetGlobalDragResizeType
 * @tc.desc: check GetGlobalDragResizeType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetGlobalDragResizeType, TestSize.Level1)
{
    DragResizeType dragResizeType = DragResizeType::RESIZE_TYPE_UNDEFINED;
    auto ret = WindowManager::GetInstance().GetGlobalDragResizeType(dragResizeType);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetAppDragResizeType
 * @tc.desc: check SetAppDragResizeType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetAppDragResizeType, TestSize.Level1)
{
    DragResizeType dragResizeType = DragResizeType::RESIZE_EACH_FRAME;
    const std::string bundleName = "test";
    auto ret = WindowManager::GetInstance().SetAppDragResizeType(bundleName, dragResizeType);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetAppDragResizeType
 * @tc.desc: check GetAppDragResizeType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetAppDragResizeType, TestSize.Level1)
{
    DragResizeType dragResizeType = DragResizeType::RESIZE_TYPE_UNDEFINED;
    const std::string bundleName = "test";
    auto ret = WindowManager::GetInstance().GetAppDragResizeType(bundleName, dragResizeType);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: EffectiveDragResizeType
 * @tc.desc: test EffectiveDragResizeType
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, EffectiveDragResizeType, TestSize.Level1)
{
    DragResizeType dragResizeType = DragResizeType::RESIZE_TYPE_UNDEFINED;
    const std::string bundleName = "test";

    DragResizeType globalDragResizeType = DragResizeType::RESIZE_WHEN_DRAG_END;
    DragResizeType appDragResizeType = DragResizeType::RESIZE_EACH_FRAME;
    WindowManager::GetInstance().SetGlobalDragResizeType(globalDragResizeType);
    WindowManager::GetInstance().SetAppDragResizeType(bundleName, appDragResizeType);
    WindowManager::GetInstance().GetAppDragResizeType(bundleName, dragResizeType);
    ASSERT_EQ(dragResizeType, globalDragResizeType);
    WindowManager::GetInstance().SetGlobalDragResizeType(DragResizeType::RESIZE_TYPE_UNDEFINED);
    WindowManager::GetInstance().GetAppDragResizeType(bundleName, dragResizeType);
    ASSERT_EQ(dragResizeType, appDragResizeType);
}

/**
 * @tc.name: SetAppKeyFramePolicy01
 * @tc.desc: check SetAppKeyFramePolicy enable
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetAppKeyFramePolicy01, TestSize.Level1)
{
    const std::string bundleName = "test";
    KeyFramePolicy keyFramePolicy;
    keyFramePolicy.dragResizeType_ = DragResizeType::RESIZE_KEY_FRAME;
    keyFramePolicy.animationDelay_ = 200;
    auto ret = WindowManager::GetInstance().SetAppKeyFramePolicy(bundleName, keyFramePolicy);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetAppKeyFramePolicy02
 * @tc.desc: check SetAppKeyFramePolicy disable
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetAppKeyFramePolicy02, TestSize.Level1)
{
    const std::string bundleName = "test";
    KeyFramePolicy keyFramePolicy;
    keyFramePolicy.dragResizeType_ = DragResizeType::RESIZE_TYPE_UNDEFINED;
    auto ret = WindowManager::GetInstance().SetAppKeyFramePolicy(bundleName, keyFramePolicy);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyWMSConnected
 * @tc.desc: check NotifyWMSConnected
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWMSConnected, TestSize.Level1)
{
    WMError ret = WindowManager::GetInstance().ShiftAppWindowFocus(0, 1);
    ASSERT_NE(WMError::WM_OK, ret);
    WindowManager::GetInstance().pImpl_->NotifyWMSConnected(1, 2);
}

/**
 * @tc.name: GetAllWindowLayoutInfo
 * @tc.desc: check GetAllWindowLayoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetAllWindowLayoutInfo, TestSize.Level1)
{
    DisplayId displayId = 1;
    std::vector<sptr<WindowLayoutInfo>> infos;
    auto ret = WindowManager::GetInstance().GetAllWindowLayoutInfo(displayId, infos);
    ASSERT_EQ(SingletonContainer::Get<WindowAdapter>().GetAllWindowLayoutInfo(displayId, infos), ret);
}

/**
 * @tc.name: GetTopNavDestinationName
 * @tc.desc: test GetTopNavDestinationName rpc is ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetTopNavDestinationName, TestSize.Level1)
{
    int32_t windowId = 888;
    std::string topNavDestName;
    auto ret = WindowManager::GetInstance().GetTopNavDestinationName(windowId, topNavDestName);
    EXPECT_EQ(SingletonContainer::Get<WindowAdapter>().GetTopNavDestinationName(windowId, topNavDestName), ret);
}

/**
 * @tc.name: SetWatermarkImageForApp
 * @tc.desc: test SetWatermarkImageForApp rpc is ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetWatermarkImageForApp, TestSize.Level1)
{
    auto ret = WindowManager::GetInstance().SetWatermarkImageForApp(nullptr);
    EXPECT_EQ(SingletonContainer::Get<WindowAdapter>().SetWatermarkImageForApp(nullptr), ret);
}

/**
 * @tc.name: ShiftAppWindowPointerEvent
 * @tc.desc: check ShiftAppWindowPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, ShiftAppWindowPointerEvent, TestSize.Level1)
{
    int32_t sourceWindowId = 1;
    int32_t targetWindowId = 1;
    int32_t fingerId = 0;
    auto ret = WindowManager::GetInstance().ShiftAppWindowPointerEvent(sourceWindowId, targetWindowId, fingerId);
    ASSERT_EQ(SingletonContainer::Get<WindowAdapter>().ShiftAppWindowPointerEvent(
        sourceWindowId, targetWindowId, fingerId), ret);
}

/**
 * @tc.name: OnWMSConnectionChanged
 * @tc.desc: check OnWMSConnectionChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, OnWMSConnectionChanged, TestSize.Level1)
{
    int32_t userId = 1;
    int32_t screenId = 1;
    int32_t isConnected = 1;

    WMError ret_1 = WindowManager::GetInstance().ShiftAppWindowFocus(0, 1);
    ASSERT_NE(WMError::WM_OK, ret_1);
    WindowManager::GetInstance().OnWMSConnectionChanged(userId, screenId, isConnected);

    isConnected = 0;
    WMError ret_2 = WindowManager::GetInstance().ShiftAppWindowFocus(0, 1);
    ASSERT_NE(WMError::WM_OK, ret_2);
    WindowManager::GetInstance().OnWMSConnectionChanged(userId, screenId, isConnected);
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: check RequestFocus
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RequestFocus, TestSize.Level1)
{
    WindowManager windowManager;
    int32_t persistentId = 1;
    bool isFocused = true;
    bool byForeground = true;
    WindowFocusChangeReason reason = WindowFocusChangeReason::CLICK;
    auto result = windowManager.RequestFocus(
        persistentId, isFocused, byForeground, reason);
    ASSERT_NE(result, WMError::WM_OK);
}

/**
 * @tc.name: MinimizeByWindowId
 * @tc.desc: Check MinimizeByWindowId
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, MinimizeByWindowId, TestSize.Level1)
{
    auto& windowManager = WindowManager::GetInstance();
    std::vector<int32_t> windowIds;
    WMError ret_1 = windowManager.MinimizeByWindowId(windowIds);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret_1);
    windowIds = {-1, 0};
    WMError ret_2 = windowManager.MinimizeByWindowId(windowIds);
    ASSERT_EQ(WMError::WM_OK, ret_2);
}

/**
 * @tc.name: UpdateAnimationSpeedMultiplierForPid
 * @tc.desc: Check UpdateAnimationSpeedMultiplierForPid
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UpdateAnimationSpeedMultiplierForPid, TestSize.Level1)
{
    auto& windowManager = WindowManager::GetInstance();
    pid_t pid = 15234;
    float multiplier = 2.0f;
    WMError ret_1 = windowManager.UpdateAnimationSpeedMultiplierForPid(pid, multiplier);
    ASSERT_EQ(WMError::WM_OK, ret_1);
    multiplier = 1.0f;
    WMError ret_2 = windowManager.UpdateAnimationSpeedMultiplierForPid(pid, multiplier);
    ASSERT_EQ(WMError::WM_OK, ret_2);
}

/**
 * @tc.name: ProcessRegisterWindowInfoChangeCallback01
 * @tc.desc: Check ProcessRegisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, ProcessRegisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    WindowInfoKey observedInfo = WindowInfoKey::VISIBILITY_STATE;
    auto ret = WindowManager::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    observedInfo = WindowInfoKey::DISPLAY_ID;
    ret = WindowManager::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    observedInfo = WindowInfoKey::WINDOW_RECT;
    ret = WindowManager::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    ret = WindowManager::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    observedInfo = WindowInfoKey::BUNDLE_NAME;
    ret = WindowManager::GetInstance().ProcessRegisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: ProcessUnregisterWindowInfoChangeCallback01
 * @tc.desc: Check ProcessUnregisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, ProcessUnregisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    WindowInfoKey observedInfo = WindowInfoKey::VISIBILITY_STATE;
    auto ret = WindowManager::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_OK, ret);
    observedInfo = WindowInfoKey::DISPLAY_ID;
    ret = WindowManager::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_OK, ret);
    observedInfo = WindowInfoKey::WINDOW_RECT;
    ret = WindowManager::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = WindowManager::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    observedInfo = WindowInfoKey::BUNDLE_NAME;
    ret = WindowManager::GetInstance().ProcessUnregisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: RegisterWindowInfoChangeCallback01
 * @tc.desc: Check RegisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    auto interestInfoSizeOld = listener->GetInterestInfo().size();
    std::unordered_set<WindowInfoKey> observedInfo;
    observedInfo.insert(WindowInfoKey::VISIBILITY_STATE);
    auto ret = WindowManager::GetInstance().RegisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    EXPECT_EQ(interestInfoSizeOld + 1, listener->GetInterestInfo().size());
    std::unordered_set<WindowInfoKey> observedInfo1;
    observedInfo1.insert(WindowInfoKey::BUNDLE_NAME);
    ret = WindowManager::GetInstance().RegisterWindowInfoChangeCallback(observedInfo1, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = WindowManager::GetInstance().RegisterWindowInfoChangeCallback(observedInfo, nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    listener->SetInterestWindowIds({1, 2, 3});
    interestInfoSizeOld = listener->GetInterestInfo().size();
    ret = WindowManager::GetInstance().RegisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(interestInfoSizeOld + 1, listener->GetInterestInfo().size());
}

/**
 * @tc.name: UnregisterWindowInfoChangeCallback01
 * @tc.desc: Check UnregisterWindowInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowInfoChangeCallback01, Function | SmallTest | Level2)
{
    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    auto interestInfoSizeOld = listener->GetInterestInfo().size();
    std::unordered_set<WindowInfoKey> observedInfo;
    observedInfo.insert(WindowInfoKey::VISIBILITY_STATE);
    auto ret = WindowManager::GetInstance().UnregisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(WMError::WM_OK, ret);
    EXPECT_EQ(interestInfoSizeOld + 1, listener->GetInterestInfo().size());
    std::unordered_set<WindowInfoKey> observedInfo1;
    observedInfo1.insert(WindowInfoKey::BUNDLE_NAME);
    ret = WindowManager::GetInstance().UnregisterWindowInfoChangeCallback(observedInfo1, listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = WindowManager::GetInstance().UnregisterWindowInfoChangeCallback(observedInfo, nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    listener->SetInterestWindowIds({1, 2, 3});
    interestInfoSizeOld = listener->GetInterestInfo().size();
    ret = WindowManager::GetInstance().UnregisterWindowInfoChangeCallback(observedInfo, listener);
    EXPECT_EQ(interestInfoSizeOld + 1, listener->GetInterestInfo().size());
}

/**
 * @tc.name: RegisterVisibilityStateChangedListener01
 * @tc.desc: check RegisterVisibilityStateChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterVisibilityStateChangedListener01, Function | SmallTest | Level2)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowVisibilityStateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowVisibilityStateListeners_;
    windowManager->pImpl_->windowVisibilityStateListenerAgent_ = nullptr;
    windowManager->pImpl_->windowVisibilityStateListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterVisibilityStateChangedListener(nullptr));

    sptr<TestWindowVisibilityStateListener> listener = sptr<TestWindowVisibilityStateListener>::MakeSptr();
    windowManager->RegisterVisibilityStateChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

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
HWTEST_F(WindowManagerTest, UnregisterVisibilityStateChangedListener01, Function | SmallTest | Level2)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowVisibilityStateListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowVisibilityStateListeners_;
    windowManager->pImpl_->windowVisibilityStateListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
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
 * @tc.name: RegisterDisplayIdChangedListener01
 * @tc.desc: check RegisterDisplayIdChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterDisplayIdChangedListener01, Function | SmallTest | Level2)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->WindowDisplayIdChangeListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowDisplayIdChangeListeners_;
    windowManager->pImpl_->WindowDisplayIdChangeListenerAgent_ = nullptr;
    windowManager->pImpl_->windowDisplayIdChangeListeners_.clear();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterVisibilityStateChangedListener(nullptr));

    sptr<TestWindowDisplayIdChangeListener> listener = sptr<TestWindowDisplayIdChangeListener>::MakeSptr();
    windowManager->RegisterVisibilityStateChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    // to check that the same listner can not be registered twice
    windowManager->RegisterVisibilityStateChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->WindowDisplayIdChangeListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowDisplayIdChangeListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterDisplayIdChangedListener01
 * @tc.desc: check UnregisterDisplayIdChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterDisplayIdChangedListener01, Function | SmallTest | Level2)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->WindowDisplayIdChangeListenerAgent_;
    auto oldListeners = windowManager->pImpl_->windowDisplayIdChangeListeners_;
    windowManager->pImpl_->WindowDisplayIdChangeListenerAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager->pImpl_->windowDisplayIdChangeListeners_.clear();

    // check nullpter
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->UnregisterDisplayIdChangedListener(nullptr));

    sptr<TestWindowDisplayIdChangeListener> listener1 = sptr<TestWindowDisplayIdChangeListener>::MakeSptr();
    sptr<TestWindowDisplayIdChangeListener> listener2 = sptr<TestWindowDisplayIdChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowManager->UnregisterDisplayIdChangedListener(listener1));

    windowManager->RegisterVisibilityStateChangedListener(listener1);
    windowManager->RegisterVisibilityStateChangedListener(listener2);

    EXPECT_EQ(WMError::WM_OK, windowManager->UnregisterDisplayIdChangedListener(listener1));
    EXPECT_EQ(WMError::WM_OK, windowManager->UnregisterDisplayIdChangedListener(listener2));
    EXPECT_EQ(0, windowManager->pImpl_->windowDisplayIdChangeListeners_.size());
    ASSERT_EQ(nullptr, windowManager->pImpl_->WindowDisplayIdChangeListenerAgent_);

    windowManager->pImpl_->windowDisplayIdChangeListeners_.emplace_back(listener1);
    EXPECT_EQ(WMError::WM_OK, windowManager->UnregisterDisplayIdChangedListener(listener1));
    EXPECT_EQ(0, windowManager->pImpl_->windowDisplayIdChangeListeners_.size());

    windowManager->pImpl_->WindowDisplayIdChangeListenerAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowDisplayIdChangeListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWindowSystemBarPropertyChangedListener
 * @tc.desc: check RegisterWindowSystemBarPropertyChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowSystemBarPropertyChangedListener, Function | SmallTest | Level2)
{
    auto windowManager = WindowManager::GetInstance(-1);
    auto oldWindowManagerAgent = windowManager->pImpl_->windowSystemBarPropertyChangeAgent_;
    auto oldListeners = windowManager->pImpl_->windowSystemBarPropertyChangedListeners_;
    windowManager->pImpl_->windowSystemBarPropertyChangeAgent_ = nullptr;
    windowManager->pImpl_->windowSystemBarPropertyChangedListeners_.clear();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager->RegisterWindowSystemBarPropertyChangedListener(nullptr));

    sptr<TestWindowSystemBarPropertyChangedListener> listener =
        sptr<TestWindowSystemBarPropertyChangedListener>::MakeSptr();
    windowManager->RegisterWindowSystemBarPropertyChangedListener(listener);
    ASSERT_NE(nullptr, windowManager);

    windowManager->pImpl_->windowSystemBarPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager->pImpl_->windowSystemBarPropertyChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowSystemBarPropertyChangedListener
 * @tc.desc: check UnregisterWindowSystemBarPropertyChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowSystemBarPropertyChangedListener, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowSystemBarPropertyChangeAgent_;
    auto oldListeners = windowManager.pImpl_->windowSystemBarPropertyChangedListeners_;
    windowManager.pImpl_->windowSystemBarPropertyChangeAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager.pImpl_->windowSystemBarPropertyChangedListeners_.clear();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterWindowSystemBarPropertyChangedListener(nullptr));
    sptr<TestWindowSystemBarPropertyChangedListener> listener =
        sptr<TestWindowSystemBarPropertyChangedListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_DO_NOTHING, windowManager.UnregisterWindowSystemBarPropertyChangedListener(listener));
    windowManager.pImpl_->windowSystemBarPropertyChangedListeners_.emplace_back(listener);
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterWindowSystemBarPropertyChangedListener(listener));
    windowManager.pImpl_->windowSystemBarPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowSystemBarPropertyChangedListeners_ = oldListeners;
}

/**
 * @tc.name: NotifyWindowSystemBarPropertyChange
 * @tc.desc: check NotifyWindowSystemBarPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWindowSystemBarPropertyChange, TestSize.Level1)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldListeners = windowManager.pImpl_->windowSystemBarPropertyChangedListeners_;
    SystemBarProperty systemBarProperty;
    WindowManager::GetInstance().pImpl_->NotifyWindowSystemBarPropertyChange(
        WindowType::WINDOW_TYPE_STATUS_BAR, systemBarProperty);
    sptr<TestWindowSystemBarPropertyChangedListener> listener =
        sptr<TestWindowSystemBarPropertyChangedListener>::MakeSptr();
    windowManager.pImpl_->windowSystemBarPropertyChangedListeners_.emplace_back(listener);
    EXPECT_EQ(1, windowManager.pImpl_->windowSystemBarPropertyChangedListeners_.size());
    WindowManager::GetInstance().pImpl_->NotifyWindowSystemBarPropertyChange(
        WindowType::WINDOW_TYPE_STATUS_BAR, systemBarProperty);
    EXPECT_EQ(1, listener->count_);
    windowManager.pImpl_->windowSystemBarPropertyChangedListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterFloatingScaleChangedListener01
 * @tc.desc: check RegisterFloatingScaleChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterFloatingScaleChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowPropertyChangeAgent_;
    auto oldListeners = windowManager.pImpl_->floatingScaleChangeListeners_;
    windowManager.pImpl_->windowPropertyChangeAgent_ = nullptr;
    windowManager.pImpl_->floatingScaleChangeListeners_.clear();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterFloatingScaleChangedListener(nullptr));

    sptr<TestFloatingScaleChangedListener> listener = sptr<TestFloatingScaleChangedListener>::MakeSptr();

    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowManager.RegisterFloatingScaleChangedListener(listener));
    EXPECT_EQ(0, windowManager.pImpl_->floatingScaleChangeListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowManager.RegisterFloatingScaleChangedListener(listener));
    EXPECT_EQ(0, windowManager.pImpl_->floatingScaleChangeListeners_.size());

    windowManager.pImpl_->windowPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->floatingScaleChangeListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterFloatingScaleChangedListener01
 * @tc.desc: check UnregisterFloatingScaleChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterFloatingScaleChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowPropertyChangeAgent_;
    auto oldListeners = windowManager.pImpl_->floatingScaleChangeListeners_;
    windowManager.pImpl_->windowPropertyChangeAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager.pImpl_->floatingScaleChangeListeners_.clear();

    // check nullpter
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterFloatingScaleChangedListener(nullptr));

    sptr<TestFloatingScaleChangedListener> listener1 = sptr<TestFloatingScaleChangedListener>::MakeSptr();
    sptr<TestFloatingScaleChangedListener> listener2 = sptr<TestFloatingScaleChangedListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowManager.UnregisterFloatingScaleChangedListener(listener1));

    windowManager.RegisterFloatingScaleChangedListener(listener1);
    windowManager.RegisterFloatingScaleChangedListener(listener2);
    EXPECT_EQ(0, windowManager.pImpl_->floatingScaleChangeListeners_.size());

    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterFloatingScaleChangedListener(listener1));
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterFloatingScaleChangedListener(listener2));
    EXPECT_EQ(0, windowManager.pImpl_->floatingScaleChangeListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowPropertyChangeAgent_);

    windowManager.pImpl_->floatingScaleChangeListeners_.emplace_back(listener1);
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterFloatingScaleChangedListener(listener1));
    EXPECT_EQ(0, windowManager.pImpl_->floatingScaleChangeListeners_.size());

    windowManager.pImpl_->windowPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->floatingScaleChangeListeners_ = oldListeners;
}

/**
 * @tc.name: NotifyFloatingScaleChange
 * @tc.desc: check NotifyFloatingScaleChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyFloatingScaleChange, TestSize.Level1)
{
    std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>> windowInfoList;
    windowInfoList.push_back({{WindowInfoKey::DISPLAY_ID, 5}});

    auto& windowManager = WindowManager::GetInstance();
    windowManager.pImpl_->floatingScaleChangeListeners_.clear();
    windowManager.pImpl_->NotifyFloatingScaleChange(windowInfoList);

    windowManager.pImpl_->floatingScaleChangeListeners_.push_back(nullptr);
    windowManager.pImpl_->NotifyFloatingScaleChange(windowInfoList);

    sptr<TestFloatingScaleChangedListener> listener =
        sptr<TestFloatingScaleChangedListener>::MakeSptr();

    windowManager.pImpl_->NotifyFloatingScaleChange(windowInfoList);
    EXPECT_EQ(listener->count_, 0);
}

/**
 * @tc.name: RegisterWindowModeChangedListenerForPropertyChange01
 * @tc.desc: check RegisterWindowModeChangedListenerForPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowModeChangedListenerForPropertyChange01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowPropertyChangeAgent_;
    auto oldListeners = windowManager.pImpl_->windowModeChangeListeners_;
    windowManager.pImpl_->windowPropertyChangeAgent_ = nullptr;
    windowManager.pImpl_->windowModeChangeListeners_.clear();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWindowModeChangedListenerForPropertyChange(nullptr));

    sptr<TestWindowModeChangedListenerForPropertyChange> listener =
        sptr<TestWindowModeChangedListenerForPropertyChange>::MakeSptr();

    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION,
        windowManager.RegisterWindowModeChangedListenerForPropertyChange(listener));
    EXPECT_EQ(0, windowManager.pImpl_->windowModeChangeListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION,
        windowManager.RegisterWindowModeChangedListenerForPropertyChange(listener));
    EXPECT_EQ(0, windowManager.pImpl_->windowModeChangeListeners_.size());

    windowManager.pImpl_->windowPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowModeChangeListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowModeChangedListenerForPropertyChange01
 * @tc.desc: check UnregisterWindowModeChangedListenerForPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowModeChangedListenerForPropertyChange01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowPropertyChangeAgent_;
    auto oldListeners = windowManager.pImpl_->windowModeChangeListeners_;
    windowManager.pImpl_->windowPropertyChangeAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager.pImpl_->windowModeChangeListeners_.clear();

    // check nullpter
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterWindowModeChangedListenerForPropertyChange(nullptr));

    sptr<TestWindowModeChangedListenerForPropertyChange> listener1 =
        sptr<TestWindowModeChangedListenerForPropertyChange>::MakeSptr();
    sptr<TestWindowModeChangedListenerForPropertyChange> listener2 =
        sptr<TestWindowModeChangedListenerForPropertyChange>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION,
        windowManager.UnregisterWindowModeChangedListenerForPropertyChange(listener1));

    windowManager.RegisterWindowModeChangedListenerForPropertyChange(listener1);
    windowManager.RegisterWindowModeChangedListenerForPropertyChange(listener2);
    EXPECT_EQ(0, windowManager.pImpl_->windowModeChangeListeners_.size());

    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterWindowModeChangedListenerForPropertyChange(listener1));
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterWindowModeChangedListenerForPropertyChange(listener2));
    EXPECT_EQ(0, windowManager.pImpl_->windowModeChangeListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowPropertyChangeAgent_);

    windowManager.pImpl_->windowModeChangeListeners_.emplace_back(listener1);
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterWindowModeChangedListenerForPropertyChange(listener1));
    EXPECT_EQ(0, windowManager.pImpl_->windowModeChangeListeners_.size());

    windowManager.pImpl_->windowPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowModeChangeListeners_ = oldListeners;
}

/**
 * @tc.name: NotifyWindowModeChangeForPropertyChange
 * @tc.desc: check NotifyWindowModeChangeForPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWindowModeChangeForPropertyChange, TestSize.Level1)
{
    std::vector<std::unordered_map<WindowInfoKey, WindowChangeInfoType>> windowInfoList;
    windowInfoList.push_back({{WindowInfoKey::DISPLAY_ID, 5}});

    auto& windowManager = WindowManager::GetInstance();
    windowManager.pImpl_->windowModeChangeListeners_.clear();
    windowManager.pImpl_->NotifyWindowModeChangeForPropertyChange(windowInfoList);

    windowManager.pImpl_->windowModeChangeListeners_.push_back(nullptr);
    windowManager.pImpl_->NotifyWindowModeChangeForPropertyChange(windowInfoList);

    sptr<TestWindowModeChangedListenerForPropertyChange> listener =
        sptr<TestWindowModeChangedListenerForPropertyChange>::MakeSptr();

    windowManager.pImpl_->NotifyWindowModeChangeForPropertyChange(windowInfoList);
    EXPECT_EQ(listener->count_, 0);
}

/**
 * @tc.name: RegisterRectChangedListener01
 * @tc.desc: check RegisterRectChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterRectChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowPropertyChangeAgent_;
    auto oldListeners = windowManager.pImpl_->windowRectChangeListeners_;
    windowManager.pImpl_->windowPropertyChangeAgent_ = nullptr;
    windowManager.pImpl_->windowRectChangeListeners_.clear();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterRectChangedListener(nullptr));
 
    sptr<TestWindowRectChangedListener> listener = sptr<TestWindowRectChangedListener>::MakeSptr();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
 
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, windowManager.RegisterRectChangedListener(listener));
    EXPECT_EQ(0, windowManager.pImpl_->windowRectChangeListeners_.size());
 
    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, windowManager.RegisterRectChangedListener(listener));
    EXPECT_EQ(0, windowManager.pImpl_->windowRectChangeListeners_.size());
 
    windowManager.pImpl_->windowPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowRectChangeListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterRectChangedListener01
 * @tc.desc: check UnregisterRectChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterRectChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowPropertyChangeAgent_;
    auto oldListeners = windowManager.pImpl_->windowRectChangeListeners_;
    windowManager.pImpl_->windowPropertyChangeAgent_ = sptr<WindowManagerAgent>::MakeSptr();
    windowManager.pImpl_->windowRectChangeListeners_.clear();

    // check nullpter
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterRectChangedListener(nullptr));

    sptr<TestWindowRectChangedListener> listener1 = sptr<TestWindowRectChangedListener>::MakeSptr();
    sptr<TestWindowRectChangedListener> listener2 = sptr<TestWindowRectChangedListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterRectChangedListener(listener1));

    windowManager.RegisterRectChangedListener(listener1);
    windowManager.RegisterRectChangedListener(listener2);
    EXPECT_EQ(0, windowManager.pImpl_->windowRectChangeListeners_.size());

    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterRectChangedListener(listener1));
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterRectChangedListener(listener2));
    EXPECT_EQ(0, windowManager.pImpl_->windowRectChangeListeners_.size());

    windowManager.pImpl_->windowRectChangeListeners_.emplace_back(listener1);
    EXPECT_EQ(WMError::WM_OK, windowManager.UnregisterRectChangedListener(listener1));
    EXPECT_EQ(0, windowManager.pImpl_->windowRectChangeListeners_.size());

    windowManager.pImpl_->windowPropertyChangeAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowRectChangeListeners_ = oldListeners;
}

/**
 * @tc.name: AnimateTo01
 * @tc.desc: check AnimateTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, AnimateTo01, Function | SmallTest | Level2)
{
    int32_t windowId = 1;
    WindowAnimationProperty animationProperty;
    WindowAnimationOption animationOption;
    animationProperty.targetScale = 1.5f;
    auto instance = WindowManager::GetInstance(-1);

    ASSERT_NE(nullptr, instance);
    instance->AnimateTo(windowId, animationProperty, animationOption);
}

/**
 * @tc.name: RegisterWindowLifeCycleListener01
 * @tc.desc: check RegisterWindowLifeCycleCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowLifeCycleListener01, TestSize.Level1)
{
    WMError ret;
    sptr<TestIWindowLifeCycleListener> listener = sptr<TestIWindowLifeCycleListener>::MakeSptr();
    ret = WindowManager::GetInstance().RegisterWindowLifeCycleCallback(listener);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().RegisterWindowLifeCycleCallback(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);

    ret = WindowManager::GetInstance().RegisterWindowLifeCycleCallback(listener);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: UnregisterWindowLifeCycleListener01
 * @tc.desc: check UnregisterWindowLifeCycleCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowLifeCycleListener01, TestSize.Level1)
{
    WMError ret;
    sptr<TestIWindowLifeCycleListener> listener = sptr<TestIWindowLifeCycleListener>::MakeSptr();
    ret = WindowManager::GetInstance().UnregisterWindowLifeCycleCallback(listener);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = WindowManager::GetInstance().UnregisterWindowLifeCycleCallback(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);

    ret = WindowManager::GetInstance().UnregisterWindowLifeCycleCallback(listener);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyWMSWindowDestroyed01
 * @tc.desc: check NotifyWMSWindowDestroyed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWMSWindowDestroyed01, TestSize.Level1)
{
    WMError ret;
    sptr<TestIWindowLifeCycleListener> listener = sptr<TestIWindowLifeCycleListener>::MakeSptr();
    ret = WindowManager::GetInstance().RegisterWindowLifeCycleCallback(listener);
    EXPECT_EQ(WMError::WM_OK, ret);

    WindowLifeCycleInfo lifeCycleInfo;
    lifeCycleInfo.windowId = 101;
    lifeCycleInfo.windowType = WindowType::APP_WINDOW_BASE;
    lifeCycleInfo.windowName = "window101";
    WindowManager::GetInstance().NotifyWMSWindowDestroyed(lifeCycleInfo);
    EXPECT_EQ(lifeCycleInfo.windowId, listener->listenerLifeCycleInfo.windowId);
    EXPECT_EQ(lifeCycleInfo.windowType, listener->listenerLifeCycleInfo.windowType);
    EXPECT_EQ(lifeCycleInfo.windowName, listener->listenerLifeCycleInfo.windowName);

    lifeCycleInfo.windowId = 102;
    lifeCycleInfo.windowType = WindowType::APP_SUB_WINDOW_BASE;
    lifeCycleInfo.windowName = "window102";
    WindowManager::GetInstance().NotifyWMSWindowDestroyed(lifeCycleInfo);
    EXPECT_EQ(lifeCycleInfo.windowId, listener->listenerLifeCycleInfo.windowId);
    EXPECT_EQ(lifeCycleInfo.windowType, listener->listenerLifeCycleInfo.windowType);
    EXPECT_EQ(lifeCycleInfo.windowName, listener->listenerLifeCycleInfo.windowName);

    lifeCycleInfo.windowId = 103;
    lifeCycleInfo.windowType = WindowType::SYSTEM_WINDOW_BASE;
    lifeCycleInfo.windowName = "window103";
    WindowManager::GetInstance().NotifyWMSWindowDestroyed(lifeCycleInfo);
    EXPECT_EQ(lifeCycleInfo.windowId, listener->listenerLifeCycleInfo.windowId);
    EXPECT_EQ(lifeCycleInfo.windowType, listener->listenerLifeCycleInfo.windowType);
    EXPECT_EQ(lifeCycleInfo.windowName, listener->listenerLifeCycleInfo.windowName);

    lifeCycleInfo.windowId = 104;
    lifeCycleInfo.windowType = WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE;
    lifeCycleInfo.windowName = "window104";
    WindowManager::GetInstance().NotifyWMSWindowDestroyed(lifeCycleInfo);
    EXPECT_EQ(lifeCycleInfo.windowId, listener->listenerLifeCycleInfo.windowId);
    EXPECT_EQ(lifeCycleInfo.windowType, listener->listenerLifeCycleInfo.windowType);
    EXPECT_EQ(lifeCycleInfo.windowName, listener->listenerLifeCycleInfo.windowName);

    lifeCycleInfo.windowId = 105;
    lifeCycleInfo.windowType = WindowType::SYSTEM_SUB_WINDOW_BASE;
    lifeCycleInfo.windowName = "window105";
    WindowManager::GetInstance().NotifyWMSWindowDestroyed(lifeCycleInfo);
    EXPECT_EQ(lifeCycleInfo.windowId, listener->listenerLifeCycleInfo.windowId);
    EXPECT_EQ(lifeCycleInfo.windowType, listener->listenerLifeCycleInfo.windowType);
    EXPECT_EQ(lifeCycleInfo.windowName, listener->listenerLifeCycleInfo.windowName);
}

/**
 * @tc.name: NotifyWMSWindowDestroyed02
 * @tc.desc: check NotifyWMSWindowDestroyed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWMSWindowDestroyed02, TestSize.Level1)
{
    WMError ret;
    sptr<TestIWindowLifeCycleListener> listener = sptr<TestIWindowLifeCycleListener>::MakeSptr();
    ret = WindowManager::GetInstance().UnregisterWindowLifeCycleCallback(listener);
    EXPECT_EQ(WMError::WM_OK, ret);

    WindowLifeCycleInfo lifeCycleInfo;
    lifeCycleInfo.windowId = 101;
    lifeCycleInfo.windowType = WindowType::APP_WINDOW_BASE;
    lifeCycleInfo.windowName = "window101";
    auto func = [](const std::string& windowName) {
        std::cout << "window name: " << windowName << std::endl;
        return napi_value();
    };
    WindowManager::GetInstance().RegisterGetJSWindowCallback(func);
    WindowManager::GetInstance().NotifyWMSWindowDestroyed(lifeCycleInfo);
    EXPECT_NE(lifeCycleInfo.windowId, listener->listenerLifeCycleInfo.windowId);
    EXPECT_NE(lifeCycleInfo.windowType, listener->listenerLifeCycleInfo.windowType);
    EXPECT_NE(lifeCycleInfo.windowName, listener->listenerLifeCycleInfo.windowName);
}

/**
 * @tc.name: AddSessionBlackList01
 * @tc.desc: check AddSessionBlackList
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, AddSessionBlackList01, TestSize.Level1)
{
    WMError ret;
    std::unordered_set<std::string> bundleNames;
    std::unordered_set<std::string> privacyWindowTags;
    ret = WindowManager::GetInstance().AddSessionBlackList(bundleNames, privacyWindowTags);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: RemoveSessionBlackList01
 * @tc.desc: check RemoveSessionBlackList
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RemoveSessionBlackList01, TestSize.Level1)
{
    WMError ret;
    std::unordered_set<std::string> bundleNames;
    std::unordered_set<std::string> privacyWindowTags;
    ret = WindowManager::GetInstance().RemoveSessionBlackList(bundleNames, privacyWindowTags);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: GetInstance
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetInstanceMulti, TestSize.Level1)
{
    sptr<WindowManager> instance = nullptr;
    int32_t userId = 101;
    instance = WindowManager::GetInstance(userId);
    ASSERT_NE(instance, nullptr);

    // branch overried
    instance = WindowManager::GetInstance(userId);
    ASSERT_NE(instance, nullptr);
}

/**
 * @tc.name: RemoveInstanceByUserId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RemoveInstanceByUserId, TestSize.Level1)
{
    ASSERT_EQ(WMError::WM_OK, WindowManager::RemoveInstanceByUserId(101));
}

/**
 * @tc.name: RegisterWMSConnectionChangedListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWMSConnectionChangedListener, TestSize.Level1)
{
    auto instance = WindowManager::GetInstance(-1);
    sptr<TestWMSListener> listener = new TestWMSListener();
    ASSERT_NE(nullptr, instance->pImpl_);
    instance->pImpl_->wmsConnectionChangedListener_ = nullptr;
    ASSERT_EQ(WMError::WM_OK, instance->RegisterWMSConnectionChangedListener(listener));
}
}
} // namespace
} // namespace Rosen
} // namespace OHOS
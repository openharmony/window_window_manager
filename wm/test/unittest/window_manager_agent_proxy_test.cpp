/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include <functional>
#include <gtest/gtest.h>

#include "display_manager_adapter.h"
#include "scene_board_judgement.h"
#include "iremote_object_mocker.h"
#include "window_manager_agent_proxy.h"
#include "test/mock/mock_message_parcel.h"

using namespace testing;
using namespace testing::ext;
namespace {
    std::string logMsg;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        logMsg = msg;
    }
}

namespace OHOS {
namespace Rosen {
class WindowManagerAgentProxyTest : public testing::Test {
public:
    static void SetUpTestSuite();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowManagerAgentProxy> windowManagerAgentProxy;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void WindowManagerAgentProxyTest::SetUpTestSuite()
{
    ASSERT_TRUE(SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy());
}

void WindowManagerAgentProxyTest::SetUp()
{
    if (windowManagerAgentProxy) {
        return;
    }

    sptr<IRemoteObject> impl;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(SingletonContainer::Get<ScreenManagerAdapter>().screenSessionManagerServiceProxy_, nullptr);
        impl = SingletonContainer::Get<ScreenManagerAdapter>().screenSessionManagerServiceProxy_->AsObject();
    } else {
        ASSERT_NE(SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_, nullptr);
        impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    }

    windowManagerAgentProxy = new (std::nothrow) WindowManagerAgentProxy(impl);
    ASSERT_NE(windowManagerAgentProxy, nullptr);
}

void WindowManagerAgentProxyTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: UpdateFocusChangeInfo01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateFocusChangeInfo01, TestSize.Level1)
{
    sptr<FocusChangeInfo> focusChangeInfo = new(std::nothrow) FocusChangeInfo();
    ASSERT_TRUE(focusChangeInfo != nullptr);
    bool focused = true;

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateFocusChangeInfo(focusChangeInfo, focused);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateFocusChangeInfo02
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateFocusChangeInfo02, TestSize.Level1)
{
    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateFocusChangeInfo(nullptr, false);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateWindowModeTypeInfo
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowModeTypeInfo, TestSize.Level1)
{
    int resultValue = 0;
    WindowModeType type = WindowModeType::WINDOW_MODE_SPLIT_FLOATING;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateWindowModeTypeInfo(type);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateSystemBarRegionTints
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateSystemBarRegionTints, TestSize.Level1)
{
    DisplayId displayId = 0;
    SystemBarRegionTints tints = {};

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateSystemBarRegionTints(displayId, tints);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyAccessibilityWindowInfo
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyAccessibilityWindowInfo, TestSize.Level1)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos = {};
    WindowUpdateType type = WindowUpdateType::WINDOW_UPDATE_REMOVED;

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->NotifyAccessibilityWindowInfo(infos, type);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateWindowVisibilityInfo
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowVisibilityInfo, TestSize.Level1)
{
    std::vector<sptr<WindowVisibilityInfo>> visibilityInfos = {};

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateWindowVisibilityInfo(visibilityInfos);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateWindowDrawingContentInfo
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowDrawingContentInfo, TestSize.Level1)
{
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos = {};

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateWindowDrawingContentInfo(windowDrawingContentInfos);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraFloatWindowStatus, TestSize.Level1)
{
    uint32_t accessTokenId = 0;
    bool isShowing = true;

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWaterMarkFlagChangedResult, TestSize.Level1)
{
    bool showWaterMark = true;

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->NotifyWaterMarkFlagChangedResult(showWaterMark);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateVisibleWindowNum
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateVisibleWindowNum, TestSize.Level1)
{    int resultValue = 0;
    VisibleWindowNumInfo info;
    info.displayId = 1;
    info.visibleWindowNum = 1;
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    visibleWindowNumInfo.push_back(info);
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateVisibleWindowNum(visibleWindowNumInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyGestureNavigationEnabledResult
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyGestureNavigationEnabledResult, TestSize.Level1)
{
    bool enable = true;

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->NotifyGestureNavigationEnabledResult(enable);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdateCameraWindowStatus
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraWindowStatus, TestSize.Level1)
{
    uint32_t accessTokenId = 1;
    bool isShowing = false;

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->UpdateCameraWindowStatus(accessTokenId, isShowing);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyWindowStyleChange
 * @tc.desc: test NotifyWindowStyleChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowStyleChange, TestSize.Level1)
{
    WindowStyleType type = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->NotifyWindowStyleChange(type);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyWindowPidVisibilityChanged
 * @tc.desc: test NotifyWindowPidVisibilityChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowPidVisibilityChanged, TestSize.Level1)
{
    sptr<WindowPidVisibilityInfo> info = new WindowPidVisibilityInfo();

    int resultValue = 0;
    std::function<void()> func = [&]() {
        windowManagerAgentProxy->NotifyWindowPidVisibilityChanged(info);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyWindowPropertyChange
 * @tc.desc: test NotifyWindowPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowPropertyChange, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t propertyDirtyFlags = 0;
    std::vector<std::unordered_map<WindowInfoKey, std::any>> windowInfoList;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    windowManagerAgentProxy->NotifyWindowPropertyChange(propertyDirtyFlags, windowInfoList);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

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
#include "../../../window_scene/test/mock/mock_message_parcel.h"

using namespace testing;
using namespace testing::ext;
namespace {
    std::string g_logMsg;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        g_logMsg = msg;
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
}

void WindowManagerAgentProxyTest::SetUp()
{
    if (windowManagerAgentProxy) {
        return;
    }

    sptr<IRemoteObject> impl;
    impl = sptr<IRemoteObjectMocker>::MakeSptr();

    windowManagerAgentProxy = sptr<WindowManagerAgentProxy>::MakeSptr(impl);
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
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<FocusChangeInfo> focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr();
    ASSERT_TRUE(focusChangeInfo != nullptr);
    bool focused = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateFocusChangeInfo(focusChangeInfo, focused);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: UpdateFocusChangeInfo02
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateFocusChangeInfo02, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<FocusChangeInfo> focusChangeInfo = new(std::nothrow) FocusChangeInfo();
    ASSERT_TRUE(focusChangeInfo != nullptr);
    bool focused = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    windowManagerAgentProxy->UpdateFocusChangeInfo(focusChangeInfo, focused);
    EXPECT_TRUE(g_logMsg.find("Write displayId failed") != std::string::npos);
}

/**
 * @tc.name: UpdateFocusChangeInfo03
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateFocusChangeInfo03, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<FocusChangeInfo> focusChangeInfo = new(std::nothrow) FocusChangeInfo();
    ASSERT_TRUE(focusChangeInfo != nullptr);
    bool focused = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    windowManagerAgentProxy->UpdateFocusChangeInfo(focusChangeInfo, focused);
    EXPECT_TRUE(g_logMsg.find("Write Focus failed") != std::string::npos);
}

/**
 * @tc.name: UpdateWindowModeTypeInfo01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowModeTypeInfo01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    WindowModeType type = WindowModeType::WINDOW_MODE_SPLIT_FLOATING;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateWindowModeTypeInfo(type);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: UpdateSystemBarRegionTints01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateSystemBarRegionTints01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    DisplayId displayId = 0;
    SystemBarRegionTints tints = {};
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateSystemBarRegionTints(displayId, tints);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyAccessibilityWindowInfo
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyAccessibilityWindowInfo, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::vector<sptr<AccessibilityWindowInfo>> infos = {};
    WindowUpdateType type = WindowUpdateType::WINDOW_UPDATE_REMOVED;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->NotifyAccessibilityWindowInfo(infos, type);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: UpdateWindowVisibilityInfo01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowVisibilityInfo01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::vector<sptr<WindowVisibilityInfo>> visibilityInfos = {};
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateWindowVisibilityInfo(visibilityInfos);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: UpdateWindowVisibilityInfo02
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowVisibilityInfo02, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::vector<sptr<WindowVisibilityInfo>> visibilityInfos = {};
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    windowManagerAgentProxy->UpdateWindowVisibilityInfo(visibilityInfos);
    EXPECT_TRUE(g_logMsg.find("write windowVisibilityInfos size failed") != std::string::npos);
}

/**
 * @tc.name: UpdateWindowDrawingContentInfo01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowDrawingContentInfo01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos = {};
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateWindowDrawingContentInfo(windowDrawingContentInfos);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: UpdateWindowDrawingContentInfo02
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowDrawingContentInfo02, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos = {};
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    windowManagerAgentProxy->UpdateWindowDrawingContentInfo(windowDrawingContentInfos);
    EXPECT_TRUE(g_logMsg.find("write windowDrawingContentInfos size failed") != std::string::npos);
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraFloatWindowStatus, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 0;
    bool isShowing = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraFloatWindowStatus01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 0;
    bool isShowing = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    windowManagerAgentProxy->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    EXPECT_TRUE(g_logMsg.find("Write accessTokenId failed") != std::string::npos);
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus02
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraFloatWindowStatus02, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 0;
    bool isShowing = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    windowManagerAgentProxy->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    EXPECT_TRUE(g_logMsg.find("Write is showing status failed") != std::string::npos);
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWaterMarkFlagChangedResult, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    bool showWaterMark = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->NotifyWaterMarkFlagChangedResult(showWaterMark);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWaterMarkFlagChangedResult01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    bool showWaterMark = true;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    windowManagerAgentProxy->NotifyWaterMarkFlagChangedResult(showWaterMark);
    EXPECT_TRUE(g_logMsg.find("Write is showing status failed") != std::string::npos);
}
/**
 * @tc.name: UpdateVisibleWindowNum
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateVisibleWindowNum, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    VisibleWindowNumInfo info;
    info.displayId = 1;
    info.visibleWindowNum = 1;
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    visibleWindowNumInfo.push_back(info);

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateVisibleWindowNum(visibleWindowNumInfo);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyGestureNavigationEnabledResult
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyGestureNavigationEnabledResult, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    bool enable = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->NotifyGestureNavigationEnabledResult(enable);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyGestureNavigationEnabledResult01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyGestureNavigationEnabledResult01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    bool enable = true;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    windowManagerAgentProxy->NotifyGestureNavigationEnabledResult(enable);
    EXPECT_TRUE(g_logMsg.find("Write is showing status failed") != std::string::npos);
}


/**
 * @tc.name: UpdateCameraWindowStatus
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraWindowStatus, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 1;
    bool isShowing = false;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->UpdateCameraWindowStatus(accessTokenId, isShowing);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: UpdateCameraWindowStatus01
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraWindowStatus01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 1;
    bool isShowing = false;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    windowManagerAgentProxy->UpdateCameraWindowStatus(accessTokenId, isShowing);
    EXPECT_TRUE(g_logMsg.find("Write accessTokenId failed") != std::string::npos);
}

/**
 * @tc.name: UpdateCameraWindowStatus02
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraWindowStatus02, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t accessTokenId = 1;
    bool isShowing = false;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    windowManagerAgentProxy->UpdateCameraWindowStatus(accessTokenId, isShowing);
    EXPECT_TRUE(g_logMsg.find("Write isShowing status failed") != std::string::npos);
}

/**
 * @tc.name: NotifyWindowStyleChange
 * @tc.desc: test NotifyWindowStyleChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowStyleChange, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    WindowStyleType type = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->NotifyWindowStyleChange(type);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyWindowPidVisibilityChanged
 * @tc.desc: test NotifyWindowPidVisibilityChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowPidVisibilityChanged, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<WindowPidVisibilityInfo> info = sptr<WindowPidVisibilityInfo>::MakeSptr();

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->NotifyWindowPidVisibilityChanged(info);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyWindowPidVisibilityChanged01
 * @tc.desc: test NotifyWindowPidVisibilityChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowPidVisibilityChanged01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<WindowPidVisibilityInfo> info = sptr<WindowPidVisibilityInfo>::MakeSptr();

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    windowManagerAgentProxy->NotifyWindowPidVisibilityChanged(info);
    EXPECT_TRUE(g_logMsg.find("Write windowPidVisibilityInfo failed") != std::string::npos);
}

/**
 * @tc.name: NotifyWindowPropertyChange
 * @tc.desc: test NotifyWindowPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowPropertyChange, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t propertyDirtyFlags = 0;
    std::vector<std::unordered_map<WindowInfoKey, std::any>> windowInfoList;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    windowManagerAgentProxy->NotifyWindowPropertyChange(propertyDirtyFlags, windowInfoList);
    EXPECT_TRUE(g_logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyWindowPropertyChange01
 * @tc.desc: test NotifyWindowPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyWindowPropertyChange01, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    uint32_t propertyDirtyFlags = 0;
    std::vector<std::unordered_map<WindowInfoKey, std::any>> windowInfoList;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    windowManagerAgentProxy->NotifyWindowPropertyChange(propertyDirtyFlags, windowInfoList);
    EXPECT_TRUE(g_logMsg.find("Write propertyDirtyFlags failed") != std::string::npos);
}

} // namespace
} // namespace Rosen
} // namespace OHOS

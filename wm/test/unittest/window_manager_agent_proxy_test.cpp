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
#include "window_manager_agent_proxy.h"
#include "window_manager_agent.h"
#include "display_manager_adapter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowManagerAgentProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowManagerAgentProxyTest::SetUpTestCase()
{
}

void WindowManagerAgentProxyTest::TearDownTestCase()
{
}

void WindowManagerAgentProxyTest::SetUp()
{
}

void WindowManagerAgentProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateFocusChangeInfo
 * @tc.desc: test InterfaceToken check failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateFocusChangeInfo, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    ;
    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo();
    bool focused = true;

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        windowManagerAgentProxy->UpdateFocusChangeInfo(focusChangeInfo, focused);
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
HWTEST_F(WindowManagerAgentProxyTest, UpdateSystemBarRegionTints, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    DisplayId displayId = 0;
    SystemBarRegionTints tints = {};

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
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
HWTEST_F(WindowManagerAgentProxyTest, NotifyAccessibilityWindowInfo, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    ;
    std::vector<sptr<AccessibilityWindowInfo>> infos = {};
    WindowUpdateType type = WindowUpdateType::WINDOW_UPDATE_REMOVED;

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
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
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowVisibilityInfo, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    std::vector<sptr<WindowVisibilityInfo>> visibilityInfos = {};

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
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
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowDrawingContentInfo, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos = {};

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
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
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraFloatWindowStatus, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    uint32_t accessTokenId = 0;
    bool isShowing = true;

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
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
HWTEST_F(WindowManagerAgentProxyTest, NotifyWaterMarkFlagChangedResult, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    bool showWaterMark = true;

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        windowManagerAgentProxy->NotifyWaterMarkFlagChangedResult(showWaterMark);
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
HWTEST_F(WindowManagerAgentProxyTest, NotifyGestureNavigationEnabledResult, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    bool enable = true;

    sptr<WindowManagerAgentProxy> windowManagerAgentProxy = new WindowManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        windowManagerAgentProxy->NotifyGestureNavigationEnabledResult(enable);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}
}
}
}
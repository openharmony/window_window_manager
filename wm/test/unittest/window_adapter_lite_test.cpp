/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#include <map>
#include "window_adapter_lite.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

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
class WindowAdapterLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowAdapterLiteTest::SetUpTestCase() {}

void WindowAdapterLiteTest::TearDownTestCase() {}

void WindowAdapterLiteTest::SetUp() {}

void WindowAdapterLiteTest::TearDown() {}

namespace {
/**
 * @tc.name: UnregisterWindowManagerAgent
 * @tc.desc: WindowAdapterLite/UnregisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, UnregisterWindowManagerAgent, TestSize.Level1)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;

    std::set<sptr<IWindowManagerAgent>> iWindowManagerAgent = { nullptr };
    windowAdapterLite_->windowManagerLiteAgentMap_.insert(
        std::make_pair(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT, iWindowManagerAgent));
    int32_t pid = 0;
    auto ret = windowAdapterLite_->CheckWindowId(0, pid);

    ASSERT_NE(WMError::WM_OK, ret);
}

/**
 * @tc.name: UnregisterWindowManagerAgent01
 * @tc.desc: WindowAdapterLite/UnregisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, UnregisterWindowManagerAgent01, TestSize.Level1)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->ReregisterWindowManagerLiteAgent();
    windowAdapterLite_->OnUserSwitch();
    windowAdapterLite_->ClearWindowAdapter();

    sptr<WMSDeathRecipient> wmSDeathRecipient = sptr<WMSDeathRecipient>::MakeSptr();
    ASSERT_NE(wmSDeathRecipient, nullptr);
    wptr<IRemoteObject> wptrDeath;
    wmSDeathRecipient->OnRemoteDied(wptrDeath);

    FocusChangeInfo focusInfo;
    windowAdapterLite_->GetFocusWindowInfo(focusInfo);
    WindowModeType windowModeType;
    windowAdapterLite_->GetWindowModeType(windowModeType);
    int32_t pid = 0;
    auto ret = windowAdapterLite_->CheckWindowId(0, pid);

    ASSERT_NE(WMError::WM_OK, ret);
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: WindowAdapterLite/OnRemoteDied
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, OnRemoteDied, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    sptr<WMSDeathRecipient> wmSDeathRecipient = sptr<WMSDeathRecipient>::MakeSptr();
    ASSERT_NE(wmSDeathRecipient, nullptr);
    wptr<IRemoteObject> wptrDeath = nullptr;
    wmSDeathRecipient->OnRemoteDied(wptrDeath);
    EXPECT_TRUE(g_logMsg.find("wptrDeath is null") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: GetAllMainWindowInfos
 * @tc.desc: WindowAdapterLite/GetAllMainWindowInfos
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, GetAllMainWindowInfos, TestSize.Level1)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    MainWindowInfo info;
    std::vector<MainWindowInfo> infos;
    infos.push_back(info);
    ASSERT_EQ(WMError::WM_OK, windowAdapterLite_->GetAllMainWindowInfos(infos));
}

/**
 * @tc.name: ClearMainSessions
 * @tc.desc: WindowAdapterLite/ClearMainSessions
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, ClearMainSessions, TestSize.Level1)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    const std::vector<int32_t> persistentIds;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowAdapterLite_->ClearMainSessions(persistentIds));
}

/**
 * @tc.name: ClearMainSessions01
 * @tc.desc: WindowAdapterLite/ClearMainSessions
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, ClearMainSessions01, TestSize.Level1)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    std::vector<int32_t> persistentIds;
    std::vector<int32_t> clearFailedIds;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION,
        windowAdapterLite_->ClearMainSessions(persistentIds, clearFailedIds));
}

/**
 * @tc.name: RaiseWindowToTop
 * @tc.desc: WindowAdapterLite/RaiseWindowToTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, RaiseWindowToTop, TestSize.Level1)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowAdapterLite_->RaiseWindowToTop(0));
}

/**
 * @tc.name: GetWindowStyleType
 * @tc.desc: WindowAdapterLite/GetWindowStyleType
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, GetWindowStyleType, TestSize.Level1)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    WindowStyleType windowStyleType = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowAdapterLite_->GetWindowStyleType(windowStyleType));
}

/**
 * @tc.name: TerminateSessionByPersistentId
 * @tc.desc: WindowAdapterLite/TerminateSessionByPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, TerminateSessionByPersistentId, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, windowAdapterLite_->TerminateSessionByPersistentId(0));
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: CloseTargetFloatWindow
 * @tc.desc: WindowAdapterLite/CloseTargetFloatWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, CloseTargetFloatWindow, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    const std::string& bundleName = "test";
    ASSERT_EQ(WMError::WM_OK, windowAdapterLite_->CloseTargetFloatWindow(bundleName));
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: CloseTargetPiPWindow
 * @tc.desc: WindowAdapterLite/CloseTargetPiPWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, CloseTargetPiPWindow, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    const std::string& bundleName = "test";
    ASSERT_EQ(WMError::WM_OK, windowAdapterLite_->CloseTargetPiPWindow(bundleName));
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: GetCurrentPiPWindowInfo
 * @tc.desc: WindowAdapterLite/GetCurrentPiPWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, GetCurrentPiPWindowInfo, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    ASSERT_NE(windowAdapterLite_, nullptr);
    windowAdapterLite_->OnUserSwitch();

    std::string bundleName = "test";
    ASSERT_EQ(WMError::WM_OK, windowAdapterLite_->GetCurrentPiPWindowInfo(bundleName));
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: ListWindowInfo01
 * @tc.desc: WindowAdapter/ListWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, ListWindowInfo01, Function | SmallTest | Level2)
{
    WindowAdapterLite WindowAdapterLite;
    WindowInfoOption windowInfoOption;
    std::vector<sptr<WindowInfo>> infos;
    auto err = WindowAdapterLite.ListWindowInfo(windowInfoOption, infos);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, err);
}

/**
 * @tc.name: SendPointerEventForHover
 * @tc.desc: SendPointerEventForHover
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, SendPointerEventForHover, Function | SmallTest | Level2)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    auto err = windowAdapterLite_->SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, err);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
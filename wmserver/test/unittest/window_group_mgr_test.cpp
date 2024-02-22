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

#include <cstdint>
#include <gtest/gtest.h>
#include "display_manager.h"
#include "window_agent.h"
#include "window_group_mgr.h"
#include "window_impl.h"
#include "window_property.h"
#include "window_root.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowGroupMgrTest"};
}

class WindowGroupMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<WindowGroupMgr> windowGroupMgr_;
    static sptr<WindowRoot> windowRoot_;
    static DisplayId defalutDisplayId_;
    static int missionCount_;
};


sptr<WindowGroupMgr> WindowGroupMgrTest::windowGroupMgr_ = nullptr;
sptr<WindowRoot> WindowGroupMgrTest::windowRoot_;
DisplayId WindowGroupMgrTest::defalutDisplayId_ = -1;
int WindowGroupMgrTest::missionCount_ = 3;

void WindowGroupMgrTest::SetUpTestCase()
{
    WLOGI("SetUpTestCase");
    windowRoot_ = new WindowRoot(nullptr);
    windowGroupMgr_ = new WindowGroupMgr(windowRoot_);

    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    ASSERT_TRUE((displayInfo != nullptr));
    auto container = windowRoot_->CreateWindowNodeContainer(0, displayInfo);
    defalutDisplayId_ = display->GetId();

    for (int i = 0; i < missionCount_; i++) {
        int32_t missindId =  i + 1;
        sptr<WindowProperty> property = new WindowProperty();
        AbilityInfo abilityInfo({"bundle_test" + std::to_string(missindId), "ability_test" + std::to_string(missindId),
            missindId});
        property->SetAbilityInfo(abilityInfo);
        property->SetWindowId(static_cast<uint32_t>(missindId));
        property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        property->SetDisplayId(defalutDisplayId_);
        sptr<WindowNode> node = new WindowNode(property);
        node->SetWindowProperty(property);
        node->abilityInfo_ = abilityInfo;
        sptr<WindowOption> windowOption = new WindowOption();
        sptr<WindowImpl> windowImpl = new WindowImpl(windowOption);
        windowImpl->SetWindowState(WindowState::STATE_SHOWN);
        sptr<IWindow> window = new WindowAgent(windowImpl);
        node->SetWindowToken(window);
        WLOGFI("windowId: %{public}u, missionId: %{public}d", node->GetWindowId(), node->abilityInfo_.missionId_);
        windowRoot_->SaveWindow(node);
    }
}

void WindowGroupMgrTest::TearDownTestCase()
{
    windowGroupMgr_ = nullptr;
    windowRoot_ = nullptr;
    defalutDisplayId_ = -1;
}

void WindowGroupMgrTest::SetUp()
{
}

void WindowGroupMgrTest::TearDown()
{
}

namespace {
/**
 * @tc.name: MoveMissionsToForeground01
 * @tc.desc: move missions to foreground
 * @tc.type: FUNC
 */
HWTEST_F(WindowGroupMgrTest, MoveMissionsToForeground01, Function | SmallTest | Level2)
{
    WLOGI("MoveMissionsToForeground01");
    auto rs = windowGroupMgr_->MoveMissionsToForeground({1, 2, 3}, 2);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, rs);
    rs = windowGroupMgr_->MoveMissionsToForeground({1, 2, 3}, -1);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, rs);
}

/**
 * @tc.name: MoveMissionsToBackground01
 * @tc.desc: move missions to background
 * @tc.type: FUNC
 */
HWTEST_F(WindowGroupMgrTest, MoveMissionsToBackground01, Function | SmallTest | Level2)
{
    std::vector<int32_t> moveRs;
    auto rs = windowGroupMgr_->MoveMissionsToBackground({1, 2, 3}, moveRs);
    ASSERT_EQ(WMError::WM_OK, rs);
    ASSERT_EQ(0, moveRs.size());
}

/**
 * @tc.name: OnWindowDestroyed01
 * @tc.desc: OnWindowDestroyed test
 * @tc.type: FUNC
 */
HWTEST_F(WindowGroupMgrTest, OnWindowDestroyed01, Function | SmallTest | Level2)
{
    windowGroupMgr_->OnWindowDestroyed(1);
    ASSERT_EQ(0, windowGroupMgr_->backupWindowModes_.count(1));
}
}
}
}
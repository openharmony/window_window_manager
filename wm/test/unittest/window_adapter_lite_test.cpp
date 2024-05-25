/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowAdapterLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
   
};

void WindowAdapterLiteTest::SetUpTestCase()
{
}

void WindowAdapterLiteTest::TearDownTestCase()
{
}

void WindowAdapterLiteTest::SetUp()
{
    
}

void WindowAdapterLiteTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UnregisterWindowManagerAgent
 * @tc.desc: WindowAdapterLite/UnregisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterLiteTest, UnregisterWindowManagerAgent, Function | SmallTest | Level2)
{
    std::shared_ptr<WindowAdapterLite> windowAdapterLite_ = std::make_shared<WindowAdapterLite>();
    if (!windowAdapterLite_){
        return;
    }
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT;
    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;
    std::set<sptr<IWindowManagerAgent>> aa = {nullptr};
    windowAdapterLite_->windowManagerLiteAgentMap_.insert(std::make_pair(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT, aa));
    auto ret = windowAdapterLite_->UnregisterWindowManagerAgent(type, windowManagerAgent);

    windowAdapterLite_->ReregisterWindowManagerLiteAgent();
    windowAdapterLite_->OnUserSwitch();
    windowAdapterLite_->ClearWindowAdapter();

    sptr<WMSDeathRecipient> wmSDeathRecipient = new WMSDeathRecipient();
    wptr<IRemoteObject> wptrDeath;
    wmSDeathRecipient->OnRemoteDied(wptrDeath);

    FocusChangeInfo focusInfo;
    windowAdapterLite_->GetFocusWindowInfo(focusInfo);

    WindowModeType windowModeType;
    windowAdapterLite_->GetWindowModeType(windowModeType);
    ASSERT_EQ(WMError::WM_OK, ret);
}
}
}
}
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
#include "display_group_controller.h"
#include "display_group_info.h"
#include "display_manager.h"
#include "window_layout_policy.h"
#include "window_layout_policy_cascade.h"
#include "window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class WindowLayoutPolicyCascadeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<WindowNodeContainer> container_;
    static sptr<DisplayGroupInfo> displayGroupInfo_;
    static sptr<DisplayInfo> defaultDisplayInfo_;
    static sptr<DisplayGroupController> displayGroupController_;
    static sptr<WindowLayoutPolicyCascade> layoutPolicyCascade_;
};

sptr<WindowNodeContainer> WindowLayoutPolicyCascadeTest::container_ = nullptr;
sptr<DisplayGroupInfo> WindowLayoutPolicyCascadeTest::displayGroupInfo_ = nullptr;
sptr<DisplayGroupController> WindowLayoutPolicyCascadeTest::displayGroupController_ = nullptr;
sptr<DisplayInfo> WindowLayoutPolicyCascadeTest::defaultDisplayInfo_ = nullptr;
sptr<WindowLayoutPolicyCascade> WindowLayoutPolicyCascadeTest::layoutPolicyCascade_ = nullptr;

void WindowLayoutPolicyCascadeTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));

    defaultDisplayInfo_ = display->GetDisplayInfo();
    ASSERT_TRUE((defaultDisplayInfo_ != nullptr));

    container_ = new WindowNodeContainer(defaultDisplayInfo_, display->GetScreenId());
    displayGroupInfo_ = container_->displayGroupInfo_;
    displayGroupController_ = container_->displayGroupController_;
    layoutPolicyCascade_ = new WindowLayoutPolicyCascade(displayGroupInfo_,
        displayGroupController_->displayGroupWindowTree_);
}

void WindowLayoutPolicyCascadeTest::TearDownTestCase()
{
    container_ = nullptr;
    displayGroupInfo_ = nullptr;
    defaultDisplayInfo_ = nullptr;
    displayGroupController_ = nullptr;
    layoutPolicyCascade_ = nullptr;
}

void WindowLayoutPolicyCascadeTest::SetUp()
{
    displayGroupInfo_->AddDisplayInfo(defaultDisplayInfo_);
}

void WindowLayoutPolicyCascadeTest::TearDown()
{
    displayGroupInfo_->displayInfosMap_.clear();
}

namespace {
/**
 * @tc.name: InitCascadeRectCfg01
 * @tc.desc: test InitCascadeRectCfg
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyCascadeTest, InitCascadeRectCfg01, Function | SmallTest | Level2)
{
    std::vector<int> numbers {82, 121, 1068, 706};
    WindowLayoutPolicyCascade::SetDefaultFloatingWindow(numbers);
    auto displayId = defaultDisplayInfo_->GetDisplayId();
    ASSERT_EQ(layoutPolicyCascade_->InitCascadeRectCfg(displayId), true);

    Rect resRect = WindowLayoutPolicyCascade::defaultFloatingWindow_;
    auto vpr = displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
    resRect.width_ = static_cast<uint32_t>(vpr * resRect.width_);
    resRect.height_ = static_cast<uint32_t>(vpr * resRect.height_);
    resRect.posX_ = static_cast<int32_t>(vpr * resRect.posX_);
    resRect.posY_ = static_cast<int32_t>(vpr * resRect.posY_);

    ASSERT_EQ(layoutPolicyCascade_->cascadeRectsMap_[displayId].defaultCascadeRect_, resRect);
}
}
}
}


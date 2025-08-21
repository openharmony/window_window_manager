/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <ui/rs_surface_node.h>

#include "session/host/include/session.h"
#include "session/host/include/main_session.h"
#include "session/host/include/sub_session.h"
#include "session/screen/include/screen_session.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_property.h"
#include "window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SubSessionLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
};

void SubSessionLayoutTest::SetUpTestCase() {}

void SubSessionLayoutTest::TearDownTestCase() {}

void SubSessionLayoutTest::SetUp() {}

void SubSessionLayoutTest::TearDown() {}

RSSurfaceNode::SharedPtr SubSessionLayoutTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: HandleCrossSurfaceNodeByWindowAnchor
 * @tc.desc: Check cloneNodeDuringCross when handleCrossSurfaceNodeByWindowAnchor
 * @tc.type: FUNC
 */
HWTEST_F(SubSessionLayoutTest, HandleCrossSurfaceNodeByWindowAnchor, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "HandleCrossSurfaceNodeByWindowAnchor";
    info.bundleName_ = "HandleCrossSurfaceNodeByWindowAnchor";
    sptr<SubSession> sceneSession = sptr<SubSession>::MakeSptr(info, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->HandleCrossSurfaceNodeByWindowAnchor(SizeChangeReason::UNDEFINED,
        std::numeric_limits<uint32_t>::max());
    sceneSession->HandleCrossSurfaceNodeByWindowAnchor(SizeChangeReason::UNDEFINED, 0);
    sceneSession->SetFindScenePanelRsNodeByZOrderFunc([this](uint64_t screenId, uint32_t targetZOrder) {
        return CreateRSSurfaceNode();
    });
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = info.abilityName_;
    RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::DEFAULT;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType);
    ASSERT_NE(surfaceNode, nullptr);
    sceneSession->SetSurfaceNode(surfaceNode);

    sceneSession->cloneNodeCountDuringCross_.store(0);
    sceneSession->HandleCrossSurfaceNodeByWindowAnchor(SizeChangeReason::DRAG, 0);
    EXPECT_EQ(1, sceneSession->cloneNodeCountDuringCross_.load());
    sceneSession->HandleCrossSurfaceNodeByWindowAnchor(SizeChangeReason::DRAG_MOVE, 0);
    EXPECT_EQ(2, sceneSession->cloneNodeCountDuringCross_.load());
    sceneSession->HandleCrossSurfaceNodeByWindowAnchor(SizeChangeReason::DRAG_END, 0);
    EXPECT_EQ(1, sceneSession->cloneNodeCountDuringCross_.load());
    sceneSession->HandleCrossSurfaceNodeByWindowAnchor(SizeChangeReason::UNDEFINED, 0);
    EXPECT_EQ(1, sceneSession->cloneNodeCountDuringCross_.load());
}
} // namespace
} // namespace Rosen
} // namespace OHOS
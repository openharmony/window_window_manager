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
#include "dm_rs_surface_node.h"

using namespace testing::ext;

namespace OHOS::Rosen {
/**
 * @tc.name: Marshalling
 * @tc.desc: Marshalling test with an empty surfaceNode
 * @tc.type: FUNC
 */
HWTEST(DmRsSurfaceNodeTest, Marshalling, TestSize.Level1)
{
    DmRsSurfaceNode DmRsSurfaceNode;
    Parcel parcel;
    bool ret = DmRsSurfaceNode.Marshalling(parcel);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: MarshallingUnmarshalling_CustomValues
 * @tc.desc: Marshalling and unmarshalling test with custom values
 * @tc.type: FUNC
 */
HWTEST(DmRsSurfaceNodeTest, MarshallingUnmarshalling_CustomValues, TestSize.Level1)
{
    RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 10);
    DmRsSurfaceNode dmRsSurfaceNode(surfaceNode);

    Parcel parcel;
    bool ret = dmRsSurfaceNode.Marshalling(parcel);
    ASSERT_TRUE(ret);

    sptr<DmRsSurfaceNode> node = DmRsSurfaceNode::Unmarshalling(parcel);
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->GetSurfaceNode()->isRenderServiceNode_);
    EXPECT_EQ(node->GetSurfaceNode()->id_, 10);
    EXPECT_EQ(node->GetSurfaceNode()->isTextureExportNode_, rsSurfaceNodeConfig.isTextureExportNode);
    EXPECT_EQ(node->GetSurfaceNode()->name_, rsSurfaceNodeConfig.SurfaceNodeName);
    EXPECT_NE(node->surfaceNode_, nullptr);
}
} // namespace OHOS::Rosen

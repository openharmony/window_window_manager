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
 * @tc.name: Marshalling01
 * @tc.desc: Marshalling test
 * @tc.type: FUNC
 */
HWTEST(DmRsSurfaceNodeTest, Marshalling01, TestSize.Level1)
{
    DmRsSurfaceNode DmRsSurfaceNode;
    Parcel parcel;
    bool ret = DmRsSurfaceNode.Marshalling(parcel);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: MarshallingUnmarshalling02
 * @tc.desc: MarshallingUnmarshalling test
 * @tc.type: FUNC
 */
HWTEST(DmRsSurfaceNodeTest, MarshallingUnmarshalling02, TestSize.Level1)
{
    RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 10);
    DmRsSurfaceNode dmRsSurfaceNode(surfaceNode);

    Parcel parcel;
    bool ret = dmRsSurfaceNode.Marshalling(parcel);
    ASSERT_TRUE(ret);

    DmRsSurfaceNode* node = DmRsSurfaceNode::Unmarshalling(parcel);
    ASSERT_TRUE(node->GetSurfaceNode()->isRenderServiceNode_);
    ASSERT_EQ(node->GetSurfaceNode()->id_, 10);
    ASSERT_EQ(node->GetSurfaceNode()->isTextureExportNode_, rsSurfaceNodeConfig.isTextureExportNode);
    ASSERT_EQ(node->GetSurfaceNode()->name_, rsSurfaceNodeConfig.SurfaceNodeName);
    ASSERT_NE(node, nullptr);
    delete node;
}
} // namespace OHOS::Rosen
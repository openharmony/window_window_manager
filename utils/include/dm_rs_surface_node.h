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

#ifndef OHOS_ROSEN_DM_RS_SURFACE_NODE_H
#define OHOS_ROSEN_DM_RS_SURFACE_NODE_H

#include <parcel.h>
#include <ui/rs_surface_node.h>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
class DmRsSurfaceNode : public Parcelable {
public:
    DmRsSurfaceNode() = default;

    explicit DmRsSurfaceNode(const std::shared_ptr<RSSurfaceNode>& surfaceNode) : surfaceNode_(surfaceNode) {}

    bool Marshalling(Parcel& parcel) const override
    {
        if (surfaceNode_) {
            return surfaceNode_->Marshalling(parcel);
        }
        TLOGE(WmsLogTag::DMS, "surfaceNode_ is null");
        return false;
    }

    static DmRsSurfaceNode* Unmarshalling(Parcel& parcel)
    {
        std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(parcel);
        return new (std::nothrow) DmRsSurfaceNode(surfaceNode);
    }

    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const
    {
        return surfaceNode_;
    }

private:
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
};
} // namespace OHOS::Rosen

#endif /* OHOS_ROSEN_DM_RS_SURFACE_NODE_H */

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_SNAPSHOT_CONTROLLER_H
#define OHOS_ROSEN_SNAPSHOT_CONTROLLER_H

#include <snapshot.h>
#include <transaction/rs_interfaces.h>
#include "window_root.h"
#include "snapshot_stub.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class SnapshotController : public SnapshotStub {
public:
    SnapshotController(sptr<WindowRoot>& root) : windowRoot_(root), rsInterface_(RSInterfaces::GetInstance()) {};
    SnapshotController() : windowRoot_(nullptr), rsInterface_(RSInterfaces::GetInstance()) {};
    ~SnapshotController() = default;
    void Init(sptr<WindowRoot>& root);

    int32_t GetSnapshot(const sptr<IRemoteObject> &token, AAFwk::Snapshot& snapshot) override;

private:
    sptr<WindowRoot> windowRoot_;
    RSInterfaces& rsInterface_;

    WMError TakeSnapshot(const std::shared_ptr<RSSurfaceNode>& surfaceNode, AAFwk::Snapshot& snapshot);

    class GetSurfaceCapture : public SurfaceCaptureCallback {
    public:
        GetSurfaceCapture() = default;
        ~GetSurfaceCapture() {};
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
        {
            if (flag_ == false) {
                flag_ = true;
                pixelMap_ = pixelmap;
            }
        }
        bool IsPixelMapOk()
        {
            return flag_;
        }
        std::shared_ptr<Media::PixelMap> GetPixelMap()
        {
            return pixelMap_;
        }
    private:
        bool flag_ = false;
        std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    };
};
}
}
#endif // OHOS_ROSEN_SNAPSHOT_CONTROLLER_H

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
 
#ifndef WM_GET_SNAPSHOT_CALLBACK_H
#define WM_GET_SNAPSHOT_CALLBACK_H
 
#include <iremote_broker.h>
#include "future.h"
#include "pixel_map.h"
#include "wm_common.h"
#include "zidl/get_snapshot_callback_stub.h"
 
namespace OHOS {
namespace Rosen {
using NotifyGetMainWindowSnapshotFunc =
    std::function<void(WMError errCode, std::vector<std::shared_ptr<OHOS::Media::PixelMap>> pixelMaps)>;
 
class GetSnapshotCallback : public GetSnapshotCallbackStub {
public:
    void RegisterFunc(const NotifyGetMainWindowSnapshotFunc& func);
 
    void OnReceived(WMError errCode, const std::vector<std::shared_ptr<OHOS::Media::PixelMap>>& pixelMaps) override;

    void OnNotifyResult();

    void GetSyncResult(long timeOut);

private:
    NotifyGetMainWindowSnapshotFunc getMainWindowSnapshotFunc_;

    RunnableFuture<int> future_{};
};
} // Rosen
} // OHOS
#endif // WM_GET_SNAPSHOT_CALLBACK_H
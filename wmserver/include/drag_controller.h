/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DRAG_CONTROLLER_H
#define OHOS_ROSEN_DRAG_CONTROLLER_H

#include <refbase.h>

#include "window_root.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class DragController : public RefBase {
public:
    DragController(sptr<WindowRoot>& root) : windowRoot_(root) {}
    ~DragController() = default;
    void StartDrag(uint32_t windowId);
    void UpdateDragInfo(uint32_t windowId);
    void FinishDrag(uint32_t windowId);
private:
    sptr<WindowNode> GetHitWindow(DisplayId id, const PointInfo point);
    bool GetHitPoint(uint32_t windowId, PointInfo& point);
    sptr<WindowRoot> windowRoot_;
    uint64_t hitWindowId_ = 0;
};
}
}
#endif // OHOS_ROSEN_DRAG_CONTROLLER_H


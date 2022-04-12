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

#ifndef OHOS_ROSEN_AVOID_AREA_CONTROLLER_H
#define OHOS_ROSEN_AVOID_AREA_CONTROLLER_H

#include <map>
#include <set>
#include <vector>
#include <refbase.h>

#include "window_node.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
enum class AvoidControlType : uint32_t {
    AVOID_NODE_ADD,
    AVOID_NODE_UPDATE,
    AVOID_NODE_REMOVE,
    AVOID_NODE_UNKNOWN,
};

using UpdateAvoidAreaFunc = std::function<void (std::vector<Rect>& avoidArea, DisplayId displayId)>;

class AvoidAreaController : public RefBase {
public:
    AvoidAreaController(DisplayId displayId, UpdateAvoidAreaFunc callback);
    ~AvoidAreaController() = default;

    WMError AvoidControl(const sptr<WindowNode>& node, AvoidControlType type);

    bool IsAvoidAreaNode(const sptr<WindowNode>& node) const;
    std::vector<Rect> GetAvoidArea(DisplayId displayId) const;
    std::vector<Rect> GetAvoidAreaByType(AvoidAreaType avoidAreaType, DisplayId displayId) const;
    void UpdateAvoidNodesMap(DisplayId displayId, bool isAdd);

private:
    std::map<uint32_t, sptr<WindowNode>>* GetAvoidNodesByDisplayId(DisplayId displayId);
    void UseCallbackNotifyAvoidAreaChanged(std::vector<Rect>& avoidArea, DisplayId displayId) const;
    void DumpAvoidArea(const std::vector<Rect>& avoidArea) const;
    AvoidPosType GetAvoidPosType(const Rect& rect, DisplayId displayId) const;

    std::map<DisplayId, std::unique_ptr<std::map<uint32_t, sptr<WindowNode>>>> avoidNodesMaps_;
    UpdateAvoidAreaFunc updateAvoidAreaCallBack_;
};
}
}
#endif // OHOS_ROSEN_AVOID_AREA_CONTROLLER_H

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

#ifndef OHOS_ROSEN_AVOID_AREA_CONTROLLER_H
#define OHOS_ROSEN_AVOID_AREA_CONTROLLER_H

#include <map>
#include <set>
#include <vector>
#include <refbase.h>

#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
enum class AvoidPos : uint32_t {
    AVOID_POS_LEFT,
    AVOID_POS_TOP,
    AVOID_POS_RIGHT,
    AVOID_POS_BOTTOM,
    AVOID_POS_UNKNOWN,
};

using UpdateAvoidAreaFunc = std::function<void (std::vector<Rect>& avoidArea)>;

class AvoidAreaController : public RefBase {
public:
    AvoidAreaController(UpdateAvoidAreaFunc callback): updateAvoidAreaCallBack_(callback) {};
    ~AvoidAreaController() = default;

    WMError AddAvoidAreaNode(const sptr<WindowNode>& node);
    WMError RemoveAvoidAreaNode(const sptr<WindowNode>& node);
    WMError UpdateAvoidAreaNode(const sptr<WindowNode>& node);

    bool IsAvoidAreaNode(const sptr<WindowNode>& node) const;
    std::vector<Rect> GetAvoidArea() const;
    std::vector<Rect> GetAvoidAreaByType(AvoidAreaType avoidAreaType) const;

private:
    std::map<uint32_t, sptr<WindowNode>> avoidNodes_;    // key: windowId
    const std::set<WindowType> avoidType_ {
        WindowType::WINDOW_TYPE_STATUS_BAR,
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
    };
    UpdateAvoidAreaFunc updateAvoidAreaCallBack_;
    void UseCallbackNotifyAvoidAreaChanged(std::vector<Rect>& avoidArea) const;
    void DumpAvoidArea(const std::vector<Rect>& avoidArea) const;
};
}
}
#endif // OHOS_ROSEN_AVOID_AREA_CONTROLLER_H

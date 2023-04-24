/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_GROUP_MGR_H
#define OHOS_ROSEN_WINDOW_GROUP_MGR_H

#include <refbase.h>
#include <unordered_map>

#include "window_root.h"
#include "window_transition_info.h"
#include "wm_common.h"
#include "wm_occlusion_region.h"

namespace OHOS {
namespace Rosen {
class WindowGroupMgr : public RefBase {
public:
    explicit WindowGroupMgr(sptr<WindowRoot>& root) : windowRoot_(root) {
    }

    static constexpr int32_t DEFAULT_GROUP_ID = 0;
    static constexpr int32_t START_GROUP_ID = 1;
    static constexpr int32_t MAX_GROUP_NUM = 3;

    static constexpr int32_t DEFAULT_MISSION_ID = -1;

    WMError MoveMissionsToForeground(const std::vector<int32_t>& missionIds, int32_t topMissionId);
    WMError MoveMissionsToBackground(const std::vector<int32_t>& missionIds, std::vector<int32_t>& result);

    void OnWindowDestroyed(uint32_t windowId);
    void OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);

private:
    WMError MoveMissionToForeground(int32_t missionId);
    sptr<WindowRoot> windowRoot_;
    std::unordered_map<uint32_t, WindowMode> backupWindowModes_;
    std::map<DisplayId, Rect> backupDividerWindowRect_;

    template<typename T> std::string DumpVector(std::vector<T> vec)
    {
        std::stringstream oss;
        oss << "[ ";
        for (T& v : vec) {
            oss << v << ", ";
        }
        oss << " ]";
        return oss.str();
    }
};

} // Rosen
} // OHOS
#endif // OHOS_ROSEN_WINDOW_GROUP_MGR_H

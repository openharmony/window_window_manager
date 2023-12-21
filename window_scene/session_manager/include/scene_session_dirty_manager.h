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

#ifndef OHOS_ROSEN_SCENE_SESSION_DIRTY_MANAGER_H
#define OHOS_ROSEN_SCENE_SESSION_DIRTY_MANAGER_H


#include <map>

#include "common/rs_vector4.h"
#include "session/host/include/scene_session.h"
#include "session/screen/include/screen_session.h"
#include "session_manager/include/screen_session_manager.h"

namespace OHOS::MMI {
    struct WindowInfo;
}
namespace OHOS::Rosen {
class SceneSessionDirtyManager {
private:
    enum WindowAction : uint32_t {
        UNKNOWN = 0,
        WINDOW_ADD,
        WINDOW_DELETE,
        WINDOW_CHANGE,
    };

using ScreenInfoChangeListener = std::function<void(int32_t)>;
public:
    SceneSessionDirtyManager() = default;
    virtual ~SceneSessionDirtyManager() = default;

    void RegisterScreenInfoChangeListener();
    void Init();
    bool IsScreenChange();
    void SetScreenChange(const uint64_t id);
    void SetScreenChange(const bool value);
    void NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession,
        const WindowUpdateType& type, int32_t sceneBoardPid = -1);
    std::vector<MMI::WindowInfo> GetFullWindowInfoList();
    std::map<uint64_t, std::vector<MMI::WindowInfo>> GetIncrementWindowInfoList();

private:
    void Clear();
    std::vector<MMI::WindowInfo> FullSceneSessionInfoUpdate() const;
    bool IsWindowBackGround(const sptr<SceneSession>& sceneSession) const;
    MMI::WindowInfo GetWindowInfo(const sptr<SceneSession>& sceneSession, const WindowAction& action) const;
    void PushWindowInfoList(uint64_t displayID, const MMI::WindowInfo& windowinfo);
    WindowAction GetSceneSessionAction(const WindowUpdateType& type);
    void PrintLogGetFullWindowInfoList(const std::vector<MMI::WindowInfo>& windowInfoList);
    void PrintLogGetIncrementWindowInfoList(const std::map<uint64_t, std::vector<MMI::WindowInfo>>& screen2windowInfo);
    void CalTramform(const sptr<SceneSession> sceneSession, Matrix3f& tranform) const;
    MMI::WindowInfo PrepareWindowInfo(sptr<SceneSession> sceneSession, int action) const;

    std::map<WindowUpdateType, WindowAction> windowType2Action_;
    std::map<uint64_t, std::vector<MMI::WindowInfo>> screen2windowInfo_;
    bool isScreenSessionChange_ = true;
    std::vector<MMI::WindowInfo> windowInfoList_;
    std::mutex mutexlock_;
};
} //namespace OHOS::Rosen

#endif 
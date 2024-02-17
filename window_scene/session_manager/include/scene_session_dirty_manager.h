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
#include "display_manager.h"
#include "session/host/include/scene_session.h"
#include "session/screen/include/screen_session.h"
#include "session_manager/include/screen_session_manager.h"

namespace OHOS::MMI {
    struct WindowInfo;
    struct Rect;
}
namespace OHOS::Rosen {
class SceneSessionDirtyManager {
private:
    enum WindowAction : uint32_t {
        UNKNOWN = 0,
        WINDOW_ADD,
        WINDOW_DELETE,
        WINDOW_CHANGE,
        WINDOW_ADD_END
    };

using ScreenInfoChangeListener = std::function<void(int32_t)>;
using FlushWindowInfoCallback = std::function<void()>;
public:
    SceneSessionDirtyManager() = default;
    virtual ~SceneSessionDirtyManager() = default;

    void NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession,
        const WindowUpdateType& type, const bool startMoving = false);
    std::vector<MMI::WindowInfo> GetFullWindowInfoList();
    void RegisterFlushWindowInfoCallback(const FlushWindowInfoCallback &&callback);
    void ResetSessionDirty();

private:
    std::vector<MMI::WindowInfo> FullSceneSessionInfoUpdate() const;
    bool IsFilterSession(const sptr<SceneSession>& sceneSession) const;
    MMI::WindowInfo GetWindowInfo(const sptr<SceneSession>& sceneSession, const WindowAction& action) const;
    void CalNotRotateTramform(const sptr<SceneSession> sceneSession, Matrix3f& tranform) const;
    void CalTramform(const sptr<SceneSession> sceneSession, Matrix3f& tranform) const;
    std::map<int32_t, sptr<SceneSession>> GetDialogSessionMap(
        const std::map<int32_t, sptr<SceneSession>>& sessionMap) const;
    void UpdateHotAreas(sptr<SceneSession> sceneSession, std::vector<MMI::Rect>& touchHotAreas,
        std::vector<MMI::Rect>& pointerHotAreas) const;
    void UpdateDefaultHotAreas(sptr<SceneSession> sceneSession, std::vector<MMI::Rect>& touchHotAreas,
        std::vector<MMI::Rect>& pointerHotAreas) const;
    void UpdatePointerAreas(sptr<SceneSession> sceneSession, std::vector<int32_t>& pointerChangeAreas) const;
    std::mutex mutexlock_;
    FlushWindowInfoCallback flushWindowInfoCallback_;
    std::atomic_bool sessionDirty_ { false };
    std::atomic_bool hasPostTask_ { false };
};
} //namespace OHOS::Rosen

#endif
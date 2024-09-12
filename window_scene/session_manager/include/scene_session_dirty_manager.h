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
#include "screen_session_manager/include/screen_session_manager.h"
#include "input_manager.h"

namespace OHOS::Rosen {
struct SecSurfaceInfo;
struct SecRectInfo;
MMI::Direction ConvertDegreeToMMIRotation(float degree, MMI::DisplayMode displayMode);
std::string DumpWindowInfo(const MMI::WindowInfo& info);
std::string DumpRect(const std::vector<MMI::Rect>& rects);
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
    std::pair<std::vector<MMI::WindowInfo>, std::vector<std::shared_ptr<Media::PixelMap>>>
        GetFullWindowInfoList(const std::vector<MMI::WindowInfo>& lastWindowInfoList);
    void RegisterFlushWindowInfoCallback(FlushWindowInfoCallback&& callback);
    void ResetSessionDirty();
    void UpdateSecSurfaceInfo(const std::map<uint64_t, std::vector<SecSurfaceInfo>>& secSurfaceInfoMap);

private:
    std::vector<MMI::WindowInfo> FullSceneSessionInfoUpdate() const;
    bool IsFilterSession(const sptr<SceneSession>& sceneSession) const;
    std::pair<MMI::WindowInfo, std::shared_ptr<Media::PixelMap>>
        GetWindowInfo(const sptr<SceneSession>& sceneSession, const std::vector<MMI::WindowInfo>& lastWindowInfoList,
        const WindowAction& action) const;
    void CalNotRotateTransform(const sptr<SceneSession>& sceneSession, Matrix3f& transform,
        bool useUIExtension = false) const;
    void CalTransform(const sptr<SceneSession>& sceneSession, Matrix3f& transform, bool useUIExtension = false) const;
    void UpdatePrivacyMode(const sptr<SceneSession>& sceneSession,
        MMI::WindowInfo& windowInfo) const;
    std::map<int32_t, sptr<SceneSession>> GetDialogSessionMap(
        const std::map<int32_t, sptr<SceneSession>>& sessionMap) const;
    void UpdateHotAreas(sptr<SceneSession> sceneSession, std::vector<MMI::Rect>& touchHotAreas,
        std::vector<MMI::Rect>& pointerHotAreas) const;
    void UpdateDefaultHotAreas(sptr<SceneSession> sceneSession, std::vector<MMI::Rect>& touchHotAreas,
        std::vector<MMI::Rect>& pointerHotAreas) const;
    void UpdatePointerAreas(sptr<SceneSession> sceneSession, std::vector<int32_t>& pointerChangeAreas) const;
    void UpdateWindowFlags(DisplayId displayId, const sptr<SceneSession>& sceneSession,
        MMI::WindowInfo& windowInfo) const;
    void AddModalExtensionWindowInfo(std::vector<MMI::WindowInfo>& windowInfoList, MMI::WindowInfo windowInfo,
        const sptr<SceneSession>& sceneSession);
    std::vector<MMI::WindowInfo> GetSecSurfaceWindowinfoList(const sptr<SceneSession>& sceneSession,
        const MMI::WindowInfo& hostWindowinfo, const Matrix3f& hostTransform) const;
    MMI::WindowInfo GetSecComponentWindowInfo(const SecSurfaceInfo& secSurfaceInfo,
        const MMI::WindowInfo& hostWindowinfo, const sptr<SceneSession>& sceneSession,
        const Matrix3f hostTransform) const;
    MMI::WindowInfo GetHostComponentWindowInfo(const SecSurfaceInfo& secSurfaceInfo,
        const MMI::WindowInfo& hostWindowinfo, const Matrix3f hostTransform) const;
    MMI::WindowInfo MakeWindowInfoFormHostWindow(const SecRectInfo& secRectInfo,
        const MMI::WindowInfo& hostWindowinfo) const;
    void ResetFlushWindowInfoTask();
    std::mutex mutexlock_;
    mutable std::shared_mutex secSurfaceInfoMutex_;
    FlushWindowInfoCallback flushWindowInfoCallback_;
    std::atomic_bool sessionDirty_ { false };
    std::atomic_bool hasPostTask_ { false };
    std::map<uint64_t, std::vector<SecSurfaceInfo>> secSurfaceInfoMap_;
};
} //namespace OHOS::Rosen

#endif
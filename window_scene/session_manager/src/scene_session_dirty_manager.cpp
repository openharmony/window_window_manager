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

#include "scene_session_dirty_manager.h"

#include <cinttypes>
#include <memory>
#include <sstream>

#include "input_manager.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"

namespace OHOS::Rosen {
namespace {
constexpr float DIRECTION0 = 0 ;
constexpr float DIRECTION90 = 90 ;
constexpr float DIRECTION180 = 180 ;
constexpr float DIRECTION270 = 270 ;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionDirtyManager"};
constexpr int POINTER_CHANGE_AREA_SEXTEEN = 16;
constexpr int POINTER_CHANGE_AREA_FIVE = 5;
} //namespace

void SceneSessionDirtyManager::PrintLogGetFullWindowInfoList(const std::vector<MMI::WindowInfo>& windowInfoList)
{
    WLOGFD("[WMSEvent] GetFullWindowInfoList Start WindowInfoList_.size = %{public}d", int(windowInfoList.size()));
    for (const auto& e: windowInfoList) {
        auto sessionleft = SceneSessionManager::GetInstance().GetSceneSession(e.id);
        WLOGFD("[WMSEvent] GetFullWindowInfoList windowInfoList id = %{public}d area.x = %{public}d  area.y = %{public}d"
            "area.w = %{public}d area.h = %{public}d  agentWindowId = %{public}d flags = %{pulic}d "
            "GetZOrder = %{public}d", e.id , e.area.x, e.area.y, e.area.width, e.area.height, e.agentWindowId,
            e.flags, int(sessionleft->GetZOrder()));
    }
}

void SceneSessionDirtyManager::PrintLogGetIncrementWindowInfoList(
    const std::map<uint64_t, std::vector<MMI::WindowInfo>>& screen2windowInfo)
{
    for (const auto& windowinfolist : screen2windowInfo) {
        WLOGFD("[WMSEvent] GetIncrementWindowInfoList screen id = %{public}d windowinfolist = %{public}d",
            int(windowinfolist.first), int(windowinfolist.second.size()));
        for (const auto& e: windowinfolist.second) {
            WLOGFD("[WMSEvent] GetIncrementWindowInfoList  windowInfoList id = %{public}d action = %{public}d"
                "area.x = %{public}d area.y = %{public}d area.w = %{publid}d area.h = %{public}d"
                " agentWindowId = %{public}d  flags = %{public}d", e.id, int(e.action), e.area.x,
                e.area.y, e.area.width, e.area.height, e.agentWindowId, e.flags);
        }
    }
}

void SceneSessionDirtyManager::CalTramform(const sptr<SceneSession> sceneSession, Matrix3f& tranform) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }

    Vector2f scale(sceneSession->GetScaleX(), sceneSession->GetScaleY());
    WSRect windowRect = sceneSession->GetSessionRect();
    Vector2f translate(windowRect.posX_, windowRect.posY_);
    tranform = Matrix3f::IDENTITY;
    tranform = tranform.Translate(translate);
    tranform = tranform.Scale(scale, sceneSession->GetPivotX(), sceneSession->GetPivotY());
    tranform = tranform.Inverse();
}

MMI::WindowInfo SceneSessionDirtyManager::PrepareWindowInfo(sptr<SceneSession> sceneSession, int action) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return {};
    }

    Matrix3f tranform;
    WSRect windowRect = sceneSession->GetSessionRect();
    auto pid = sceneSession->GetCallingPid();
    auto uid = sceneSession->GetCallingUid();
    auto windowId = sceneSession->GetWindowId();
    auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
    CalTramform(sceneSession, tranform);
    const unsigned int len = 9;
    std::vector<float> transformData(tranform.GetData(), tranform.GetData() + len);

    auto agentWindowId = sceneSession->GetWindowId();
    auto zOrder = sceneSession->GetZOrder();
    const std::vector<int32_t> pointerChangeAreas{ POINTER_CHANGE_AREA_SEXTEEN, POINTER_CHANGE_AREA_FIVE,
        POINTER_CHANGE_AREA_SEXTEEN, POINTER_CHANGE_AREA_FIVE, POINTER_CHANGE_AREA_SEXTEEN,
        POINTER_CHANGE_AREA_FIVE, POINTER_CHANGE_AREA_SEXTEEN, POINTER_CHANGE_AREA_FIVE };
    const std::vector<Rect>& hotAreas = sceneSession->GetTouchHotAreas();
    std::vector<MMI::Rect> mmiHotAreas;
    for (auto area : hotAreas) {
        MMI::Rect rect;
        rect.x = area.posX_;
        rect.y = area.posY_;
        rect.width = area.width_;
        rect.height = area.height_;
        mmiHotAreas.emplace_back(rect);
    }

    MMI::Rect rect{ 0, 0, windowRect.width_, windowRect.height_ };
    if (mmiHotAreas.empty()) {
        mmiHotAreas.emplace_back(rect);
    }
    rect.x = windowRect.posX_;
    rect.y = windowRect.posY_;
    MMI::WindowInfo windowInfo = {
        .id = windowId,
        .pid = pid,
        .uid = uid,
        .area = rect,
        .defaultHotAreas = mmiHotAreas,
        .pointerHotAreas = mmiHotAreas,
        .agentWindowId = agentWindowId,
        .flags = (!sceneSession->GetSystemTouchable()),
        .displayId = displayId,
        .pointerChangeAreas = pointerChangeAreas,
        .action = static_cast<MMI::WINDOW_UPDATE_ACTION>(action),
        .transform = transformData,
        .zOrder = zOrder
    };
    return windowInfo;
}

void SceneSessionDirtyManager::Clear()
{
    screen2windowInfo_.clear();
    isScreenSessionChange_ = false;
}

void SceneSessionDirtyManager::Init()
{
    windowType2Action_ = {
        {WindowUpdateType::WINDOW_UPDATE_ADDED, WindowAction::WINDOW_ADD},
        {WindowUpdateType::WINDOW_UPDATE_REMOVED, WindowAction::WINDOW_DELETE},
        {WindowUpdateType::WINDOW_UPDATE_FOCUSED, WindowAction::WINDOW_CHANGE},
        {WindowUpdateType::WINDOW_UPDATE_BOUNDS, WindowAction::WINDOW_CHANGE},
        {WindowUpdateType::WINDOW_UPDATE_ACTIVE, WindowAction::WINDOW_ADD},
        {WindowUpdateType::WINDOW_UPDATE_PROPERTY, WindowAction::WINDOW_CHANGE},
    };
    RegisterScreenInfoChangeListener();
}

std::vector<MMI::WindowInfo> SceneSessionDirtyManager::FullSceneSessionInfoUpdate() const
{
    std::vector<MMI::WindowInfo> windowInfoList;
    const auto& sceneSessionMap = Rosen::SceneSessionManager::GetInstance().GetSceneSessionMap();
    for (const auto& sceneSessionValuePair : sceneSessionMap) {
        const auto& sceneSessionValue = sceneSessionValuePair.second;
        if (sceneSessionValue == nullptr) {
            continue;
        }
        WLOGFD("[EventDispatch] FullSceneSessionInfoUpdate windowName = %{public}s bundleName = %{public}s"
            " windowId = %{public}d", sceneSessionValue->GetWindowName().c_str(),
            sceneSessionValue->GetSessionInfo().bundleName_.c_str(), sceneSessionValue->GetWindowId());
        if (IsWindowBackGround(sceneSessionValue)) {
            continue;
        }
        const auto windowinfo = GetWindowInfo(sceneSessionValue, WindowAction::UNKNOWN);
        windowInfoList.emplace_back(windowinfo);
    }
    return windowInfoList;
}

bool SceneSessionDirtyManager::IsWindowBackGround(const sptr<SceneSession>& sceneSession) const
{
    if (sceneSession == nullptr) {
        return true;
    }

    if (sceneSession->IsSystemInput()) {
        return false;
    } else if (sceneSession->IsSystemSession() && sceneSession->GetVisible() && sceneSession->IsSystemActive()) {
        return false;
    }
    if (!Rosen::SceneSessionManager::GetInstance().IsSessionVisible(sceneSession)) {
        return true;
    }
    return false;
}

void SceneSessionDirtyManager::NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession,
    const WindowUpdateType& type, int32_t sceneBoardPid)
{
    MMI::WindowInfo windowinfo;
    if (sceneSession == nullptr) {
        WLOGFW("sceneSession is null");
        return;
    }

    WindowAction action = GetSceneSessionAction(type);
    if (action == WindowAction::UNKNOWN) {
        WLOGFW("windowAction UNKNOW WindowUpdateType type = %{public}d", int(type));
        return;
    }

    auto windowId = sceneSession->GetWindowId();
    if (action == WindowAction::WINDOW_DELETE) {
        windowinfo.action = static_cast<MMI::WINDOW_UPDATE_ACTION>(WindowAction::WINDOW_DELETE);
        windowinfo.id = windowId;
        windowinfo.displayId = sceneSession->GetSessionProperty()->GetDisplayId();
        windowinfo.pid = sceneSession->GetCallingPid();
        windowinfo.uid = sceneSession->GetCallingUid();
    } else {
        if (action == WindowAction::WINDOW_CHANGE && IsWindowBackGround(sceneSession)) {
            return;
        }
        windowinfo = GetWindowInfo(sceneSession, action);
    }
    if (sceneBoardPid > 0) {
        windowinfo.pid = sceneBoardPid;
    }
    PushWindowInfoList(windowinfo.displayId, windowinfo);
}

std::vector<MMI::WindowInfo> SceneSessionDirtyManager::GetFullWindowInfoList()
{
    auto windowInfoList = FullSceneSessionInfoUpdate();
    PrintLogGetFullWindowInfoList(windowInfoList);
    Clear();
    return windowInfoList;
}

std::map<uint64_t, std::vector<MMI::WindowInfo>> SceneSessionDirtyManager::GetIncrementWindowInfoList()
{
    auto screen2windowInfo = screen2windowInfo_;
    PrintLogGetIncrementWindowInfoList(screen2windowInfo);
    Clear();
    return screen2windowInfo;
}

SceneSessionDirtyManager::WindowAction SceneSessionDirtyManager::GetSceneSessionAction(const WindowUpdateType& type)
{
    auto iter = windowType2Action_.find(type);
    if (iter != windowType2Action_.end()) {
        return windowType2Action_[type];
    }
    WLOGFW("WindowAction UNKNOW WindowUpdateType type = %{public}d", int(type));
    return WindowAction::UNKNOWN;
}

bool SceneSessionDirtyManager::IsScreenChange()
{
    return isScreenSessionChange_;
}

void SceneSessionDirtyManager::SetScreenChange(bool value)
{
    std::lock_guard<std::mutex> lock(mutexlock_);
    isScreenSessionChange_ = value;
}

void SceneSessionDirtyManager::SetScreenChange(uint64_t id)
{
    WLOGFD("SetScreenChange screenID = %{public}d ",static_cast<int>(id));
    SetScreenChange(true);
}

void SceneSessionDirtyManager::PushWindowInfoList(uint64_t displayID, const MMI::WindowInfo& windowinfo)
{
    if (screen2windowInfo_.find(displayID) == screen2windowInfo_.end()) {
        screen2windowInfo_.emplace(displayID, std::vector<MMI::WindowInfo>());
    }
    auto& twindowinlist = screen2windowInfo_[displayID];
    twindowinlist.emplace_back(windowinfo);
}

MMI::WindowInfo SceneSessionDirtyManager::GetWindowInfo(const sptr<SceneSession>& sceneSession,
    const SceneSessionDirtyManager::WindowAction& action) const
{
    return PrepareWindowInfo(sceneSession, static_cast<int32_t>(action));
}

void SceneSessionDirtyManager::RegisterScreenInfoChangeListener()
{
    auto fun = [this](uint64_t screenID) {
        SetScreenChange(screenID);
    };
    ScreenSessionManagerClient::GetInstance().RegisterScreenInfoChangeListener(fun);
    WLOGFI("RegisterScreenInfoChangeListener");
}
} //namespace OHOS::Rosen

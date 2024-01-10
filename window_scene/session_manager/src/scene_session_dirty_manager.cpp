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
#include "window_helper.h"
#include "wm_common_inner.h"

namespace OHOS::Rosen {
namespace {
constexpr float DIRECTION0 = 0 ;
constexpr float DIRECTION90 = 90 ;
constexpr float DIRECTION180 = 180 ;
constexpr float DIRECTION270 = 270 ;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionDirtyManager"};
constexpr unsigned int POINTER_CHANGE_AREA_COUNT = 8;
constexpr int POINTER_CHANGE_AREA_SEXTEEN = 16;
constexpr int POINTER_CHANGE_AREA_FIVE = 5;
constexpr int UPDATE_TASK_DURATION = 10;
const std::string UPDATE_WINDOW_INFO_TASK = "UpdateWindowInfoTask";
} //namespace

static bool operator==(const MMI::Rect left, const MMI::Rect right)
{
    return ((left.x == right.x) && (left.y == right.y) && (left.width == right.width) && (left.height == right.height));
}

void SceneSessionDirtyManager::PrintLogGetFullWindowInfoList(const std::vector<MMI::WindowInfo>& windowInfoList)
{
    WLOGFD("[WMSEvent] GetFullWindowInfoList Start WindowInfoList_.size = %{public}d", int(windowInfoList.size()));
    for (const auto& e: windowInfoList) {
        auto sessionleft = SceneSessionManager::GetInstance().GetSceneSession(e.id);
        WLOGFD("[WMSEvent] GetFullWindowInfoList windowInfoList id = %{public}d area.x = %{public}d  area.y = %{public}d"
            "area.w = %{public}d area.h = %{public}d  agentWindowId = %{public}d flags = %{public}d "
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
                "area.x = %{public}d area.y = %{public}d area.w = %{public}d area.h = %{public}d"
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

void SceneSessionDirtyManager::UpdateDefaultHotAreas(sptr<SceneSession> sceneSession,
    std::vector<MMI::Rect>& touchHotAreas,
    std::vector<MMI::Rect>& pointerHotAreas) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    WSRect windowRect = sceneSession->GetSessionRect();
    uint32_t touchOffset = 0;
    uint32_t pointerOffset = 0;
    if ((sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) ||
        (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW) ||
        (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_PIP)) {
        float vpr = 1.5f; // 1.5: default vp
        auto display = DisplayManager::GetInstance().GetDefaultDisplay();
        if (display) {
            vpr = display->GetVirtualPixelRatio();
        }
        touchOffset = static_cast<uint32_t>(HOTZONE_TOUCH * vpr);
        pointerOffset = static_cast<uint32_t>(HOTZONE_POINTER * vpr);
    }

    MMI::Rect touchRect = {
        .x = -touchOffset,
        .y = -touchOffset,
        .width = windowRect.width_ + touchOffset * 2,  // 2 : double touchOffset
        .height = windowRect.height_ + touchOffset * 2 // 2 : double touchOffset
    };

    MMI::Rect pointerRect = {
        .x = -pointerOffset,
        .y = -pointerOffset,
        .width = windowRect.width_ + pointerOffset * 2,  // 2 : double pointerOffset
        .height = windowRect.height_ + pointerOffset * 2 // 2 : double pointerOffset
    };

    touchHotAreas.emplace_back(touchRect);
    pointerHotAreas.emplace_back(pointerRect);
}


void SceneSessionDirtyManager::UpdateHotAreas(sptr<SceneSession> sceneSession, std::vector<MMI::Rect>& touchHotAreas,
    std::vector<MMI::Rect>& pointerHotAreas) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }

    WSRect windowRect = sceneSession->GetSessionRect();
    const std::vector<Rect>& hotAreas = sceneSession->GetTouchHotAreas();
    for (auto area : hotAreas) {
        MMI::Rect rect;
        rect.x = area.posX_;
        rect.y = area.posY_;
        rect.width = area.width_;
        rect.height = area.height_;
        auto iter = std::find_if(touchHotAreas.begin(), touchHotAreas.end(),
            [&rect](const MMI::Rect& var) { return rect == var; });
        if (iter != touchHotAreas.end()) {
            continue;
        }
        touchHotAreas.emplace_back(rect);
        pointerHotAreas.emplace_back(rect);
        if (touchHotAreas.size() == static_cast<int>(MMI::WindowInfo::MAX_HOTAREA_COUNT)) {
            auto sessionid = sceneSession->GetWindowId();
            WLOGFE("id = %{public}d hotAreas size > %{public}d", sessionid, static_cast<int>(hotAreas.size()));
            break;
        }
    }

    if (touchHotAreas.empty()) {
        return UpdateDefaultHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    }
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
    std::vector<int32_t> pointerChangeAreas(POINTER_CHANGE_AREA_COUNT, 0);
    auto windowMode = sceneSession->GetSessionProperty()->GetWindowMode();
    if (windowMode == Rosen::WindowMode::WINDOW_MODE_FLOATING &&
        Rosen::WindowHelper::IsMainWindow(sceneSession->GetSessionProperty()->GetWindowType())) {
        pointerChangeAreas = { POINTER_CHANGE_AREA_SEXTEEN, POINTER_CHANGE_AREA_FIVE,
        POINTER_CHANGE_AREA_SEXTEEN, POINTER_CHANGE_AREA_FIVE, POINTER_CHANGE_AREA_SEXTEEN,
        POINTER_CHANGE_AREA_FIVE, POINTER_CHANGE_AREA_SEXTEEN, POINTER_CHANGE_AREA_FIVE };
    }
    std::vector<MMI::Rect> touchHotAreas;
    std::vector<MMI::Rect> pointerHotAreas;
    UpdateHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    MMI::WindowInfo windowInfo = {
        .id = windowId,
        .pid = sceneSession->IsStartMoving() ? static_cast<int32_t>(getpid()) : pid,
        .uid = uid,
        .area = { windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_ },
        .defaultHotAreas = touchHotAreas,
        .pointerHotAreas = pointerHotAreas,
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

std::map<int32_t, sptr<SceneSession>> SceneSessionDirtyManager::GetDialogSessionMap(
    const std::map<int32_t, sptr<SceneSession>>& sessionMap) const
{
    std::map<int32_t, sptr<SceneSession>> dialogMap;

    for (const auto& elem: sessionMap) {
        const auto& session = elem.second;
        if (session != nullptr && session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
            const auto& parentSession = session->GetParentSession();
            if (parentSession == nullptr) {
                continue;
            }
            dialogMap.insert(std::make_pair(parentSession->GetPersistentId(), session));
            WLOGFI("Add dialog session, id: %{public}d, parentId: %{public}d",
                session->GetPersistentId(), parentSession->GetPersistentId());
        }
    }
    return dialogMap;
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
        if (IsFilterSession(sceneSessionValue)) {
            continue;
        }
        auto windowInfo = GetWindowInfo(sceneSessionValue, WindowAction::WINDOW_ADD);

        // all input event should trans to dialog window if dialog exists
        const auto& dialogMap = GetDialogSessionMap(sceneSessionMap);
        auto iter = (sceneSessionValue->GetParentPersistentId() == INVALID_SESSION_ID) ?
            dialogMap.find(sceneSessionValue->GetPersistentId()) :
            dialogMap.find(sceneSessionValue->GetParentPersistentId());
        if (iter != dialogMap.end() && iter->second != nullptr) {
            windowInfo.agentWindowId = static_cast<int32_t>(iter->second->GetPersistentId());
            WLOGFI("Change agentId, dialogId: %{public}d, parentId: %{public}d",
                iter->second->GetPersistentId(), sceneSessionValue->GetPersistentId());
        }
        windowInfoList.emplace_back(windowInfo);
    }
    return windowInfoList;
}

bool SceneSessionDirtyManager::IsFilterSession(const sptr<SceneSession>& sceneSession) const
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
    const WindowUpdateType& type, const bool startMoving)
{
    if (sceneSession == nullptr) {
        WLOGFW("sceneSession is null");
        return;
    }

    sessionDirty_.store(true);
    if (!hasPostTask_.load()) {
        hasPostTask_.store(true);
        auto task = [this]() {
            hasPostTask_.store(false);
            if (!sessionDirty_.load() || flushWindowInfoCallback_ == nullptr) {
                return;
            }
            flushWindowInfoCallback_();
        };
        SceneSessionManager::GetInstance().PostFlushWindowInfoTask(task,
            UPDATE_WINDOW_INFO_TASK, UPDATE_TASK_DURATION);
    }
}

std::vector<MMI::WindowInfo> SceneSessionDirtyManager::GetFullWindowInfoList()
{
    auto windowInfoList = FullSceneSessionInfoUpdate();
    PrintLogGetFullWindowInfoList(windowInfoList);
    if (screen2windowInfo_.size() > 0) {
        std::lock_guard<std::mutex> lock(mutexlock_);
        screen2windowInfo_.clear();
    }
    return windowInfoList;
}

std::map<uint64_t, std::vector<MMI::WindowInfo>> SceneSessionDirtyManager::GetIncrementWindowInfoList()
{
    std::lock_guard<std::mutex> lock(mutexlock_);
    auto screen2windowInfo = screen2windowInfo_;
    PrintLogGetIncrementWindowInfoList(screen2windowInfo);
    screen2windowInfo_.clear();
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
    std::lock_guard<std::mutex> lock(mutexlock_);
    if (screen2windowInfo_.find(displayID) == screen2windowInfo_.end()) {
        screen2windowInfo_.emplace(displayID, std::vector<MMI::WindowInfo>());
    }
    auto& twindowinlist = screen2windowInfo_[displayID];
    if (static_cast<WindowAction>(windowinfo.action) == WindowAction::WINDOW_CHANGE) {
        auto iter = std::find_if(twindowinlist.rbegin(), twindowinlist.rend(),
        [&windowinfo](const MMI::WindowInfo& var) {
            return var.id == windowinfo.id;
        });
        if (iter != twindowinlist.rend()) {
            auto action = iter->action;
            *iter = windowinfo;
            iter->action = action;
            return;
        }
    }

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

void SceneSessionDirtyManager::RegisterFlushWindowInfoCallback(const FlushWindowInfoCallback &&callback)
{
    flushWindowInfoCallback_ = std::move(callback);
}

void SceneSessionDirtyManager::ResetSessionDirty()
{
    sessionDirty_.store(false);
}
} //namespace OHOS::Rosen

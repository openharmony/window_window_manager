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

#include "scene_input_manager.h"

#include <hitrace_meter.h>
#include "input_manager.h"
#include "scene_session_dirty_manager.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneInputManager" };
const std::string SCENE_INPUT_MANAGER_THREAD = "SceneInputManager";
const std::string FLUSH_DISPLAY_INFO_THREAD = "OS_FlushDisplayInfoThread";
std::recursive_mutex g_instanceMutex;

constexpr float DIRECTION0 = 0;
constexpr float DIRECTION90 = 90;
constexpr float DIRECTION180 = 180;
constexpr float DIRECTION270 = 270;
constexpr int MAX_WINDOWINFO_NUM = 15;
constexpr int DEFALUT_DISPLAYID = 0;

MMI::Direction ConvertDegreeToMMIRotation(float degree, MMI::DisplayMode displayMode)
{
    MMI::Direction rotation = MMI::DIRECTION0;
    if (NearEqual(degree, DIRECTION0)) {
        rotation = MMI::DIRECTION0;
    }
    if (NearEqual(degree, DIRECTION90)) {
        rotation = MMI::DIRECTION90;
    }
    if (NearEqual(degree, DIRECTION180)) {
        rotation = MMI::DIRECTION180;
    }
    if (NearEqual(degree, DIRECTION270)) {
        rotation = MMI::DIRECTION270;
    }
    if (displayMode == MMI::DisplayMode::FULL) {
        switch (rotation) {
            case MMI::DIRECTION0: 
                rotation = MMI::DIRECTION90; 
                break;
            case MMI::DIRECTION90: 
                rotation = MMI::DIRECTION180;
                break;
            case MMI::DIRECTION180: 
                rotation = MMI::DIRECTION270;
                break;
            case MMI::DIRECTION270:
                rotation = MMI::DIRECTION0;
                break;
            default:
                rotation = MMI::DIRECTION0;
                break;
        }
    }
    return rotation;
}

std::string DumpRect(const std::vector<MMI::Rect>& rects)
{
    std::string rectStr = "";
    for (const auto& rect : rects) {
        rectStr = rectStr + " hot : [ " + std::to_string(rect.x) +" , " + std::to_string(rect.y) + 
        " , " + std::to_string(rect.width) + " , " + std::to_string(rect.height) + "]"; 
    }
    return rectStr;
}

std::string DumpWindowInfo(const MMI::WindowInfo& info)
{
    std::string infoStr = "windowInfo:";
    infoStr = infoStr + "windowId: " + std::to_string(info.id) + " pid : " + std::to_string(info.pid) +
        " uid: " + std::to_string(info.uid) + " area: [ " + std::to_string(info.area.x) + " , " +
        std::to_string(info.area.y) +  " , " + std::to_string(info.area.width) + " , " +
        std::to_string(info.area.height) + "] agentWindowId:" + std::to_string(info.agentWindowId) + " flags:" +
        std::to_string(info.flags)  +" displayId: " + std::to_string(info.displayId) +
        " action: " + std::to_string(static_cast<int>(info.action)) + " zOrder: " + std::to_string(info.zOrder);
    return infoStr + DumpRect(info.defaultHotAreas);
}

std::string DumpDisplayInfo(const MMI::DisplayInfo& info)
{
    std::string infoStr =  "DisplayInfo: ";
    infoStr = infoStr + " id: " + std::to_string(info.id) + " x: " + std::to_string(info.x) +
        "y: " + std::to_string(info.y) + " width: " + std::to_string(info.width) + 
        "height: " + std::to_string(info.height) + " dpi: " + std::to_string(info.dpi) + " name:" + info.name +
        " uniq: " + info.uniq + " displayMode: " + std::to_string(static_cast<int>(info.displayMode)) +
        " direction : " + std::to_string( static_cast<int>(info.direction));
    return infoStr;
}
} //namespace

SceneInputManager& SceneInputManager::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static SceneInputManager *instance = nullptr;
    if (instance == nullptr) {
        instance = new SceneInputManager();
        instance->Init();
    }
    return *instance;
}

void SceneInputManager::Init()
{
    sceneSessionDirty_ = std::make_shared<SceneSessionDirtyManager>();
    eventLoop_ = AppExecFwk::EventRunner::Create(FLUSH_DISPLAY_INFO_THREAD);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (sceneSessionDirty_) {
        auto callback = [this]() {
            FlushDisplayInfoToMMI();
        };
        sceneSessionDirty_->RegisterFlushWindowInfoCallback(callback);
    }
}

void SceneInputManager::ConstructDisplayInfos(std::vector<MMI::DisplayInfo>& displayInfos)
{
    std::unordered_map<ScreenId, ScreenProperty> screensProperties =
        Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    auto displayMode = Rosen::ScreenSessionManagerClient::GetInstance().GetFoldDisplayMode();
    for (auto& iter: screensProperties) {
        auto screenId = iter.first;
        auto& screenProperty = iter.second;
        auto screenSession = Rosen::ScreenSessionManagerClient::GetInstance().GetScreenSessionById(screenId);
        MMI::Direction displayRotation;
        if (screenSession && screenSession->GetDisplayNode()) {
            displayRotation = ConvertDegreeToMMIRotation(
                screenSession->GetDisplayNode()->GetStagingProperties().GetRotation(),
                static_cast<MMI::DisplayMode>(displayMode));
        } else {
            displayRotation = ConvertDegreeToMMIRotation(screenProperty.GetRotation(),
                static_cast<MMI::DisplayMode>(displayMode));
        }
        MMI::DisplayInfo displayInfo = {
            .id = screenId,
            .x = screenProperty.GetOffsetX(),
            .y = screenProperty.GetOffsetY(),
            .width = screenProperty.GetBounds().rect_.GetWidth(),
            .height = screenProperty.GetBounds().rect_.GetHeight(),
            .dpi = screenProperty.GetDensity() *  DOT_PER_INCH,
            .name = "display" + std::to_string(screenId),
            .uniq = "default" + std::to_string(screenId),
            .direction = ConvertDegreeToMMIRotation(screenProperty.GetRotation(),
                static_cast<MMI::DisplayMode>(displayMode)),
            .displayDirection = displayRotation,
            .displayMode = static_cast<MMI::DisplayMode>(displayMode)};
        displayInfos.emplace_back(displayInfo);
    }
}

void SceneInputManager::FlushFullInfoToMMI(const std::vector<MMI::WindowInfo>& windowInfoList)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    ConstructDisplayInfos(displayInfos);
    int mainScreenWidth = 0; 
    int mainScreenHeight = 0;
    if (!displayInfos.empty()) {
        mainScreenWidth = displayInfos[0].width;
        mainScreenHeight = displayInfos[0].height;
    }
    if (sceneSessionDirty_ == nullptr) {
        WLOGFE("scene session dirty is null");
        return;
    }

    int32_t focusId = Rosen::SceneSessionManager::GetInstance().GetFocusedSession();
    MMI::DisplayGroupInfo displayGroupInfo = {
        .width = mainScreenWidth,
        .height = mainScreenHeight,
        .focusWindowId = focusId,
        .windowsInfo = windowInfoList,
        .displaysInfo = displayInfos};
        for (const auto& displayInfo : displayGroupInfo.displaysInfo) {
            TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] - %s", DumpDisplayInfo(displayInfo).c_str());
        }
        std::string windowinfolst = "windowinfo  ";
        for (const auto& windowInfo : displayGroupInfo.windowsInfo) {
            windowinfolst.append(DumpWindowInfo(windowInfo).append("  ||  "));
        }
        TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] - %s", windowinfolst.c_str());
    MMI::InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);
} 

void SceneInputManager::NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession, const WindowUpdateType& type)
{
    if (sceneSessionDirty_) {
        sceneSessionDirty_->NotifyWindowInfoChange(sceneSession, type);
    }
}

void SceneInputManager::NotifyMMIWindowPidChange(const sptr<SceneSession>& sceneSession, const bool startMoving)
{
    if (sceneSessionDirty_) {
        sceneSessionDirty_->NotifyWindowInfoChange(sceneSession,
            WindowUpdateType::WINDOW_UPDATE_PROPERTY, startMoving);
        if (sceneSession == nullptr) {
            return;
        }
        sceneSession->SetIsStartMoving(startMoving);
    }
}

void SceneInputManager::NotifyWindowInfoChangeFromSession(const sptr<SceneSession>& sceneSesion)
{
    if (sceneSessionDirty_) {
        sceneSessionDirty_->NotifyWindowInfoChange(sceneSesion, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    }
}

void SceneInputManager::FlushChangeInfoToMMI(const std::map<uint64_t, std::vector<MMI::WindowInfo>>& screenId2Windows)
{
    for (auto& iter : screenId2Windows) {
        auto displayId = iter.first;
        auto& windowInfos = iter.second;
        std::string windowinfolst = "windowinfo  ";
        for (auto& windowInfo : windowInfos) {
            windowinfolst.append(DumpWindowInfo(windowInfo).append("  ||  "));
        }
        TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] --- %s", windowinfolst.c_str());
        int32_t focusId = Rosen::SceneSessionManager::GetInstance().GetFocusedSession();
        MMI::WindowGroupInfo windowGroup = {focusId, displayId, windowInfos};
        MMI::InputManager::GetInstance()->UpdateWindowInfo(windowGroup);
    }
}

void SceneInputManager::FlushDisplayInfoToMMI()
{
    auto task = [this]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "FlushDisplayInfoToMMI");
        if (sceneSessionDirty_ == nullptr) {
            return;
        }
        sceneSessionDirty_->ResetSessionDirty();

        std::vector<MMI::WindowInfo> windowInfoList = sceneSessionDirty_->GetFullWindowInfoList();
        TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] - windowInfo:windowList = %{public}d",
            static_cast<int>(windowInfoList.size()));
        if (windowInfoList.size() == 0) {
            FlushFullInfoToMMI(windowInfoList);
            return;
        }
        windowInfoList.back().action = MMI::WINDOW_UPDATE_ACTION::ADD_END;
        if (windowInfoList.size() <= MAX_WINDOWINFO_NUM) {
            FlushFullInfoToMMI(windowInfoList);
            return;
        }

        auto iterBegin = windowInfoList.begin();
        auto iterEnd = windowInfoList.end();
        auto iterNext = std::next(iterBegin, MAX_WINDOWINFO_NUM);
        FlushFullInfoToMMI(std::vector<MMI::WindowInfo>(iterBegin, iterNext));
        while (iterNext != iterEnd) {
            auto iterNewBegin = iterNext;
            if (std::distance(iterNewBegin, iterEnd) <= MAX_WINDOWINFO_NUM) {
                iterNext = iterEnd;
            } else {
                iterNext = std::next(iterNewBegin, MAX_WINDOWINFO_NUM);
            }
            std::map<uint64_t, std::vector<MMI::WindowInfo>> screenToWindowInfoList;
            screenToWindowInfoList.emplace(DEFALUT_DISPLAYID, std::vector<MMI::WindowInfo>(iterNewBegin, iterNext));
            FlushChangeInfoToMMI(screenToWindowInfoList);
        }
    };
    if (eventHandler_) {
        eventHandler_->PostTask(task);
    }
}
}
} // namespace OHOS::Rosen

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
#include "perform_reporter.h"
#include "scene_session_dirty_manager.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session_manager/include/scene_session_manager.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneInputManager" };
const std::string SCENE_INPUT_MANAGER_THREAD = "SceneInputManager";
const std::string FLUSH_DISPLAY_INFO_THREAD = "OS_FlushDisplayInfoThread";

constexpr int MAX_WINDOWINFO_NUM = 15;
constexpr int DEFALUT_DISPLAYID = 0;
constexpr int EMPTY_FOCUS_WINDOW_ID = -1;
constexpr int INVALID_PERSISTENT_ID = 0;
constexpr int DEFAULT_SCREEN_POS = 0;
constexpr int DEFAULT_SCREEN_SCALE = 100;
constexpr int DEFAULT_EXPAND_HEIGHT = 0;

bool IsEqualUiExtentionWindowInfo(const std::vector<MMI::WindowInfo>& a, const std::vector<MMI::WindowInfo>& b);
constexpr unsigned int TRANSFORM_DATA_LEN = 9;

bool operator!=(const MMI::Rect& a, const MMI::Rect& b)
{
    if (a.x != b.x || a.y != b.y || a.width != b.width || a.height != b.height) {
        return true;
    }
    return false;
}

bool operator==(const MMI::DisplayInfo& a, const MMI::DisplayInfo& b)
{
    if (a.id != b.id || a.x != b.x || a.y != b.y || a.width != b.width ||
        a.height != b.height || a.dpi != b.dpi || a.name != b.name || a.uniq != b.uniq ||
        static_cast<int32_t>(a.direction) != static_cast<int32_t>(b.direction) ||
        static_cast<int32_t>(a.displayDirection) != static_cast<int32_t>(b.displayDirection) ||
        static_cast<int32_t>(a.displayMode) != static_cast<int32_t>(b.displayMode) ||
        a.transform != b.transform) {
        return false;
    }
    return true;
}

bool operator!=(const std::vector<float>& a, const std::vector<float>& b)
{
    if (a.size() != b.size()) {
        return true;
    }
    int sizeOfA = static_cast<int>(a.size());
    for (int index = 0; index < sizeOfA; index++) {
        if (a[index] != b[index]) {
            return true;
        }
    }
    return false;
}

bool IsEqualWindowInfo(const MMI::WindowInfo& a, const MMI::WindowInfo& b)
{
    if (a.id != b.id || a.pid != b.pid || a.uid != b.uid || a.agentWindowId != b.agentWindowId || a.flags != b.flags ||
        a.displayId != b.displayId || a.zOrder != b.zOrder) {
        return false;
    }

    if (a.windowInputType != b.windowInputType || a.privacyMode != b.privacyMode ||
        a.windowType != b.windowType || a.pixelMap != b.pixelMap) {
        return false;
    }
    return true;
}

bool operator==(const MMI::WindowInfo& a, const MMI::WindowInfo& b)
{
    if (!IsEqualWindowInfo(a, b)) {
        return false;
    }

    if (a.area != b.area || a.defaultHotAreas.size() != b.defaultHotAreas.size() ||
        a.pointerHotAreas.size() != b.pointerHotAreas.size() ||
        a.pointerChangeAreas.size() != b.pointerChangeAreas.size() || a.transform.size() != b.transform.size()) {
        return false;
    }

    int sizeOfDefaultHotAreas = static_cast<int>(a.defaultHotAreas.size());
    for (int index = 0; index < sizeOfDefaultHotAreas; index++) {
        if (a.defaultHotAreas[index] != b.defaultHotAreas[index]) {
            return false;
        }
    }
    int sizeOfPointerHotAreas = static_cast<int>(a.pointerHotAreas.size());
    for (int index = 0; index < sizeOfPointerHotAreas; index++) {
        if (a.pointerHotAreas[index] != b.pointerHotAreas[index]) {
            return false;
        }
    }
    int sizeOfPointerChangeAreas = static_cast<int>(a.pointerChangeAreas.size());
    for (int index = 0; index < sizeOfPointerChangeAreas; index++) {
        if (a.pointerChangeAreas[index] != b.pointerChangeAreas[index]) {
            return false;
        }
    }

    if (a.transform != b.transform) {
        return false;
    }
    if (!IsEqualUiExtentionWindowInfo(a.uiExtentionWindowInfo, b.uiExtentionWindowInfo)) {
        return false;
    }
    return true;
}

bool operator!=(const MMI::WindowInfo& a, const MMI::WindowInfo& b)
{
    if (a == b) {
        return false;
    }
    return true;
}

bool IsEqualUiExtentionWindowInfo(const std::vector<MMI::WindowInfo>& a, const std::vector<MMI::WindowInfo>& b)
{
    if (a.size() != b.size()) {
        return false;
    }
    int size = static_cast<int>(a.size());
    for (int i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

std::string DumpTransformInDisplayInfo(const std::vector<float>& transform)
{
    std::stringstream stream("[");
    for (float transformItem : transform) {
        stream << transformItem << ",";
    }
    stream << "]";
    return stream.str();
}

std::string DumpDisplayInfo(const MMI::DisplayInfo& info)
{
    std::ostringstream infoStream("DisplayInfo: ");
    infoStream << " id: " << info.id << " x: " << info.x << " y: " << info.y
               << " width: " << info.width << " height: " << info.height << " dpi: " << info.dpi
               << " name: " << info.name << " uniq: " << info.uniq
               << " displayMode: " << static_cast<int>(info.displayMode)
               << " direction: " << static_cast<int>(info.direction)
               << " transform: " << DumpTransformInDisplayInfo(info.transform);
    std::string infoStr = infoStream.str();
    return infoStr;
}
} //namespace


WM_IMPLEMENT_SINGLE_INSTANCE(SceneInputManager)

void SceneInputManager::Init()
{
    sceneSessionDirty_ = std::make_shared<SceneSessionDirtyManager>();
    eventLoop_ = AppExecFwk::EventRunner::Create(FLUSH_DISPLAY_INFO_THREAD);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    SceneSession::RegisterGetConstrainedModalExtWindowInfo(
        [](const sptr<SceneSession>& sceneSession) -> std::optional<ExtensionWindowEventInfo> {
            return SceneInputManager::GetInstance().GetConstrainedModalExtWindowInfo(sceneSession);
        });
}

void SceneInputManager::RegisterFlushWindowInfoCallback(FlushWindowInfoCallback&& callback)
{
    sceneSessionDirty_->RegisterFlushWindowInfoCallback(std::move(callback));
}

void SceneInputManager::ResetSessionDirty()
{
    sceneSessionDirty_->ResetSessionDirty();
}

auto SceneInputManager::GetFullWindowInfoList() ->
    std::pair<std::vector<MMI::WindowInfo>, std::vector<std::shared_ptr<Media::PixelMap>>>
{
    return sceneSessionDirty_->GetFullWindowInfoList();
}

void SceneInputManager::ConstructDisplayInfos(std::vector<MMI::DisplayInfo>& displayInfos)
{
    std::map<ScreenId, ScreenProperty> screensProperties =
        ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    if (screensProperties.empty()) {
        TLOGE(WmsLogTag::WMS_EVENT, "screensProperties is empty");
        return;
    }
    auto displayMode = ScreenSessionManagerClient::GetInstance().GetFoldDisplayMode();
    for (auto& [screenId, screenProperty] : screensProperties) {
        auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSessionById(screenId);
        if (screenSession == nullptr) {
            TLOGE(WmsLogTag::WMS_EVENT, "screenSession get failed, screenId: %{public}" PRIu64"", screenId);
            continue;
        }
        auto screenCombination = screenSession->GetScreenCombination();
        auto screenWidth = screenProperty.GetPhysicalTouchBounds().rect_.GetWidth();
        auto screenHeight = screenProperty.GetPhysicalTouchBounds().rect_.GetHeight();
        auto transform = Matrix3f::IDENTITY;
        Vector2f scale(screenProperty.GetScaleX(), screenProperty.GetScaleY());
        transform = transform.Scale(scale, screenProperty.GetPivotX() * screenProperty.GetBounds().rect_.GetWidth(),
            screenProperty.GetPivotY() * screenProperty.GetBounds().rect_.GetHeight()).Inverse();
        std::vector<float> transformData(transform.GetData(), transform.GetData() + TRANSFORM_DATA_LEN);
        int32_t screenOneHandX = DEFAULT_SCREEN_POS;
        int32_t screenOneHandY = DEFAULT_SCREEN_POS;
        int32_t scalePercent = DEFAULT_SCREEN_SCALE;
        int32_t expandHeight = DEFAULT_EXPAND_HEIGHT;
        const SingleHandScreenInfo& singleHandScreenInfo = SceneSessionManager::GetInstance().GetSingleHandScreenInfo();
        if (screenId == ScreenSessionManagerClient::GetInstance().GetDefaultScreenId() &&
            singleHandScreenInfo.mode != SingleHandMode::MIDDLE) {
            SingleHandTransform singleHandTransform = SceneSessionManager::GetInstance().GetNormalSingleHandTransform();
            WSRect originRect = SceneSessionManager::GetInstance().GetOriginRect();
            screenOneHandX = singleHandTransform.posX;
            screenOneHandY = singleHandTransform.posY;
            scalePercent = singleHandTransform.scaleX * DEFAULT_SCREEN_SCALE;
            expandHeight = screenProperty.GetBounds().rect_.GetHeight() - originRect.height_;
        }
        MMI::DisplayInfo displayInfo = {
            .id = screenId,
            .x = screenProperty.GetStartX(),
            .y = screenProperty.GetStartY(),
            .width = screenWidth,
            .height = screenHeight,
            .dpi = screenProperty.GetDensity() *  DOT_PER_INCH,
            .name = "display" + std::to_string(screenId),
            .uniq = "default" + std::to_string(screenProperty.GetRsId()),
            .direction = ConvertDegreeToMMIRotation(screenProperty.GetPhysicalRotation()),
            .displayDirection = ConvertDegreeToMMIRotation(screenProperty.GetScreenComponentRotation()),
            .displayMode = static_cast<MMI::DisplayMode>(displayMode),
            .transform = transformData,
            .offsetX = screenProperty.GetInputOffsetX(),
            .offsetY = screenProperty.GetInputOffsetY(),
            .ppi = screenProperty.GetXDpi(),
            .isCurrentOffScreenRendering = screenProperty.GetCurrentOffScreenRendering(),
            .screenRealWidth = screenProperty.GetScreenRealWidth(),
            .screenRealHeight = screenProperty.GetScreenRealHeight(),
            .screenRealPPI = screenProperty.GetScreenRealPPI(),
            .screenRealDPI = static_cast<int32_t>(screenProperty.GetScreenRealDPI()),
            .screenCombination = static_cast<MMI::ScreenCombination>(screenCombination),
            .oneHandX = screenOneHandX,
            .oneHandY = screenOneHandY,
            .scalePercent = scalePercent,
            .expandHeight = expandHeight,
            .validWidth = screenProperty.GetValidWidth(),
            .validHeight = screenProperty.GetValidHeight(),
            .fixedDirection = ConvertDegreeToMMIRotation(screenProperty.GetDefaultDeviceRotationOffset()),
            .physicalWidth = screenProperty.GetPhyWidth(),
            .physicalHeight = screenProperty.GetPhyHeight(),
            .pointerActiveWidth = screenProperty.GetPointerActiveWidth(),
            .pointerActiveHeight = screenProperty.GetPointerActiveHeight(),
            .uniqueId = screenProperty.GetRsId()
        };
        displayInfos.emplace_back(displayInfo);
    }
}

std::unordered_map<DisplayId, int32_t> SceneInputManager::GetFocusedSessionMap() const
{
    std::unordered_map<DisplayId, int32_t> focusInfoMap;
    auto focusInfoMapArray = SceneSessionManager::GetInstance().GetAllFocusedSessionList();
    for (const auto& item : focusInfoMapArray) {
        int32_t focusedSessionId = item.second;
        auto focusedSession = SceneSessionManager::GetInstance().GetSceneSession(focusedSessionId);
        if (focusedSession == nullptr) {
            TLOGE(WmsLogTag::WMS_EVENT, "focusedSession is null, id: %{public}d", focusedSessionId);
            focusInfoMap[item.first] = INVALID_PERSISTENT_ID;
            continue;
        }
        if (auto modalUIExtensionEventInfo = focusedSession->GetLastModalUIExtensionEventInfo()) {
            focusedSessionId = modalUIExtensionEventInfo.value().persistentId;
        }
        focusInfoMap[item.first] = focusedSessionId;
    }
    return focusInfoMap;
}

void SceneInputManager::FlushFullInfoToMMI(const std::vector<MMI::DisplayInfo>& displayInfos,
    const std::vector<MMI::WindowInfo>& windowInfoList, bool isOverBatchSize)
{
    auto focusInfoMap = GetFocusedSessionMap();
    std::map<DisplayId, std::vector<MMI::DisplayInfo>> displayInfoMap;
    for (const auto& displayInfo : displayInfos) {
        auto displayGroupId = SceneSessionManager::GetInstance().GetDisplayGroupId(displayInfo.id);
        displayInfoMap[displayGroupId].emplace_back(displayInfo);
    }
    std::unordered_map<DisplayId, std::vector<MMI::WindowInfo>> windowInfoMap;
    for (const auto& windowInfo : windowInfoList) {
        windowInfoMap[windowInfo.groupId].emplace_back(windowInfo);
    }
    std::vector<MMI::DisplayGroupInfo> DisplayGroupInfoArray;
    for (const auto& pair : displayInfoMap) {
        auto displayGroupId = pair.first;
        auto displayInfoList = pair.second;
        int mainScreenWidth = 0;
        int mainScreenHeight = 0;
        if (!displayInfoList.empty()) {
            mainScreenWidth = displayInfoList[0].width;
            mainScreenHeight = displayInfoList[0].height;
        }
        if (!isOverBatchSize && !windowInfoMap[displayGroupId].empty()) {
            windowInfoMap[displayGroupId].back().action = MMI::WINDOW_UPDATE_ACTION::ADD_END;
        }
        MMI::DisplayGroupInfo displayGroupInfo = {
            .groupId = displayGroupId,
            .isMainGroup = displayGroupId == DEFAULT_DISPLAY_ID,
            .width = mainScreenWidth,
            .height = mainScreenHeight,
            .focusWindowId = focusInfoMap[displayGroupId],
            .currentUserId = currentUserId_,
            .windowsInfo = windowInfoMap[displayGroupId],
            .displaysInfo = displayInfoList
        };
        DisplayGroupInfoArray.push_back(displayGroupInfo);
    }

    MMI::InputManager::GetInstance()->UpdateDisplayInfo(DisplayGroupInfoArray);

    for (auto groupInfo : DisplayGroupInfoArray) {
        TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] - displayGroupId: %{public}d", groupInfo.groupId);
        for (const auto& displayInfo : groupInfo.displaysInfo) {
            TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] - %{public}s", DumpDisplayInfo(displayInfo).c_str());
        }
        std::string windowInfoListDump = "windowinfo  ";
        for (const auto& windowInfo : groupInfo.windowsInfo) {
            windowInfoListDump.append(DumpWindowInfo(windowInfo).append("  ||  "));
        }
        TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] - %{public}s", windowInfoListDump.c_str());
    }
}

void SceneInputManager::FlushEmptyInfoToMMI()
{
    auto task = [this]() {
        std::vector<MMI::DisplayInfo> displayInfos;
        ConstructDisplayInfos(displayInfos);
        int mainScreenWidth = 0;
        int mainScreenHeight = 0;
        if (!displayInfos.empty()) {
            mainScreenWidth = displayInfos[0].width;
            mainScreenHeight = displayInfos[0].height;
        }
        MMI::DisplayGroupInfo displayGroupInfo = {
            .width = mainScreenWidth,
            .height = mainScreenHeight,
            .focusWindowId = EMPTY_FOCUS_WINDOW_ID,
            .currentUserId = currentUserId_,
            .displaysInfo = displayInfos
        };
        TLOGI(WmsLogTag::WMS_EVENT, "userId:%{public}d width:%{public}d height:%{public}d",
            currentUserId_, mainScreenWidth, mainScreenHeight);
        MMI::InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);
    };
    if (eventHandler_) {
        eventHandler_->PostTask(task);
    }
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
    } else {
        TLOGD(WmsLogTag::WMS_EVENT, "sceneSessionDirty is nullptr");
    }
}

void SceneInputManager::FlushChangeInfoToMMI(const std::map<uint64_t, std::vector<MMI::WindowInfo>>& screenId2Windows)
{
    for (auto& iter : screenId2Windows) {
        auto displayId = iter.first;
        auto& windowInfos = iter.second;
        std::string windowInfoListDump = "windowinfo  ";
        for (auto& windowInfo : windowInfos) {
            windowInfoListDump.append(DumpWindowInfo(windowInfo).append("  ||  "));
        }
        TLOGD(WmsLogTag::WMS_EVENT, "[EventDispatch] --- %{public}s", windowInfoListDump.c_str());
        MMI::WindowGroupInfo windowGroup = {focusedSessionId_, displayId, windowInfos};
        MMI::InputManager::GetInstance()->UpdateWindowInfo(windowGroup);
    }
}

bool SceneInputManager::CheckNeedUpdate(const std::vector<MMI::DisplayInfo>& displayInfos,
    const std::vector<MMI::WindowInfo>& windowInfoList)
{
    int32_t focusId = SceneSessionManager::GetInstance().GetFocusedSessionId();
    if (focusId != lastFocusId_) {
        lastFocusId_ = focusId;
        lastDisplayInfos_ = displayInfos;
        lastWindowInfoList_ = windowInfoList;
        return true;
    }

    if (displayInfos.size() != lastDisplayInfos_.size() || windowInfoList.size() != lastWindowInfoList_.size()) {
        lastDisplayInfos_ = displayInfos;
        lastWindowInfoList_ = windowInfoList;
        return true;
    }

    int sizeOfDisplayInfos = static_cast<int>(displayInfos.size());
    for (int index = 0; index < sizeOfDisplayInfos; index++) {
        if (!(displayInfos[index] == lastDisplayInfos_[index])) {
            lastDisplayInfos_ = displayInfos;
            lastWindowInfoList_ = windowInfoList;
            return true;
        }
    }

    int sizeOfWindowInfoList = static_cast<int>(windowInfoList.size());
    for (int index = 0; index < sizeOfWindowInfoList; index++) {
        if (!(windowInfoList[index] == lastWindowInfoList_[index])) {
            lastWindowInfoList_ = windowInfoList;
            return true;
        }
    }
    return false;
}

void SceneInputManager::UpdateFocusedSessionId(int32_t focusedSessionId)
{
    auto focusedSceneSession = SceneSessionManager::GetInstance().GetSceneSession(focusedSessionId);
    if (focusedSceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "focusedSceneSession is null");
        return;
    }
    if (auto modalUIExtensionEventInfo = focusedSceneSession->GetLastModalUIExtensionEventInfo()) {
        focusedSessionId_ = modalUIExtensionEventInfo.value().persistentId;
    }
}

void DumpUIExtentionWindowInfo(const MMI::WindowInfo& windowInfo)
{
    auto sceneSession = SceneSessionManager::GetInstance().GetSceneSession(windowInfo.id);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "sceneSession is null");
        return;
    }
    auto surfaceNode = sceneSession->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "surfaceNode is null");
        return;
    }
    auto surfaceId = surfaceNode->GetId();
    TLOGI(WmsLogTag::WMS_EVENT, "wid:%{public}d sid:%{public}" PRIu64 " uiExtWindowInfo:%{public}d",
        windowInfo.id, surfaceId, static_cast<int>(windowInfo.uiExtentionWindowInfo.size()));
    for (const auto& uiExtWindowInfo : windowInfo.uiExtentionWindowInfo) {
        auto str = DumpWindowInfo(uiExtWindowInfo);
        str = "sec:" + std::to_string(uiExtWindowInfo.privacyUIFlag) + " " + str;
        TLOGI(WmsLogTag::WMS_EVENT, "uiExtWindowInfo:%{public}s", str.c_str());
    }
}

void SceneInputManager::PrintWindowInfo(const std::vector<MMI::WindowInfo>& windowInfoList)
{
    int windowListSize = static_cast<int>(windowInfoList.size());
    std::ostringstream idListStream;
    static std::string lastIdList;
    static uint32_t windowEventID = 0;
    if (windowEventID == UINT32_MAX) {
        windowEventID = 0;
    }
    focusedSessionId_ = SceneSessionManager::GetInstance().GetFocusedSessionId();
    std::unordered_map<int32_t, MMI::Rect> currWindowDefaultHotArea;
    static std::unordered_map<int32_t, MMI::Rect> lastWindowDefaultHotArea;
    for (auto& e : windowInfoList) {
        idListStream << e.id << "|" << e.flags << "|" << e.zOrder << "|"
                     << e.pid << "|" << e.defaultHotAreas.size();

        if (e.defaultHotAreas.size() > 0) {
            auto iter = lastWindowDefaultHotArea.find(e.id);
            if (iter == lastWindowDefaultHotArea.end() || iter->second != e.defaultHotAreas[0]) {
                idListStream << "|" << e.defaultHotAreas[0].x << "|" << e.defaultHotAreas[0].y
                             << "|" << e.defaultHotAreas[0].width << "|" << e.defaultHotAreas[0].height;
            }
            currWindowDefaultHotArea.insert({e.id, e.defaultHotAreas[0]});
        }
        idListStream << ",";
        if ((focusedSessionId_ == e.id) && (e.id == e.agentWindowId)) {
            UpdateFocusedSessionId(focusedSessionId_);
        }
        if (e.uiExtentionWindowInfo.size() > 0) {
            DumpUIExtentionWindowInfo(e);
        }
    }
    lastWindowDefaultHotArea = currWindowDefaultHotArea;
    SingleHandTransform transform = SceneSessionManager::GetInstance().GetNormalSingleHandTransform();
    idListStream << focusedSessionId_ << "|" << transform.posX << "|" << transform.posY
        << "|" << transform.scaleX << "|" << transform.scaleY;
    std::string idList = idListStream.str();
    if (lastIdList != idList) {
        windowEventID++;
        TLOGNI(WmsLogTag::WMS_EVENT, "LogWinInfo: eid:%{public}d,size:%{public}d,idList:%{public}s",
            windowEventID, windowListSize, idList.c_str());
        lastIdList = idList;
    }
}

void SceneInputManager::PrintDisplayInfo(const std::vector<MMI::DisplayInfo>& displayInfos)
{
    int displayListSize = static_cast<int>(displayInfos.size());
    std::ostringstream displayListStream;
    static std::string lastDisplayList = "";
    for (auto& displayInfo : displayInfos) {
        displayListStream << displayInfo.id << "|" << displayInfo.x << "|" << displayInfo.y << "|"
                          << displayInfo.width << "|" << displayInfo.height << "|"
                          << static_cast<int32_t>(displayInfo.direction) << "|"
                          << static_cast<int32_t>(displayInfo.displayDirection) << "|"
                          << static_cast<int32_t>(displayInfo.displayMode) << "|"
                          << displayInfo.offsetX << "|" << displayInfo.offsetY << "|"
                          << displayInfo.isCurrentOffScreenRendering << "|"
                          << displayInfo.screenRealWidth << "|" << displayInfo.screenRealHeight << "|"
                          << displayInfo.screenRealPPI << "|" << displayInfo.screenRealDPI << "|"
                          << static_cast<int32_t>(displayInfo.screenCombination) << "|"
                          << displayInfo.validWidth << "|" << displayInfo.validHeight << "|"
                          << displayInfo.fixedDirection << "|" << displayInfo.physicalWidth << "|"
                          << displayInfo.physicalHeight << "|" << displayInfo.oneHandX << "|"
                          << displayInfo.oneHandY << "|" << displayInfo.scalePercent << "|"
                          << displayInfo.expandHeight << "|" << displayInfo.uniqueId << ",";
    }

    std::string displayList = displayListStream.str();
    if (lastDisplayList != displayList) {
        TLOGI(WmsLogTag::WMS_EVENT, "num:%{public}d,list:%{public}s", displayListSize, displayList.c_str());
        lastDisplayList = displayList;
    }
}

void SceneInputManager::SetUserBackground(bool userBackground)
{
    TLOGD(WmsLogTag::WMS_MULTI_USER, "userBackground=%{public}d", userBackground);
    isUserBackground_.store(userBackground);
}

void SceneInputManager::SetCurrentUserId(int32_t userId)
{
    TLOGD(WmsLogTag::WMS_MULTI_USER, "Current userId=%{public}d", userId);
    currentUserId_ = userId;
    MMI::InputManager::GetInstance()->SetCurrentUser(userId);
}

void SceneInputManager::UpdateDisplayAndWindowInfo(const std::vector<MMI::DisplayInfo>& displayInfos,
    std::vector<MMI::WindowInfo> windowInfoList)
{
    if (windowInfoList.size() == 0) {
        return;
    }
    int32_t windowBatchSize = MAX_WINDOWINFO_NUM;
    if (windowInfoList[0].defaultHotAreas.size() > MMI::WindowInfo::DEFAULT_HOTAREA_COUNT) {
        windowBatchSize = MMI::InputManager::GetInstance()->GetWinSyncBatchSize(
            static_cast<int32_t>(windowInfoList[0].defaultHotAreas.size()),
            static_cast<int32_t>(displayInfos.size()));
    }
    int32_t windowListSize = static_cast<int32_t>(windowInfoList.size());
    if (windowListSize <= windowBatchSize) {
        FlushFullInfoToMMI(displayInfos, windowInfoList);
        return;
    }
    std::unordered_map<int32_t, std::vector<int32_t>> windowIndexMap;
    int32_t index = 0;
    for (const auto& windowInfo : windowInfoList) {
        windowIndexMap[windowInfo.groupId].emplace_back(index);
        index++;
    }
    for (auto& [displayGroupId, indexInfos] : windowIndexMap) {
        windowInfoList[indexInfos.back()].action = MMI::WINDOW_UPDATE_ACTION::ADD_END;
    }
    auto iterBegin = windowInfoList.begin();
    auto iterEnd = windowInfoList.end();
    auto iterNext = std::next(iterBegin, windowBatchSize);
    FlushFullInfoToMMI(displayInfos, std::vector<MMI::WindowInfo>(iterBegin, iterNext), true);
    while (iterNext != iterEnd) {
        auto iterNewBegin = iterNext;
        if (iterNewBegin->defaultHotAreas.size() <= MMI::WindowInfo::DEFAULT_HOTAREA_COUNT) {
            windowBatchSize = MAX_WINDOWINFO_NUM;
        }
        if (std::distance(iterNewBegin, iterEnd) <= windowBatchSize) {
            iterNext = iterEnd;
        } else {
            iterNext = std::next(iterNewBegin, windowBatchSize);
        }
        std::map<uint64_t, std::vector<MMI::WindowInfo>> screenToWindowInfoList;
        screenToWindowInfoList.emplace(DEFALUT_DISPLAYID, std::vector<MMI::WindowInfo>(iterNewBegin, iterNext));
        FlushChangeInfoToMMI(screenToWindowInfoList);
    }
}

void SceneInputManager::FlushDisplayInfoToMMI(std::vector<MMI::WindowInfo>&& windowInfoList,
                                              std::vector<std::shared_ptr<Media::PixelMap>>&& pixelMapList,
                                              const bool forceFlush)
{
    eventHandler_->PostTask([this, windowInfoList = std::move(windowInfoList),
                            pixelMapList = std::move(pixelMapList), forceFlush]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "FlushDisplayInfoToMMI");
        if (isUserBackground_.load()) {
            TLOGND(WmsLogTag::WMS_MULTI_USER, "User in background, no need to flush display info");
            return;
        }
        if (sceneSessionDirty_ == nullptr) {
            TLOGNE(WmsLogTag::WMS_EVENT, "sceneSessionDirty_ is nullptr");
            return;
        }
        std::vector<MMI::DisplayInfo> displayInfos;
        ConstructDisplayInfos(displayInfos);
        if (displayInfos.empty()) {
            std::ostringstream oss;
            oss << "displayInfos flush to MMI is empty!";
            int32_t ret = WindowInfoReporter::GetInstance().ReportEventDispatchException(
                static_cast<int32_t>(WindowDFXHelperType::WINDOW_FLUSH_EMPTY_DISPLAY_INFO_TO_MMI_EXCEPTION),
                getpid(), oss.str()
            );
            if (ret != 0) {
                TLOGNI(WmsLogTag::WMS_EVENT, "ReportEventDispatchException message failed, ret: %{public}d", ret);
            }
        }
        if (!forceFlush && !CheckNeedUpdate(displayInfos, windowInfoList)) {
            return;
        }
        PrintDisplayInfo(displayInfos);
        PrintWindowInfo(windowInfoList);
        if (windowInfoList.size() == 0) {
            FlushFullInfoToMMI(displayInfos, windowInfoList);
            return;
        }
        UpdateDisplayAndWindowInfo(displayInfos, std::move(windowInfoList));
    });
}

void SceneInputManager::UpdateSecSurfaceInfo(const std::map<uint64_t, std::vector<SecSurfaceInfo>>& secSurfaceInfoMap)
{
    if (sceneSessionDirty_ == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "sceneSessionDirty_ is nullptr");
        return;
    }
    sceneSessionDirty_->UpdateSecSurfaceInfo(secSurfaceInfoMap);
}

void SceneInputManager::UpdateConstrainedModalUIExtInfo(
    const std::map<uint64_t, std::vector<SecSurfaceInfo>>& constrainedModalUIExtInfoMap)
{
    if (sceneSessionDirty_ == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "sceneSessionDirty_ is nullptr");
        return;
    }
    sceneSessionDirty_->UpdateConstrainedModalUIExtInfo(constrainedModalUIExtInfoMap);
}

std::optional<ExtensionWindowEventInfo> SceneInputManager::GetConstrainedModalExtWindowInfo(
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "sceneSession is nullptr");
        return std::nullopt;
    }
    if (sceneSessionDirty_ == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "sceneSessionDirty_ is nullptr");
        return std::nullopt;
    }
    SecSurfaceInfo constrainedModalUIExtInfo;
    if (!sceneSessionDirty_->GetLastConstrainedModalUIExtInfo(sceneSession, constrainedModalUIExtInfo)) {
        return std::nullopt;
    }
    auto persistentId = sceneSession->GetUIExtPersistentIdBySurfaceNodeId(constrainedModalUIExtInfo.uiExtensionNodeId);
    if (persistentId == INVALID_PERSISTENT_ID) {
        TLOGE(WmsLogTag::WMS_EVENT, "invalid persistentId");
        return std::nullopt;
    }
    return ExtensionWindowEventInfo {
        .persistentId = persistentId,
        .pid = constrainedModalUIExtInfo.uiExtensionPid,
        .isConstrainedModal = true };
}
}
} // namespace OHOS::Rosen

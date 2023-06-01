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

#include "session_manager/include/scene_session_manager.h"

#include <ability_manager_client.h>
#include <parameters.h>
#include <start_options.h>
#include <want.h>

#include "color_parser.h"
#include "common/include/message_scheduler.h"
#include "root_scene.h"
#include "session/host/include/scene_session.h"
#include "session_info.h"
#include "window_manager_hilog.h"
#include "common/include/permission.h"
#include "wm_math.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager" };
const std::string SCENE_SESSION_MANAGER_THREAD = "SceneSessionManager";
}

WM_IMPLEMENT_SINGLE_INSTANCE(SceneSessionManager)

SceneSessionManager::SceneSessionManager()
{
    Init();
}

void SceneSessionManager::Init()
{
    WLOGFI("scene session manager init");
    msgScheduler_ = std::make_shared<MessageScheduler>(SCENE_SESSION_MANAGER_THREAD);
    LoadWindowSceneXml();
}

void SceneSessionManager::LoadWindowSceneXml()
{
    if (WindowSceneConfig::LoadConfigXml()) {
        if (WindowSceneConfig::GetConfig().IsMap()) {
            WindowSceneConfig::DumpConfig(*WindowSceneConfig::GetConfig().mapValue_);
        }
        ConfigWindowSceneXml();
    } else {
        WLOGFE("Load window scene xml failed");
    }
}

void SceneSessionManager::ConfigWindowSceneXml()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem item = config["windowEffect"];
    if (item.IsMap()) {
        ConfigWindowEffect(item);
    }
    item = config["decor"];
    if (item.IsMap()) {
        ConfigDecor(item);
    }
    item = config["defaultWindowMode"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 &&
            (numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
             numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(numbers[0]));
        }
    }
}

void SceneSessionManager::ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig)
{
    WindowSceneConfig::ConfigItem item = decorConfig.GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.isSystemDecorEnable_ = item.boolValue_;
        std::vector<std::string> supportedModes;
        item = decorConfig["supportedMode"];
        if (item.IsStrings()) {
            systemConfig_.decorModeSupportInfo_ = 0;
            supportedModes = *item.stringsValue_;
        }
        for (auto mode : supportedModes) {
            if (mode == "fullscreen") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN;
            } else if (mode == "floating") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
            } else if (mode == "pip") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_PIP;
            } else if (mode == "split") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY;
            } else {
                WLOGFW("Invalid supporedMode");
                systemConfig_.decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
                break;
            }
        }
    }
}

void SceneSessionManager::ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig)
{
    AppWindowSceneConfig config;
    // config corner radius
    WindowSceneConfig::ConfigItem item = effectConfig["appWindows"]["cornerRadius"];
    if (item.IsMap()) {
        if (ConfigAppWindowCornerRadius(item["fullScreen"], config.fullScreenCornerRadius_) &&
            ConfigAppWindowCornerRadius(item["split"], config.splitCornerRadius_) &&
            ConfigAppWindowCornerRadius(item["float"], config.floatCornerRadius_)) {
            appWindowSceneConfig_ = config;
        }
    }

    // config shadow
    item = effectConfig["appWindows"]["shadow"]["focused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.focusedShadow_)) {
            appWindowSceneConfig_.focusedShadow_ = config.focusedShadow_;
        }
    }

    item = effectConfig["appWindows"]["shadow"]["unfocused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.unfocusedShadow_)) {
            appWindowSceneConfig_.unfocusedShadow_ = config.unfocusedShadow_;
        }
    }

    WLOGFI("Config window effect successfully");
}

bool SceneSessionManager::ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out)
{
    std::map<std::string, float> stringToCornerRadius = {
        {"off", 0.0f}, {"defaultCornerRadiusXS", 4.0f}, {"defaultCornerRadiusS", 8.0f},
        {"defaultCornerRadiusM", 12.0f}, {"defaultCornerRadiusL", 16.0f}, {"defaultCornerRadiusXL", 24.0f}
    };

    if (item.IsString()) {
        auto value = item.stringValue_;
        if (stringToCornerRadius.find(value) != stringToCornerRadius.end()) {
            out = stringToCornerRadius[value];
            return true;
        }
    }
    return false;
}

bool SceneSessionManager::ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig,
    WindowShadowConfig& outShadow)
{
    WindowSceneConfig::ConfigItem item = shadowConfig["elevation"];
    if (item.IsFloats()) {
        auto elevation = *item.floatsValue_;
        if (elevation.size() != 1 || MathHelper::LessNotEqual(elevation[0], 0.0)) {
            return false;
        }
        outShadow.elevation_ = elevation[0];
    }

    item = shadowConfig["color"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return false;
        }
        outShadow.color_ = color;
    }

    item = shadowConfig["offsetX"];
    if (item.IsFloats()) {
        auto offsetX = *item.floatsValue_;
        if (offsetX.size() != 1) {
            return false;
        }
        outShadow.offsetX_ = offsetX[0];
    }

    item = shadowConfig["offsetY"];
    if (item.IsFloats()) {
        auto offsetY = *item.floatsValue_;
        if (offsetY.size() != 1) {
            return false;
        }
        outShadow.offsetY_ = offsetY[0];
    }

    item = shadowConfig["alpha"];
    if (item.IsFloats()) {
        auto alpha = *item.floatsValue_;
        if (alpha.size() != 1 ||
            (MathHelper::LessNotEqual(alpha[0], 0.0) && MathHelper::GreatNotEqual(alpha[0], 1.0))) {
            return false;
        }
        outShadow.alpha_ = alpha[0];
    }

    item = shadowConfig["radius"];
    if (item.IsFloats()) {
        auto radius = *item.floatsValue_;
        if (radius.size() != 1 || MathHelper::LessNotEqual(radius[0], 0.0)) {
            return false;
        }
        outShadow.radius_ = radius[0];
    }

    return true;
}

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    auto task = [this]() {
        if (rootSceneSession_ != nullptr) {
            return rootSceneSession_;
        }

        system::SetParameter("bootevent.boot.completed", "true");

        rootSceneSession_ = new (std::nothrow) RootSceneSession();
        rootScene_ = new (std::nothrow) RootScene();
        if (!rootSceneSession_ || !rootScene_) {
            WLOGFE("rootSceneSession or rootScene is nullptr");
            return sptr<RootSceneSession>(nullptr);
        }
        rootSceneSession_->SetLoadContentFunc(
            [rootScene = rootScene_](const std::string& contentUrl, NativeEngine* engine, NativeValue* storage,
                AbilityRuntime::Context* context) { rootScene->LoadContent(contentUrl, engine, storage, context); });
        return rootSceneSession_;
    };

    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    return msgScheduler_->PostSyncTask(task);
}

sptr<SceneSession> SceneSessionManager::GetSceneSession(uint64_t persistentId)
{
    auto iter = abilitySceneMap_.find(persistentId);
    if (iter == abilitySceneMap_.end()) {
        WLOGFE("Error found scene session with id: %{public}" PRIu64, persistentId);
        return nullptr;
    }
    return iter->second;
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo)
{
    sptr<SceneSession::SpecificSessionCallback> specificCallback = new (std::nothrow)
        SceneSession::SpecificSessionCallback();
    if (specificCallback == nullptr) {
        WLOGFE("SpecificSessionCallback is nullptr");
        return nullptr;
    }
    specificCallback->onCreate_ = std::bind(&SceneSessionManager::RequestSceneSession,
        this, std::placeholders::_1);
    specificCallback->onDestroy_ = std::bind(&SceneSessionManager::DestroyAndDisconnectSpecificSession,
        this, std::placeholders::_1);
    auto task = [this, sessionInfo, specificCallback]() {
        WLOGFI("sessionInfo: bundleName: %{public}s, abilityName: %{public}s", sessionInfo.bundleName_.c_str(),
            sessionInfo.abilityName_.c_str());
        sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, specificCallback);
        if (sceneSession == nullptr) {
            WLOGFE("sceneSession is nullptr!");
            return sceneSession;
        }
        uint64_t persistentId = GeneratePersistentId();
        sceneSession->SetPersistentId(persistentId);
        sceneSession->SetSystemConfig(systemConfig_);
        abilitySceneMap_.insert({ persistentId, sceneSession });
        WLOGFI("create session persistentId: %{public}" PRIu64 "", persistentId);
        return sceneSession;
    };
    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    return msgScheduler_->PostSyncTask(task);
}

sptr<AAFwk::SessionInfo> SceneSessionManager::SetAbilitySessionInfo(const sptr<SceneSession>& scnSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    if (!abilitySessionInfo) {
        WLOGFE("abilitySessionInfo is nullptr");
        return nullptr;
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    abilitySessionInfo->sessionToken = scnSession;
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->persistentId = scnSession->GetPersistentId();
    return abilitySessionInfo;
}

WSError SceneSessionManager::RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("active persistentId: %{public}" PRIu64 "", persistentId);
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        AAFwk::Want want;
        auto sessionInfo = scnSession->GetSessionInfo();
        want.SetElementName(sessionInfo.bundleName_, sessionInfo.abilityName_);
        AAFwk::StartOptions startOptions;
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        // to add StartAbility
        AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(want, startOptions, scnSessionInfo);
        return WSError::WS_OK;
    };
    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("background session persistentId: %{public}" PRIu64 "", persistentId);
        scnSession->SetActive(false);
        scnSession->Background();
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        // to add MinimizeAbility
        AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo);
        return WSError::WS_OK;
    };

    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("destroy session persistentId: %{public}" PRIu64 "", persistentId);
        scnSession->Disconnect();
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        abilitySceneMap_.erase(persistentId);
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        // to add TerminateAbility
        AAFwk::AbilityManagerClient::GetInstance()->CloseUIAbilityBySCB(scnSessionInfo);
        return WSError::WS_OK;
    };

    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartedByInputMethod()) {
        WLOGFE("check input method permission failed");
    }
    auto task = [this, sessionStage, eventChannel, surfaceNode, property, &persistentId, &session]() {
        // create specific session
        SessionInfo info;
        sptr<SceneSession> sceneSession = RequestSceneSession(info);
        if (sceneSession == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        // connect specific session and sessionStage
        WSError errCode = sceneSession->Connect(sessionStage, eventChannel, surfaceNode, systemConfig_, property);
        if (property) {
            persistentId = property->GetPersistentId();
        }
        if (createSpecificSessionFunc_) {
            createSpecificSessionFunc_(sceneSession);
        }
        session = sceneSession;
        return errCode;
    };
    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostSyncTask(task);
    return WSError::WS_OK;
}

void SceneSessionManager::SetCreateSpecificSessionListener(const NotifyCreateSpecificSessionFunc& func)
{
    WLOGFD("SetCreateSpecificSessionListener");
    createSpecificSessionFunc_ = func;
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSession(const uint64_t& persistentId)
{
    auto task = [this, persistentId]() {
        WLOGFI("Deatroy session persistentId: %{public}" PRIu64 "", persistentId);
        auto iter = abilitySceneMap_.find(persistentId);
        if (iter == abilitySceneMap_.end()) {
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        const auto& sceneSession = iter->second;
        if (sceneSession == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        auto ret = sceneSession->UpdateActiveStatus(false);
        ret = sceneSession->Disconnect();
        abilitySceneMap_.erase(persistentId);
        return ret;
    };

    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostSyncTask(task);
    return WSError::WS_OK;
}

const AppWindowSceneConfig& SceneSessionManager::GetWindowSceneConfig() const
{
    return appWindowSceneConfig_;
}

} // namespace OHOS::Rosen

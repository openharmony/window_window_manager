/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "scene_session.h"

#include "window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSession"};
}
SceneSession::SceneSession(const AbilityInfo& info) : Session(info.bundleName_), abilityInfo_(info)
{
}

WSError SceneSession::Connect(sptr<ISceneSessionStage>& sessionStage, sptr<IWindowEventChannel>& eventChannel)
{
    sceneSessionStage_ = sessionStage;
    windowEventChannel_ = eventChannel;
    return WSError::WS_OK;
}

WSError SceneSession::Foreground()
{
    WLOGFI("Foreground");
return WSError::WS_OK;
}

WSError SceneSession::Background()
{
return WSError::WS_OK;
}

WSError SceneSession::Disconnect()
{
return WSError::WS_OK;
}

WSError SceneSession::Minimize()
{
return WSError::WS_OK;
}

WSError SceneSession::Close()
{
return WSError::WS_OK;
}

WSError SceneSession::Recover()
{
return WSError::WS_OK;
}

WSError SceneSession::Maximum()
{
return WSError::WS_OK;
}

WSError SceneSession::RequestSceneSessionActivation(const AbilityInfo& info)
{
return WSError::WS_OK;
}

WSError SceneSession::SetActive(bool active)
{
return WSError::WS_OK;
}

const AbilityInfo& SceneSession::GetAbilityInfo() const
{
    return abilityInfo_;
}
}

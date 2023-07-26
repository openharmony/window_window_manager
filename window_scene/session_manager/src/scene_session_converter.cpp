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

#include "scene_session_converter.h"

using namespace std;
namespace OHOS {
namespace Rosen {

WSError SceneSessionConverter::ConvertToMissionInfos(std::vector<sptr<SceneSession>>& sceneSessionInfos,
                                                     std::vector<AAFwk::MissionInfo>& missionInfos)
{
    if (sceneSessionInfos.empty()) {
        return WSError::WS_OK;
    }
    for (auto iter = sceneSessionInfos.begin(); iter != sceneSessionInfos.end(); iter++) {
        AAFwk::MissionInfo missionInfo;
        missionInfo.id = (*iter)->GetPersistentId();
        missionInfo.runningState = (*iter)->IsActive();
        missionInfo.lockedState = ((*iter)->GetSessionInfo()).lockedState;
        missionInfo.label = ((*iter)->GetSessionInfo()).label;
        missionInfo.iconPath = ((*iter)->GetSessionInfo()).iconPath;
        missionInfo.want = *(((*iter)->GetSessionInfo()).want);
        missionInfo.continuable = ((*iter)->GetSessionInfo()).continuable;
        missionInfo.time = ((*iter)->GetSessionInfo()).time;
        missionInfo.continueState = (AAFwk::ContinueState)(AAFwk::ContinueState::CONTINUESTATE_UNKNOWN
            + (((*iter)->GetSessionInfo()).continueState - Rosen::ContinueState::CONTINUESTATE_UNKNOWN));
        missionInfos.push_back(missionInfo);
    }
    return WSError::WS_OK;
}

WSError SceneSessionConverter::ConvertToMissionInfo(sptr<SceneSession>& sceneSession,
                                                    OHOS::AAFwk::MissionInfo& missionInfo)
{
    if (sceneSession == nullptr) {
        return WSError::WS_OK;
    }
    missionInfo.id = sceneSession->GetPersistentId();
    missionInfo.runningState = sceneSession->IsActive();
    missionInfo.lockedState = (sceneSession->GetSessionInfo()).lockedState;
    missionInfo.label = (sceneSession->GetSessionInfo()).label;
    missionInfo.iconPath = (sceneSession->GetSessionInfo()).iconPath;
    missionInfo.want = *((sceneSession->GetSessionInfo()).want);
    missionInfo.continuable = (sceneSession->GetSessionInfo()).continuable;
    missionInfo.time = (sceneSession->GetSessionInfo()).time;
    missionInfo.continueState = (AAFwk::ContinueState) (AAFwk::ContinueState::CONTINUESTATE_UNKNOWN
        + ((sceneSession->GetSessionInfo()).continueState - Rosen::ContinueState::CONTINUESTATE_UNKNOWN));
    return WSError::WS_OK;
}
}
}

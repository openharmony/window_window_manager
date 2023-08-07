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

#ifndef OHOS_ROSEN_WINDOW_SCENE_DISTRIBUTED_CLIENT_H
#define OHOS_ROSEN_WINDOW_SCENE_DISTRIBUTED_CLIENT_H


#include <string>

#include "mission_info.h"
#include "mission_snapshot.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Rosen {
class DistributedClient {
public:
    DistributedClient() = default;
    virtual ~DistributedClient() = default;
    int32_t GetMissionInfos(const std::string& deviceId, int32_t numMissions,
                            std::vector<AAFwk::MissionInfo>& missionInfos);
    int32_t GetRemoteMissionSnapshotInfo(const std::string& deviceId, int32_t missionId,
                                         std::unique_ptr<AAFwk::MissionSnapshot>& missionSnapshot);
    int32_t SetMissionContinueState(int32_t missionId, const AAFwk::ContinueState &state);
    enum {
        START_REMOTE_ABILITY = 1,
        CONNECT_REMOTE_ABILITY = 6,
        DISCONNECT_REMOTE_ABILITY = 7,
        START_CONTINUATION = 11,
        NOTIFY_COMPLETE_CONTINUATION = 12,
        CONTINUE_MISSION = 36,
        CONTINUE_MISSION_OF_BUNDLENAME = 37,
        GET_MISSION_INFOS = 80,
        REGISTER_MISSION_LISTENER = 84,
        UNREGISTER_MISSION_LISTENER = 85,
        START_SYNC_MISSIONS = 92,
        STOP_SYNC_MISSIONS = 98,
        GET_REMOTE_MISSION_SNAPSHOT_INFO = 99,
        START_REMOTE_ABILITY_BY_CALL = 150,
        RELEASE_REMOTE_ABILITY = 151,
        START_REMOTE_FREE_INSTALL = 200,
        STOP_REMOTE_EXTERNSION_ABILITY = 225,
        REGISTER_ON_LISTENER = 260,
        REGISTER_OFF_LISTENER = 261,
        SET_MISSION_CONTINUE_STATE = 300
    };
private:
    sptr<IRemoteObject> GetDmsProxy();
    bool ReadMissionInfosFromParcel(Parcel& parcel, std::vector<AAFwk::MissionInfo>& missionInfos);
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SCENE_DISTRIBUTED_CLIENT_H

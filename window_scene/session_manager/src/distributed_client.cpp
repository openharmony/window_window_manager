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
#include "distributed_client.h"

#include "ability_manager_errors.h"
#include "distributed_parcel_helper.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::u16string DMS_PROXY_INTERFACE_TOKEN = u"ohos.distributedschedule.accessToken";
}
sptr<IRemoteObject> DistributedClient::GetDmsProxy()
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        WLOGFE("fail to get samgr.");
        return nullptr;
    }
    return samgrProxy->CheckSystemAbility(DISTRIBUTED_SCHED_SA_ID);
}

int32_t DistributedClient::GetMissionInfos(const std::string& deviceId, int32_t numMissions,
                                           std::vector<AAFwk::MissionInfo>& missionInfos)
{
    WLOGFI("called");
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        WLOGFE("remote system abiity is null");
        return AAFwk::INVALID_PARAMETERS_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DMS_PROXY_INTERFACE_TOKEN)) {
        return ERR_FLATTEN_OBJECT;
    }
    PARCEL_WRITE_HELPER(data, String16, Str8ToStr16(deviceId));
    PARCEL_WRITE_HELPER(data, Int32, numMissions);
    int32_t ret = remote->SendRequest(GET_MISSION_INFOS, data, reply, option);
    if (ret != ERR_NONE) {
        WLOGFW("sendRequest fail, error: %{public}d", ret);
        return ret;
    }
    return ReadMissionInfosFromParcel(reply, missionInfos) ? ERR_NONE : ERR_FLATTEN_OBJECT;
}

int32_t DistributedClient::GetRemoteMissionSnapshotInfo(const std::string& deviceId, int32_t missionId,
                                                        std::unique_ptr<AAFwk::MissionSnapshot>& missionSnapshot)
{
    if (deviceId.empty()) {
        WLOGFE("deviceId is null");
        return ERR_NULL_OBJECT;
    }
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return AAFwk::INVALID_PARAMETERS_ERR;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(DMS_PROXY_INTERFACE_TOKEN)) {
        return ERR_FLATTEN_OBJECT;
    }
    PARCEL_WRITE_HELPER(data, String, deviceId);
    PARCEL_WRITE_HELPER(data, Int32, missionId);
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(GET_REMOTE_MISSION_SNAPSHOT_INFO, data, reply, option);
    if (error != ERR_NONE) {
        WLOGFE("transact failed, error: %{public}d", error);
        return error;
    }
    std::unique_ptr<AAFwk::MissionSnapshot> missionSnapshotPtr(reply.ReadParcelable<AAFwk::MissionSnapshot>());
    missionSnapshot = std::move(missionSnapshotPtr);
    return ERR_NONE;
}

bool DistributedClient::ReadMissionInfosFromParcel(Parcel& parcel,
                                                   std::vector<AAFwk::MissionInfo>& missionInfos)
{
    int32_t hasMissions = parcel.ReadInt32();
    if (hasMissions == 1) {
        int32_t len = parcel.ReadInt32();
        WLOGFD("readLength is:%{public}d", len);
        if (len < 0) {
            return false;
        }
        size_t size = static_cast<size_t>(len);
        if ((size > parcel.GetReadableBytes()) || (missionInfos.max_size() < size)) {
            WLOGFE("Failed to read MissionInfo vector, size = %{public}zu", size);
            return false;
        }
        missionInfos.clear();
        for (size_t i = 0; i < size; i++) {
            AAFwk::MissionInfo *ptr = parcel.ReadParcelable<AAFwk::MissionInfo>();
            if (ptr == nullptr) {
                WLOGFW("read MissionInfo failed");
                return false;
            }
            missionInfos.emplace_back(*ptr);
            delete ptr;
        }
    }
    WLOGFI("info size is:%{public}zu", missionInfos.size());
    return true;
}

int32_t DistributedClient::SetMissionContinueState(int32_t missionId, const AAFwk::ContinueState &state)
{
    WLOGFI("SetMissionContinueState called. Mission id: %{public}d, state: %{public}d", missionId, state);
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        WLOGFI("remote system ablity is null");
        return AAFwk::INVALID_PARAMETERS_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(DMS_PROXY_INTERFACE_TOKEN)) {
        return ERR_FLATTEN_OBJECT;
    }
    PARCEL_WRITE_HELPER(data, Int32, missionId);
    PARCEL_WRITE_HELPER(data, Int32, static_cast<int32_t>(state));
    PARCEL_TRANSACT_SYNC_RET_INT(remote, SET_MISSION_CONTINUE_STATE, data, reply);
}
} // namespace Rosen
} // namespace OHOS

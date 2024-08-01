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
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::u16string DMS_PROXY_INTERFACE_TOKEN = u"ohos.distributedschedule.accessToken";
}
WM_IMPLEMENT_SINGLE_INSTANCE(DistributedClient)

void DistributedClient::DmsDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    TLOGI(WmsLogTag::DEFAULT, "dms died");
    DistributedClient::GetInstance().ClearDmsProxy();
}

sptr<IRemoteObject> DistributedClient::GetDmsProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (dmsProxy_ == nullptr) {
        ConnectDistributedSchedLocked();
    }
    return dmsProxy_;
}
 
void DistributedClient::ConnectDistributedSchedLocked()
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "fail to get samgr");
        return;
    }
    auto proxy = samgrProxy->CheckSystemAbility(DISTRIBUTED_SCHED_SA_ID);
    if (!proxy) {
        TLOGW(WmsLogTag::DEFAULT, "get dms proxy failed");
        return;
    }
    dmsDeath_ = sptr<DmsDeathRecipient>::MakeSptr();
    if (proxy->IsProxyObject() && !proxy->AddDeathRecipient(dmsDeath_)) {
        TLOGE(WmsLogTag::DEFAULT, "failed to add death recipient");
        return;
    }
    dmsProxy_ = proxy;
    TLOGI(WmsLogTag::DEFAULT, "success");
}

void DistributedClient::ClearDmsProxy()
{
    TLOGI(WmsLogTag::DEFAULT, "in");
    std::lock_guard<std::mutex> lock(mutex_);
    dmsProxy_ = nullptr;
    dmsDeath_ = nullptr;
}

int32_t DistributedClient::GetMissionInfos(const std::string& deviceId, int32_t numMissions,
                                           std::vector<AAFwk::MissionInfo>& missionInfos)
{
    TLOGI(WmsLogTag::DEFAULT, "called");
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote system abiity is null");
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
        TLOGW(WmsLogTag::DEFAULT, "sendRequest fail, error: %{public}d", ret);
        return ret;
    }
    return ReadMissionInfosFromParcel(reply, missionInfos) ? ERR_NONE : ERR_FLATTEN_OBJECT;
}

int32_t DistributedClient::GetRemoteMissionSnapshotInfo(const std::string& deviceId, int32_t missionId,
                                                        AAFwk::MissionSnapshot& missionSnapshot)
{
    TLOGI(WmsLogTag::DEFAULT, "SnapshotInfo");
    if (deviceId.empty()) {
        TLOGE(WmsLogTag::DEFAULT, "deviceId is null");
        return ERR_NULL_OBJECT;
    }
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
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
        TLOGE(WmsLogTag::DEFAULT, "transact failed, error: %{public}d", error);
        return error;
    }
    std::unique_ptr<AAFwk::MissionSnapshot> missionSnapshotPtr(reply.ReadParcelable<AAFwk::MissionSnapshot>());
    if (missionSnapshotPtr == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "missionSnapshotPtr is null");
        return ERR_UNKNOWN_OBJECT;
    }
    missionSnapshot = *missionSnapshotPtr;
    return ERR_NONE;
}

bool DistributedClient::ReadMissionInfosFromParcel(Parcel& parcel,
                                                   std::vector<AAFwk::MissionInfo>& missionInfos)
{
    int32_t hasMissions = parcel.ReadInt32();
    if (hasMissions == 1) {
        int32_t len = parcel.ReadInt32();
        TLOGD(WmsLogTag::DEFAULT, "readLength is:%{public}d", len);
        if (len < 0) {
            return false;
        }
        size_t size = static_cast<size_t>(len);
        if ((size > parcel.GetReadableBytes()) || (missionInfos.max_size() < size)) {
            TLOGE(WmsLogTag::DEFAULT, "Failed to read MissionInfo vector, size = %{public}zu", size);
            return false;
        }
        missionInfos.clear();
        for (size_t i = 0; i < size; i++) {
            AAFwk::MissionInfo *ptr = parcel.ReadParcelable<AAFwk::MissionInfo>();
            if (ptr == nullptr) {
                TLOGW(WmsLogTag::DEFAULT, "read MissionInfo failed");
                return false;
            }
            missionInfos.emplace_back(*ptr);
            delete ptr;
        }
    }
    TLOGI(WmsLogTag::DEFAULT, "info size is:%{public}zu", missionInfos.size());
    return true;
}

int32_t DistributedClient::SetMissionContinueState(int32_t missionId, const AAFwk::ContinueState &state)
{
    TLOGI(WmsLogTag::DEFAULT, "Mission id: %{public}d, state: %{public}d", missionId, state);
    sptr<IRemoteObject> remote = GetDmsProxy();
    if (remote == nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "remote system ablity is null");
        return AAFwk::INVALID_PARAMETERS_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(DMS_PROXY_INTERFACE_TOKEN)) {
        return ERR_FLATTEN_OBJECT;
    }
    PARCEL_WRITE_HELPER(data, Int32, missionId);
    PARCEL_WRITE_HELPER(data, Int32, static_cast<int32_t>(state));
    int32_t error = remote->SendRequest(SET_MISSION_CONTINUE_STATE, data, reply, option);
    if (error != ERR_NONE) {
        TLOGE(WmsLogTag::DEFAULT, "transact failed, error: %{public}d", error);
        return error;
    }
    return ERR_NONE;
}
} // namespace Rosen
} // namespace OHOS

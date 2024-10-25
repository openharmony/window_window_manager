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

#include "zidl/mock_session_manager_service_stub.h"

#include <ipc_skeleton.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerServiceStub"};
constexpr int32_t MAX_USER_SIZE = 1000;
}

int32_t MockSessionManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return ERR_TRANSACTION_FAILED;
    }
    auto msgId = static_cast<MockSessionManagerServiceMessage>(code);
    WLOGFI("Receive MockSessionManagerServiceMessage = %{public}u", msgId);
    switch (msgId) {
        case MockSessionManagerServiceMessage::TRANS_ID_GET_SESSION_MANAGER_SERVICE: {
            sptr<IRemoteObject> remoteObject = GetSessionManagerService();
            reply.WriteRemoteObject(remoteObject);
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_GET_SCREEN_SESSION_MANAGER: {
            sptr<IRemoteObject> remoteObject = GetScreenSessionManagerLite();
            reply.WriteRemoteObject(remoteObject);
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_NOTIFY_SCENE_BOARD_AVAILABLE: {
            NotifySceneBoardAvailable();
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_REGISTER_SMS_RECOVER_LISTENER: {
            sptr<IRemoteObject> listenerObject = data.ReadRemoteObject();
            if (listenerObject == nullptr) {
                TLOGE(WmsLogTag::WMS_RECOVER, "ReadRemoteObject failed");
                return ERR_INVALID_DATA;
            }
            RegisterSMSRecoverListener(listenerObject);
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_UNREGISTER_SMS_RECOVER_LISTENER: {
            UnregisterSMSRecoverListener();
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_REGISTER_SMS_LITE_RECOVER_LISTENER: {
            sptr<IRemoteObject> listenerObject = data.ReadRemoteObject();
            if (listenerObject == nullptr) {
                TLOGE(WmsLogTag::WMS_RECOVER, "ReadRemoteObject failed");
                return ERR_INVALID_DATA;
            }
            RegisterSMSLiteRecoverListener(listenerObject);
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_UNREGISTER_SMS_LITE_RECOVER_LISTENER: {
            UnregisterSMSLiteRecoverListener();
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_SET_SNAPSHOT_SKIP_BY_USERID_AND_BUNDLENAMES: {
            return HandleSetSnapshotSkipByUserIdAndBundleNames(data, reply);
        }
        case MockSessionManagerServiceMessage::TRANS_ID_SET_SNAPSHOT_SKIP_BY_MAP: {
            return HandleSetSnapshotSkipByIdNamesMap(data, reply);
        }
        default:
            WLOGFW("unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}

int32_t MockSessionManagerServiceStub::HandleSetSnapshotSkipByUserIdAndBundleNames(
    MessageParcel& data, MessageParcel& reply)
{
    int32_t userId = 0;
    if (!data.ReadInt32(userId)) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Failed to readInt32 userId");
        return ERR_INVALID_DATA;
    }
    std::vector<std::string> bundleNameList;
    if (!data.ReadStringVector(&bundleNameList)) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Fail to read bundleNameList");
        return ERR_INVALID_DATA;
    }
    int32_t errCode = SetSnapshotSkipByUserIdAndBundleNames(userId, bundleNameList);
    reply.WriteInt32(errCode);
    return ERR_NONE;
}

int32_t MockSessionManagerServiceStub::HandleSetSnapshotSkipByIdNamesMap(MessageParcel& data, MessageParcel& reply)
{
    int32_t mapSize = 0;
    if (!data.ReadInt32(mapSize)) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Fail to read mapSize");
        return ERR_INVALID_DATA;
    }
    std::unordered_map<int32_t, std::vector<std::string>> idBundlesMap;
    if (mapSize > MAX_USER_SIZE) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Too many users!");
        return ERR_INVALID_DATA;
    }
    for (int i = 0; i < mapSize; i++) {
        int32_t userId = data.ReadInt32();
        std::vector<std::string> bundleNameList;
        if (!data.ReadStringVector(&bundleNameList)) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "Fail to read bundleNameList");
            return ERR_INVALID_DATA;
        }
        idBundlesMap[userId] = bundleNameList;
    }
    int32_t errCode = SetSnapshotSkipByIdNamesMap(idBundlesMap);
    reply.WriteInt32(errCode);
    return ERR_NONE;
}
} // namespace Rosen
} // namespace OHOS
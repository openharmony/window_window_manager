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

#include "setting_provider.h"
#include <thread>
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "window_manager_hilog.h"
#include "rdb_errno.h"
#include "result_set.h"
#include "uri.h"

namespace OHOS {
namespace Rosen {
SettingProvider* SettingProvider::instance_;
std::mutex SettingProvider::mutex_;
sptr<IRemoteObject> SettingProvider::remoteObj_;
namespace {
const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
const std::string SETTING_COLUMN_VALUE = "VALUE";
const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "SettingProvider" };
constexpr int32_t PARAM_NUM_TEN = 10;
} // namespace

SettingProvider::~SettingProvider()
{
    instance_ = nullptr;
    remoteObj_ = nullptr;
}

SettingProvider& SettingProvider::GetInstance(int32_t systemAbilityId)
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = new SettingProvider();
            Initialize(systemAbilityId);
        }
    }
    return *instance_;
}

ErrCode SettingProvider::GetIntValue(const std::string& key, int32_t& value)
{
    int64_t valueLong;
    ErrCode ret = GetLongValue(key, valueLong);
    if (ret != ERR_OK) {
        return ret;
    }
    value = static_cast<int32_t>(valueLong);
    return ERR_OK;
}

ErrCode SettingProvider::GetLongValue(const std::string& key, int64_t& value)
{
    std::string valueStr;
    ErrCode ret = GetStringValue(key, valueStr);
    if (ret != ERR_OK) {
        return ret;
    }
    value = static_cast<int64_t>(strtoll(valueStr.c_str(), nullptr, PARAM_NUM_TEN));
    return ERR_OK;
}

ErrCode SettingProvider::GetBoolValue(const std::string& key, bool& value)
{
    std::string valueStr;
    ErrCode ret = GetStringValue(key, valueStr);
    if (ret != ERR_OK) {
        return ret;
    }
    value = (valueStr == "true");
    return ERR_OK;
}

ErrCode SettingProvider::PutIntValue(const std::string& key, int32_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

ErrCode SettingProvider::PutLongValue(const std::string& key, int64_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

ErrCode SettingProvider::PutBoolValue(const std::string& key, bool value, bool needNotify)
{
    std::string valueStr = value ? "true" : "false";
    return PutStringValue(key, valueStr, needNotify);
}

bool SettingProvider::IsValidKey(const std::string& key)
{
    std::string value;
    ErrCode ret = GetStringValue(key, value);
    return (ret != ERR_NAME_NOT_FOUND) && (!value.empty());
}

sptr<SettingObserver> SettingProvider::CreateObserver(const std::string& key, SettingObserver::UpdateFunc& func)
{
    sptr<SettingObserver> observer = new SettingObserver();
    observer->SetKey(key);
    observer->SetUpdateFunc(func);
    return observer;
}

void SettingProvider::ExecRegisterCb(const sptr<SettingObserver>& observer)
{
    if (observer == nullptr) {
        WLOGFE("observer is nullptr");
        return;
    }
    observer->OnChange();
}

ErrCode SettingProvider::RegisterObserver(const sptr<SettingObserver>& observer)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    helper->RegisterObserver(uri, observer);
    helper->NotifyChange(uri);
    std::thread execCb(SettingProvider::ExecRegisterCb, observer);
    execCb.detach();
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    WLOGFD("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

ErrCode SettingProvider::UnregisterObserver(const sptr<SettingObserver>& observer)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    helper->UnregisterObserver(uri, observer);
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    WLOGFD("succeed to unregister observer of uri=%{public}s", uri.ToString().c_str());
    return ERR_OK;
}

void SettingProvider::Initialize(int32_t systemAbilityId)
{
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        WLOGFE("GetSystemAbilityManager return nullptr");
        return;
    }
    auto remoteObj = sam->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        WLOGFE("GetSystemAbility return nullptr, systemAbilityId=%{public}d", systemAbilityId);
        return;
    }
    remoteObj_ = remoteObj;
}

ErrCode SettingProvider::GetStringValue(const std::string& key, std::string& value)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    std::vector<std::string> columns = {SETTING_COLUMN_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    Uri uri(AssembleUri(key));
    auto resultSet = helper->Query(uri, predicates, columns);
    ReleaseDataShareHelper(helper);
    if (resultSet == nullptr) {
        WLOGFE("helper->Query return nullptr");
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_INVALID_OPERATION;
    }
    int32_t count;
    resultSet->GetRowCount(count);
    if (count == 0) {
        WLOGFW("not found value, key=%{public}s, count=%{public}d", key.c_str(), count);
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        resultSet->Close();
        return ERR_NAME_NOT_FOUND;
    }
    const int32_t INDEX = 0;
    resultSet->GoToRow(INDEX);
    int32_t ret = resultSet->GetString(INDEX, value);
    if (ret != NativeRdb::E_OK) {
        WLOGFW("resultSet->GetString return not ok, ret=%{public}d", ret);
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        resultSet->Close();
        return ERR_INVALID_VALUE;
    }
    resultSet->Close();
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    return ERR_OK;
}

ErrCode SettingProvider::PutStringValue(const std::string& key, const std::string& value, bool needNotify)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto helper = CreateDataShareHelper();
    if (helper == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return ERR_NO_INIT;
    }
    DataShare::DataShareValueObject keyObj(key);
    DataShare::DataShareValueObject valueObj(value);
    DataShare::DataShareValuesBucket bucket;
    bucket.Put(SETTING_COLUMN_KEYWORD, keyObj);
    bucket.Put(SETTING_COLUMN_VALUE, valueObj);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    Uri uri(AssembleUri(key));
    if (helper->Update(uri, predicates, bucket) <= 0) {
        WLOGFD("no data exist, insert one row");
        helper->Insert(uri, bucket);
    }
    if (needNotify) {
        helper->NotifyChange(AssembleUri(key));
    }
    ReleaseDataShareHelper(helper);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    return ERR_OK;
}

std::shared_ptr<DataShare::DataShareHelper> SettingProvider::CreateDataShareHelper()
{
    auto helper = DataShare::DataShareHelper::Creator(remoteObj_, SETTING_URI_PROXY, SETTINGS_DATA_EXT_URI);
    if (helper == nullptr) {
        WLOGFW("helper is nullptr, uri=%{public}s", SETTING_URI_PROXY.c_str());
        return nullptr;
    }
    return helper;
}

bool SettingProvider::ReleaseDataShareHelper(std::shared_ptr<DataShare::DataShareHelper>& helper)
{
    if (!helper->Release()) {
        WLOGFW("release helper fail");
        return false;
    }
    return true;
}

Uri SettingProvider::AssembleUri(const std::string& key)
{
    Uri uri(SETTING_URI_PROXY + "&key=" + key);
    return uri;
}
} // namespace Rosen
} // namespace OHOS
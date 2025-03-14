/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rdb/starting_window_rdb_manager.h"

#include "rdb/scope_guard.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string DB_PRIMARY_KEY = "ID";
const std::string DB_BUNDLE_NAME = "BUNDLE_NAME";
const std::string DB_MODULE_NAME = "MODULE_NAME";
const std::string DB_ABILITY_NAME = "ABILITY_NAME";
const std::string DB_DARK_MODE = "DARK_MODE";
const std::string DB_BACKGROUND_COLOR_EARLY_VERSION = "BACKGROUND_COLOR_EARLY_VERSION";
const std::string DB_ICON_PATH_EARLY_VERSION = "ICON_PATH_EARLY_VERSION";
const std::string DB_CONFIG_FILE_ENABLED = "CONFIG_FILE_ENABLED";
const std::string DB_BACKGROUND_COLOR = "BACKGROUND_COLOR";
const std::string DB_ICON_PATH = "ICON_PATH";
const std::string DB_ILLUSTRATION_PATH = "ILLUSTRATION_PATH";
const std::string DB_BRANDING_PATH = "BRANDING_PATH";
const std::string DB_BACKGROUND_IMAGE_PATH = "BACKGROUND_IMAGE_PATH";
const std::string DB_BACKGROUND_IMAGE_FIT = "BACKGROUND_IMAGE_FIT";
constexpr int32_t DB_PRIMARY_KEY_INDEX = 0;
constexpr int32_t DB_BUNDLE_NAME_INDEX = 1;
constexpr int32_t DB_MODULE_NAME_INDEX = 2;
constexpr int32_t DB_ABILITY_NAME_INDEX = 3;
constexpr int32_t DB_DARK_MODE_INDEX = 4;
constexpr int32_t DB_BACKGROUND_COLOR_EARLY_VERSION_INDEX = 5;
constexpr int32_t DB_ICON_PATH_EARLY_VERSION_INDEX = 6;
constexpr int32_t DB_CONFIG_FILE_ENABLED_INDEX = 7;
constexpr int32_t DB_BACKGROUND_COLOR_INDEX = 8;
constexpr int32_t DB_ICON_PATH_INDEX = 9;
constexpr int32_t DB_ILLUSTRATION_PATH_INDEX = 10;
constexpr int32_t DB_BRANDING_PATH_INDEX = 11;
constexpr int32_t DB_BACKGROUND_IMAGE_PATH_INDEX = 12;
constexpr int32_t DB_BACKGROUND_IMAGE_FIT_INDEX = 13;

inline NativeRdb::ValuesBucket BuildValuesBucket(const StartingWindowRdbItemKey& key, const StartingWindowInfo& value)
{
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.PutString(DB_BUNDLE_NAME, key.bundleName);
    valuesBucket.PutString(DB_MODULE_NAME, key.moduleName);
    valuesBucket.PutString(DB_ABILITY_NAME, key.abilityName);
    valuesBucket.PutBool(DB_DARK_MODE, key.darkMode);
    valuesBucket.PutInt(DB_BACKGROUND_COLOR_EARLY_VERSION, value.backgroundColorEarlyVersion_);
    valuesBucket.PutString(DB_ICON_PATH_EARLY_VERSION, value.iconPathEarlyVersion_);
    valuesBucket.PutBool(DB_CONFIG_FILE_ENABLED, value.configFileEnabled_);
    valuesBucket.PutInt(DB_BACKGROUND_COLOR, value.backgroundColor_);
    valuesBucket.PutString(DB_ICON_PATH, value.iconPath_);
    valuesBucket.PutString(DB_ILLUSTRATION_PATH, value.illustrationPath_);
    valuesBucket.PutString(DB_BRANDING_PATH, value.brandingPath_);
    valuesBucket.PutString(DB_BACKGROUND_IMAGE_PATH, value.backgroundImagePath_);
    valuesBucket.PutString(DB_BACKGROUND_IMAGE_FIT, value.backgroundImageFit_);
    return valuesBucket;
}

inline NativeRdb::AbsRdbPredicates BuildPredicates(cosnt std::string tableName, const StartingWindowRdbItemKey& key)
{
    NativeRdb::AbsRdbPredicates absRdbPredicates(tableName);
    absRdbPredicates.EqualTo(DB_BUNDLE_NAME, key.bundleName)->And()->
                    .EqualTo(DB_MODULE_NAME, key.moduleName)->And()->
                    .EqualTo(DB_ABILITY_NAME, key.abilityName)->And()->
                    .EqualTo(DB_DARK_MODE, key.darkMode);
    return absRdbPredicates;
}

inline bool CheckRdbResult(int resCode)
{
    if (rescode != NativeRdb::E_OK) {
        TLOGE(WmsLogTag::WMS_PATTERN, "rdb failed, ret:%{public}d", resCode);
        return false;
    }
    return true;
}
} // namespace

StartingWindowRdbManager::StartingWindowRdbManager(const WmsRdbConfig& rdbConfig)
{
    std::string uniqueConstraint = std::string("CONSTRAINT uniqueConstraint UNIQUE (" +
        DB_BUNDLE_NAME + ", " + DB_MODULE_NAME + ", " + DB_ABILITY_NAME + ", " + DB_DARK_MODE + ")");
    wmsRdbConfig_.createTableSql = std::string("CREATE TABLE IF NOT EXISTS " + wmsRdbConfig_.tableName +
        "(" + DB_PRIMARY_KEY + "INTEGER PRIMARY KEY AUTOINCREMENT, " + DB_BUNDLE_NAME + " TEXT NOT NULL, " +
        DB_MODULE_NAME + " TEXT NOT NULL, " + DB_ABILITY_NAME + " TEXT NOT NULL, " + DB_DARK_MODE + " BOOLEAN, " +
        DB_BACKGROUND_COLOR_EARLY_VERSION + " INTEGER, " + DB_ICON_PATH_EARLY_VERSION + " TEXT, " +
        DB_CONFIG_FILE_ENABLED + " BOOLEAN, " + DB_BACKGROUND_COLOR + " INTEGER, " + DB_ICON_PATH + " TEXT, " +
        DB_ICON_PATH + " TEXT, " + DB_BRANDING_PATH + " TEXT, " +
        DB_BACKGROUND_IMAGE_PATH + " TEXT, " + DB_BACKGROUND_IMAGE_FIT + " TEXT, " +
        uniqueConstraint + ");");
}

StartingWindowRdbManager::~StartingWindowRdbManager()
{
    rdbStore_ = nullptr;
}

std::shared_ptr<NativeRdb::RdbStore> StartingWindowRdbManager::GetRdbStore()
{
    std::lock_guard<std::mutex> lock(rdbMutex_);
    if (rdbStore_ != nullptr) {
        return rdbStore_;
    }
    NativeRdb::RdbStoreConfig rdbStoreConfig(wmsRdbConfig_.dbPath, wmsRdbConfig_.dbName);
    rdbStoreConfig.SetSecurityLevel(NativeRdb::SecurityLevel::S1);
    int32_t resCode = NativeRdb::E_OK;
    WmsRdbOpenCallback wmsCallback(wmsRdbConfig_);
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(
        rdbStoreConfig, wmsRdbConfig_.version, wmsCallback, resCode);
    TLOGD(WmsLogTag::WMS_PATTERN, "resCode:%{public}d", resCode);
    return rdbStore_;
}

bool StartingWindowRdbManager::Init()
{
    auto rdbStore = GetRdbStore();
    if (rdbStore == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "failed");
        return false;
    }
    return true;
}

bool StartingWindowRdbManager::InsertData(const StartingWindowRdbItemKey& key, const StartingWindowInfo& value)
{
    auto rdbStore = GetRdbStore();
    if (rdbStore == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "RdbStore is null");
        return false;
    }
    int64_t rowId = -1;
    auto valuesBucket = BuildValuesBucket(key, value);
    auto ret = rdbStore->InsertWithConflitResolution(
        rowId, wmsRdbConfig_.tableName, valuesBucket, NativeRdb::ConflictResolution::ON_CONFLICT_REPLACE);
    return CheckRdbResult(ret);
}

bool StartingWindowRdbManager::BatchInsert(int64_t& outInsertNum,
    const std::vector<std::pair<StartingWindowRdbItemKey, StartingWindowInfo>>& inputValues)
{
    auto rdbStore = GetRdbStore();
    if (rdbStore == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "RdbStore is null");
        return false;
    }
    int64_t rowId = -1;
    std::vector<NativeRdb::ValuesBucket> valuesBuckets;
    for (const auto& pair : inputValues) {
        auto valuesBucket = BuildValuesBucket(pair.first, pair.second);
        valuesBuckets.emplace_back(valuesBucket);
    }
    auto ret = rdbStore->BatchInsert(outInsertNum, wmsRdbConfig_.tableName, valuesBuckets);
    return CheckRdbResult(ret);
}

bool StartingWindowRdbManager::DeleteDataByBundleName(const std::string& bundleName)
{
    auto rdbStore = GetRdbStore();
    if (rdbStore == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "RdbStore is null");
        return false;
    }
    int64_t deletedRows = -1;
    NativeRdb::AbsRdbPredicates absRdbPredicates(wmsRdbConfig_.tableName);
    absRdbPredicates.EqualTo(DB_BUNDLE_NAME, bundleName);
    auto ret = rdbStore->Delete(deletedRows, absRdbPredicates);
    return CheckRdbResult(ret);
}

bool StartingWindowRdbManager::DeleteDataByBundleName(const StartingWindowRdbItemKey& key, StartingWindowInfo& value)
{
    auto rdbStore = GetRdbStore();
    if (rdbStore == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "RdbStore is null");
        return false;
    }
    auto absRdbPredicates = BuildPredicates(wmsRdbConfig_.tableName, key);
    auto absSharedResultSet = rdbStore_->QueryByStep(absRdbPredicates, std::vector<std::string>());
    if (absSharedResultSet == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "absSharedResultSet failed");
        return false;
    }
    ScopeGuard stateGuard([&] { absSharedResultSet->Close(); });
    auto ret = absSharedResultSet->GoToFirstRow();
    if (ret != NativeRdb::E_OK) {
        TLOGE(WmsLogTag::WMS_PATTERN, "GoToFirstRow failed, ret:%{public}d", ret);
        return false;
    }
    int backgroundColorEarlyVersion = 0;
    int backgroundColor = 0;
    int configFileEnabled = 0;
    if (!CheckRdbResult(absSharedResultSet->GetInt(
        DB_BACKGROUND_COLOR_EARLY_VERSION_INDEX, backgroundColorEarlyVersion)) ||
        !CheckRdbResult(absSharedResultSet->GetString(
            DB_ICON_PATH_EARLY_VERSION_INDEX, value.iconPathEarlyVersion_)) ||
        !CheckRdbResult(absSharedResultSet->GetInt(DB_BACKGROUND_COLOR_INDEX, backgroundColor)) ||
        !CheckRdbResult(absSharedResultSet->GetString(DB_ICON_PATH_INDEX, value.iconPath_)) ||
        !CheckRdbResult(absSharedResultSet->GetInt(DB_CONFIG_FILE_ENABLED_INDEX, configFileEnabled)) ||
        !CheckRdbResult(absSharedResultSet->GetString(DB_ILLUSTRATION_PATH_INDEX, value.illustrationPath_)) ||
        !CheckRdbResult(absSharedResultSet->GetString(DB_BRANDING_PATH_INDEX, value.brandingPath_)) ||
        !CheckRdbResult(absSharedResultSet->GetString(DB_BACKGROUND_IMAGE_PATH_INDEX, value.backgroundImagePath_)) ||
        !CheckRdbResult(absSharedResultSet->GetString(DB_BACKGROUND_IMAGE_FIT_INDEX, value.backgroundImageFit_))) {
        return false;
    }
    value.backgroundColorEarlyVersion_ = backgroundColorEarlyVersion;
    value.backgroundColor_ = backgroundColor;
    value.configFileEnabled_ = configFileEnabled;
    return true;
}
} // namespace Rosen
} // namespace OHOS
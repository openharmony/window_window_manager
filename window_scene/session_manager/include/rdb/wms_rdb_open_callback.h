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

#ifndef OHOS_ROSEN_WINDOW_SCENE_WMS_RDB_OPEN_CALLBACK_H
#define OHOS_ROSEN_WINDOW_SCENE_WMS_RDB_OPEN_CALLBACK_H

#include "rdb_errno.h"
#include "rdb_open_callback.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr static const char* STARTING_WINDOW_RDB_NAME = "/starting_window_config.db";
constexpr static const char* STARTING_WINDOW_TABLE_NAME = "starting_window_config";
constexpr static int32_t STARTING_WINDOW_RDB_VERSION = 2;
} // namespace

struct WmsRdbConfig {
    std::string dbPath;
    std::string dbName { STARTING_WINDOW_RDB_NAME };
    std::string tableName { STARTING_WINDOW_TABLE_NAME };
    std::string createTableSql;
    int32_t version { STARTING_WINDOW_RDB_VERSION };
};

class WmsRdbOpenCallback : public NativeRdb::RdbOpenCallback {
public:
    WmsRdbOpenCallback(const WmsRdbConfig& wmsRdbConfig);
    int32_t OnCreate(NativeRdb::RdbStore& rdbStore) override;
    int32_t OnUpgrade(NativeRdb::RdbStore& rdbStore, int currentVersion, int targetVersion) override;
    int32_t OnDowngrade(NativeRdb::RdbStore& rdbStore, int currentVersion, int targetVersion) override;
    int32_t OnOpen(NativeRdb::RdbStore& rdbStore) override;
    int32_t onCorruption(std::string databaseFile) override;
    
private:
    void UpgradeDbToNextVersion(NativeRdb::RdbStore& rdbStore, int newVersion);
    void AddColumn(NativeRdb::RdbStore& rdbStore, const std::string columnInfo);
    WmsRdbConfig wmsRdbConfig_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SCENE_WMS_RDB_OPEN_CALLBACK_H
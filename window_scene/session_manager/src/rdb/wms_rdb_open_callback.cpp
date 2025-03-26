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

#include "rdb/wms_rdb_open_callback.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
WmsRdbOpenCallback::WmsRdbOpenCallback(const WmsRdbConfig& wmsRdbConfig)\
    : wmsRdbConfig_(wmsRdbConfig) {}
int32_t WmsRdbOpenCallback::OnCreate(NativeRdb::RdbStore& rdbStore)
{
    TLOGI(WmsLogTag::WMS_PATTERN, "called");
    auto sqlResult = rdbStore.ExecuteSql(wmsRdbConfig_.createTableSql);
    if (sqlResult != NativeRdb::E_OK) {
        TLOGE(WmsLogTag::WMS_PATTERN, "execute sql error");
    }
    return sqlResult;
}

int32_t WmsRdbOpenCallback::OnUpgrade(NativeRdb::RdbStore& rdbStore, int currentVersion, int targetVersion)
{
    TLOGD(WmsLogTag::WMS_PATTERN, "called");
    return NativeRdb::E_OK;
}

int32_t WmsRdbOpenCallback::OnDowngrade(NativeRdb::RdbStore& rdbStore, int currentVersion, int targetVersion)
{
    TLOGD(WmsLogTag::WMS_PATTERN, "called");
    return NativeRdb::E_OK;
}

int32_t WmsRdbOpenCallback::OnOpen(NativeRdb::RdbStore& rdbStore)
{
    TLOGD(WmsLogTag::WMS_PATTERN, "called");
    return NativeRdb::E_OK;
}

int32_t WmsRdbOpenCallback::onCorruption(std::string databaseFile)
{
    TLOGD(WmsLogTag::WMS_PATTERN, "called");
    return NativeRdb::E_OK;
}
} // namespace Rosen
} // namespace OHOS
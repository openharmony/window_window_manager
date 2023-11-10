/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef PARCEL_UTILS_H
#define PARCEL_UTILS_H

#include "parcel.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "ParcelUtils"};
}
#define READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(type, parcel, data)            \
    do {                                                                    \
        if (!(parcel).Read##type(data)) {                                   \
            WLOGFE("Fail to read %{public}s type from parcel", #type); \
            return false;                                                   \
        }                                                                   \
    } while (0)

#define WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(type, parcel, data)               \
    do {                                                                        \
        if (!(parcel).Write##type(data)) {                                      \
            WLOGFE("Fail to write %{public}s type from parcel", #type);    \
            return false;                                                       \
        }                                                                       \
    } while (0)

inline bool ContainerSecurityVerify(Parcel &parcel, int readContainerSize, size_t valSize)
{
    size_t readAbleDataSize = (parcel).GetReadableBytes();
    size_t readSize = static_cast<size_t>(readContainerSize);
    if ((readSize > readAbleDataSize) || (valSize < readSize)) {
        WLOGFE("Failed to read container, readSize = %{public}zu, readAbleDataSize = %{public}zu",
            readSize, readAbleDataSize);
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
#endif // PARCEL_UTILS_H
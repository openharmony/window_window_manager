/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTIL_H
#define UTIL_H

#include <cinttypes>

namespace OHOS {
namespace Rosen {

int64_t GetSysClockTime();

int64_t GetMillisTime();

template<typename T>
bool AddInt(T op1, T op2, T minVal, T maxVal, T &res)
{
    if (op1 >= 0) {
        if (op2 > maxVal - op1) {
            return false;
        }
    } else {
        if (op2 < minVal - op1) {
            return false;
        }
    }
    res = op1 + op2;
    return true;
}

inline bool AddInt32(int32_t op1, int32_t op2, int32_t &res)
{
    return AddInt(op1, op2, INT32_MIN, INT32_MAX, res);
}

inline bool AddInt64(int64_t op1, int64_t op2, int64_t &res)
{
    return AddInt(op1, op2, INT64_MIN, INT64_MAX, res);
}

} // namespace Rosen
} // namespace OHOS
#endif // UTIL_H
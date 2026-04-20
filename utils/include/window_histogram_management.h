/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef WINDOW_HISTOGRAM_MANAGEMENT_H
#define WINDOW_HISTOGRAM_MANAGEMENT_H

#include "histogram_plugin_macros.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {

/**
 * @brief Boolean values for histogram recording
 */
enum class HistogramBoolean : int32_t {
    HISTOGRAM_BOOLEAN_FALSE = 0,
    HISTOGRAM_BOOLEAN_TRUE = 1,
};

/**
 * @brief Base value for WM error codes
 */
constexpr int32_t WM_ERROR_BASE = 1300000;

/**
 * @brief Maximum value for WM error code histogram
 * @note Equals to WM_ERROR_DEVICE_NOT_SUPPORT (801),
 *       which is the maximum value below WM_ERROR_BASE
 */
constexpr int32_t WM_ERROR_HISTOGRAM_MAX = 801;

/**
 * @brief Convert WmErrorCode to histogram index
 * @param error - WmErrorCode enum value
 * @return Index value (0, 1, 2, ...), values >= WM_ERROR_BASE will be converted by subtracting WM_ERROR_BASE
 * @note constexpr function enables compile-time evaluation when error is a compile-time constant
 */
constexpr int32_t WmErrorCodeToIndex(WmErrorCode error)
{
    int32_t value = static_cast<int32_t>(error);
    return (value >= WM_ERROR_BASE) ? (value - WM_ERROR_BASE) : value;
}

/**
 * @brief Histogram enumeration macro for WmErrorCode
 * @param name - The histogram name (string)
 * @param errorCode - WmErrorCode enum value, will be converted to index via WmErrorCodeToIndex()
 * @note Compile-time type check ensures errorCode is WmErrorCode type
 */
#define HISTOGRAM_ENUMERATION_ERROR_CODE(name, errorCode) \
    static_assert(std::is_same_v<decltype(errorCode), WmErrorCode>, \
        "HISTOGRAM_ENUMERATION_ERROR_CODE: errorCode parameter must be WmErrorCode type"); \
    HISTOGRAM_ENUMERATION(name, WmErrorCodeToIndex(errorCode), WM_ERROR_HISTOGRAM_MAX)

} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_HISTOGRAM_MANAGEMENT_H
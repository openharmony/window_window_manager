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

/**
 * @brief Convert WmErrorCode to histogram index
 * @param error - WmErrorCode enum value
 * @return Index value (0, 1, 2, ...), values >= 1300000 will be converted by subtracting 1300000
 * @note constexpr function enables compile-time evaluation when error is a compile-time constant
 */
constexpr int32_t WmErrorCodeToIndex(WmErrorCode error) {
    int32_t value = static_cast<int32_t>(error);
    return (value >= 1300000) ? (value - 1300000) : value;
}

/**
 * @brief Histogram enumeration macro for WmErrorCode
 * @param name - The histogram name (string)
 * @param error_code - WmErrorCode enum value, will be converted to index via WmErrorCodeToIndex()
 * @param max - Maximum value (element with the maximum value in WmErrorCode)
 */
#define HISTOGRAM_ENUMERATION_ERROR_CODE(name, errorCode) \
    HISTOGRAM_ENUMERATION(name, WmErrorCodeToIndex(errorCode), 801)

#endif // WINDOW_HISTOGRAM_MANAGEMENT_H

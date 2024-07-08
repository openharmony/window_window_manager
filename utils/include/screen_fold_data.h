/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_SCREEN_FOLD_DATA_H
#define OHOS_ROSEN_SCREEN_FOLD_DATA_H

#include <cstdint>
#include <string>

namespace OHOS::Rosen {

struct ScreenFoldData {
    ScreenFoldData() {}
    ~ScreenFoldData() {}

    bool GetTypeCThermalWithUtil();
    void SetFocusedPkgName(const std::string& packageName);
    void SetInvalid();

    constexpr static int32_t DMS_PARAM_NUMBER = 5;
    constexpr static int32_t INVALID_VALUE = -1;
    constexpr static int32_t HALF_FOLD_REPORT_TRIGGER_DURATION = 15;

    // DMS
    int32_t currentScreenFoldStatus_ = INVALID_VALUE;
    int32_t nextScreenFoldStatus_ = 0;
    int32_t currentScreenFoldStatusDuration_ = 0;
    int32_t screenRotation_ = 0;
    float postureAngle_ = 0.0f;

    // WMS
    int32_t screenDisplayMode_ = 0;
    std::string focusedPackageName_;

    // Others
    int32_t typeCThermal_ = 0;
    int32_t screenThermal_ = 0;
    int32_t angleOfABAxis_ = 0;
    int32_t isTentMode_ = 0;
};
}

#endif // OHOS_ROSEN_SCREEN_FOLD_DATA_H
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

#ifndef SCREEN_POWER_UTILS_H
#define SCREEN_POWER_UTILS_H

#include <atomic>
#include <mutex>

namespace OHOS {
namespace Rosen {
class ScreenPowerUtils {
public:
    ScreenPowerUtils() {}
    ~ScreenPowerUtils() {}
    static void EnablePowerForceTimingOut();
    static void DisablePowerForceTimingOut();
    static void LightAndLockScreen(const std::string& detail);
    static bool GetEnablePowerForceTimingOut();

private:
    static std::mutex powerTimingMutex_;
    static std::mutex powerLockMutex_;
    static bool isEnablePowerForceTimingOut_;
};
} // Rosen
} // OHOS
#endif


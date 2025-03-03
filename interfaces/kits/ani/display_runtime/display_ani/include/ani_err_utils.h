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

#ifndef WINDOW_WINDOW_MANAGER_ANI_ERROR_UTILS_H
#define WINDOW_WINDOW_MANAGER_ANI_ERROR_UTILS_H

// #include "native_engine/native_engine.h"
// #include "native_engine/native_value.h"

#include "ani.h"
#include "dm_common.h"
#include "wm_common.h"

namespace OHOS::Rosen {
class AniErrUtils {
public:
    static ani_object CreateAniError(ani_env* env, const WMError& errorCode, std::string msg = "");
    static ani_object CreateAniError(ani_env* env, const WmErrorCode& errorCode, std::string msg = "");
    static ani_object CreateAniError(ani_env* env, const DMError& errorCode, std::string msg = "");
    static ani_object CreateAniError(ani_env* env, const DmErrorCode& errorCode, std::string msg = "");

private:
    static std::string GetErrorMsg(const WMError& errorCode);
    static std::string GetErrorMsg(const WmErrorCode& errorCode);
    static std::string GetErrorMsg(const DMError& errorCode);
    static std::string GetErrorMsg(const DmErrorCode& errorCode);
};
} // namespace OHOS::Rosen

#endif //WINDOW_WINDOW_MANAGER_ANI_ERROR_UTILS_H

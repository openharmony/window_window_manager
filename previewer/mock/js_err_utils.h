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

#ifndef WINDOW_WINDOW_MANAGER_JS_ERROR_UTILS_H
#define WINDOW_WINDOW_MANAGER_JS_ERROR_UTILS_H

#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"

#include "dm_common.h"
#include "wm_common.h"

namespace OHOS::Rosen {
class JsErrUtils {
public:
    static inline napi_value CreateJsError(napi_env env, const WMError& errorCode, std::string msg = "")
    {
        return nullptr;
    }

    static inline napi_value CreateJsError(napi_env env, const WmErrorCode& errorCode, std::string msg = "")
    {
        return nullptr;
    }

    static inline napi_value CreateJsError(napi_env env, const DMError& errorCode, std::string msg = "")
    {
        return nullptr;
    }

    static inline napi_value CreateJsError(napi_env env, const DmErrorCode& errorCode, std::string msg = "")
    {
        return nullptr;
    }
private:
    static inline napi_value CreateJsNumber(napi_env env, int32_t value)
    {
        return nullptr;
    }

    static inline napi_value CreateJsNumber(napi_env env, uint32_t value)
    {
        return nullptr;
    }

    static inline napi_value CreateJsNumber(napi_env env, int64_t value)
    {
        return nullptr;
    }

    static inline napi_value CreateJsNumber(napi_env env, double value)
    {
        return nullptr;
    }

    template<class T>
    static napi_value CreateJsValue(napi_env env, const T& value)
    {
        return nullptr;
    }

    static std::string GetErrorMsg(const WMError& errorCode)
    {
        return "";
    }

    static std::string GetErrorMsg(const WmErrorCode& errorCode)
    {
        return "";
    }

    static std::string GetErrorMsg(const DMError& errorCode)
    {
        return "";
    }

    static std::string GetErrorMsg(const DmErrorCode& errorCode)
    {
        return "";
    }
};
} // namespace OHOS::Rosen

#endif //WINDOW_WINDOW_MANAGER_JS_ERROR_UTILS_H
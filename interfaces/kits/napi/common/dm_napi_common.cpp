/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "dm_napi_common.h"

#include <securec.h>
#include <string>

#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "ipc_skeleton.h"

namespace OHOS {
napi_status SetMemberInt32(napi_env env, napi_value result, const char *key, int32_t value)
{
    napi_value num;
    napi_status ret = napi_ok;
    ret = napi_create_int32(env, value, &num);
    if (ret != napi_ok) {
        TLOGI(WmsLogTag::DMS, "napi_create_int32 error, code is %{public}d", ret);
        return ret;
    }
    ret = napi_set_named_property(env, result, key, num);
    if (ret != napi_ok) {
        TLOGI(WmsLogTag::DMS, "napi_set_named_property error, code is %{public}d", ret);
        return ret;
    }
    return ret;
}

napi_status SetMemberUint32(napi_env env, napi_value result, const char *key, uint32_t value)
{
    napi_value num;
    napi_status ret = napi_ok;
    ret = napi_create_uint32(env, value, &num);
    if (ret != napi_ok) {
        TLOGI(WmsLogTag::DMS, "napi_create_uint32 error, code is %{public}d", ret);
        return ret;
    }
    ret = napi_set_named_property(env, result, key, num);
    if (ret != napi_ok) {
        TLOGI(WmsLogTag::DMS, "napi_set_named_property error, code is %{public}d", ret);
        return ret;
    }
    return ret;
}

napi_status SetMemberUndefined(napi_env env, napi_value result, const char *key)
{
    napi_value undefined;
    napi_status ret = napi_ok;
    ret = napi_get_undefined(env, &undefined);
    if (ret != napi_ok) {
        TLOGI(WmsLogTag::DMS, "napi_get_undefined error, code is %{public}d", ret);
        return ret;
    }
    ret = napi_set_named_property(env, result, key, undefined);
    if (ret != napi_ok) {
        TLOGI(WmsLogTag::DMS, "napi_set_named_property error, code is %{public}d", ret);
        return ret;
    }
    return ret;
}

bool CheckCallingPermission(const std::string &permission)
{
    WLOGI("CheckCallingPermission, permission:%{public}s", permission.c_str());
    if (!permission.empty() &&
        Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission)
        != AppExecFwk::Constants::PERMISSION_GRANTED) {
        WLOGFE("%{public}s permission not granted.", permission.c_str());
        return false;
    }
    WLOGI("CheckCallingPermission end.");
    return true;
}

void SetErrorInfo(napi_env env, Rosen::DmErrorCode wret, const std::string& errMessage, napi_value result[], int count)
{
    if (count != 2 || result == nullptr) { // input param number is 2
        TLOGI(WmsLogTag::DMS, "Error, input param number must be 2");
        return;
    }
    napi_value code = nullptr;
    napi_value message = nullptr;
    napi_create_int32(env, static_cast<int32_t>(wret), &code);
    napi_create_string_utf8(env, errMessage.c_str(), strlen(errMessage.c_str()), &message);
    napi_create_error(env, code, message, &result[0]);
    napi_get_undefined(env, &result[1]);
}

void ProcessPromise(napi_env env, Rosen::DmErrorCode wret, napi_deferred deferred, napi_value result[], int count)
{
    if (count != 2 || result == nullptr) { // input param number is 2
        TLOGI(WmsLogTag::DMS, "Error, input param number must be 2");
        return;
    }
    TLOGI(WmsLogTag::DMS, "AsyncProcess: Promise");
    if (wret == Rosen::DmErrorCode::DM_OK) {
        TLOGI(WmsLogTag::DMS, "AsyncProcess: Promise resolve");
        napi_resolve_deferred(env, deferred, result[1]);
    } else {
        TLOGI(WmsLogTag::DMS, "AsyncProcess: Promise reject");
        napi_reject_deferred(env, deferred, result[0]);
    }
}

void ProcessCallback(napi_env env, napi_ref ref, napi_value result[], int count)
{
    if (count != 2 || result == nullptr) { // input param number is 2
        TLOGI(WmsLogTag::DMS, "Error, input param number must be 2");
        return;
    }
    TLOGI(WmsLogTag::DMS, "AsyncProcess Callback");
    napi_value callback = nullptr;
    napi_get_reference_value(env, ref, &callback);
    napi_call_function(env, nullptr, callback, 2, result, nullptr); // 2: callback func input number
    napi_delete_reference(env, ref);
}

bool NAPICall(napi_env env, napi_status status)
{
    if (status == napi_ok) {
        return true;
    }

    const napi_extended_error_info *errorInfo = nullptr;
    bool isPending = false;
    napi_get_last_error_info(env, &errorInfo);
    napi_is_exception_pending(env, &isPending);
    if (!isPending && errorInfo != nullptr) {
        const char *errorMessage =
            errorInfo->error_message != nullptr ? errorInfo->error_message : "empty error message";
        napi_throw_error(env, nullptr, errorMessage);
    }

    return false;
}
} // namespace OHOS

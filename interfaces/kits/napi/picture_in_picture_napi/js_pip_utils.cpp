/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "js_pip_utils.h"

#include <string>
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiThrowInvalidParam(napi_env env)
{
    napi_throw(env, AbilityRuntime::CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
    return NapiGetUndefined(env);
}

bool NapiIsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

napi_valuetype GetType(napi_env env, napi_value value)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, value, &res);
    return res;
}

static napi_status SetNamedProperty(napi_env env, napi_value& obj, const std::string& name, int32_t value)
{
    napi_value property = nullptr;
    napi_status status = napi_create_int32(env, value, &property);
    if (status != napi_ok) {
        return status;
    }
    status = napi_set_named_property(env, obj, name.c_str(), property);
    if (status != napi_ok) {
        return status;
    }
    return status;
}

static napi_value ExportPictureInPictureTemplateType(napi_env env)
{
    napi_value result = nullptr;
    napi_create_object(env, &result);
    (void)SetNamedProperty(env, result, "VIDEO_PLAY", static_cast<int32_t>(PiPTemplateType::VIDEO_PLAY));
    (void)SetNamedProperty(env, result, "VIDEO_CALL", static_cast<int32_t>(PiPTemplateType::VIDEO_CALL));
    (void)SetNamedProperty(env, result, "VIDEO_MEETING", static_cast<int32_t>(PiPTemplateType::VIDEO_MEETING));
    (void)SetNamedProperty(env, result, "VIDEO_LIVE", static_cast<int32_t>(PiPTemplateType::VIDEO_LIVE));
    napi_object_freeze(env, result);
    return result;
}

static napi_value ExportPictureInPictureState(napi_env env)
{
    napi_value result = nullptr;
    napi_create_object(env, &result);
    (void)SetNamedProperty(env, result, "ABOUT_TO_START", static_cast<int32_t>(PiPState::ABOUT_TO_START));
    (void)SetNamedProperty(env, result, "STARTED", static_cast<int32_t>(PiPState::STARTED));
    (void)SetNamedProperty(env, result, "ABOUT_TO_STOP", static_cast<int32_t>(PiPState::ABOUT_TO_STOP));
    (void)SetNamedProperty(env, result, "STOPPED", static_cast<int32_t>(PiPState::STOPPED));
    (void)SetNamedProperty(env, result, "ABOUT_TO_RESTORE", static_cast<int32_t>(PiPState::ABOUT_TO_RESTORE));
    (void)SetNamedProperty(env, result, "ERROR", static_cast<int32_t>(PiPState::ERROR));
    napi_object_freeze(env, result);
    return result;
}

static napi_value ExportPictureInPictureControlGroup(napi_env env)
{
    napi_value result = nullptr;
    napi_create_object(env, &result);
    (void)SetNamedProperty(env, result, "VIDEO_PREVIOUS_NEXT", static_cast<int32_t>(PiPControlGroup::VIDEO_PREVIOUS_NEXT));
    (void)SetNamedProperty(env, result, "FAST_FORWARD_BACKWARD", static_cast<int32_t>(PiPControlGroup::FAST_FORWARD_BACKWARD));
    (void)SetNamedProperty(env, result, "MICROPHONE_SWITCH", static_cast<int32_t>(PiPControlGroup::MICROPHONE_SWITCH));
    (void)SetNamedProperty(env, result, "HANG_UP_BUTTON", static_cast<int32_t>(PiPControlGroup::HANG_UP_BUTTON));
    (void)SetNamedProperty(env, result, "CAMERA_SWITCH", static_cast<int32_t>(PiPControlGroup::CAMERA_SWITCH));
    (void)SetNamedProperty(env, result, "MUTE_SWITCH", static_cast<int32_t>(PiPControlGroup::MUTE_SWITCH));
    napi_object_freeze(env, result);
    return result;
}

napi_status InitEnums(napi_env env, napi_value exports)
{
    const napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("PiPTemplateType", ExportPictureInPictureTemplateType(env)),
        DECLARE_NAPI_PROPERTY("PiPState", ExportPictureInPictureState(env)),
        DECLARE_NAPI_PROPERTY("PiPControlGroup", ExportPictureInPictureControlGroup(env)),
    };
    size_t count = sizeof(properties) / sizeof(napi_property_descriptor);
    return napi_define_properties(env, exports, count, properties);
}
} // Rosen
} // OHOS
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

#include "ui_effect_controller_common.h"
constexpr int32_t POINTER_VALID = 1;

namespace OHOS::Rosen {
template <typename T>
bool g_writeParcelableForEffect(std::shared_ptr<T> rsObject, Parcel& parcel)
{
    if (rsObject == nullptr) {
        return parcel.WriteInt32(0);
    }
    if (!parcel.WriteInt32(POINTER_VALID)) {
        return false;
    }
    return rsObject->Marshalling(parcel);
}

template <typename T>
bool g_readParcelableForEffect(std::shared_ptr<T>& rsObject, Parcel& parcel)
{
    int32_t hasPtr = 0;
    if (!parcel.ReadInt32(hasPtr)) {
        return false;
    }
    if (hasPtr == POINTER_VALID) {
        if (!T::Unmarshalling(parcel, rsObject)) {
            return false;
        }
        if (rsObject == nullptr) {
            return false;
        }
    }
    return true;
}

bool UIEffectParams::Marshalling(Parcel& parcel) const
{
    if (!g_writeParcelableForEffect<Filter>(backgroundFilter_, parcel)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "write background filter failed");
        return false;
    }
    if (!g_writeParcelableForEffect<VisualEffect>(effect_, parcel)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "write visual effect failed");
        return false;
    }
    return true;
}

UIEffectParams* UIEffectParams::Unmarshalling(Parcel& parcel)
{
    UIEffectParams* param = new UIEffectParams();
    if (!g_readParcelableForEffect(param->backgroundFilter_, parcel)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "read background filter failed");
        delete param;
        return nullptr;
    }
    if (!g_readParcelableForEffect(param->effect_, parcel)) {
        TLOGE(WmsLogTag::WMS_ANIMATION, "read visual effect failed");
        delete param;
        return nullptr;
    }
    return param;
}

napi_status UIEffectParams::ConvertToJsValue(napi_env env, napi_value& final_value)
{
    NAPI_CHECK(napi_create_object(env, &final_value), "create obj failed");
    if (backgroundFilter_ != nullptr) {
        napi_value jsFilter = nullptr;
        NAPI_CHECK(napi_create_object(env, &jsFilter), "create jsFilter failed");
        Filter* backgroundFilterCopy = new Filter(*backgroundFilter_);
        napi_status status = napi_wrap(env, jsFilter, backgroundFilterCopy, WrapObjectDestructor<Filter>,
            nullptr, nullptr);
        if (status != napi_status::napi_ok) {
            delete backgroundFilterCopy;
            TLOGE(WmsLogTag::WMS_ANIMATION, "create js filter failed, reason: %{public}d", status);
            return status;
        }
        NAPI_CHECK(napi_set_named_property(env, final_value, "backgroundFilter", jsFilter), "backgroundFilter");
    }
    if (effect_ != nullptr) {
        napi_value jsEffect = nullptr;
        NAPI_CHECK(napi_create_object(env, &jsEffect), "create jsEffect failed");
        VisualEffect* effectCopy = new VisualEffect(*effect_);
        napi_status status = napi_wrap(env, jsEffect, effectCopy, WrapObjectDestructor<VisualEffect>, nullptr, nullptr);
        if (status != napi_status::napi_ok) {
            delete effectCopy;
            TLOGE(WmsLogTag::WMS_ANIMATION, "create js effect failed, reason: %{public}d", status);
            return status;
        }
        NAPI_CHECK(napi_set_named_property(env, final_value, "visualEffect", jsEffect), "visualEffect");
    }
    return napi_status::napi_ok;
}

napi_status UIEffectParams::ConvertFromJsValue(napi_env env, napi_value val)
{
    bool hasProperty = false;
    NAPI_CHECK(napi_has_named_property(env, val, "backgroundFilter", &hasProperty), "has backgroundFilter");
    if (hasProperty) {
        backgroundFilter_ = nullptr;
        napi_value jsBackgroundFilter = nullptr;
        NAPI_CHECK(napi_get_named_property(env, val, "backgroundFilter", &jsBackgroundFilter), "filter");
        Filter* filterObj = nullptr;
        NAPI_CHECK(napi_unwrap(env, jsBackgroundFilter, reinterpret_cast<void**>(&filterObj)), "filter unwrap");
        if (filterObj != nullptr) {
            backgroundFilter_ = std::make_shared<Filter>(*filterObj);
        }
    }
    NAPI_CHECK(napi_has_named_property(env, val, "visualEffect", &hasProperty), "has effect");
    if (hasProperty) {
        effect_ = nullptr;
        napi_value jsEffect = nullptr;
        NAPI_CHECK(napi_get_named_property(env, val, "visualEffect", &jsEffect), "effect");
        VisualEffect* effectObj = nullptr;
        NAPI_CHECK(napi_unwrap(env, jsEffect, reinterpret_cast<void**>(&effectObj)), "effect unwrap");
        if (effectObj != nullptr) {
            effect_ = std::make_shared<VisualEffect>(*effectObj);
        }
    }
    return napi_status::napi_ok;
}
}
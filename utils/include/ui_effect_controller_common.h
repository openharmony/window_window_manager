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

#ifndef OHOS_ROSEN_UI_EFFECT_CONTROLLER_COMMON_H
#define OHOS_ROSEN_UI_EFFECT_CONTROLLER_COMMON_H
#include <message_parcel.h>
#include <parcel.h>

#include "ui_effect/effect/include/visual_effect.h"
#include "ui_effect/filter/include/filter.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
#define NAPI_CHECK(func, msg)                                                                           \
    do {                                                                                                \
        napi_status status = func;                                                                      \
        if (status != napi_status::napi_ok) {                                                           \
            TLOGE(WmsLogTag::DEFAULT, "Failed with reason %{public}s, code:%{public}d", msg, status);   \
            return status;                                                                              \
        }                                                                                               \
    } while (0)

template<typename T>
void WrapObjectDestructor(napi_env env, void* nativeObject, void*)
{
    T* obj = static_cast<T*>(nativeObject);
    if (obj) {
        delete obj;
    }
}

const int32_t UIEFFECT_INVALID_ID = -1;
class UIEffectParams : public Parcelable {
public:
    bool Marshalling(Parcel& parcel) const override;
    static UIEffectParams* Unmarshalling(Parcel& parcel);

    bool IsEmpty()
    {
        return backgroundFilter_ == nullptr && effect_ == nullptr;
    }

    napi_status ConvertToJsValue(napi_env env, napi_value& val);
    napi_status ConvertFromJsValue(napi_env env, napi_value val);

private:
    std::shared_ptr<Filter> backgroundFilter_ = nullptr;
    std::shared_ptr<VisualEffect> effect_ = nullptr;
};
} // namespace OHOS::Rosen
#endif
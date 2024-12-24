/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "window_manager_hilog.h"
#include "extension_manager.h"

namespace OHOS {
namespace Rosen {
ExtensionManager::ExtensionManager()
{
}

ExtensionManager::~ExtensionManager()
{
}

void ExtensionManager::Finalizer(napi_en env, void* data, void* hint)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "Finalizer");
    std::unique_ptr<ExtensionManager>(static_cast<ExtensionManager*>(data));
}

static napi_status SetNamedProperty(napi_env env, napi_value& obj, const std::string& name, int32_t value)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "called");
    napi_value property = nullptr;
    napi_status status = napi_create_int32(env, value, &property);
    if (status != napi_ok) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to call napi_create_int32");
        return status;
    }
    status = napi_set_named_property(env, obj, name.c_str(), property);
    if (status != napi_ok) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to call napi_set_named_property");
        return status;
    }
    return status;
}

static napi_value ExportRectChangeReasonType(napi_en env)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "called");
    napi_value result = nullptr;
    napi_create_object(env, &result);
    if (result == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Failed to get object");
        return nullptr;
    }

    (void)SetNamedProperty(env, result, "HOST_WINDOW_RECT_CHANGE", static_cast<int32_t>(1));

    napi_object_freeze(env, result);
    return result;
}

napi_value ExtensionModuleInit(napi_env env, napi_value exportObj)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "called");
    if (env == nullptr || exportObj == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "env or exportObj is nullptr");
        return nullptr;
    }
    std::unique_ptr<ExtensionManager> extensionManager = std::make_unique<ExtensionManager>();
    napi_wrap(env, exportObj, extensionManager.release(), ExtensionManager::Finalizer, nullptr, nullptr);
    
    napi_set_named_property(env, exportObj, "RectChangeReason", ExportRectChangeReasonType(env));

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}
} // namespace Rosen
} // namespace OHOS
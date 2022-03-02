/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "js_display.h"

#include <cinttypes>
#include <map>
#include "display.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsDisplay"};
}

static std::map<DisplayId, std::shared_ptr<NativeReference>> g_JsDisplayMap;
std::recursive_mutex g_mutex;

JsDisplay::JsDisplay(const sptr<Display>& display) : display_(display)
{
}

JsDisplay::~JsDisplay()
{
    WLOGFI("JsDisplay::~JsDisplay is called");
}

void JsDisplay::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsDisplay::Finalizer is called");
    auto jsDisplay = std::unique_ptr<JsDisplay>(static_cast<JsDisplay*>(data));
    if (jsDisplay == nullptr) {
        WLOGFE("JsDisplay::Finalizer jsDisplay is null");
        return;
    }
    sptr<Display> display = jsDisplay->display_;
    if (display == nullptr) {
        WLOGFE("JsDisplay::Finalizer display is null");
        return;
    }
    DisplayId displayId = display->GetId();
    WLOGFI("JsDisplay::Finalizer displayId : %{public}" PRIu64"", displayId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsDisplayMap.find(displayId) != g_JsDisplayMap.end()) {
        WLOGFI("JsDisplay::Finalizer Display is destroyed: %{public}" PRIu64"", displayId);
        g_JsDisplayMap.erase(displayId);
    }
}

NativeValue* CreateJsDisplayObject(NativeEngine& engine, sptr<Display>& display)
{
    WLOGFI("JsDisplay::CreateJsDisplay is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    std::unique_ptr<JsDisplay> jsDisplay = std::make_unique<JsDisplay>(display);
    object->SetNativePointer(jsDisplay.release(), JsDisplay::Finalizer, nullptr);

    object->SetProperty("id", CreateJsValue(engine, static_cast<uint32_t>(display->GetId())));
    object->SetProperty("width", CreateJsValue(engine, display->GetWidth()));
    object->SetProperty("height", CreateJsValue(engine, display->GetHeight()));
    object->SetProperty("refreshRate", CreateJsValue(engine, display->GetFreshRate()));
    object->SetProperty("name", engine.CreateUndefined());
    object->SetProperty("alive", engine.CreateUndefined());
    object->SetProperty("state", engine.CreateUndefined());
    object->SetProperty("rotation", engine.CreateUndefined());
    object->SetProperty("densityDPI", engine.CreateUndefined());
    object->SetProperty("densityPixels", engine.CreateUndefined());
    object->SetProperty("scaledDensity", engine.CreateUndefined());
    object->SetProperty("xDPI", engine.CreateUndefined());
    object->SetProperty("yDPI", engine.CreateUndefined());

    std::shared_ptr<NativeReference> jsDisplayRef;
    jsDisplayRef.reset(engine.CreateReference(objValue, 1));
    DisplayId displayId = display->GetId();
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_JsDisplayMap[displayId] = jsDisplayRef;
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS

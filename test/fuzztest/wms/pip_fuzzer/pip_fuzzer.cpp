/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "marshalling_helper.h"
#include <securec.h>
#include <iremote_broker.h>
#include "ability_context.h"
#include "ability_context_impl.h"
#include "pip_fuzzer.h"
#include "picture_in_picture_controller.h"
#include "picture_in_picture_manager.h"
#include "picture_in_picture_option.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    std::string name = "PipFuzzTest";
    size_t startPos = 0;
    sptr<PipOption> option = new PipOption();
    AbilityRuntime::Context* context = nullptr;
    startPos += GetObject(context, data + startPos, size - startPos);
    option->SetContext(static_cast<void*>(context));
    std::string navigationId = "nav_id";
    option->SetNavigationId(navigationId);
    std::shared_ptr<XComponentController> xComponentController;
    startPos += GetObject(xComponentController, data + startPos, size - startPos);
    option->SetXComponentController(xComponentController);
    uint32_t templateType = 0;
    startPos += GetObject<uint32_t>(templateType, data + startPos, size - startPos);
    option->SetPipTemplate(templateType);
    uint32_t height = 0;
    startPos += GetObject<uint32_t>(height, data + startPos, size - startPos);
    uint32_t width = 0;
    startPos += GetObject<uint32_t>(width, data + startPos, size - startPos);
    option->SetContentSize(width, height);
    int32_t windowId;
    startPos += GetObject(windowId, data + startPos, size - startPos);
    napi_env env = nullptr;
    startPos += GetObject(env, data + startPos, size - startPos);
    sptr<Window> window = new Window();
    sptr<PictureInPictureController> controller = new PictureInPictureController(option, window, windowId, env);
    if (controller == nullptr) {
        return false;
    }
    bool boolVal = false;
    startPos += GetObject(boolVal, data + startPos, size - startPos);
    controller->SetAutoStartEnabled(boolVal);
    uint32_t w = 0;
    startPos += GetObject<uint32_t>(w, data + startPos, size - startPos);
    uint32_t h = 0;
    startPos += GetObject<uint32_t>(h, data + startPos, size - startPos);
    controller->UpdateContentSize(w, h);
    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
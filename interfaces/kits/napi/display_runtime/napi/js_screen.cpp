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

#include "js_screen.h"

#include "screen.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsScreen"};
}

static std::map<ScreenId, std::shared_ptr<NativeReference>> g_JsScreenMap;
std::recursive_mutex g_mutex;

JsScreen::JsScreen(const sptr<Screen>& screen) : screen_(screen)
{
}

JsScreen::~JsScreen()
{
    WLOGFI("JsScreen::~JsScreen is called");
}

void JsScreen::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsScreen::Finalizer is called");
    auto jsScreen = std::unique_ptr<JsScreen>(static_cast<JsScreen*>(data));
    sptr<Screen> screen = jsScreen->screen_;
    if (screen == nullptr) {
        WLOGFE("JsScreen::Finalizer screen is null");
        return;
    }
    ScreenId screenId = screen->GetId();
    WLOGFI("JsScreen::Finalizer screenId : %{public}" PRIu64"", screenId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsScreenMap.find(screenId) != g_JsScreenMap.end()) {
        WLOGFI("JsScreen::Finalizer screen is destroyed: %{public}" PRIu64"", screenId);
        g_JsScreenMap.erase(screenId);
    }
}

NativeValue* JsScreen::RequestRotation(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(engine, info);
    return (me != nullptr) ? me->OnRequestRotation(*engine, *info) : nullptr;
}

NativeValue* JsScreen::OnRequestRotation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreen::OnRequestRotation is called");

    AsyncTask::CompleteCallback complete =
        [this, &info](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (info.argc != ARGC_ONE) {
                WLOGFE("Params not match");
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM),
                                                  "JsScreen::OnRequestRotation failed."));
                return;
            }

            Rotation rotation;
            if (!ConvertFromJsValue(engine, info.argv[0], rotation)) {
                WLOGFE("Failed to convert parameter to rotation");
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM),
                                                  "JsScreen::OnRequestRotation failed."));
                return;
            }
            if (rotation != Rotation::ROTATION_0 &&
                rotation != Rotation::ROTATION_90 &&
                rotation != Rotation::ROTATION_180 &&
                rotation != Rotation::ROTATION_270) {
                WLOGE("Rotation param error! rotation value must from enum Rotation");
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM),
                                                  "JsScreen::OnRequestRotation failed."));
                return;
            }

            bool res = screen_->RequestRotation(rotation);
            if (res) {
                task.Resolve(engine, CreateJsValue(engine, true));
                WLOGFI("JsScreen::OnRequestRotation success");
            } else {
                task.Resolve(engine, CreateJsError(engine, false, "JsScreen::OnRequestRotation failed."));
                WLOGFE("JsScreen::OnRequestRotation failed");
            }
        };
    NativeValue* lastParam = nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}


NativeValue* JsScreen::SetScreenActiveMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsScreen::SetScreenActiveMode is called");
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(engine, info);
    return (me != nullptr) ? me->OnSetScreenActiveMode(*engine, *info) : nullptr;
}

NativeValue* JsScreen::OnSetScreenActiveMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreen::OnSetScreenActiveMode is called");
    if (info.argc != ARGC_ONE) {
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }

    uint32_t modeId;
    if (!ConvertFromJsValue(engine, info.argv[0], modeId)) {
        WLOGFE("Failed to convert parameter to modeId");
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete =
        [this, modeId](NativeEngine& engine, AsyncTask& task, int32_t status) {
            bool res = screen_->SetScreenActiveMode(modeId);
            if (res) {
                task.Resolve(engine, CreateJsValue(engine, true));
                WLOGFI("JsScreen::OnSetScreenActiveMode success");
            } else {
                task.Reject(engine, CreateJsError(engine, false, "JsScreen::OnSetScreenActiveMode failed."));
                WLOGFE("JsScreen::OnSetScreenActiveMode failed");
            }
        };
    NativeValue* lastParam = nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}


NativeValue* CreateJsScreenObject(NativeEngine& engine, sptr<Screen>& screen)
{
    WLOGFI("JsScreen::CreateJsScreen is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    std::unique_ptr<JsScreen> jsScreen = std::make_unique<JsScreen>(screen);
    object->SetNativePointer(jsScreen.release(), JsScreen::Finalizer, nullptr);

    object->SetProperty("id", CreateJsValue(engine, static_cast<uint32_t>(screen->GetId())));
    object->SetProperty("parent", CreateJsValue(engine, static_cast<uint32_t>(screen->GetParentId())));
    object->SetProperty("rotation", CreateJsValue(engine, screen->GetRotation()));
    object->SetProperty("activeModeIndex", CreateJsValue(engine, screen->GetModeId()));
    object->SetProperty("supportedModeInfo", CreateJsScreenModeArrayObject(engine, screen->GetSupportedModes()));

    std::shared_ptr<NativeReference> JsScreenRef;
    JsScreenRef.reset(engine.CreateReference(objValue, 1));
    ScreenId screenId = screen->GetId();
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_JsScreenMap[screenId] = JsScreenRef;
    BindNativeFunction(engine, *object, "setScreenActiveMode", JsScreen::SetScreenActiveMode);
    BindNativeFunction(engine, *object, "requestRotation", JsScreen::RequestRotation);
    return objValue;
}

NativeValue* CreateJsScreenModeArrayObject(NativeEngine& engine, std::vector<sptr<SupportedScreenModes>> screenModes)
{
    NativeValue* arrayValue = engine.CreateArray(screenModes.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    size_t i = 0;
    for (auto& mode : screenModes) {
        array->SetElement(i++, CreateJsScreenModeObject(engine, mode));
    }
    return arrayValue;
}

NativeValue* CreateJsScreenModeObject(NativeEngine& engine, sptr<SupportedScreenModes>& mode)
{
    WLOGFI("JsScreen::CreateJsScreenMode is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* optionObject = ConvertNativeValueTo<NativeObject>(objValue);
    if (optionObject == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    uint32_t width = mode->width_;
    uint32_t height = mode->height_;
    uint32_t freshRate = mode->freshRate_;
    optionObject->SetProperty("width", CreateJsValue(engine, width));
    optionObject->SetProperty("height", CreateJsValue(engine, height));
    optionObject->SetProperty("freshRate", CreateJsValue(engine, freshRate));
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS

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

#include <cinttypes>
#include "screen.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsScreen"};
}

static thread_local std::map<ScreenId, std::shared_ptr<NativeReference>> g_JsScreenMap;
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
    if (jsScreen == nullptr) {
        WLOGFE("jsScreen::Finalizer jsScreen is null");
        return;
    }
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

NativeValue* JsScreen::SetOrientation(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(engine, info);
    return (me != nullptr) ? me->OnSetOrientation(*engine, *info) : nullptr;
}

NativeValue* JsScreen::OnSetOrientation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreen::OnSetOrientation is called");
    bool paramValidFlag = true;
    Orientation orientation = Orientation::UNSPECIFIED;
    if (info.argc != ARGC_ONE) {
        WLOGFE("OnSetOrientation Params not match, info argc: %{public}zu", info.argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], orientation)) {
            paramValidFlag = false;
            WLOGFE("Failed to convert parameter to orientation");
        }
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!paramValidFlag) {
                WLOGE("JsScreen::OnSetOrientation paramValidFlag error");
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM),
                                                  "JsScreen::OnSetOrientation failed."));
                return;
            }
            if (orientation < Orientation::BEGIN || orientation > Orientation::END) {
                WLOGE("Orientation param error! orientation value must from enum Orientation");
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM),
                                                  "JsScreen::OnSetOrientation failed."));
                return;
            }

            bool res = screen_->SetOrientation(orientation);
            if (res) {
                task.Resolve(engine, CreateJsValue(engine, true));
                WLOGFI("JsScreen::OnSetOrientation success");
            } else {
                task.Resolve(engine, CreateJsError(engine, false, "JsScreen::OnSetOrientation failed."));
                WLOGFE("JsScreen::OnSetOrientation failed");
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
    bool paramValidFlag = true;
    uint32_t modeId = 0;
    if (info.argc != ARGC_ONE) {
        WLOGFE("OnSetScreenActiveMode Params not match %{public}zu", info.argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], modeId)) {
            WLOGFE("Failed to convert parameter to modeId");
            paramValidFlag = false;
        }
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!paramValidFlag) {
                WLOGFE("JsScreen::OnSetScreenActiveMode paramValidFlag error");
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM),
                                                  "JsScreen::OnSetScreenActiveMode failed."));
                return;
            }
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

    object->SetProperty("parent", CreateJsValue(engine, static_cast<uint32_t>(screen->GetParentId())));
    object->SetProperty("orientation", CreateJsValue(engine, screen->GetOrientation()));
    object->SetProperty("activeModeIndex", CreateJsValue(engine, screen->GetModeId()));
    object->SetProperty("supportedModeInfo", CreateJsScreenModeArrayObject(engine, screen->GetSupportedModes()));

    std::shared_ptr<NativeReference> JsScreenRef;
    JsScreenRef.reset(engine.CreateReference(objValue, 1));
    ScreenId screenId = screen->GetId();
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_JsScreenMap[screenId] = JsScreenRef;
    BindNativeFunction(engine, *object, "setScreenActiveMode", JsScreen::SetScreenActiveMode);
    BindNativeFunction(engine, *object, "setOrientation", JsScreen::SetOrientation);
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
    uint32_t refreshRate = mode->refreshRate_;
    optionObject->SetProperty("width", CreateJsValue(engine, width));
    optionObject->SetProperty("height", CreateJsValue(engine, height));
    optionObject->SetProperty("refreshRate", CreateJsValue(engine, refreshRate));
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS

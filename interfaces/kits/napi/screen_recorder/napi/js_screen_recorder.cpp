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

#include "js_screen_recorder.h"

#include <cinttypes>
#include "screen_manager.h"
#include "screen_recorder.h"
#include "singleton_container.h"
#include "surface_utils.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_ZERO = 0;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsScreenRecorder"};
}

static thread_local std::map<ScreenId, std::shared_ptr<NativeReference>> g_JsScreenRecorderMap;
std::recursive_mutex g_recursive_mutex;

JsVirtualScreenRecorder::JsVirtualScreenRecorder(const sptr<ScreenRecorder>& screenRecorder)
    : screenRecorder_(screenRecorder)
{
}

JsVirtualScreenRecorder::~JsVirtualScreenRecorder()
{
    WLOGFI("JsVirtualScreenRecorder::~JsVirtualScreenRecorder is called");
}

void JsVirtualScreenRecorder::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsVirtualScreenRecorder::Finalizer is called");
    auto jsVirtualScreenRecorder =
        std::unique_ptr<JsVirtualScreenRecorder>(static_cast<JsVirtualScreenRecorder*>(data));
    if (jsVirtualScreenRecorder == nullptr) {
        WLOGFE("JsVirtualScreenRecorder::Finalizer jsVirtualScreenRecorder is null");
        return;
    }
    sptr<ScreenRecorder> screenRecorder = jsVirtualScreenRecorder->screenRecorder_;
    if (screenRecorder == nullptr) {
        WLOGFE("JsScreenRecorder::Finalizer screen is null");
        return;
    }
    auto surface = screenRecorder->GetInputSurface();
    if (surface != nullptr) {
        auto uniqueId = surface->GetUniqueId();
        SurfaceError res = SurfaceUtils::GetInstance()->Remove(uniqueId);
        if (res != GSERROR_OK) {
            WLOGFI("JsVirtualScreenRecorder::Finalizer Remove uniqueId: %{public}" PRIu64" failed", uniqueId);
        }
    }
    ScreenId screenId = screenRecorder->GetId();
    WLOGFI("JsVirtualScreenRecorder::Finalizer screenId : %{public}" PRIu64"", screenId);
    std::lock_guard<std::recursive_mutex> lock(g_recursive_mutex);
    if (g_JsScreenRecorderMap.find(screenId) != g_JsScreenRecorderMap.end()) {
        WLOGFI("JsVirtualScreenRecorder::Finalizer screen is destroyed: %{public}" PRIu64"", screenId);
        g_JsScreenRecorderMap.erase(screenId);
    }
}

NativeValue* JsVirtualScreenRecorder::GetInputSurface(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsVirtualScreenRecorder* me = CheckParamsAndGetThis<JsVirtualScreenRecorder>(engine, info);
    return (me != nullptr) ? me->OnGetInputSurface(*engine, *info) : nullptr;
}

NativeValue* JsVirtualScreenRecorder::OnGetInputSurface(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenRecorder::OnGetInputSurface is called");
    DMError errCode = DMError::DM_OK;
    if (info.argc != ARGC_ONE && info.argc != ARGC_ZERO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }
    AsyncTask::CompleteCallback complete =
        [errCode, this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK || screenRecorder_ == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode),
                    "JsScreenRecorder::OnGetInputSurface, Invalidate params."));
                WLOGFE("JsScreenRecorder::OnGetInputSurface failed, Invalidate params.");
            } else {
                auto surface = screenRecorder_->GetInputSurface();
                if (surface == nullptr) {
                    task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_NULLPTR),
                        "JsScreenRecorder::OnGetInputSurface, surface is nullptr."));
                    WLOGFE("JsScreenRecorder::OnGetInputSurface failed, surface is nullptr.");
                    return;
                }
                auto uniqueId = surface->GetUniqueId();
                SurfaceError res = SurfaceUtils::GetInstance()->Add(uniqueId, surface);
                if (res != GSERROR_OK) {
                    task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_UNKNOWN),
                        "JsScreenRecorder::OnGetInputSurface, Add surface failed."));
                    WLOGFE("JsScreenRecorder::OnGetInputSurface failed, Add surface failed.");
                    return;
                }
                auto surfaceId = std::to_string(uniqueId);
                task.Resolve(engine, CreateJsValue(engine, surfaceId));
                WLOGFI("JsScreenRecorder::OnGetInputSurface success");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_ONE && info.argv[ARGC_ONE - 1] != nullptr &&
        info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_ONE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsVirtualScreenRecorder::OnGetInputSurface",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsVirtualScreenRecorder::Start(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("Start is called");
    JsVirtualScreenRecorder* me = CheckParamsAndGetThis<JsVirtualScreenRecorder>(engine, info);
    return (me != nullptr) ? me->OnStart(*engine, *info) : nullptr;
}

std::vector<int> GetFds(NativeEngine& engine, DMError errCode, NativeArray* array)
{
    std::vector<int> fds;
    if (errCode != DMError::DM_ERROR_INVALID_PARAM) {
        uint32_t size = array->GetLength();
        for (uint32_t i = 0; i < size; i++) {
            int fd;
            NativeValue* value = array->GetElement(i);
            if (!ConvertFromJsValue(engine, value, fd)) {
                WLOGFE("Failed to convert parameter to fd");
                continue;
            }
            fds.emplace_back(fd);
        }
    }
    return fds;
}

NativeValue* JsVirtualScreenRecorder::OnStart(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenRecorder::OnStart is called");
    DMError errCode = DMError::DM_OK;
    std::vector<int> fds;
    if (info.argc != ARGC_ONE && info.argc != ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    } else {
        NativeArray* array = ConvertNativeValueTo<NativeArray>(info.argv[ARGC_ZERO]);
        if (array == nullptr) {
            WLOGFE("Failed to get fds");
            errCode = DMError::DM_ERROR_INVALID_PARAM;
        }
        fds = GetFds(engine, errCode, array);
    }
    AsyncTask::CompleteCallback complete =
        [errCode, fds, this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK || screenRecorder_ == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode),
                    "JsScreenRecorder::OnStart, Invalidate params."));
                WLOGFE("JsScreenRecorder::OnStart failed, Invalidate params.");
            } else {
                auto res = screenRecorder_->Start(fds);
                if (res != DMError::DM_OK) {
                    task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(res),
                        "JsScreenRecorder::OnStart failed."));
                    return;
                }
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsScreenRecorder::OnStart success");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsVirtualScreenRecorder::OnStart",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsVirtualScreenRecorder::Stop(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("Stop is called");
    JsVirtualScreenRecorder* me = CheckParamsAndGetThis<JsVirtualScreenRecorder>(engine, info);
    return (me != nullptr) ? me->OnStop(*engine, *info) : nullptr;
}

NativeValue* JsVirtualScreenRecorder::OnStop(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenRecorder::OnStop is called");
    DMError errCode = DMError::DM_OK;
    if (info.argc != ARGC_ONE && info.argc != ARGC_ZERO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }
    AsyncTask::CompleteCallback complete =
        [errCode, this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK || screenRecorder_ == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode),
                    "JsScreenRecorder::OnStop, Invalidate params."));
                WLOGFE("JsScreenRecorder::OnStop failed, Invalidate params.");
            } else {
                auto res = screenRecorder_->Stop();
                if (res != DMError::DM_OK) {
                    task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(res),
                        "JsScreenRecorder::OnStop failed."));
                    return;
                }
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsScreenRecorder::OnStop success");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_ONE && info.argv[ARGC_ONE - 1] != nullptr &&
        info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_ONE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsVirtualScreenRecorder::OnStop",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* CreateJsVirtualScreenRecorderObject(NativeEngine& engine, sptr<ScreenRecorder>& screenRecorder)
{
    WLOGFI("JsScreenRecorder::CreateJsScreenRecorderObject is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    std::unique_ptr<JsVirtualScreenRecorder> jsScreen = std::make_unique<JsVirtualScreenRecorder>(screenRecorder);
    object->SetNativePointer(jsScreen.release(), JsVirtualScreenRecorder::Finalizer, nullptr);
    ScreenId screenId = screenRecorder->GetId();
    object->SetProperty("id",
        CreateJsValue(engine, screenId == SCREEN_ID_INVALID ? -1 : static_cast<int64_t>(screenId)));

    std::shared_ptr<NativeReference> JsScreenRef;
    JsScreenRef.reset(engine.CreateReference(objValue, 1));
    std::lock_guard<std::recursive_mutex> lock(g_recursive_mutex);
    g_JsScreenRecorderMap[screenId] = JsScreenRef;
    BindNativeFunction(engine, *object, "getInputSurface", JsVirtualScreenRecorder::GetInputSurface);
    BindNativeFunction(engine, *object, "start", JsVirtualScreenRecorder::Start);
    BindNativeFunction(engine, *object, "stop", JsVirtualScreenRecorder::Stop);
    return objValue;
}

void JsScreenRecorder::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsScreenRecorder::Finalizer is called");
    std::unique_ptr<JsScreenRecorder>(static_cast<JsScreenRecorder*>(data));
}

NativeValue* JsScreenRecorder::GetVirtualScreenRecorder(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("GetVirtualScreenRecorder is called");
    JsScreenRecorder* me = CheckParamsAndGetThis<JsScreenRecorder>(engine, info);
    return (me != nullptr) ? me->OnGetVirtualScreenRecorder(*engine, *info) : nullptr;
}

NativeValue* JsScreenRecorder::OnGetVirtualScreenRecorder(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenRecorder::OnGetVirtualScreenRecorder is called");
    DMError errCode = DMError::DM_OK;
    int64_t screenId = -1LL;
    if (info.argc != ARGC_ONE && info.argc != ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], screenId) || screenId == -1LL) {
            WLOGFE("Failed to convert parameter to screenId");
            errCode = DMError::DM_ERROR_INVALID_PARAM;
        }
    }
    AsyncTask::CompleteCallback complete =
        [errCode, screenId](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode),
                    "JsScreenRecorder::OnGetVirtualScreenRecorder, Invalidate params."));
                WLOGFE("JsScreenRecorder::OnGetVirtualScreenRecorder failed, Invalidate params.");
            } else {
                auto screen = SingletonContainer::Get<ScreenManager>().GetScreenById(static_cast<ScreenId>(screenId));
                if (screen == nullptr) {
                    task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_NULLPTR),
                        "JsScreenRecorder::OnGetVirtualScreenRecorder, screen is null."));
                    return;
                }
                sptr<ScreenRecorder> screenRecorder = new(std::nothrow) ScreenRecorder(screen);
                if (screenRecorder == nullptr) {
                    task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DMError::DM_ERROR_NULLPTR),
                        "JsScreenRecorder::OnGetVirtualScreenRecorder, screenRecorder is null."));
                    return;
                }
                task.Resolve(engine, CreateJsVirtualScreenRecorderObject(engine, screenRecorder));
                WLOGFI("JsScreenRecorder::OnGetVirtualScreenRecorder success");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenRecorder::OnGetVirtualScreenRecorder",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsScreenRecorderInit(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGFI("JsScreenManagerInit is called");

    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("JsScreenManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("JsScreenManagerInit object is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsScreenRecorder> jsScreenRecorder = std::make_unique<JsScreenRecorder>(engine);
    object->SetNativePointer(jsScreenRecorder.release(), JsScreenRecorder::Finalizer, nullptr);

    BindNativeFunction(*engine, *object, "getVirtualScreenRecorder", JsScreenRecorder::GetVirtualScreenRecorder);
    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS

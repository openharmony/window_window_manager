/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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


#include "js_rss_session.h"

#include <functional>
#include <js_runtime_utils.h>

#include "js_scene_utils.h"
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
#include "res_sched_client.h"
#include "res_type.h"
#endif
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
using namespace ResourceSchedule;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsRssSession" };
static constexpr size_t ARG_COUNT_ONE = 1;
static constexpr size_t ARG_COUNT_TWO = 2;
static constexpr int32_t INDENT = -1;
} // namespace

using OnRssEventCb = std::function<void(napi_env, napi_value, int32_t,
    std::unordered_map<std::string, std::string>)>;

struct CallBackContext {
    napi_env env = nullptr;
    std::shared_ptr<NativeReference> callbackRef = nullptr;
    OnRssEventCb eventCb = nullptr;
    int32_t eventType = 0;
    std::unordered_map<std::string, std::string> extInfo;
};

#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
RssEventListener::RssEventListener(napi_env env, napi_value callbackObj, OnRssEventCb callback)
    : napiEnv_(env), eventCb_(callback)
{
    if (napiEnv_ == nullptr || callbackObj == nullptr) {
        return;
    }
    napi_ref tmpRef = nullptr;
    napi_create_reference(napiEnv_, callbackObj, 1, &tmpRef);
    callbackRef_.reset(reinterpret_cast<NativeReference*>(tmpRef));
    napi_value callbackWorkName  = nullptr;
    napi_create_string_utf8(env, "ThreadSafeFunction in SystemloadListener", NAPI_AUTO_LENGTH, &callbackWorkName);
    napi_create_threadsafe_function(env, nullptr, nullptr, callbackWorkName, 0, 1, nullptr, nullptr, nullptr,
        ThreadSafeCallBack, &threadSafeFunction_);
}

void RssEventListener::ThreadSafeCallBack(napi_env ThreadSafeEnv, napi_value js_cb, void* context, void* data)
{
    WLOGFI("RssEventListener ThreadSafeCallBack start");
    CallBackContext* callBackContext = reinterpret_cast<CallBackContext*>(data);
    callBackContext->eventCb(callBackContext->env,
        callBackContext->callbackRef->GetNapiValue(), callBackContext->eventType, callBackContext->extInfo);
    delete callBackContext;
}

void RssEventListener::OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
    std::unordered_map<std::string, std::string> extInfo)
{
    if (napiEnv_ == nullptr || callbackRef_ == nullptr || eventCb_ == nullptr) {
        return;
    }
    CallBackContext* callBackContext = new CallBackContext();
    callBackContext->env = napiEnv_;
    callBackContext->callbackRef = callbackRef_;
    callBackContext->eventType = eventType;
    callBackContext->extInfo = extInfo;
    callBackContext->eventCb = eventCb_;
    napi_acquire_threadsafe_function(threadSafeFunction_);
    napi_call_threadsafe_function(threadSafeFunction_, callBackContext, napi_tsfn_blocking);
}

RssSession& RssSession::GetInstance()
{
    static RssSession session;
    return session;
}

RssSession::~RssSession()
{
    jsCallBackMap_.clear();
}

napi_value RssSession::RegisterRssData(napi_env env, napi_callback_info info)
{
    return GetInstance().RegisterRssDataCallback(env, info);
}

napi_value RssSession::UnregisterRssData(napi_env env, napi_callback_info info)
{
    return GetInstance().UnRegisterRssDataCallback(env, info);
}

template<typename T>
void SetMapValue(napi_env env, const std::string& key, const T& value, napi_value &object)
{
    napi_value keyInfo = nullptr;
    napi_create_string_utf8(env, key.c_str(), NAPI_AUTO_LENGTH, &keyInfo);
    napi_value valueInfo = nullptr;
    if constexpr (std::is_same_v<T, bool>) {
        napi_get_boolean(env, value, &valueInfo);
    } else if constexpr (std::is_same_v<T, std::string>) {
        napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &valueInfo);
    }
    napi_set_property(env, object, keyInfo, valueInfo);
}

napi_value RssSession::DealRssReply(napi_env env, const nlohmann::json& payload, const nlohmann::json& reply)
{
    WLOGFI("[NAPI]DealRssReply");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return NapiGetUndefined(env);
    }

    if (!reply.contains("result") || !reply["result"].is_string() || reply.at("result") == "") {
        WLOGFE("[NAPI]Reply not find result key!");
        return NapiGetUndefined(env);
    }
    std::string result = reply["result"].get<std::string>();
    bool resultVal = result == std::to_string(ResType::HeavyLoadMutexAddReasons::HeavyLoadMutexStatusAddFailByMutex);
    SetMapValue(env, "result", resultVal, objValue);

    std::string detail = "{}";
    if (!reply.contains("mutex") || !reply["mutex"].is_string() || reply.at("mutex") == "") {
        SetMapValue(env, "details", detail, objValue);
    } else {
        std::string mutexStr = reply["mutex"].get<std::string>();
        ParseMutex(mutexStr, payload, detail);
        SetMapValue(env, "details", detail, objValue);
    }
    return objValue;
}

void RssSession::ParseMutex(const std::string& mutexStr, const nlohmann::json& payload, std::string& detailStr)
{
    nlohmann::json root = nlohmann::json::parse(mutexStr, nullptr, false);
    if (root.is_discarded()) {
        WLOGFE("[NAPI] Parse json data failed!");
        return;
    }

    if (!root.is_array()) {
        WLOGFE("[NAPI]Parse json data failed!");
        return;
    }

    std::string bundleName = "";
    if (payload.contains("bundleName") && payload["bundleName"].is_string()) {
        bundleName = payload["bundleName"].get<std::string>();
    }

    nlohmann::json detail;
    detail["appInfo"]["domain"] = "";
    detail["appInfo"]["bundleName"] = bundleName;
    detail["reason"] = nlohmann::json::array();
    for (auto& item : root) {
        nlohmann::json tmp;
        if (item.contains("domain") && item["domain"].is_number()) {
            tmp["domain"] = std::to_string(item["domain"].get<int32_t>());
        } else {
            tmp["domain"] = "";
        }

        if (item.contains("bundlename") && item["bundlename"].is_string()) {
            tmp["bundleName"] = item["bundlename"].get<std::string>();
        } else {
            tmp["bundleName"] = "";
        }
        detail["reason"].push_back(tmp);
    }
    detailStr = detail.dump(INDENT, ' ', false, nlohmann::json::error_handler_t::replace);
}

void RssSession::ParseCallbackMutex(const std::string& mutexStr, std::string& bundleName)
{
    nlohmann::json root = nlohmann::json::parse(mutexStr, nullptr, false);
    if (root.is_discarded()) {
        WLOGFE("[NAPI] Parse json data failed!");
        return;
    }

    if (!root.is_array()) {
        WLOGFE("[NAPI]Parse json data failed!");
        return;
    }

    for (auto& item : root) {
        if (item.contains("bundlename") && item["bundlename"].is_string()) {
            bundleName = item["bundlename"].get<std::string>();
            break;
        }
    }
}

void RssSession::OnReceiveEvent(napi_env env, napi_value callbackObj, int32_t eventType,
    const std::unordered_map<std::string, std::string>& extInfo)
{
    WLOGFI("OnReceiveEvent asyncCallback.");
    std::lock_guard<std::mutex> autoLock(jsCallbackMapLock_);
    if (jsCallBackMap_.find(eventType) == jsCallBackMap_.end()) {
        WLOGFE("OnReceiveEvent cb type has not register yet.");
        return;
    }
    auto& callbackList = jsCallBackMap_[eventType];
    auto iter = callbackList.begin();
    bool isEqual = false;
    for (; iter != callbackList.end(); iter++) {
        NAPI_CALL_RETURN_VOID(env, napi_strict_equals(env, callbackObj, iter->first->GetNapiValue(), &isEqual));
        if (isEqual) {
            break;
        }
    }
    if (!isEqual) {
        WLOGFE("OnReceiveEvent level callback not found in registered array.");
        return;
    }
    std::unique_ptr<RssSessionCbInfo> cbInfo = std::make_unique<RssSessionCbInfo>(env);
    if (cbInfo == nullptr) {
        WLOGFE("OnReceiveEvent cbInfo null.");
        return;
    }
    cbInfo->extInfo = extInfo;
    napi_value resourceName = nullptr;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_latin1(env, "OnReceiveEvent", NAPI_AUTO_LENGTH, &resourceName));

    NAPI_CALL_RETURN_VOID(env,
        napi_create_reference(env, iter->first->GetNapiValue(), 1, &cbInfo->callback));

    NAPI_CALL_RETURN_VOID(env, napi_create_async_work(env, nullptr, resourceName,
        [] (napi_env env, void* data) {},
        CompleteCb,
        static_cast<void *>(cbInfo.get()),
        &cbInfo->asyncWork));
    NAPI_CALL_RETURN_VOID(env, napi_queue_async_work(env, cbInfo->asyncWork));
    cbInfo.release();
    napi_value result = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result));
    WLOGFI("OnReceiveEvent asyncCallback end");
}

napi_value RssSession::RegisterRssDataCallback(napi_env env, napi_callback_info info)
{
    WLOGFD("Regster RssData Callback");
    int32_t eventType;
    napi_value jsCallback = nullptr;

    if (!CheckCallbackParam(env, info, eventType, &jsCallback)) {
        WLOGFE("Register RssData Callback parameter error.");
        return NapiGetUndefined(env);
    }

    napi_ref tempRef = nullptr;
    napi_create_reference(env, jsCallback, 1, &tempRef);
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(reinterpret_cast<NativeReference*>(tempRef));
    std::lock_guard<std::mutex> autoLock(jsCallbackMapLock_);
    if (jsCallBackMap_.find(eventType) == jsCallBackMap_.end()) {
        jsCallBackMap_[eventType] = std::list<CallBackPair>();
    }
    auto& callbackList = jsCallBackMap_[eventType];
    auto iter = callbackList.begin();
    for (; iter != callbackList.end(); iter++) {
        bool isEqual = false;
        napi_strict_equals(env, jsCallback, iter->first->GetNapiValue(), &isEqual);
        if (isEqual) {
            WLOGFW("Register a exist callback type.");
            return NapiGetUndefined(env);
        }
    }
    auto rssDataCb = [](napi_env env, napi_value callbackObj, int32_t eventType,
        std::unordered_map<std::string, std::string> extInfo) {
        RssSession::GetInstance().OnReceiveEvent(env, callbackObj, eventType, extInfo);
    };
    sptr<RssEventListener> eventListener =
        new (std::nothrow) RssEventListener(env, jsCallback, rssDataCb);
    if (eventListener == nullptr) {
        WLOGFE("Register RssData listener nullptr.");
        return NapiGetUndefined(env);
    }
    ResSchedClient::GetInstance().RegisterEventListener(eventListener, eventType);
    callbackList.emplace_back(std::move(callbackRef), eventListener);
    return NapiGetUndefined(env);
}

napi_value RssSession::UnRegisterRssDataCallback(napi_env env, napi_callback_info info)
{
    WLOGFD("UnRegster RssData Callback");
    int32_t eventType;
    napi_value jsCallback = nullptr;

    if (!CheckCallbackParam(env, info, eventType, &jsCallback)) {
        WLOGFE("UnRegister RssData Callback parameter error.");
        return NapiGetUndefined(env);
    }

    std::lock_guard<std::mutex> autoLock(jsCallbackMapLock_);
    if (jsCallBackMap_.find(eventType) == jsCallBackMap_.end()) {
        WLOGFE("unRegister eventType has not registered");
        return NapiGetUndefined(env);
    }
    auto& callbackList = jsCallBackMap_[eventType];
    for (auto iter = callbackList.begin(); iter != callbackList.end(); iter++) {
        bool isEqual = false;
        napi_strict_equals(env, jsCallback, iter->first->GetNapiValue(), &isEqual);
        if (isEqual) {
            ResSchedClient::GetInstance().UnRegisterEventListener(iter->second, eventType);
            callbackList.erase(iter);
            break;
        }
    }
    return NapiGetUndefined(env);
}

void RssSession::CompleteCb(napi_env env, napi_status status, void* data)
{
    WLOGFI("CompleteCb, main event thread complete callback.");
    auto* info = static_cast<RssSessionCbInfo*>(data);
    if (info == nullptr) {
        WLOGFW("Complete cb info is nullptr.");
        return;
    }
    std::unique_ptr<RssSessionCbInfo> cbInfo(info);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result = nullptr;
    napi_create_object(env, &result);
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, cbInfo->callback, &callback));

    std::string appInfo = cbInfo->extInfo["selfBundleName"];
    SetMapValue(env, "appInfo", appInfo, result);

    std::string reason;
    ParseCallbackMutex(cbInfo->extInfo["mutex"], reason);
    SetMapValue(env, "reason", reason, result);

    // call js callback
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, 1, &result, &callResult));
    // delete resources
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, cbInfo->asyncWork));
    cbInfo->asyncWork = nullptr;
    if (cbInfo->callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, cbInfo->callback));
        cbInfo->callback = nullptr;
    }
    WLOGFI("CompleteCb, main event thread complete end.");
}

bool RssSession::CheckCallbackParam(napi_env env, napi_callback_info info,
                                    int32_t &eventType, napi_value *jsCallback)
{
    if (jsCallback == nullptr) {
        WLOGFE("Input callback is nullptr.");
        return false;
    }
    size_t argc = ARG_COUNT_TWO;
    napi_value argv[ARG_COUNT_TWO] = { 0 };
    NAPI_CALL_BASE(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), false);
    if (argc != ARG_COUNT_TWO) {
        WLOGFE("Parameter error. The type of \"number of parameters\" must be 2");
        return false;
    }
    if (!ConvertFromJsValue(env, argv[0], eventType)) {
        WLOGFE("Parameter error. The type of \"type\" must be string");
        return false;
    }

    *jsCallback = argv[ARG_COUNT_ONE];
    if (*jsCallback == nullptr) {
        WLOGFE("listenerObj is nullptr");
        return false;
    }
    bool isCallable = false;
    napi_is_callable(env, *jsCallback, &isCallable);
    if (!isCallable) {
        WLOGFE("Parameter error. The type of \"callback\" must be Callback");
        return false;
    }
    return true;
}
#endif
} // OHOS
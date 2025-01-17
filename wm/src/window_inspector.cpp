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
#include "window_inspector.h"

#include <dlfcn.h>

#include "nlohmann/json.hpp"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr char ARK_CONNECT_LIB_PATH[] = "libark_connect_inspector.z.so";
constexpr char SET_WMS_CALLBACK[] = "SetWMSCallback";
constexpr char SEND_WMS_MESSAGE[] = "SendMessage";
const std::string METHOD_NAME = "WMS.windowList";
const std::string COMMAND_NAME = "getCurrentProcessWindowList";
} // namespace

std::unordered_map<std::string, std::weak_ptr<GetWMSWindowListCallback>> WindowInspector::getWMSWindowListCallbacks_;

sptr<WindowInspector> WindowInspector::CreateInstance()
{
    sptr<WindowInspector> windowInspector = new WindowInspector();
    return windowInspector;
}

WindowInspector& WindowInspector::GetInstance()
{
    static sptr<WindowInspector> instance = CreateInstance();
    return *instance;
}

WindowInspector::WindowInspector() { ConnectServer(); }

WindowInspector::~WindowInspector() { UnregisterAllCallbacks(); }

void WindowInspector::ConnectServer()
{
    handlerConnectServerSo_ = dlopen(ARK_CONNECT_LIB_PATH, RTLD_NOW);
    if (handlerConnectServerSo_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "can't open %{public}s", ARK_CONNECT_LIB_PATH);
        return;
    }
    setWMSCallbackFunc_ = reinterpret_cast<SetWMSCallbackFunc>(dlsym(handlerConnectServerSo_, SET_WMS_CALLBACK));
    sendWMSMessageFunc_ = reinterpret_cast<SendWMSMessageFunc>(dlsym(handlerConnectServerSo_, SEND_WMS_MESSAGE));
    if (setWMSCallbackFunc_ == nullptr || sendWMSMessageFunc_ == nullptr) {
        CloseConnectServer();
        setWMSCallbackFunc_ = nullptr;
        sendWMSMessageFunc_ = nullptr;
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "load failed: %{public}s", dlerror());
        return;
    }
    setWMSCallbackFunc_([this](const char* message) {
        std::string jsonWindowListInfoStr;
        if (ProcessArkUIInspectorMessage(message, jsonWindowListInfoStr)) {
            SendMessageToIDE(jsonWindowListInfoStr);
        }
    });
    isConnectServerSuccess_ = true;
    CloseConnectServer();
}

void WindowInspector::CloseConnectServer()
{
    isConnectServerSuccess_ = false;
    dlclose(handlerConnectServerSo_);
    handlerConnectServerSo_ = nullptr;
}

bool WindowInspector::IsConnectServerSuccess() const { return isConnectServerSuccess_; }

void WindowInspector::RegisterGetWMSWindowListCallback(
    std::string windowName const std::weak_ptr<GetWMSWindowListCallback>& func)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "windowName: %{public}s", windowName.c_str());
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto [_, result] = getWMSWindowListCallbacks_.insert_or_assign(windowName, func);
    if (result) {
        TLOGW(WmsLogTag::WMS_ATTRIBUTE, "callback has registered", windowName.c_str());
    }
}

void WindowInspector::UnregisterGetWMSWindowListCallback(std::string windowName)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "windowName: %{public}s", windowName.c_str());
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto iter = getWMSWindowListCallbacks_.find(windowName);
    if (iter == getWMSWindowListCallbacks_.end()) {
        TLOGW(WmsLogTag::WMS_ATTRIBUTE, "callback not registered", windowName.c_str());
        return;
    }
    getWMSWindowListCallbacks_.erase(iter);
}

void WindowInspector::UnregisterAllCallbacks()
{
    setWMSCallbackFunc_(nullptr);
    setWMSCallbackFunc_ = nullptr;
    sendWMSMessageFunc_ = nullptr;
    std::unique_lock<std::mutex> lock(callbackMutex_);
    getWMSWindowListCallbacks_.clear();
}

bool WindowInspector::ProcessArkUIInspectorMessage(const std::string& message, std::string& jsonStr)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "receive callback");
    nlohmann::json jsonMessage = nlohmann::json::parse(message, nullptr, false);
    if (!jsonMessage.contains("method") || jsonMessage["method"].get<std::string>() != METHOD_NAME) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "received method err");
        return false;
    }
    if (!jsonMessage.contains("params") || !jsonMessage["params"].contains("command") ||
        jsonMessage["params"]["command"].get<std::string>() != COMMAND_NAME) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "received params.command err");
        return false;
    }
    std::vector<WindowListInfo> windowListInfoVec;
    {
        std::unique_lock<std::mutex> lock(callbackMutex_);
        for (auto iter = getWMSWindowListCallbacks_.begin(); iter != getWMSWindowListCallbacks_.end();) {
            auto callback = iter->second.lock();
            if (callback == nullptr) {
                iter = getWMSWindowListCallbacks_.erase(iter);
                continue;
            }
            auto windowListInfo = (*callback)();
            windowListInfoVec.push_back(std::move(windowListInfo));
            iter++;
        }
    }
    if (windowListInfoVec.empty()) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "has no window");
        return false;
    }
    CreateArkUIInspectorJson(windowListInfoVec, jsonStr);
    return true;
}

void WindowInspector::CreateArkUIInspectorJson(
    const std::vector<WindowListInfo>& windowListInfo, std::string& jsonStr)
{
    nlohmann::ordered_json jsonWindowListInfo;
    jsonWindowListInfo["type"] = "window";
    jsonWindowListInfo["content"] = nlohmann::json::array();
    for (const auto& info : windowListInfo) {
        nlohmann::ordered_json jsonInfo;
        jsonInfo["windowName"] = info.windowName;
        jsonInfo["winId"] = std::to_string(info.windowId);
        jsonInfo["type"] = std::to_string(info.windowType);
        nlohmann::ordered_json jsonRectInfo;
        jsonRectInfo["startX"] = std::to_string(info.windowRect.posX_);
        jsonRectInfo["startY"] = std::to_string(info.windowRect.posY_);
        jsonRectInfo["width"] = std::to_string(info.windowRect.width_);
        jsonRectInfo["height"] = std::to_string(info.windowRect.height_);
        jsonInfo["rect"] = jsonRectInfo;

        jsonWindowListInfo["content"].push_back(std::move(jsonInfo));
    }
    jsonStr = jsonWindowListInfo.dump();
}

void WindowInspector::SendMessageToIDE(std::string& jsonStr)
{
    if (sendWMSMessageFunc_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sendMessage is null");
        return;
    }
    sendWMSMessageFunc_(jsonStr);
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "%{public}s", jsonStr.c_str());
}
} // namespace Rosen::OHOS
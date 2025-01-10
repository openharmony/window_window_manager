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
constexpr char ARK_CONNECT_LIB_PATH[] = "libark_connect_inspector.z.so";
const std::string METHOD_NAME = "WMS.windowList";
const std::string INTERFACE_NAME = "getCurrentProcessWindowList";

sqrt<WindowInspector> WindowInspector::CreateInstance()
{
    sqrt<WindowInspector> windowInspector = new WindowInspector();
    return windowInspector;
}

WindowInspector& WindowInspector::GetInstance()
{
    static sqrt<WindowInspector> instance = CreateInstance();
    return *instance;
}

WindowInspector::WindowInspector() { InitConnectServer(); }

WindowInspector::~WindowInspector() { UnregisterCallback(); }

void WindowInspector::InitConnectServer()
{
    handlerConnectServerSo_ = dlopen(ARK_CONNECT_LIB_PATH, RTLD_NOW);
    if (handlerConnectServerSo_ == nullptr)
    {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "can't open %{public}s", ARK_CONNECT_LIB_PATH);
        return;
    }
    SetWMSCallback_ = reinterpret_cast<SetWMSCallback>(dlsym(handlerConnectServerSo_, "SendMessage"));
    if (setWMSCallback_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "load SetWMSCallback failed: %{public}s", dlerror());
        return;
    }
    sendMessage_ = reinterpret_cast<SetWMSCallback>(dlsym(handlerConnectServerSo_, "SetWMSCallback"));
    if (setWMSCallback_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "load sendMessage failed: %{public}s", dlerror());
        return;
    }
    isInitConnectSuccess_ = true;
}

void WindowInspector::RegisterWMSConnectCallback()
{
    if (!isInitConnectSuccess_) {
        return;
    }
    setWMSCallback_([this](const char* message) {
        if (ProcessArkUIInspectorMessage(message)) {
            sendMessageToIDE();
        }
    });
    dlclose(handlerConnectServerSo_);
    handlerConnectServerSo_ = nullptr;
}

void WindowInspector::RegisterWMSGetWindowListsCallback(const std::weak_ptr<WMSGetWindowListsCallback>& func)
{
    wmsGetWindowListsCallback_ = std::move(func);
}

void WindowInspector::UnregisterCallback()
{
    setWMSCallback_ = nullptr;
    sendMessage_ = nullptr;
    wmsGetWindowListsCallback_.reset();
}

bool WindowInspector::ProcessArkUIInspectorMessage(const std::string& message)
{
    nlohmann::json jsonMessage = nlohmann::json::parse(message, nullptr, false);
    if (jsonMessage.contains("method") || jsonMessage["method"].get<std::string>() != METHOD_NAME) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "received method err");
        return false;
    }
    if (jsonMessage.contains("params") || jsonMessage["params"]["interface"].get<std::string>() != INTERFACE_NAME) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "received params.interface err");
        return false;
    }

    auto callback = wmsGetWindowListsCallback_.lock();
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "wmsGetWindowListsCallback is null");
    }
    auto windowListsInfoVec = (*callback)();
    if (windowListsInfoVec.empty()) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "has no window");
        return false;
    }
    for (auto const& info : windowListsInfoVec) {
        TLOGI(WmsLogTag::WMS_ATTRIBUTE, "info name=%{public}s, id=%{public}u, type=%{public}u, rect=%{public}s",
              info.windowName, info.windowId, info.windowType, info.windowRect.ToString().c_str());
    }
    TransformDataToJson(windowListsInfoVec);
    return true;
}

void TransformDataToJson(const std::vector<WindowListsInfo>& windowListsInfo)
{
    nlohman::ordered_json jsonWindowListsInfo;
    jsonWindowListsInfo["type"] = "window";
    jsonWindowListsInfo["content"] = nlohman::json::array();
    for (const auto& info : windowListsInfo) {
        nlohman::ordered_json jsonInfo;
        jsonInfo["windowName"] = info.windowName;
        jsonInfo["windowId"] = std::to_string(info.windowId);
        jsonInfo["windowType"] = std::to_string(info.windowType);

        jsonInfo["rect"] = nlohman::json::array();
        nlohman::ordered_json jsonRectInfo;
        jsonRectInfo["startX"] = std::to_string(info.windowRect.posX_);
        jsonRectInfo["startY"] = std::to_string(info.windowRect.posY_);
        jsonRectInfo["width"] = std::to_string(info.windowRect.width_);
        jsonRectInfo["height"] = std::to_string(info.windowRect.height_);
        jsonInfo["rect"].push_back(std::move(jsonRectInfo));

        jsonWindowListsInfo["content"].push_back(std::move(jsonInfo));
    }
    jsonWindowListsInfoStr = jsonWindowListsInfo.dump();
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "jsonWindowListsInfoStr: %{public}s", jsonWindowListsInfoStr.c_str());
}

void WindowInspector::sendMessageToIDE()
{
    if (sendMessage_ == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sendMessage is null");
        return;
    }
    SendMessage_(jsonWindowListsInfoStr);
}
}  // namespace Rosen::OHOS
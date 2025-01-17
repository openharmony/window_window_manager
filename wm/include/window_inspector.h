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

#ifndef WINDOW_INSPECTOR_H
#define WINDOW_INSPECTOR_H

#include <mutex>

#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
struct WindowListInfo {
    std::string windowName;
    uint32_t windowId;
    uint32_t windowType;
    Rect windowRect;
};
using SendWMSMessageFunc = void (*)(const std::string& message);
using SetWMSCallbackFunc = void (*)(const std::function<void(const char*)>& wmsCallback);
using GetWMSWindowListCallback = std::function<WindowListInfo()>;

class WindowInspector : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowInspector);
public:
    bool IsConnectServerSuccess() const;
    void RegisterGetWMSWindowListCallback(const std::string& windowName,
        std::shared_ptr<GetWMSWindowListCallback>&& func);
    void UnregisterGetWMSWindowListCallback(const std::string& windowName);

protected:
    WindowInspector();
    virtual ~WindowInspector();

private:
    bool isConnectServerSuccess_ = false;
    void* handlerConnectServerSo_ = nullptr;
    SendWMSMessageFunc sendWMSMessageFunc_ = nullptr;
    SetWMSCallbackFunc setWMSCallbackFunc_ = nullptr;

    std::mutex callbackMutex_;
    std::unordered_map<std::string, std::shared_ptr<GetWMSWindowListCallback>> getWMSWindowListCallbacks_;
    // Above guarded by callbackMutex_

    void ConnectServer();
    void CloseConnectFromServer();
    void UnregisterAllCallbacks();
    bool ProcessArkUIInspectorMessage(const std::string& message, std::string& jsonStr);
    void CreateArkUIInspectorJson(const std::vector<WindowListInfo>& windowListInfo, std::string& jsonStr);
    void SendMessageToIDE(std::string& jsonStr);
};
} // namespace OHOS::Rosen

#endif // WINDOW_INSPECTOR_H
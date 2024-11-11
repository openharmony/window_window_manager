/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_CJ_DISPLAY_LISTENER_H
#define OHOS_CJ_DISPLAY_LISTENER_H

#include <cstdint>
#include <mutex>

#include "display_manager.h"
#include "dm_common.h"
#include "native_engine/native_engine.h"
#include "refbase.h"

namespace OHOS {
namespace Rosen {
const std::string EVENT_ADD = "add";
const std::string EVENT_REMOVE = "remove";
const std::string EVENT_CHANGE = "change";
const std::string EVENT_PRIVATE_MODE_CHANGE = "privateModeChange";
const std::string EVENT_FOLD_STATUS_CHANGED = "foldStatusChange";
const std::string EVENT_FOLD_ANGLE_CHANGED = "foldAngleChange";
const std::string EVENT_CAPTURE_STATUS_CHANGED = "captureStatusChange";
const std::string EVENT_DISPLAY_MODE_CHANGED = "foldDisplayModeChange";
const std::string EVENT_AVAILABLE_AREA_CHANGED = "availableAreaChange";

class CJDisplayListener : public DisplayManager::IDisplayListener,
                          public DisplayManager::IPrivateWindowListener,
                          public DisplayManager::IFoldStatusListener,
                          public DisplayManager::IFoldAngleListener,
                          public DisplayManager::ICaptureStatusListener,
                          public DisplayManager::IDisplayModeListener,
                          public DisplayManager::IAvailableAreaListener {
public:
    explicit CJDisplayListener();
    ~CJDisplayListener() override;
    void AddCallback(const std::string& type, int64_t callbackObject);
    void RemoveAllCallback();
    void RemoveCallback(const std::string& type, int64_t callbackObject);
    void OnCreate(DisplayId id) override;
    void OnDestroy(DisplayId id) override;
    void OnChange(DisplayId id) override;
    void OnPrivateWindow(bool hasPrivate) override;
    void OnFoldStatusChanged(FoldStatus foldStatus) override;
    void OnFoldAngleChanged(std::vector<float> foldAngles) override;
    void OnCaptureStatusChanged(bool isCapture) override;
    void OnDisplayModeChanged(FoldDisplayMode displayMode) override;
    void OnAvailableAreaChanged(DMRect area) override;

private:
    void CallCJMethod(const std::string& methodName, void* argv);
    std::mutex mtx_;
    std::map<std::string, std::map<int64_t, std::function<void(void*)>>> cjCallBack_;
};
} // namespace Rosen
} // namespace OHOS

#endif /* OHOS_CJ_DISPLAY_LISTENER_H */
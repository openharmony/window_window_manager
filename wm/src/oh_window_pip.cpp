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

#include "oh_window_pip.h"

#include "oh_window_comm.h"
#include "web_picture_in_picture_controller_interface.h"
#include "window_manager_hilog.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace Rosen {
namespace {
    std::map<uint32_t, sptr<WebPictureInPictureControllerInterface>> g_ControllerIds;
}

inline bool ControllerIdsIsFull()
{
    if (g_ControllerIds.size() >= static_cast<size_t>(UINT32_MAX - 1)) {
        return true;
    }
    return false;
}

inline uint32_t FindNextAvailableId()
{
    if (ControllerIdsIsFull()) {
        return 0;
    }
    uint32_t nextId = 0;
    while (nextId < UINT32_MAX && g_ControllerIds.find(nextId) != g_ControllerIds.end()) {
        nextId++;
    }
    return nextId;
}

inline std::vector<uint32_t> GetControlGroup(PictureInPicture_PiPControlGroup* controlGroup, uint8_t controlGroupLength)
{
    return std::vector<uint32_t>(controlGroup, controlGroup + controlGroupLength);
}

inline sptr<WebPictureInPictureControllerInterface> GetControllerFromId(uint32_t controllerId)
{
    if (g_ControllerIds.find(controllerId) == g_ControllerIds.end()) {
        return nullptr;
    }
    return g_ControllerIds.at(controllerId);
}
} // namespace Rosen
} // namespace OHOS

namespace {
/*
 * Used to map from WMError to WindowManager_ErrorCode.
 */
const std::unordered_map<WMError,              WindowManager_ErrorCode> OH_WINDOW_TO_ERROR_CODE_MAP {
    { WMError::WM_OK,                          WindowManager_ErrorCode::OK                                           },
    { WMError::WM_ERROR_INVALID_PARAM,         WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM     },
    { WMError::WM_ERROR_DEVICE_NOT_SUPPORT,    WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED},
    { WMError::WM_ERROR_PIP_DESTROY_FAILED,    WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_PIP_DESTROY_FAILED  },
    { WMError::WM_ERROR_PIP_STATE_ABNORMALLY,  WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_PIP_STATE_ABNORMAL  },
    { WMError::WM_ERROR_PIP_CREATE_FAILED,     WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_PIP_CREATE_FAILED   },
    { WMError::WM_ERROR_PIP_INTERNAL_ERROR,    WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR  },
    { WMError::WM_ERROR_PIP_REPEAT_OPERATION,  WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_PIP_REPEAT_OPERATION},
};

inline WindowManager_ErrorCode GetErrorCodeFromWMError(WMError error)
{
    if (OH_WINDOW_TO_ERROR_CODE_MAP.find(error) == OH_WINDOW_TO_ERROR_CODE_MAP.end()) {
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR;
    }
    return OH_WINDOW_TO_ERROR_CODE_MAP.at(error);
}
} //namespace

int32_t OH_PictureInPicture_CreatePip(PictureInPicture_PiPConfig* pipConfig, uint32_t* controllerId)
{
    if (ControllerIdsIsFull()) {
        TLOGE(WmsLogTag::WMS_PIP, "all IDs are used");
        return  WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR;
    }
    auto webPipControllerInterface = OHOS::sptr<WebPictureInPictureControllerInterface>::MakeSptr();
    PiPConfig config{pipConfig->mainWindowId, pipConfig->pipTemplateType, pipConfig->width, pipConfig->height,
        GetControlGroup(pipConfig->controlGroup, pipConfig->controlGroupLength), pipConfig->env};
    auto errCode = webPipControllerInterface->Create(config);
    if (errCode == WMError::WM_OK) {
        uint32_t id = FindNextAvailableId();
        *controllerId = id;
        g_ControllerIds[id] = webPipControllerInterface;
    }
    return GetErrorCodeFromWMError(errCode);
}

int32_t OH_PictureInPicture_DeletePip(uint32_t controllerId)
{
    if (g_ControllerIds.find(controllerId) == g_ControllerIds.end()) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    g_ControllerIds.erase(controllerId);
    return WindowManager_ErrorCode::OK;
}

int32_t OH_PictureInPicture_StartPip(uint32_t controllerId)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->StartPip());
}

int32_t OH_PictureInPicture_StopPip(uint32_t controllerId)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->StopPip());
}

void OH_PictureInPicture_UpdatePipContentSize(uint32_t controllerId, uint32_t width, uint32_t height)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return;
    }
    pipController->UpdateContentSize(width, height);
}

void OH_PictureInPicture_UpdatePipControlStatus(uint32_t controllerId, PictureInPicture_PiPControlType controlType,
    PictureInPicture_PiPControlStatus status)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return;
    }
    pipController->UpdatePiPControlStatus(static_cast<PiPControlType>(controlType),
        static_cast<PiPControlStatus>(status));
}

void OH_PictureInPicture_SetPiPControlEnabled(uint32_t controllerId, PictureInPicture_PiPControlType controlType,
    bool enabled)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return;
    }
    pipController->setPiPControlEnabled(static_cast<PiPControlType>(controlType), enabled);
}

int32_t OH_PictureInPicture_RegisterStartPipCallback(uint32_t controllerId, WebPipStartPipCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->RegisterStartPipListener(callback));
}

int32_t OH_PictureInPicture_UnregisterStartPipCallback(uint32_t controllerId, WebPipStartPipCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is nullptr");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterStartPipListener(callback));
}

int32_t OH_PictureInPicture_UnregisterAllStartPipCallbacks(uint32_t controllerId)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterAllPiPStart());
}

int32_t OH_PictureInPicture_RegisterLifecycleListener(uint32_t controllerId, WebPipLifecycleCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->RegisterLifeCycleListener(
        reinterpret_cast<OHOS::Rosen::NativePipLifeCycleCallback>(callback)));
}

int32_t OH_PictureInPicture_UnregisterLifecycleListener(uint32_t controllerId, WebPipLifecycleCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is nullptr");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterLifeCycleListener(
        reinterpret_cast<OHOS::Rosen::NativePipLifeCycleCallback>(callback)));
}

int32_t OH_PictureInPicture_UnregisterAllLifecycleListeners(uint32_t controllerId)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterAllPiPLifecycle());
}

int32_t OH_PictureInPicture_RegisterControlEventListener(uint32_t controllerId, WebPipControlEventCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->RegisterControlEventListener(
        reinterpret_cast<OHOS::Rosen::NativePipControlEventCallback>(callback)));
}

int32_t OH_PictureInPicture_UnregisterControlEventListener(uint32_t controllerId, WebPipControlEventCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is nullptr");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterControlEventListener(
        reinterpret_cast<OHOS::Rosen::NativePipControlEventCallback>(callback)));
}

int32_t OH_PictureInPicture_UnregisterAllControlEventListeners(uint32_t controllerId)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterAllPiPControlObserver());
}

int32_t OH_PictureInPicture_RegisterResizeListener(uint32_t controllerId, WebPipResizeCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->RegisterResizeListener(callback));
}

int32_t OH_PictureInPicture_UnregisterResizeListener(uint32_t controllerId, WebPipResizeCallback callback)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is nullptr");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterResizeListener(callback));
}

int32_t OH_PictureInPicture_UnregisterAllResizeListeners(uint32_t controllerId)
{
    auto pipController = GetControllerFromId(controllerId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controllerId not found");
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM;
    }
    return GetErrorCodeFromWMError(pipController->UnregisterAllPiPWindowSize());
}
/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "web_picture_in_picture_controller_interface.h"

#include "native_pip_window_listener.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

using namespace Ace;

namespace {
    constexpr uint32_t MAX_CONTROL_GROUP_NUM = 3;
    const std::set<PiPControlGroup> VIDEO_PLAY_CONTROLS {
        PiPControlGroup::VIDEO_PREVIOUS_NEXT,
        PiPControlGroup::FAST_FORWARD_BACKWARD,
    };
    const std::set<PiPControlGroup> VIDEO_CALL_CONTROLS {
        PiPControlGroup::VIDEO_CALL_MICROPHONE_SWITCH,
        PiPControlGroup::VIDEO_CALL_HANG_UP_BUTTON,
        PiPControlGroup::VIDEO_CALL_CAMERA_SWITCH,
        PiPControlGroup::VIDEO_CALL_MUTE_SWITCH,
    };
    const std::set<PiPControlGroup> VIDEO_MEETING_CONTROLS {
        PiPControlGroup::VIDEO_MEETING_HANG_UP_BUTTON,
        PiPControlGroup::VIDEO_MEETING_CAMERA_SWITCH,
        PiPControlGroup::VIDEO_MEETING_MUTE_SWITCH,
        PiPControlGroup::VIDEO_MEETING_MICROPHONE_SWITCH,
    };
    const std::set<PiPControlGroup> VIDEO_LIVE_CONTROLS {
        PiPControlGroup::VIDEO_PLAY_PAUSE,
        PiPControlGroup::VIDEO_LIVE_MUTE_SWITCH,
    };
    const std::map<PiPTemplateType, std::set<PiPControlGroup>> TEMPLATE_CONTROL_MAP {
        {PiPTemplateType::VIDEO_PLAY, VIDEO_PLAY_CONTROLS},
        {PiPTemplateType::VIDEO_CALL, VIDEO_CALL_CONTROLS},
        {PiPTemplateType::VIDEO_MEETING, VIDEO_MEETING_CONTROLS},
        {PiPTemplateType::VIDEO_LIVE, VIDEO_LIVE_CONTROLS},
    };
}

static int32_t checkControlsRules(uint32_t pipTemplateType, const std::vector<std::uint32_t>& controlGroups)
{
    auto iter = TEMPLATE_CONTROL_MAP.find(static_cast<PiPTemplateType>(pipTemplateType));
    auto controls = iter->second;
    if (controlGroups.size() > MAX_CONTROL_GROUP_NUM) {
        return -1;
    }
    for (auto control : controlGroups) {
        if (controls.find(static_cast<PiPControlGroup>(control)) == controls.end()) {
            TLOGE(WmsLogTag::WMS_PIP, "pipOption param error, controlGroup not matches, controlGroup: %{public}u",
                control);
            return -1;
        }
    }
    if (pipTemplateType != static_cast<uint32_t>(PiPTemplateType::VIDEO_PLAY)) {
        return 0;
    }
    auto iterFirst = std::find(controlGroups.begin(), controlGroups.end(),
        static_cast<uint32_t>(PiPControlGroup::VIDEO_PREVIOUS_NEXT));
    auto iterSecond = std::find(controlGroups.begin(), controlGroups.end(),
        static_cast<uint32_t>(PiPControlGroup::FAST_FORWARD_BACKWARD));
    if (iterFirst != controlGroups.end() && iterSecond != controlGroups.end()) {
        TLOGE(WmsLogTag::WMS_PIP, "pipOption param error, %{public}u conflicts with %{public}u in controlGroups",
        static_cast<uint32_t>(PiPControlGroup::VIDEO_PREVIOUS_NEXT),
        static_cast<uint32_t>(PiPControlGroup::FAST_FORWARD_BACKWARD));
        return -1;
    }
    return 0;
}

static int32_t checkCreatePipParams(const PiPConfig& config)
{
    if (config.mainWindowId == 0) {
        TLOGE(WmsLogTag::WMS_PIP, "mainWindowId could not be 0");
        return -1;
    }
    if (TEMPLATE_CONTROL_MAP.find(static_cast<PiPTemplateType>(config.pipTemplateType)) ==
        TEMPLATE_CONTROL_MAP.end()) {
        TLOGE(WmsLogTag::WMS_PIP, "createPip param error, pipTemplateType not exists");
        return -1;
    }
    if (config.width == 0 || config.height == 0) {
        TLOGE(WmsLogTag::WMS_PIP, "width or height could not be 0");
        return -1;
    }
    if (config.env == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "invalid env");
        return -1;
    }
    return checkControlsRules(config.pipTemplateType, config.controlGroup);
}

WMError WebPictureInPictureControllerInterface::Create(const PiPConfig& config)
{
    int32_t ret = checkCreatePipParams(config);
    if (ret == -1) {
        TLOGE(WmsLogTag::WMS_PIP, "Invalid parameters when createPip, "
            "please check if mainWindowId/width/height is zero, "
            "or env is nullptr, or controlGroup mismatch the corresponding pipTemplateType");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    sptrWebPipController_ = sptr<WebPictureInPictureController>::MakeSptr(config);
    sptrWebPipController_->GetPipPossible(isPipPossible_);
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    return WMError::WM_OK;
}

WMError WebPictureInPictureControllerInterface::StartPip()
{
    if (auto pipController = sptrWebPipController_) {
        return pipController->StartPictureInPicture(StartPipType::NATIVE_START);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::StopPip()
{
    if (auto pipController = sptrWebPipController_) {
        return pipController->StopPictureInPictureFromClient();
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UpdateContentSize(int32_t width, int32_t height)
{
    if (auto pipController = sptrWebPipController_) {
        pipController->UpdateContentSize(width, height);
        return WMError::WM_OK;
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

void WebPictureInPictureControllerInterface::UpdatePiPControlStatus(PiPControlType controlType, PiPControlStatus status)
{
    if (auto pipController = sptrWebPipController_) {
        pipController->UpdatePiPControlStatus(controlType, status);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return;
    }
}

void WebPictureInPictureControllerInterface::setPiPControlEnabled(PiPControlType controlType, bool enabled)
{
    if (auto pipController = sptrWebPipController_) {
        pipController->UpdatePiPControlStatus(controlType,
            enabled ? PiPControlStatus::ENABLED : PiPControlStatus::DISABLED);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return;
    }
}

WMError WebPictureInPictureControllerInterface::RegisterStartPipListener(NativePipStartPipCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return RegisterListenerWithType(ListenerType::PIP_START_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UnregisterStartPipListener(NativePipStartPipCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return UnregisterListenerWithType(ListenerType::PIP_START_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}


WMError WebPictureInPictureControllerInterface::RegisterLifeCycleListener(NativePipLifeCycleCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return RegisterListenerWithType(ListenerType::STATE_CHANGE_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UnregisterLifeCycleListener(NativePipLifeCycleCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return UnregisterListenerWithType(ListenerType::STATE_CHANGE_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::RegisterControlEventListener(NativePipControlEventCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return RegisterListenerWithType(ListenerType::CONTROL_EVENT_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UnregisterControlEventListener(NativePipControlEventCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return UnregisterListenerWithType(ListenerType::CONTROL_EVENT_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::RegisterResizeListener(NativePipResizeCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return RegisterListenerWithType(ListenerType::SIZE_CHANGE_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UnregisterResizeListener(NativePipResizeCallback callback)
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (callback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "callback is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto pipController = sptrWebPipController_) {
        auto listener = sptr<NativePiPWindowListener>::MakeSptr(callback);
        return UnregisterListenerWithType(ListenerType::SIZE_CHANGE_CB, listener);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}


WMError WebPictureInPictureControllerInterface::RegisterListenerWithType(ListenerType type,
    const sptr<NativePiPWindowListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "New NativePiPWindowListener failed");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (IsRegistered(type, listener)) {
        TLOGE(WmsLogTag::WMS_PIP, "Listener already registered");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    WMError ret = WMError::WM_OK;
    int32_t cbMapSize = -1;
    switch (type) {
        case ListenerType::STATE_CHANGE_CB:
            ret = ProcessStateChangeRegister(listener);
            if (ret != WMError::WM_OK) {
                return ret;
            }
            lifeCycleCallbackSet_.insert(listener);
            cbMapSize = lifeCycleCallbackSet_.size();
            break;
        case ListenerType::CONTROL_EVENT_CB:
            ret = ProcessControlEventRegister(listener);
            if (ret != WMError::WM_OK) {
                return ret;
            }
            controlEventCallbackSet_.insert(listener);
            cbMapSize = controlEventCallbackSet_.size();
            break;
        case ListenerType::SIZE_CHANGE_CB:
            ret = ProcessSizeChangeRegister(listener);
            if (ret != WMError::WM_OK) {
                return ret;
            }
            resizeCallbackSet_.insert(listener);
            cbMapSize = resizeCallbackSet_.size();
            break;
        case ListenerType::PIP_START_CB:
            ret = ProcessPipStartRegister(listener);
            if (ret != WMError::WM_OK) {
                return ret;
            }
            startPipCallbackSet_.insert(listener);
            cbMapSize = startPipCallbackSet_.size();
            break;
        default:
            break;
    }
    TLOGI(WmsLogTag::WMS_PIP, "Register type %{public}u success! callback map size: %{public}d", type, cbMapSize);
    return WMError::WM_OK;
}

WMError WebPictureInPictureControllerInterface::UnregisterListenerWithType(ListenerType type,
    const sptr<NativePiPWindowListener>& listener)
{
    if (!IsRegistered(type, listener)) {
        TLOGE(WmsLogTag::WMS_PIP, "Listener not registered");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    int32_t cbMapSize = -1;
    switch (type) {
        case ListenerType::STATE_CHANGE_CB:
            for (const auto& it : lifeCycleCallbackSet_) {
                if (it->GetLifeCycleCallbackRef() == listener->GetLifeCycleCallbackRef()) {
                    cbMapSize = ProcessStateChangeUnregister(it);
                    break;
                }
            }
            break;
        case ListenerType::CONTROL_EVENT_CB:
            for (const auto& it : controlEventCallbackSet_) {
                if (it->GetControlEventCallbackRef() == listener->GetControlEventCallbackRef()) {
                    cbMapSize = ProcessControlEventUnregister(it);
                    break;
                }
            }
            break;
        case ListenerType::SIZE_CHANGE_CB:
            for (const auto& it : resizeCallbackSet_) {
                if (it->GetResizeCallbackRef() == listener->GetResizeCallbackRef()) {
                    cbMapSize = ProcessSizeChangeUnregister(it);
                    break;
                }
            }
            break;
        case ListenerType::PIP_START_CB:
            for (const auto& it : startPipCallbackSet_) {
                if (it->GetStartPipCallbackRef() == listener->GetStartPipCallbackRef()) {
                    cbMapSize = ProcessPipStartUnregister(it);
                    break;
                }
            }
            break;
        default:
            break;
    }
    TLOGI(WmsLogTag::WMS_PIP, "Unregister type %{public}u success! callback map size: %{public}d", type, cbMapSize);
    return WMError::WM_OK;
}

WMError WebPictureInPictureControllerInterface::UnregisterAllPiPLifecycle()
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
        }
    if (auto pipController = sptrWebPipController_) {
        UnregisterAll(ListenerType::STATE_CHANGE_CB);
        return WMError::WM_OK;
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UnregisterAllPiPControlObserver()
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
        }
    if (auto pipController = sptrWebPipController_) {
        UnregisterAll(ListenerType::CONTROL_EVENT_CB);
        return WMError::WM_OK;
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UnregisterAllPiPWindowSize()
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
        }
    if (auto pipController = sptrWebPipController_) {
        UnregisterAll(ListenerType::SIZE_CHANGE_CB);
        return WMError::WM_OK;
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

WMError WebPictureInPictureControllerInterface::UnregisterAllPiPStart()
{
    if (!isPipPossible_) {
        TLOGE(WmsLogTag::WMS_PIP, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
        }
    if (auto pipController = sptrWebPipController_) {
        UnregisterAll(ListenerType::PIP_START_CB);
        return WMError::WM_OK;
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "sptrWebPipController_ is nullptr");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
}

void WebPictureInPictureControllerInterface::UnregisterAll(ListenerType type)
{
    switch (type) {
        case ListenerType::STATE_CHANGE_CB:
            lifeCycleCallbackSet_.clear();
            sptrWebPipController_->UnregisterAllPiPLifecycle();
            break;
        case ListenerType::CONTROL_EVENT_CB:
            controlEventCallbackSet_.clear();
            sptrWebPipController_->UnregisterAllPiPControlObserver();
            break;
        case ListenerType::SIZE_CHANGE_CB:
            resizeCallbackSet_.clear();
            sptrWebPipController_->UnregisterAllPiPWindowSize();
            break;
        case ListenerType::PIP_START_CB:
            startPipCallbackSet_.clear();
            sptrWebPipController_->UnregisterAllPiPStart();
            break;
        default:
            break;
    }
}

bool WebPictureInPictureControllerInterface::IsRegistered(ListenerType type,
    const sptr<NativePiPWindowListener>& listener)
{
    switch (type) {
        case ListenerType::STATE_CHANGE_CB:
            for (const auto& it : lifeCycleCallbackSet_) {
                if (it->GetLifeCycleCallbackRef() == listener->GetLifeCycleCallbackRef()) {
                    return true;
                }
            }
            break;
        case ListenerType::CONTROL_EVENT_CB:
            for (const auto& it : controlEventCallbackSet_) {
                if (it->GetControlEventCallbackRef() == listener->GetControlEventCallbackRef()) {
                    return true;
                }
            }
            break;
        case ListenerType::SIZE_CHANGE_CB:
            for (const auto& it : resizeCallbackSet_) {
                if (it->GetResizeCallbackRef() == listener->GetResizeCallbackRef()) {
                    return true;
                }
            }
            break;
        case ListenerType::PIP_START_CB:
            for (const auto& it : startPipCallbackSet_) {
                if (it->GetStartPipCallbackRef() == listener->GetStartPipCallbackRef()) {
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

WMError WebPictureInPictureControllerInterface::ProcessStateChangeRegister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPLifeCycle> thisListener(listener);
    return sptrWebPipController_->RegisterPiPLifecycle(thisListener);
}

int32_t WebPictureInPictureControllerInterface::ProcessStateChangeUnregister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPLifeCycle> thisListener(listener);
    sptrWebPipController_->UnregisterPiPLifecycle(thisListener);
    lifeCycleCallbackSet_.erase(listener);
    return lifeCycleCallbackSet_.size();
}

WMError WebPictureInPictureControllerInterface::ProcessControlEventRegister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPControlObserver> thisListener(listener);
    return sptrWebPipController_->RegisterPiPControlObserver(thisListener);
}

int32_t WebPictureInPictureControllerInterface::ProcessControlEventUnregister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPControlObserver> thisListener(listener);
    sptrWebPipController_->UnregisterPiPControlObserver(thisListener);
    controlEventCallbackSet_.erase(listener);
    return controlEventCallbackSet_.size();
}

WMError WebPictureInPictureControllerInterface::ProcessSizeChangeRegister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPWindowSize> thisListener(listener);
    return sptrWebPipController_->RegisterPiPWindowSize(thisListener);
}

int32_t WebPictureInPictureControllerInterface::ProcessSizeChangeUnregister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPWindowSize> thisListener(listener);
    sptrWebPipController_->UnregisterPiPWindowSize(thisListener);
    resizeCallbackSet_.erase(listener);
    return resizeCallbackSet_.size();
}

WMError WebPictureInPictureControllerInterface::ProcessPipStartRegister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPStartObserver> thisListener(listener);
    return sptrWebPipController_->RegisterPiPStart(thisListener);
}

int32_t WebPictureInPictureControllerInterface::ProcessPipStartUnregister(
    const sptr<NativePiPWindowListener>& listener)
{
    sptr<IPiPStartObserver> thisListener(listener);
    sptrWebPipController_->UnregisterPiPStart(thisListener);
    startPipCallbackSet_.erase(listener);
    return startPipCallbackSet_.size();
}

} // namespace Rosen
} // namespace OHOS
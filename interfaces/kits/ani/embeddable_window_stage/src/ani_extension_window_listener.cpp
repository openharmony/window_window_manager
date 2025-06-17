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

#include "ani_extension_window_listener.h"

#include <hitrace_meter.h>
#include "ani.h"
#include "event_handler.h"
#include "event_runner.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
const std::string WINDOW_SIZE_CHANGE_CB = "windowSizeChange";
const std::string SYSTEM_AVOID_AREA_CHANGE_CB = "systemAvoidAreaChange";
const std::string AVOID_AREA_CHANGE_CB = "avoidAreaChange";
const std::string LIFECYCLE_EVENT_CB = "lifeCycleEvent";
const std::string KEYBOARD_HEIGHT_CHANGE_CB = "keyboardHeightChange";
}

AniExtensionWindowListener::~AniExtensionWindowListener()
{
    if (callBack_ != nullptr) {
        env_->GlobalReference_Delete(callBack_);
    }
    if (callBackData_ != nullptr) {
        env_->GlobalReference_Delete(callBackData_);
    }
    TLOGI(WmsLogTag::WMS_UIEXT, "[ANI]~AniExtensionWindowListener");
}

bool AniExtensionWindowListener::IsSameRef(ani_ref newRef)
{
    return true;
}

void AniExtensionWindowListener::SetMainEventHandler()
{
    auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainRunner == nullptr) {
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
}

void AniExtensionWindowListener::CallBack()
{
    ani_status ret {};
    ani_function fn {};
    ani_namespace ns {};
    if ((ret = env_->FindNamespace("L@ohos/uiExtensionHost/uiExtensionHost;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]canot find ns %{public}d", ret);
        return;
    }
    if ((ret = env_->Namespace_FindFunction(ns, "callBack", "Lstd/core/Object;Lstd/core/Object;:V", &fn)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]canot find callBack %{public}d", ret);
        return;
    }
    if ((ret = env_->Function_Call_Void(fn, callBack_, callBackData_)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]canot find callBack %{public}d", ret);
        return;
    }
}

void AniExtensionWindowListener::SetSizeInfo(uint32_t width, uint32_t height)
{
    ani_status ret {};
    if ((ret = env_->Object_SetFieldByName_Double((ani_object)callBackData_, "<property>width",
        (double)width)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]canot set width %{public}d", ret);
        return;
    };
    if ((ret = env_->Object_SetFieldByName_Double((ani_object)callBackData_, "<property>height",
        (double)height)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI]canot set height %{public}d", ret);
        return;
    }
}

void AniExtensionWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[ANI]OnSizeChange, [%{public}u, %{public}u], reason=%{public}u",
        rect.width_, rect.height_, reason);
    if (currentWidth_ == rect.width_ && currentHeight_ == rect.height_ && reason != WindowSizeChangeReason::DRAG_END) {
        TLOGD(WmsLogTag::WMS_UIEXT, "[ANI]no need to change size");
        return;
    }
    auto aniCallback = [self = weakRef_, rect, eng = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AniExtensionWindowListener::OnSizeChange");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "[NAPI]this listener or eng is nullptr");
            return;
        }
        thisListener->SetSizeInfo(rect.width_, rect.height_);
        thisListener->CallBack();
    };
    if (reason == WindowSizeChangeReason::ROTATION) {
        aniCallback();
    } else {
        if (!eventHandler_) {
            TLOGE(WmsLogTag::WMS_UIEXT, "get main event handler failed!");
            return;
        }
        eventHandler_->PostTask(aniCallback, "wms:AniExtensionWindowListener::OnSizeChange", 0,
                                AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
    currentWidth_ = rect.width_;
    currentHeight_ = rect.height_;
}

void AniExtensionWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type,
    const sptr<OccupiedAreaChangeInfo>& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[ANI]OnAvoidAreaChanged");
}

void AniExtensionWindowListener::OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    TLOGI(WmsLogTag::WMS_UIEXT,
        "[ANI]OccupiedAreaChangeInfo, type: %{public}d, input rect:[%{public}d, %{public}d, %{public}d, %{public}d]",
        static_cast<uint32_t>(info->type_), info->rect_.posX_, info->rect_.posY_, info->rect_.width_,
        info->rect_.height_);
    // js callback should run in js thread
    CallBack();
}

} // namespace Rosen
} // namespace OHOS

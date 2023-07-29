/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "root_scene.h"

#include <event_handler.h>
#include <input_manager.h>
#include <ui_content.h>
#include <viewport_config.h>

#include "app_mgr_client.h"
#include "singleton.h"
#include "singleton_container.h"

#include "anr_manager.h"
#include "intention_event_manager.h"
#include "vsync_station.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "RootScene" };
const std::string INPUT_AND_VSYNC_THREAD = "InputAndVsyncThread";

class InputEventListener : public MMI::IInputEventConsumer {
public:
    explicit InputEventListener(RootScene* rootScene): rootScene_(rootScene) {}
    virtual ~InputEventListener() = default;

    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override
    {
        rootScene_->ConsumePointerEvent(pointerEvent);
    }

    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override
    {
        rootScene_->ConsumeKeyEvent(keyEvent);
    }

    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override
    {
    }

private:
    RootScene* rootScene_;
};
} // namespace

sptr<RootScene> RootScene::staticRootScene_;

RootScene::RootScene()
{
}

RootScene::~RootScene()
{
}

void RootScene::LoadContent(const std::string& contentUrl, NativeEngine* engine, NativeValue* storage,
    AbilityRuntime::Context* context)
{
    if (context == nullptr) {
        WLOGFE("context is nullptr!");
        return;
    }
    uiContent_ = Ace::UIContent::Create(context, engine);
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is nullptr!");
        return;
    }

    uiContent_->Initialize(this, contentUrl, storage);
    uiContent_->Foreground();

    RegisterInputEventListener();
    DelayedSingleton<ANRManager>::GetInstance()->Init();
    DelayedSingleton<ANRManager>::GetInstance()->SetAnrObserver(([](int32_t pid) {
        WLOGFD("Receive anr notice enter");
        AppExecFwk::AppFaultDataBySA faultData;
        faultData.faultType = AppExecFwk::FaultDataType::APP_FREEZE;
        faultData.pid = pid;
        faultData.errorObject.name = AppExecFwk::AppFreezeType::APP_INPUT_BLOCK;
        faultData.errorObject.message = "User input does not respond normally";
        faultData.errorObject.stack = "";
        if (int32_t ret = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance()->NotifyAppFaultBySA(faultData);
            ret != 0) {
            WLOGFE("NotifyAppFaultBySA failed, pid:%{public}d, errcode:%{public}d", pid, ret);
        }
        WLOGFD("Receive anr notice leave");
    }));
}

void RootScene::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is nullptr!");
        return;
    }
    Ace::ViewportConfig config;
    config.SetSize(rect.width_, rect.height_);
    config.SetPosition(rect.posX_, rect.posY_);
    config.SetDensity(density_);
    uiContent_->UpdateViewportConfig(config, reason);
}

void RootScene::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_) {
        WLOGFD("notify root scene ace");
        uiContent_->UpdateConfiguration(configuration);
    }
}
void RootScene::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    WLOGD("notify root scene ace for all");
    if (staticRootScene_) {
        staticRootScene_->UpdateConfiguration(configuration);
    }
}

void RootScene::RegisterInputEventListener()
{
    auto listener = std::make_shared<InputEventListener>(this);
    auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainEventRunner) {
        WLOGFD("MainEventRunner is available");
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
    } else {
        WLOGFD("MainEventRunner is not available");
        eventHandler_ = AppExecFwk::EventHandler::Current();
        if (!eventHandler_) {
            eventHandler_ =
                std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::Create(INPUT_AND_VSYNC_THREAD));
        }
        VsyncStation::GetInstance().SetIsMainHandlerAvailable(false);
        VsyncStation::GetInstance().SetVsyncEventHandler(eventHandler_);
    }
    if (!(DelayedSingleton<IntentionEventManager>::GetInstance()->EnableInputEventListener(
            uiContent_.get(), eventHandler_))) {
        WLOGFE("EnableInputEventListener fail");
    }
}

void RootScene::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    VsyncStation::GetInstance().RequestVsync(vsyncCallback);
}

int64_t RootScene::GetVSyncPeriod()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return VsyncStation::GetInstance().GetVSyncPeriod();
}
} // namespace Rosen
} // namespace OHOS

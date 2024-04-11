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

#include "screen_scene.h"

#include <ui_content.h>
#include <viewport_config.h>

#include "app_mgr_client.h"
#include "singleton.h"
#include "singleton_container.h"

#include "anr_manager.h"
#include "window_manager_hilog.h"
#include "window_rate_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenScene" };
} // namespace

ScreenScene::ScreenScene(std::string name) : name_(name)
{
    NodeId nodeId = 0;
    vsyncStation_ = std::make_shared<VsyncStation>(nodeId);
}

ScreenScene::~ScreenScene()
{
    uiContent_ = nullptr;
}

void ScreenScene::LoadContent(const std::string& contentUrl, napi_env env, napi_value storage,
    AbilityRuntime::Context* context)
{
    if (context == nullptr) {
        WLOGFE("context is nullptr!");
        return;
    }
    uiContent_ = Ace::UIContent::Create(context, reinterpret_cast<NativeEngine*>(env));
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is nullptr!");
        return;
    }

    uiContent_->Initialize(this, contentUrl, storage);
    uiContent_->Foreground();
    uiContent_->SetFrameLayoutFinishCallback(std::move(frameLayoutFinishCb_));
    DelayedSingleton<ANRManager>::GetInstance()->Init();
    DelayedSingleton<ANRManager>::GetInstance()->SetAnrObserver(([](int32_t pid) {
        WLOGFD("Receive anr notice enter");
        AppExecFwk::AppFaultDataBySA faultData;
        faultData.faultType = AppExecFwk::FaultDataType::APP_FREEZE;
        faultData.pid = pid;
        faultData.errorObject.name = AppExecFwk::AppFreezeType::APP_INPUT_BLOCK;
        faultData.errorObject.message = "User input does not respond normally, report by sceneBoard.";
        faultData.errorObject.stack = "";
        if (int32_t ret = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance()->NotifyAppFaultBySA(faultData);
            ret != 0) {
            WLOGFE("NotifyAppFaultBySA failed, pid:%{public}d, errcode:%{public}d", pid, ret);
        }
        WLOGFD("Receive anr notice leave");
    }));
    DelayedSingleton<ANRManager>::GetInstance()->SetAppInfoGetter(
        [](int32_t pid, std::string& bundleName, int32_t uid) {
            int32_t ret = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance()->GetBundleNameByPid(
                pid, bundleName, uid);
            if (ret != 0) {
                WLOGFE("GetBundleNameByPid failed, pid:%{public}d, errcode:%{public}d", pid, ret);
            }
        });
}

void ScreenScene::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is nullptr!");
        return;
    }
    Ace::ViewportConfig config;
    config.SetSize(rect.width_, rect.height_);
    config.SetPosition(rect.posX_, rect.posY_);
    config.SetDensity(density_);
    config.SetOrientation(orientation_);
    uiContent_->UpdateViewportConfig(config, reason);
}

void ScreenScene::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_) {
        WLOGFD("notify root scene ace");
        uiContent_->UpdateConfiguration(configuration);
    }
}

void ScreenScene::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Receive vsync request failed, vsyncStation is nullptr");
        return;
    }
    vsyncStation_->RequestVsync(vsyncCallback);
}

int64_t ScreenScene::GetVSyncPeriod()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Get vsync period failed, vsyncStation is nullptr");
        return 0;
    }
    return vsyncStation_->GetVSyncPeriod();
}

void ScreenScene::FlushFrameRate(uint32_t rate)
{
    std::lock_guard<std::mutex> lock(mutex_);
    WindowRateManager::GetInstance().FlushFrameRateForRootWindow(rate, vsyncStation_);
}

void ScreenScene::SetFrameLayoutFinishCallback(std::function<void()>&& callback)
{
    frameLayoutFinishCb_ = callback;
    if (uiContent_) {
        uiContent_->SetFrameLayoutFinishCallback(std::move(frameLayoutFinishCb_));
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "SetFrameLayoutFinishCallback end");
}
} // namespace Rosen
} // namespace OHOS

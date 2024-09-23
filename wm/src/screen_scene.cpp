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

#include "screen_scene.h"

#include <event_handler.h>
#include <ui_content.h>
#include <viewport_config.h>

#include "app_mgr_client.h"
#include "singleton.h"
#include "singleton_container.h"

#include "anr_manager.h"
#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float MIN_DPI = 1e-6;
std::atomic<bool> g_ssIsDestroyed = false;
} // namespace

ScreenScene::ScreenScene(std::string name) : name_(name)
{
    orientation_ = static_cast<int32_t>(DisplayOrientation::PORTRAIT);
    NodeId nodeId = 0;
    vsyncStation_ = std::make_shared<VsyncStation>(nodeId);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    g_ssIsDestroyed = false;
}

ScreenScene::~ScreenScene()
{
    g_ssIsDestroyed = true;
    Destroy();
}

WMError ScreenScene::Destroy()
{
    if (!uiContent_) {
        TLOGD(WmsLogTag::DMS, "Destroy uiContent_ is nullptr!");
        return WMError::WM_OK;
    }
    std::shared_ptr<Ace::UIContent> uiContent = std::move(uiContent_);
    uiContent_ = nullptr;
    vsyncStation_->Destroy();
    auto task = [uiContent]() {
        if (uiContent != nullptr) {
            uiContent->Destroy();
            TLOGD(WmsLogTag::DMS, "ScreenScene: uiContent destroy success!");
        }
    };
    if (handler_) {
        handler_->PostSyncTask(task, "ScreenScene:Destroy");
    } else {
        task();
    }
    return WMError::WM_OK;
}

void ScreenScene::LoadContent(const std::string& contentUrl, napi_env env, napi_value storage,
    AbilityRuntime::Context* context)
{
    if (g_ssIsDestroyed) {
        TLOGI(WmsLogTag::DMS, "ScreenScene has been destructed!");
        return;
    }
    if (context == nullptr) {
        TLOGE(WmsLogTag::DMS, "context is nullptr!");
        return;
    }
    uiContent_ = Ace::UIContent::Create(context, reinterpret_cast<NativeEngine*>(env));
    if (uiContent_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "uiContent_ is nullptr!");
        return;
    }

    uiContent_->Initialize(this, contentUrl, storage);
    uiContent_->Foreground();
    uiContent_->SetFrameLayoutFinishCallback(std::move(frameLayoutFinishCb_));
    DelayedSingleton<ANRManager>::GetInstance()->Init();
    DelayedSingleton<ANRManager>::GetInstance()->SetAnrObserver(([](int32_t pid) {
        TLOGD(WmsLogTag::DMS, "Receive anr notice enter");
        AppExecFwk::AppFaultDataBySA faultData;
        faultData.faultType = AppExecFwk::FaultDataType::APP_FREEZE;
        faultData.pid = pid;
        faultData.errorObject.name = AppExecFwk::AppFreezeType::APP_INPUT_BLOCK;
        faultData.errorObject.message = "User input does not respond normally, report by sceneBoard.";
        faultData.errorObject.stack = "";
        if (int32_t ret = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance()->NotifyAppFaultBySA(faultData);
            ret != 0) {
            TLOGE(WmsLogTag::DMS, "NotifyAppFaultBySA failed, errcode:%{public}d", ret);
        }
        TLOGD(WmsLogTag::DMS, "Receive anr notice leave");
    }));
    DelayedSingleton<ANRManager>::GetInstance()->SetAppInfoGetter(
        [](int32_t pid, std::string& bundleName, int32_t uid) {
            int32_t ret = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance()->GetBundleNameByPid(
                pid, bundleName, uid);
            if (ret != 0) {
                TLOGE(WmsLogTag::DMS, "GetBundleNameByPid failed, pid:%{public}d, errcode:%{public}d", pid, ret);
            }
        });
}

void ScreenScene::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason)
{
    if (g_ssIsDestroyed) {
        TLOGI(WmsLogTag::DMS, "ScreenScene has been destructed!");
        return;
    }
    if (uiContent_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "uiContent_ is nullptr!");
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
    if (g_ssIsDestroyed) {
        TLOGI(WmsLogTag::DMS, "ScreenScene has been destructed!");
        return;
    }
    if (uiContent_) {
        TLOGD(WmsLogTag::DMS, "notify root scene ace");
        uiContent_->UpdateConfiguration(configuration);
    }
}

void ScreenScene::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    vsyncStation_->RequestVsync(vsyncCallback);
}

int64_t ScreenScene::GetVSyncPeriod()
{
    return vsyncStation_->GetVSyncPeriod();
}

void ScreenScene::FlushFrameRate(uint32_t rate, int32_t animatorExpectedFrameRate, uint32_t rateType)
{
    vsyncStation_->FlushFrameRate(rate, animatorExpectedFrameRate, rateType);
}

void ScreenScene::OnBundleUpdated(const std::string& bundleName)
{
    if (g_ssIsDestroyed) {
        TLOGI(WmsLogTag::DMS, "ScreenScene has been destructed!");
        return;
    }
    TLOGD(WmsLogTag::DMS, "bundle %{public}s updated", bundleName.c_str());
    if (uiContent_) {
        uiContent_->UpdateResource();
    }
}

void ScreenScene::SetFrameLayoutFinishCallback(std::function<void()>&& callback)
{
    if (g_ssIsDestroyed) {
        TLOGI(WmsLogTag::DMS, "ScreenScene has been destructed!");
        return;
    }
    frameLayoutFinishCb_ = callback;
    if (uiContent_) {
        uiContent_->SetFrameLayoutFinishCallback(std::move(frameLayoutFinishCb_));
        frameLayoutFinishCb_ = nullptr;
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "SetFrameLayoutFinishCallback end");
}

void ScreenScene::SetDisplayDensity(float density)
{
    if (density < MIN_DPI) {
        TLOGE(WmsLogTag::DMS, "invalid density");
        return;
    }
    density_ = density;
}

void ScreenScene::SetDisplayOrientation(int32_t orientation)
{
    if (orientation < static_cast<int32_t>(DisplayOrientation::PORTRAIT) ||
        orientation > static_cast<int32_t>(DisplayOrientation::UNKNOWN)) {
        TLOGE(WmsLogTag::DMS, "invalid orientation");
        return;
    }
    orientation_ = orientation;
}
} // namespace Rosen
} // namespace OHOS

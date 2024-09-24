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

#include <bundlemgr/launcher_service.h>
#include <event_handler.h>
#include <input_manager.h>
#include <iremote_stub.h>
#include <transaction/rs_interfaces.h>
#include <ui_content.h>
#include <viewport_config.h>

#include "app_mgr_client.h"
#include "fold_screen_state_internel.h"
#include "input_transfer_station.h"
#include "singleton.h"
#include "singleton_container.h"

#include "intention_event_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "RootScene" };
const std::string INPUT_AND_VSYNC_THREAD = "InputAndVsyncThread";

class BundleStatusCallback : public IRemoteStub<AppExecFwk::IBundleStatusCallback> {
public:
    explicit BundleStatusCallback(RootScene* rootScene) : rootScene_(rootScene) {}
    virtual ~BundleStatusCallback() = default;

    void OnBundleStateChanged(const uint8_t installType,
        const int32_t resultCode, const std::string& resultMsg, const std::string& bundleName) override {}

    void OnBundleAdded(const std::string& bundleName, const int userId) override
    {
        rootScene_->OnBundleUpdated(bundleName);
    }

    void OnBundleUpdated(const std::string& bundleName, const int userId) override
    {
        rootScene_->OnBundleUpdated(bundleName);
    }

    void OnBundleRemoved(const std::string& bundleName, const int userId) override {}

private:
    RootScene* rootScene_;
};
} // namespace

sptr<RootScene> RootScene::staticRootScene_;
std::function<void(const std::shared_ptr<AppExecFwk::Configuration>&)> RootScene::configurationUpdatedCallback_;

RootScene::RootScene()
{
    launcherService_ = new AppExecFwk::LauncherService();
    if (!launcherService_->RegisterCallback(new BundleStatusCallback(this))) {
        WLOGFE("Failed to register bundle status callback.");
    }

    NodeId nodeId = 0;
    vsyncStation_ = std::make_shared<VsyncStation>(nodeId);
}

RootScene::~RootScene()
{
    uiContent_ = nullptr;
}

void RootScene::LoadContent(const std::string& contentUrl, napi_env env, napi_value storage,
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
    RegisterInputEventListener();
}

void RootScene::SetDisplayOrientation(int32_t orientation)
{
    orientation_ = orientation;
}

void RootScene::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason)
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

void RootScene::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_) {
        WLOGFD("in");
        uiContent_->UpdateConfiguration(configuration);
        if (configuration == nullptr) {
            return;
        }
        std::string colorMode = configuration->GetItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE);
        bool isDark = (colorMode == AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);
        bool ret = RSInterfaces::GetInstance().SetGlobalDarkColorMode(isDark);
        if (ret == false) {
            WLOGFE("SetGlobalDarkColorMode fail with colorMode : %{public}s", colorMode.c_str());
        }
    }
}

void RootScene::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    WLOGFD("in");
    if (staticRootScene_) {
        staticRootScene_->UpdateConfiguration(configuration);
        if (configurationUpdatedCallback_) {
            configurationUpdatedCallback_(configuration);
        }
    }
}

void RootScene::RegisterInputEventListener()
{
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
    }
    if (!(DelayedSingleton<IntentionEventManager>::GetInstance()->EnableInputEventListener(
        uiContent_.get(), eventHandler_))) {
        WLOGFE("EnableInputEventListener fail");
    }
    InputTransferStation::GetInstance().MarkRegisterToMMI();
}

void RootScene::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    vsyncStation_->RequestVsync(vsyncCallback);
}

int64_t RootScene::GetVSyncPeriod()
{
    return vsyncStation_->GetVSyncPeriod();
}

void RootScene::FlushFrameRate(uint32_t rate, int32_t animatorExpectedFrameRate, uint32_t rateType)
{
    vsyncStation_->FlushFrameRate(rate, animatorExpectedFrameRate, rateType);
}

void RootScene::OnBundleUpdated(const std::string& bundleName)
{
    WLOGFD("bundle %{public}s updated", bundleName.c_str());
    if (uiContent_) {
        uiContent_->UpdateResource();
    }
}

void RootScene::SetOnConfigurationUpdatedCallback(
    const std::function<void(const std::shared_ptr<AppExecFwk::Configuration>&)>& callback)
{
    configurationUpdatedCallback_ = callback;
}

void RootScene::SetFrameLayoutFinishCallback(std::function<void()>&& callback)
{
    frameLayoutFinishCb_ = callback;
    if (uiContent_) {
        uiContent_->SetFrameLayoutFinishCallback(std::move(frameLayoutFinishCb_));
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "end");
}

void RootScene::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    vsyncStation_->SetUiDvsyncSwitch(dvsyncSwitch);
}

WMError RootScene::GetSessionRectByType(AvoidAreaType type, WSRect& rect)
{
    if (getSessionRectCallback_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "getSessionRectCallback is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    rect = getSessionRectCallback_(type);
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_inner_manager.h"

#include "ui_service_mgr_client.h"
#include "window_manager_hilog.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInnerManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowInnerManager)

WindowInnerManager::WindowInnerManager() : eventHandler_(nullptr), eventLoop_(nullptr),
    state_(InnerWMRunningState::STATE_NOT_START)
{
}

WindowInnerManager::~WindowInnerManager()
{
    Stop();
}

bool WindowInnerManager::Init()
{
    eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    if (eventLoop_ == nullptr) {
        return false;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        return false;
    }
    WLOGFI("init window inner manager service success.");
    return true;
}

void WindowInnerManager::Start()
{
    if (state_ == InnerWMRunningState::STATE_RUNNING) {
        WLOGFI("window inner manager service has already started.");
    }
    if (!Init()) {
        WLOGFI("failed to init window inner manager service.");
        return;
    }
    state_ = InnerWMRunningState::STATE_RUNNING;
    eventLoop_->Run();
    WLOGFI("window inner manager service start success.");
}

void WindowInnerManager::Stop()
{
    WLOGFI("stop window inner manager service.");
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_.reset();
    }
    if (eventHandler_ != nullptr) {
        eventHandler_.reset();
    }
    state_ = InnerWMRunningState::STATE_NOT_START;
}

void WindowInnerManager::HandleCreateWindow(std::string name, WindowType type, Rect rect)
{
    auto dialogCallback = [this](int32_t id, const std::string& event, const std::string& params) {
        if (params == "EVENT_CANCEL_CODE") {
            Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
        }
    };
    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(name, dividerParams_, type,
        rect.posX_, rect.posY_, rect.width_, rect.height_, dialogCallback, &dialogId_);
    WLOGFI("create inner window id: %{public}d success", dialogId_);
    return;
}

void WindowInnerManager::HandleDestroyWindow()
{
    if (dialogId_ == -1) {
        return;
    }
    WLOGFI("destroy inner window id:: %{public}d.", dialogId_);
    Ace::UIServiceMgrClient::GetInstance()->CancelDialog(dialogId_);
    dialogId_ = -1;
    return;
}

void WindowInnerManager::CreateWindow(std::string name, WindowType type, Rect rect)
{
    if (dialogId_ != -1) {
        return;
    }
    eventHandler_->PostTask([this, name, type, rect]() {
        HandleCreateWindow(name, type, rect);
    });
    return;
}

void WindowInnerManager::DestroyWindow()
{
    if (dialogId_ == -1) {
        WLOGFI("inner window has destroyed.");
        return;
    }
    eventHandler_->PostTask([this]() {
        HandleDestroyWindow();
    });
    return;
}
} // Rosen
} // OHOS
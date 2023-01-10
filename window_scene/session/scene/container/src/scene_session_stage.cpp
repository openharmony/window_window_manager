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

#include "session/scene/container/include/scene_session_stage.h"

#include "session/scene/container/include/window_event_channel.h"
#include "utils/include/window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionStage"};
}
SceneSessionStage::SceneSessionStage(const sptr<ISceneSession>& sceneSession) : sceneSession_(sceneSession)
{
}

WSError SceneSessionStage::SetActive(bool active)
{
    WLOGFD("active status: %{public}d", active);
    if (active) {
        NotifyAfterActive();
    } else {
        NotifyAfterInactive();
    }
    return WSError::WS_OK;
}

WSError SceneSessionStage::UpdateSessionRect(const WSRect& rect, SessionSizeChangeReason reason)
{
    WLOGFI("update rect [%{public}d, %{public}d, %{public}u, %{public}u], reason:%{public}u",
        rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
    NotifySizeChange(rect, reason);
    return WSError::WS_OK;
}

WSError SceneSessionStage::Connect()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    sptr<SceneSessionStage> sceneSessionStage(this);
    sptr<IWindowEventChannel> eventChannel(new WindowEventChannel(sceneSessionStage));
    return sceneSession_->Connect(sceneSessionStage, eventChannel);
}

WSError SceneSessionStage::Foreground()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    WSError res = sceneSession_->Foreground();
    if (res == WSError::WS_OK) {
        NotifyAfterForeground();
    }
    return res;
}

WSError SceneSessionStage::Background()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    NotifyAfterBackground();
    return WSError::WS_OK;
}

WSError SceneSessionStage::Disconnect()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return WSError::WS_OK;
}

WSError SceneSessionStage::StartScene(const SceneAbilityInfo& info, SessionOption sessionOption)
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sceneSession_->StartScene(info, sessionOption);
}

// TODO: may be delete
WSError SceneSessionStage::Minimize()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sceneSession_->Minimize();
}

// TODO: may be delete
WSError SceneSessionStage::Close()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sceneSession_->Close();
}

// TODO: may be delete
WSError SceneSessionStage::Recover()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sceneSession_->Recover();
}

// TODO: may be delete
WSError SceneSessionStage::Maximum()
{
    if (sceneSession_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sceneSession_->Maximum();
}
}

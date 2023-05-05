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

#include "session/container/include/scene_session_stage.h"

#include "session/container/include/window_event_channel.h"
#include "window_manager_hilog.h"
namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionStage" };
}

SceneSessionStage::SceneSessionStage(const sptr<ISession>& sceneSession) : SessionStage(sceneSession) {}

WSError SceneSessionStage::Connect()
{
    if (session_ == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    sptr<SceneSessionStage> sceneSessionStage(this);
    sptr<IWindowEventChannel> eventChannel(new WindowEventChannel(sceneSessionStage));
    return session_->Connect(sceneSessionStage, eventChannel);
}

WSError SceneSessionStage::Recover()
{
    if (session_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return session_->Recover();
}

WSError SceneSessionStage::Maximize()
{
    if (session_ == nullptr) {
        WLOGFE("sceneSession is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return session_->Maximize();
}
} // namespace OHOS::Rosen
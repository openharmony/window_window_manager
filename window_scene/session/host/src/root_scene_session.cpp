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

#include "session/host/include/root_scene_session.h"

namespace OHOS::Rosen {
void RootSceneSession::SetLoadContentFunc(const LoadContentFunc& loadContentFunc)
{
    loadContentFunc_ = loadContentFunc;
}

void RootSceneSession::LoadContent(
    const std::string& contentUrl, napi_env env, napi_value storage, AbilityRuntime::Context* context)
{
    if (loadContentFunc_) {
        loadContentFunc_(contentUrl, env, storage, context);
    }
}

WSRect RootSceneSession::GetSessionRectByType(AvoidAreaType type)
{
    if (specificCallback_ == nullptr || specificCallback_->onGetSceneSessionVectorByType_ == nullptr) {
        TLOGD(WmsLogTag::WMS_IMMS, "get scene session vector callback is not set");
        return {};
    }
    std::vector<sptr<SceneSession>> sessionVector;
    int dispId = GetSessionProperty()->GetDisplayId();
    switch (type) {
        case AvoidAreaType::TYPE_SYSTEM: {
            sessionVector = specificCallback_->onGetSceneSessionVectorByType_(
                WindowType::WINDOW_TYPE_STATUS_BAR, dispId);
            break;
        }
        case AvoidAreaType::TYPE_KEYBOARD: {
            sessionVector = specificCallback_->onGetSceneSessionVectorByType_(
                WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT, dispId);
            break;
        }
        default: {
            TLOGD(WmsLogTag::WMS_IMMS, "type %{public}u don't need to handle", type);
            return {};
        }
    }

    for (auto& statusBar : sessionVector) {
        if (!(statusBar->IsVisible())) {
            continue;
        }
        const WSRect rect = statusBar->GetSessionRect();
        TLOGI(WmsLogTag::WMS_IMMS, "type: %{public}u, rect %{public}s", type, rect.ToString().c_str());
        return rect;
    }
    TLOGD(WmsLogTag::WMS_IMMS, "no satisfied session rect found, type: %{public}u", type);
    return {};
}
} // namespace OHOS::Rosen

/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ANI_EMBEDDABLE_WINDOW_STAGE_H
#define OHOS_ANI_EMBEDDABLE_WINDOW_STAGE_H

#include "ani.h"
#include "session_info.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif

class AniEmbeddableWindowStage {
public:
    explicit AniEmbeddableWindowStage(sptr<Rosen::Window> window, sptr<AAFwk::SessionInfo> sessionInfo);
    void LoadContent(void* env, std::string content);
    sptr<Window> GetWindowImpl() { return windowExtensionSessionImpl_; }
    sptr<AAFwk::SessionInfo> GetSessionInfo() { return sessionInfo_; }
private:
    sptr<Window> windowExtensionSessionImpl_;
    sptr<AAFwk::SessionInfo> sessionInfo_ = nullptr;
};

ani_object CreateAniEmbeddableWindowStage(ani_env* env, sptr<Rosen::Window> window,
    sptr<AAFwk::SessionInfo> sessionInfo);
/* window scene obj stored in ANI */
AniEmbeddableWindowStage* GetEmbeddableWindowStageFromAni(void* aniObj);
void DropEmbeddableWindowStageByAni(ani_object obj);

}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_ANI_EMBEDDABLE_WINDOW_STAGE_H

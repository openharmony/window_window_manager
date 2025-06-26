/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_SCENE_SCREEN_CHANGE_LISTENER_H
#define OHOS_ROSEN_SCENE_SCREEN_CHANGE_LISTENER_H

#include "session/screen/include/screen_session.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class SceneScreenChangeListener : public IScreenChangeListener {
WM_DECLARE_SINGLE_INSTANCE(SceneScreenChangeListener)
public:
    void OnPropertyChange(
        const ScreenProperty& newProperty, ScreenPropertyChangeReason reason, ScreenId screenId) override;

private:
    void HandleRelativePositionChange(const ScreenProperty& newProperty, ScreenId screenId);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCENE_SCREEN_CHANGE_LISTENER_H

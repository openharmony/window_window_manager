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

#ifndef OHOS_ROSEN_WINDOW_SCENE_MOCK_UI_EFFECT_CONTROLLER_STUB_H
#define OHOS_ROSEN_WINDOW_SCENE_MOCK_UI_EFFECT_CONTROLLER_STUB_H
#include <gmock/gmock.h>

#include "../../session/host/include/ui_effect_controller_client_stub.h"

namespace OHOS {
namespace Rosen {
class UIEffectControllerClientStubMocker : public UIEffectControllerClientStub {
public:
    MOCK_METHOD(WMError, SetParams, (const sptr<UIEffectParams>& params), (override));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};
}
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MOCK_SUB_SESSION_H

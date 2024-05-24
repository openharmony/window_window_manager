/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WINDOW_STAGE_IMPL_H
#define WINDOW_STAGE_IMPL_H

#include "ffi_remote_data.h"
#include "window_scene.h"
#include "window_utils.h"

namespace OHOS {
namespace Rosen {
class CJWindowStageImpl : public OHOS::FFI::FFIData {
public:
    explicit CJWindowStageImpl(const std::shared_ptr<WindowScene>& windowScene): windowScene_(windowScene) {}
    int32_t GetMainWindow(int64_t& windowId);
    int32_t CreateSubWindow(std::string name, int64_t& windowId);
    RetStruct GetSubWindow();
    int32_t OnLoadContent(const std::string& contexUrl, const std::string& storageJson, bool isLoadedByName);
    int32_t DisableWindowDecor();
    int32_t SetShowOnLockScreen(bool showOnLockScreen);
    static std::shared_ptr<CJWindowStageImpl> CreateCJWindowStage(std::shared_ptr<WindowScene> windowScene);
private:
    std::weak_ptr<Rosen::WindowScene> windowScene_;
};
}
}
#endif // WINDOW_STAGE_IMPL_H

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "screen_manager.h"
#include "window_manager_hilog.h"
#include "display_manager_adapter.h"


#include <map>
#include <vector>

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "ScreenManager"};
}
class ScreenManager::Impl : public RefBase {
friend class ScreenManager;
private:
    Impl() = default;
    ~Impl() = default;

    std::map<ScreenId, sptr<Screen>> monitorMap_;
};
WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManager)

ScreenManager::ScreenManager()
{
    pImpl_ = new Impl();
}

ScreenManager::~ScreenManager()
{
}

sptr<Screen> ScreenManager::GetScreenById(ScreenId id)
{
    return nullptr;
}

std::vector<const sptr<Screen>> ScreenManager::GetAllScreens()
{
    std::vector<const sptr<Screen>> res;
    return res;
}

void ScreenManager::RegisterScreenListener(sptr<IScreenListener> listener)
{
}

ScreenId ScreenManager::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    return SCREEN_ID_INVALID;
}

ScreenId ScreenManager::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId)
{
    WLOGFI("create mirror for screen: %{public}" PRIu64"", mainScreenId);
    // TODO: "record screen" should use another function, "MakeMirror" should return group id.
    DMError result = SingletonContainer::Get<DisplayManagerAdapter>().MakeMirror(mainScreenId, mirrorScreenId);
    if (result == DMError::DM_OK) {
        WLOGFI("create mirror success");
    }
    return SCREEN_ID_INVALID;
}

ScreenId ScreenManager::CreateVirtualScreen(VirtualScreenOption option)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().CreateVirtualScreen(option);
}

DMError ScreenManager::DestroyVirtualScreen(ScreenId screenId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().DestroyVirtualScreen(screenId);
}
} // namespace OHOS::Rosen
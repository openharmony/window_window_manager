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

#include "abstract_screen_controller.h"

#include <cinttypes>
#include <screen_manager/screen_types.h>
#include <surface.h>

#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AbstractScreenController"};
}

AbstractScreenController::AbstractScreenController(std::recursive_mutex& mutex)
    : mutex_(mutex), rsInterface_(RSInterfaces::GetInstance())
{
    dmsScreenCount_ = 0;
    // TODO : PrepareRSScreenManger
}

AbstractScreenController::~AbstractScreenController()
{
}

std::vector<ScreenId> AbstractScreenController::GetAllScreenIds()
{
    std::vector<ScreenId> tmp;
    return tmp;
}

std::shared_ptr<std::vector<ScreenId>> AbstractScreenController::ConvertToRsScreenId(ScreenId dmsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::shared_ptr<std::vector<ScreenId>> empty = std::make_shared<std::vector<ScreenId>>();
    return empty;
}

std::shared_ptr<std::vector<ScreenId>> AbstractScreenController::ConvertToDmsScreenId(ScreenId rsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::shared_ptr<std::vector<ScreenId>> empty = std::make_shared<std::vector<ScreenId>>();
    return empty;
}

void AbstractScreenController::RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    abstractScreenCallback_ = cb;
}

void AbstractScreenController::PrepareRSScreenManger()
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        defaultScreenId_ = rsInterface_.GetDefaultScreenId();
    }
    rsInterface_.SetScreenChangeCallback(
        std::bind(&AbstractScreenController::OnRsScreenChange, this, std::placeholders::_1, std::placeholders::_2));
}

void AbstractScreenController::OnRsScreenChange(ScreenId rsScreenId, ScreenEvent screenEvent)
{
    ScreenId dmsScreenId = INVALID_SCREEN_ID;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (screenEvent == ScreenEvent::CONNECTED) {
        auto iter = rs2DmsScreenIdMap_.find(rsScreenId);
        if (iter == rs2DmsScreenIdMap_.end()) {
            dmsScreenId = dmsScreenCount_++;
            std::shared_ptr<std::vector<ScreenId>> dmsVector = std::make_shared<std::vector<ScreenId>>();
            dmsVector->push_back(dmsScreenId);
            rs2DmsScreenIdMap_.insert(std::make_pair(rsScreenId, dmsVector));
            std::shared_ptr<std::vector<ScreenId>> rsVector = std::make_shared<std::vector<ScreenId>>();
            dmsVector->push_back(rsScreenId);
            dms2RsScreenIdMap_.insert(std::make_pair(dmsScreenId, rsVector));
            sptr<AbstractScreen> absScreen = new AbstractScreen(dmsScreenId, rsScreenId);
            dmsScreenMap_.insert(std::make_pair(dmsScreenId, absScreen));
            // TODO: rsInterface_.GetScreenActiveMode()
            // TODO: rsInterface_.GetScreenSupportedModes()
            AddToGroup(absScreen);
        }
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        WLOGI("connect screen");
    } else {
        WLOGE("unknow message:%{public}ud", static_cast<uint8_t>(screenEvent));
    }
}

void AbstractScreenController::AddToGroup(sptr<AbstractScreen> newScreen)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = dmsScreenMap_.find(defaultScreenId_);
    if (iter == dmsScreenMap_.end()) {
        WLOGE("did not find default screen screenId:%{public}" PRIu64"", defaultScreenId_);
        AddAsFirstScreenLocked(newScreen);
    } else {
        sptr<AbstractScreen> defaultScreen = iter->second;
        AddAsSuccedentScreenLocked(newScreen);
    }
}

void AbstractScreenController::AddAsFirstScreenLocked(sptr<AbstractScreen> newScreen)
{
    // TODO: Create default display
}

void AbstractScreenController::AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen)
{
    // TODO: Mirror to default screen
}
} // namespace OHOS::Rosen
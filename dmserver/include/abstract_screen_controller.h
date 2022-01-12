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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H
#define FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H

#include <map>
#include <vector>

#include <refbase.h>
#include <surface.h>
#include <transaction/rs_interfaces.h>

#include "abstract_screen.h"

namespace OHOS::Rosen {
class AbstractScreenController : public RefBase {
using OnAbstractScreenCallback = std::function<void(ScreenId)>;
struct AbstractScreenCallback : public RefBase {
    OnAbstractScreenCallback onConnected_;
    OnAbstractScreenCallback onDisconnected_;
    OnAbstractScreenCallback onChanged_;
};

public:
    AbstractScreenController(std::recursive_mutex& mutex);
    ~AbstractScreenController();

    std::vector<ScreenId> GetAllScreenIds();
    std::shared_ptr<std::vector<ScreenId>> ConvertToRsScreenId(ScreenId dmsScreenId);
    std::shared_ptr<std::vector<ScreenId>> ConvertToDmsScreenId(ScreenId rsScreenId);
    void RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb);

    std::map<ScreenId, sptr<AbstractScreen>> abstractDisplayMap_;

private:
    void PrepareRSScreenManger();
    void OnRsScreenChange(ScreenId rsScreenId, ScreenEvent screenEvent);
    void AddToGroup(sptr<AbstractScreen> newScreen);
    void AddAsFirstScreenLocked(sptr<AbstractScreen> newScreen);
    void AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen);

    std::recursive_mutex& mutex_;
    RSInterfaces& rsInterface_;
    volatile ScreenId dmsScreenCount_;
    std::map<ScreenId, std::shared_ptr<std::vector<ScreenId>>> rs2DmsScreenIdMap_;
    std::map<ScreenId, std::shared_ptr<std::vector<ScreenId>>> dms2RsScreenIdMap_;
    std::map<ScreenId, sptr<AbstractScreen>> dmsScreenMap_;
    ScreenId defaultScreenId_ {INVALID_SCREEN_ID};
    sptr<AbstractScreenCallback> abstractScreenCallback_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H
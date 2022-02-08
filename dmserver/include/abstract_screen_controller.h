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

#include "screen.h"
#include "dm_common.h"
#include "abstract_screen.h"

namespace OHOS::Rosen {
class AbstractScreenController : public RefBase {
using OnAbstractScreenCallback = std::function<void(sptr<AbstractScreen>)>;
public:
    struct AbstractScreenCallback : public RefBase {
        OnAbstractScreenCallback onConnected_;
        OnAbstractScreenCallback onDisconnected_;
        OnAbstractScreenCallback onChanged_;
    };

    AbstractScreenController(std::recursive_mutex& mutex);
    ~AbstractScreenController();

    void Init();
    std::vector<ScreenId> GetAllScreenIds();
    sptr<AbstractScreen> GetAbstractScreen(ScreenId dmsScreenId);
    sptr<AbstractScreenGroup> GetAbstractScreenGroup(ScreenId dmsScreenId);
    ScreenId GetDefaultAbstractScreenId();
    ScreenId ConvertToRsScreenId(ScreenId dmsScreenId);
    ScreenId ConvertToDmsScreenId(ScreenId rsScreenId);
    void RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb);
    ScreenId CreateVirtualScreen(VirtualScreenOption option);
    DMError DestroyVirtualScreen(ScreenId screenId);
    bool IsScreenGroup(ScreenId screenId) const;
    bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId);

private:
    void OnRsScreenChange(ScreenId rsScreenId, ScreenEvent screenEvent);
    void ProcessScreenDisconnected(ScreenId rsScreenId);
    bool FillAbstractScreen(sptr<AbstractScreen>& absScreen, ScreenId rsScreenId);
    sptr<AbstractScreenGroup> AddToGroupLocked(sptr<AbstractScreen> newScreen);
    sptr<AbstractScreenGroup> RemoveFromGroupLocked(sptr<AbstractScreen> newScreen);
    bool CheckScreenInScreenGroup(sptr<AbstractScreen> newScreen) const;
    sptr<AbstractScreenGroup> AddAsFirstScreenLocked(sptr<AbstractScreen> newScreen);
    sptr<AbstractScreenGroup> AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen);

    std::recursive_mutex& mutex_;
    OHOS::Rosen::RSInterfaces *rsInterface_;
    std::atomic<ScreenId> dmsScreenCount_;
    // No AbstractScreenGroup
    std::map<ScreenId, ScreenId> rs2DmsScreenIdMap_;
    std::map<ScreenId, ScreenId> dms2RsScreenIdMap_;
    std::map<ScreenId, sptr<AbstractScreen>> dmsScreenMap_;
    std::map<ScreenId, sptr<AbstractScreenGroup>> dmsScreenGroupMap_;
    sptr<AbstractScreenCallback> abstractScreenCallback_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H
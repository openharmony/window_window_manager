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

#ifndef FOUNDATION_DM_SCREEN_MANAGER_H
#define FOUNDATION_DM_SCREEN_MANAGER_H

#include <refbase.h>
#include "screen.h"
#include "dm_common.h"
#include "screen_group.h"
#include "wm_single_instance.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class ScreenManager : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenManager);
friend class DMSDeathRecipient;
public:
    class IScreenListener : public virtual RefBase {
    public:
        virtual void OnConnect(ScreenId) = 0;
        virtual void OnDisconnect(ScreenId) = 0;
        virtual void OnChange(ScreenId) = 0;
    };

    class IScreenGroupListener : public virtual RefBase {
    public:
        virtual void OnChange(const std::vector<ScreenId>&, ScreenGroupChangeEvent) = 0;
    };

    class IVirtualScreenGroupListener : public virtual RefBase {
    public:
        struct ChangeInfo {
            ScreenGroupChangeEvent event;
            std::string trigger;
            std::vector<ScreenId> ids;
        };
        virtual void OnMirrorChange(const ChangeInfo& info) {}
    };

    sptr<Screen> GetScreenById(ScreenId screenId);
    sptr<ScreenGroup> GetScreenGroup(ScreenId groupId);
    DMError GetAllScreens(std::vector<sptr<Screen>>& screens);

    DMError MakeExpand(const std::vector<ExpandOption>& options, ScreenId& screenGroupId);
    DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId, ScreenId& screenGroupId);
    DMError RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens);
    ScreenId CreateVirtualScreen(VirtualScreenOption option);
    DMError DestroyVirtualScreen(ScreenId screenId);
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface);
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason);
    ScreenPowerState GetScreenPower(ScreenId screenId);
    DMError SetScreenRotationLocked(bool isLocked);
    DMError IsScreenRotationLocked(bool& isLocked);

    DMError RegisterScreenListener(sptr<IScreenListener> listener);
    DMError UnregisterScreenListener(sptr<IScreenListener> listener);
    DMError RegisterScreenGroupListener(sptr<IScreenGroupListener> listener);
    DMError UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener);
    DMError RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);
    DMError UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);
private:
    ScreenManager();
    ~ScreenManager();
    void OnRemoteDied();

    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_MANAGER_H
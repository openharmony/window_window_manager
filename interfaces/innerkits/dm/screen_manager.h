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
        /**
         * @brief Notify when a new screen is connected.
         */
        virtual void OnConnect(ScreenId) = 0;

        /**
         * @brief Notify when a screen is disconnected.
         */
        virtual void OnDisconnect(ScreenId) = 0;

        /**
         * @brief Notify when state of the screen is changed.
         */
        virtual void OnChange(ScreenId) = 0;
    };

    class IScreenGroupListener : public virtual RefBase {
    public:
        /**
         * @brief Notify when state of the screenGroup is changed.
         */
        virtual void OnChange(const std::vector<ScreenId>&, ScreenGroupChangeEvent) = 0;
    };

    class IVirtualScreenGroupListener : public virtual RefBase {
    public:
        struct ChangeInfo {
            ScreenGroupChangeEvent event;
            std::string trigger;
            std::vector<ScreenId> ids;
        };
        /**
         * @brief Notify when an event related to screen mirror occurs.
         */
        virtual void OnMirrorChange(const ChangeInfo& info) {}
    };

    /**
     * @brief Get the screen object by screen id.
     */
    sptr<Screen> GetScreenById(ScreenId screenId);

    /**
     * @brief Get the screen group object by groupId.
     */
    sptr<ScreenGroup> GetScreenGroup(ScreenId groupId);

    /**
     * @brief Get the All Screens object.
     */
    DMError GetAllScreens(std::vector<sptr<Screen>>& screens);

    /**
     * @brief Make screens as expand-screen.
     */
    DMError MakeExpand(const std::vector<ExpandOption>& options, ScreenId& screenGroupId);

    /**
     * @brief Make screens as mirror-screen
     */
    DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId, ScreenId& screenGroupId);
    
    /**
     * @brief Remove virtual screen from group.
     */
    DMError RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens);

    /**
     * @brief Create virtual screen.
     *
     * @param option Indicates the options of the virtual screen.
     */
    ScreenId CreateVirtualScreen(VirtualScreenOption option);

    /**
     * @brief Destroy virtual screen.
     *
     * @param screenId Indicates the screen id of the virtual screen.
     */
    DMError DestroyVirtualScreen(ScreenId screenId);

    /**
     * @brief Set surface for the virtual screen.
     */
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface);

    /**
     * @brief Set the screen power states for all screens.
     */
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason);

    /**
     * @brief Get screen power state.
     */
    ScreenPowerState GetScreenPower(ScreenId screenId);

    /**
     * @brief Set screen rotation lock status.
     */
    DMError SetScreenRotationLocked(bool isLocked);

    /**
     * @brief Get screen rotation lock status.
     */
    DMError IsScreenRotationLocked(bool& isLocked);

    /**
     * @brief Register screen listener.
     */
    DMError RegisterScreenListener(sptr<IScreenListener> listener);

    /**
     * @brief Unregister screen listener.
     */
    DMError UnregisterScreenListener(sptr<IScreenListener> listener);

    /**
     * @brief Register screen group listener.
     */
    DMError RegisterScreenGroupListener(sptr<IScreenGroupListener> listener);

    /**
     * @brief Unregister screen group listener.
     */
    DMError UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener);

    /**
     * @brief Register virtual screen group listener.
     */
    DMError RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);

    /**
     * @brief Unregister virtual screen group listener.
     */
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
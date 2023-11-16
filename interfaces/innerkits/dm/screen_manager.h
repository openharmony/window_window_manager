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
         *
         * @param info Change info of screen mirror.
         */
        virtual void OnMirrorChange([[maybe_unused]]const ChangeInfo& info) {}
    };

    /**
     * @brief Get the screen object by screen id.
     *
     * @param screenId Screen id.
     * @return Screen object.
     */
    sptr<Screen> GetScreenById(ScreenId screenId);

    /**
     * @brief Get the screen group object by groupId.
     *
     * @param groupId Screen group id.
     * @return ScreenGroup object.
     */
    sptr<ScreenGroup> GetScreenGroup(ScreenId groupId);

    /**
     * @brief Get all screens object.
     *
     * @param screens All screen objects.
     * @return DM_OK means get success, others means get failed.
     */
    DMError GetAllScreens(std::vector<sptr<Screen>>& screens);

    /**
     * @brief Make screens as expand-screen.
     *
     * @param options Option of expand.
     * @param screenGroupId Screen group id.
     * @return DM_OK means make expand success, others means make expand failed.
     */
    DMError MakeExpand(const std::vector<ExpandOption>& options, ScreenId& screenGroupId);

    /**
     * @brief Make screens as mirror-screen
     *
     * @param mainScreenId Main screen id.
     * @param mirrorScreenId Mirror screen ids.
     * @param screenGroupId Screen group id.
     * @return DM_OK means make mirror success, others means make mirror failed.
     */
    DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId, ScreenId& screenGroupId);

    /**
    * @brief Make screens as unique-screen.
    *
    * @param screenIds Unique screen ids.
    * @return DM_OK means make unique screen success, others means make unique failed.
    */
    DMError MakeUniqueScreen(const std::vector<ScreenId>& screenIds);

    /**
     * @brief Stop expand screens.
     *
     * @param expandScreenIds Expand screen ids.
     * @return DM_OK means stop expand success, others means stop expand failed.
     */
    DMError StopExpand(const std::vector<ScreenId>& expandScreenIds);

    /**
     * @brief Stop mirror screens.
     *
     * @param mirrorScreenIds Mirror screen ids.
     * @return DM_OK means stop mirror success, others means stop mirror failed.
     */
    DMError StopMirror(const std::vector<ScreenId>& mirrorScreenIds);

    /**
     * @brief Disable/enable global mirror screen.
     *
     * @param disableOrNot disable mirror screen or not.
     * @return DM_OK means disable or enable mirror success, others means failed.
     */
    DMError DisableMirror(bool disableOrNot);

    /**
     * @brief Remove virtual screen from group.
     *
     * @param screens Screen ids.
     * @return DM_OK means remove success, others means remove failed.
     */
    DMError RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens);

    /**
     * @brief Create virtual screen.
     *
     * @param option Indicates the options of the virtual screen.
     * @return Screen id.
     */
    ScreenId CreateVirtualScreen(VirtualScreenOption option);

    /**
     * @brief Destroy virtual screen.
     *
     * @param screenId Indicates the screen id of the virtual screen.
     * @return DM_OK means destroy success, others means destroy failed.
     */
    DMError DestroyVirtualScreen(ScreenId screenId);

    /**
     * @brief Set surface for the virtual screen.
     *
     * @param screenId Screen id.
     * @param surface Surface object.
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface);

    /**
     * @brief Resize virtual screen
     *
     * @param screenId the id of virtual screen to be resized.
     * @param width the new width.
     * @param height the new height.
     * @return DM_OK means set success, others means set failed.
     */
    DMError ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height);

    /**
     * @brief Set virtual screen refresh rate.
     *
     * @param screenId screen id.
     * @param refreshRate the new refresh rate.
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetVirtualScreenRefreshRate(ScreenId screenId, uint32_t refreshRate);

    /**
     * @brief Set buffer auto rotate
     *
     * @param screenId Screen id.
     * @param bufferRotation auto rotate
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetVirtualMirrorScreenBufferRotation(ScreenId screenId, bool bufferRotation);

    /**
     * @brief Set the screen power states for all screens.
     *
     * @param state Screen power state.
     * @param reason Reason for power state change.
     * @return True means set success, false means set failed.
     */
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason);

    /**
     * @brief Get screen power state.
     *
     * @param screenId Screen id.
     * @return Power state of screen.
     */
    ScreenPowerState GetScreenPower(ScreenId screenId);

    /**
     * @brief Set screen rotation lock status.
     *
     * @param isLocked True means forbide to rotate screen, false means the opposite.
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetScreenRotationLocked(bool isLocked);

    /**
     * @brief Get screen rotation lock status.
     *
     * @param isLocked Query the rotation lock status.
     * @return DM_OK means query success, others means query failed.
     */
    DMError IsScreenRotationLocked(bool& isLocked);

    /**
     * @brief Register screen listener.
     *
     * @param listener IScreenListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterScreenListener(sptr<IScreenListener> listener);

    /**
     * @brief Unregister screen listener.
     *
     * @param listener IScreenListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterScreenListener(sptr<IScreenListener> listener);

    /**
     * @brief Register screen group listener.
     *
     * @param listener IScreenGroupListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterScreenGroupListener(sptr<IScreenGroupListener> listener);

    /**
     * @brief Unregister screen group listener.
     *
     * @param listener IScreenGroupListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener);

    /**
     * @brief Register virtual screen group listener.
     *
     * @param listener IVirtualScreenGroupListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);

    /**
     * @brief Unregister virtual screen group listener.
     *
     * @param listener IVirtualScreenGroupListener.
     * @return DM_OK means unregister success, others means unregister failed.
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
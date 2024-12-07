/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "dm_common.h"
#include "wm_single_instance.h"
#include "screen_info.h"

namespace OHOS::Rosen {
class ScreenManagerLite : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenManagerLite);
friend class DMSDeathRecipientLite;
public:
    /*
     * used by powermgr
     */
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

    class IScreenModeChangeListener : public virtual RefBase {
    public:

        /**
         * @brief Notify when state of the screenMode is changed.
         */
        virtual void NotifyScreenModeChange(const std::vector<sptr<ScreenInfo>>&) = 0;
    };

    /**
     * @brief Register screen mode change listener.
     *
     * @param listener IScreenModeChangeListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener);

    /**
     * @brief Unregister screen listener.
     *
     * @param listener IScreenModeChangeListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterScreenModeChangeListener(sptr<IScreenModeChangeListener> listener);

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
     * @brief Set the screen power state on the specified screen.
     *
     * @param screenId Screen id.
     * @param state Screen power state.
     * @param reason Reason for power state change.
     * @return True means set success, false means set failed.
     */
    bool SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason);

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
private:
    ScreenManagerLite();
    ~ScreenManagerLite();
    void OnRemoteDied();

    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_MANAGER_H
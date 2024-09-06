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

namespace OHOS::Rosen {
class ScreenManagerLite : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenManagerLite);
friend class DMSDeathRecipient;
public:
    /*
     * used by powermgr
     */
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
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_MANAGER_H
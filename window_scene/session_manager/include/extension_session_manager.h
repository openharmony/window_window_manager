/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_MANAGER_H

#include <event_handler.h>
#include <iremote_object.h>
#include <map>
#include <mutex>
#include <refbase.h>
#include <unistd.h>
#include "interfaces/include/ws_common.h"
#include "session_manager_base.h"
#include "wm_single_instance.h"
namespace OHOS::Rosen {
class ExtensionSession;
class ExtensionSessionManager : public SessionManagerBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(ExtensionSessionManager)
public:
    /**
     * @brief create extension session
     *
     * @param sessionInfo sessionInfo, identify extension session instance
     * @return sptr<ExtensionSession> return session if create extension session success;Otherwise, return nullptr
     */
    sptr<ExtensionSession> RequestExtensionSession(const SessionInfo& sessionInfo);

    /**
     * @brief active extension session
     *
     * @param extensionSession the extension session need to be activated
     * @return WSError
     */
    WSError RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession);

    /**
     * @brief background extension session
     *
     * @param extensionSession the extension session need to be go background
     * @return WSError
     */
    WSError RequestExtensionSessionBackground(const sptr<ExtensionSession>& extensionSession);

    /**
     * @brief destroy extension session
     *
     * @param extensionSession the extension session need to be destroyed
     * @return WSError
     */
    WSError RequestExtensionSessionDestruction(const sptr<ExtensionSession>& extensionSession);
protected:
    ExtensionSessionManager();
    virtual ~ExtensionSessionManager() = default;

private:
    void Init();
    std::map<uint32_t, std::pair<sptr<ExtensionSession>, sptr<IRemoteObject>>> abilityExtensionMap_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_MANAGER_H

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

#ifndef OHOS_ROSEN_HIDUMP_CONTROLLER_H
#define OHOS_ROSEN_HIDUMP_CONTROLLER_H

#include "wm_single_instance.h"
#include "session/host/include/scene_session.h"

namespace OHOS {
namespace Rosen {
class HidumpController {
WM_DECLARE_SINGLE_INSTANCE(HidumpController)
public:
    void GetAllSessionDumpDetailedInfo(std::ostringstream& oss,
        const std::vector<sptr<SceneSession>>& allSession, const std::vector<sptr<SceneSession>>& backgroundSession);

private:
    void DumpSceneSessionParamList(std::ostringstream& oss);
    void DumpSceneSessionParam(std::ostringstream& oss, sptr<SceneSession> session);
    void DumpSessionParamList(std::ostringstream& oss);
    void DumpSessionParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpLayoutRectParamList(std::ostringstream& oss);
    void DumpLayoutRectParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpLayoutParamList(std::ostringstream& oss);
    void DumpLayoutParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpAbilityParamList(std::ostringstream& oss);
    void DumpAbilityParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpKeyboardParamList(std::ostringstream& oss);
    void DumpKeyboardParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpSysconfigParamList(std::ostringstream& oss);
    void DumpSysconfigParam(std::ostringstream& oss, sptr<SceneSession> session);
    void DumpLifeParamList(std::ostringstream& oss);
    void DumpLifeParam(std::ostringstream& oss, sptr<SceneSession> session);
    void DumpDisplayParamList(std::ostringstream& oss);
    void DumpDisplayParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpFocusParamList(std::ostringstream& oss);
    void DumpFocusParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpInputParamList(std::ostringstream& oss);
    void DumpInputParam(std::ostringstream& oss, sptr<SceneSession> session, sptr<WindowSessionProperty> property);
    void DumpLakeParamList(std::ostringstream& oss);
    void DumpLakeParam(std::ostringstream& oss, sptr<SceneSession> session);
    void DumpCOMParamList(std::ostringstream& oss);
    void DumpCOMParam(std::ostringstream& oss, sptr<SceneSession> session);
    void DumpVisibleParamList(std::ostringstream& oss);
    void DumpVisibleParam(std::ostringstream& oss, sptr<SceneSession> session);
};
}
}
#endif // OHOS_ROSEN_HIDUMP_CONTROLLER_H

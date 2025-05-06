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
#ifndef PIPWINDOW_ANI_H
#define PIPWINDOW_ANI_H

#include "ani.h"
#include "picture_in_picture_controller.h"

namespace OHOS {
namespace Rosen {

class PiPWindowAni {
public:
    static ani_status Init(ani_env* env, ani_namespace nsp);
    static ani_boolean isPiPEnabledAni(ani_env* env);
    sptr<PictureInPictureController> GetPiPController() { return pipControllerToken_; }
    
    static void startPiPAni(ani_env* env, ani_object obj, ani_long nativeObj);
    static void stopPiPAni(ani_env* env, ani_object obj, ani_long nativeObj);

private:
    void OnstartPiPAni(ani_env* env);
    void OnstopPiPAni(ani_env* env);
    sptr<PictureInPictureController> pipControllerToken_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif

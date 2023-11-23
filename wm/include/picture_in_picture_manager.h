/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_PICTURE_IN_PICTURE_MANAGER_H
#define OHOS_PICTURE_IN_PICTURE_MANAGER_H

#include <refbase.h>
#include "picture_in_picture_controller.h"
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class PictureInPictureManager {
public:
    PictureInPictureManager();
    ~PictureInPictureManager();
    static void PutPipControllerInfo(int32_t windowId, sptr<PictureInPictureController> pipController);
    static void RemovePipControllerInfo(int32_t windowId);

    static void SetPipWindowState(PipWindowState pipWindowState);
    static PipWindowState GetPipWindowState();

    static bool IsCurrentPipControllerExist();
    static bool IsCurrentPipController(wptr<PictureInPictureController> pipController);
    static void SetCurrentPipController(sptr<PictureInPictureController> pipController);
    static void RemoveCurrentPipController();
    static void RemoveCurrentPipControllerSafety();
    static void AttachActivePipController(sptr<PictureInPictureController> pipController);
    static void DetachActivePipController(sptr<PictureInPictureController> pipController);

    static bool IsAttachedPipWindow(uint32_t windowId);
    static sptr<Window> GetCurrentWindow();
    static bool IsActiveController(wptr<PictureInPictureController> pipController);

    static void DoRestore();
    static void DoClose(bool needAnim);
    static void DoStartMove();
    static void DoScale();
    static void DoActionEvent(std::string actionName);
    static void AutoStartPipWindow();
private:
    static sptr<PictureInPictureController> curPipController_;
    static sptr<PictureInPictureController> activePipController_;
    static std::map<int32_t, sptr<PictureInPictureController>> windowToControllerMap_;
    static std::mutex pipWindowStateMutex_;
    static PipWindowState pipWindowState_;
    static sptr<IWindowLifeCycle> mainWindowLifeCycleImpl_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_PICTURE_IN_PICTURE_MANAGER_H
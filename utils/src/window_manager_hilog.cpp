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

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
const std::unordered_map<WmsLogTag, const char *> DOMAIN_CONTENTS_MAP = {
    { WmsLogTag::DEFAULT, "WMS" },
    { WmsLogTag::DMS, "DMS" },
    { WmsLogTag::WMS_MAIN, "WMSMain" },
    { WmsLogTag::WMS_SUB, "WMSSub" },
    { WmsLogTag::WMS_SCB, "WMSScb" },
    { WmsLogTag::WMS_DIALOG, "WMSDialog" },
    { WmsLogTag::WMS_SYSTEM, "WMSSystem" },
    { WmsLogTag::WMS_FOCUS, "WMSFocus" },
    { WmsLogTag::WMS_LAYOUT, "WMSLayout" },
    { WmsLogTag::WMS_IMMS, "WMSImms" },
    { WmsLogTag::WMS_LIFE, "WMSLife" },
    { WmsLogTag::WMS_KEYBOARD, "WMSKeyboard" },
    { WmsLogTag::WMS_EVENT, "WMSEvent" },
    { WmsLogTag::WMS_UIEXT, "WMSUiext" },
    { WmsLogTag::WMS_PIP, "WMSPiP" },
    { WmsLogTag::WMS_RECOVER, "WMSRecover" },
    { WmsLogTag::WMS_MULTI_USER, "WMSMultiUser" },
    { WmsLogTag::WMS_TOAST, "WMSToast" },
    { WmsLogTag::WMS_MULTI_WINDOW, "WMSMultiWindow" },
    { WmsLogTag::WMS_INPUT_KEY_FLOW, "InputKeyFlow" },
    { WmsLogTag::WMS_PIPELINE, "WMSPipeLine" },
    { WmsLogTag::WMS_HIERARCHY, "WMSHierarchy" },
};
} // namespace OHOS
}
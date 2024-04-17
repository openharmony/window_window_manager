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

#ifndef WINDOW_WINDOW_MANAGER_DMS_REPORTER_H
#define WINDOW_WINDOW_MANAGER_DMS_REPORTER_H

#include <cstdint>
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class DmsReporter {
WM_DECLARE_SINGLE_INSTANCE(DmsReporter);

public:
    void ReportRegisterSessionListener(bool isSuccess, int32_t errCode);
    void ReportQuerySessionInfo(bool isSuccess, int32_t errCode);
    void ReportContinueApp(bool isSuccess, int32_t errCode);
};
} // namespace OHOS::Rosen

#endif // WINDOW_WINDOW_MANAGER_DMS_REPORTER_H

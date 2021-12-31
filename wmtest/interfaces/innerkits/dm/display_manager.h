/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_H
#define FOUNDATION_DM_DISPLAY_MANAGER_H

#include <refbase.h>
#include <vector>

#include "display.h"
#include "single_instance.h"

namespace OHOS::Rosen {
class DisplayManagerAdapter;

class DisplayManager : public RefBase {
DECLARE_SINGLE_INSTANCE_BASE(DisplayManager);
public:
    const sptr<Display>& GetDisplay(const DisplayType type);
    std::vector<const sptr<Display>> GetAllDisplays();

    DisplayId GetDefaultDisplayId();
    const sptr<Display> GetDefaultDisplay();

    const sptr<Display> GetDisplayById(DisplayId displayId);

    std::vector<DisplayId> GetAllDisplayIds();

private:
    DisplayManager();
    ~DisplayManager();
    sptr<DisplayManagerAdapter> dmsAdapter_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_MANAGER_H
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

#include "dm_native_test.h"

#include <cstdio>
#include <inttypes.h>
#include <unistd.h>

#include "wm_common.h"
#include "display_manager.h"
#include "singleton_container.h"

using namespace OHOS::Rosen;

namespace {
DMNativeTest g_autoload;
} // namespace

std::string DMNativeTest::GetDescription() const
{
    constexpr const char *desc = "normal display";
    return desc;
}

std::string DMNativeTest::GetDomain() const
{
    constexpr const char *desc = "dmclient";
    return desc;
}

int32_t DMNativeTest::GetID() const
{
    constexpr int32_t id = 1;
    return id;
}

uint32_t DMNativeTest::GetLastTime() const
{
    constexpr uint32_t lastTime = LAST_TIME_FOREVER;
    return lastTime;
}

void DMNativeTest::Run(int32_t argc, const char **argv)
{
    printf("DMNativeTest run begin\n");
    sptr<DisplayManager> dms = DisplayManager::GetInstance();
    if (dms == nullptr) {
        printf("dms error!\n");
        return;
    }

    DisplayId displayId = dms->GetDefaultDisplayId();
    printf("defaultDisplayId: %" PRIu64"\n", displayId);

    auto display = dms->GetDefaultDisplay();
    if (display == nullptr) {
        printf("GetDefaultDisplay: failed!\n");
    } else {
        printf("GetDefaultDisplay: id %" PRIu64", w %d, h %d, fps %u\n", display->GetId(), display->GetWidth(),
            display->GetHeight(), display->GetFreshRate());
    }

    auto ids = dms->GetAllDisplayIds();
    for (auto id: ids) {
        display = dms->GetDisplayById(displayId);
        if (display == nullptr) {
            printf("GetDisplayById(%" PRIu64"): failed!\n", id);
        } else {
            printf("GetDisplayById(%" PRIu64"): id %" PRIu64", w %d, h %d, fps %u\n", id, display->GetId(), display->GetWidth(),
                display->GetHeight(), display->GetFreshRate());
        }
    }

    auto displays = dms->GetAllDisplays();
    for (auto disp: displays) {
        if (disp == nullptr) {
            printf("GetAllDisplays: failed!\n");
        } else {
            printf("GetAllDisplays: id %" PRIu64", w %d, h %d, fps %u\n", disp->GetId(), disp->GetWidth(),
                disp->GetHeight(), disp->GetFreshRate());
        }
    }

    printf("DMNativeTest run finish\n");
}

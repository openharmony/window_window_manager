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

#include <cstdio>

#include "snapshot_utils.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    CmdArgments cmdArgments;
    cmdArgments.fileName = "/data/snapshot_display_1.png";

    if (!SnapShotUtils::ProcessArgs(argc, argv, cmdArgments)) {
        return 0;
    }

    // get PixelMap from DisplayManager API
    auto pixelMap = DisplayManager::GetInstance().GetScreenshot(cmdArgments.displayId);
    bool ret = false;
    if (pixelMap != nullptr) {
        ret = SnapShotUtils::WriteToPngWithPixelMap(cmdArgments.fileName, *pixelMap);
    }
    if (!ret) {
        printf("error: snapshot display %" PRIu64 ", write to %s as png failed!\n",
            cmdArgments.displayId, cmdArgments.fileName.c_str());
        return -1;
    }

    printf("success: snapshot display %" PRIu64 ", write to %s as png\n",
        cmdArgments.displayId, cmdArgments.fileName.c_str());
    return 0;
}
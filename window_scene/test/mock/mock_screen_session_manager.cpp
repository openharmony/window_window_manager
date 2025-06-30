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
 
#ifndef MOCK_SCREEN_SESSION_MANAGER_H
#define MOCK_SCREEN_SESSION_MANAGER_H

#include "screen_session_manager/include/screen_session_manager.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class MockScreenSessionManager : public ScreenSessionManager {
public:
    MockScreenSessionManager() {};
    ~MockScreenSessionManager() {};

    MOCK_METHOD4(GetDisplayHDRSnapshot, std::vector<std::shared_ptr<Media::PixelMap>>(DisplayId displayId,
        DmErrorCode* errorCode, bool isUseDma, bool isCaptureFullOfScreen));
    MOCK_METHOD2(GetDisplayHDRSnapshotWithOption, std::vector<std::shared_ptr<Media::PixelMap>>(
        const CaptureOption& captureOption, DmErrorCode* errorCode));
};
}
}
#endif // MOCK_SCREEN_SESSION_MANAGER_H
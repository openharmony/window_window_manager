/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DM_SCREEN_RECORDER_H
#define FOUNDATION_DM_SCREEN_RECORDER_H

#include <string>

#include <event_handler.h>
#include "screen.h"
#include "surface_reader.h"
#include "surface_reader_handler_impl.h"

namespace OHOS::Rosen {
class ScreenRecorder : public RefBase {
public:
    ~ScreenRecorder();
    explicit ScreenRecorder(sptr<Screen> screen);
    ScreenId GetId() const;
    sptr<Surface> GetInputSurface() const;
    void Record();
    DMError Start(std::vector<int> fds);
    DMError Stop();
private:
    sptr<Screen> screen_;
    SurfaceReader surfaceReader_;
    sptr<SurfaceReaderHandlerImpl> surfaceReaderHandler_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    std::vector<int> fds_;
    int fileIndex_ { 0 };
    bool isStarting_ { false };
    bool stop_ { false };
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_RECORDER_H
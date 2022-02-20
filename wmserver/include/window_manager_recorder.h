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

#ifndef OHOS_ROSEN_WINDOW_MANAGER_RECORDER_H
#define OHOS_ROSEN_WINDOW_MANAGER_RECORDER_H

#include <refbase.h>
#include "wm_common.h"
#include "window_node.h"

namespace OHOS {
namespace Rosen {
enum class RecordReason : uint32_t {
    NORMAL,
    ADD_WINDOW,
    UPDATE_WINDOW,
    REMOVE_WINDOW,
    RAISE_WINDOW,
    SWITCH_LAYOUT,
    OTHERS,
};

struct WindowManagerRecordInfo {
    uint32_t id;
    uint32_t flag;
    Rect rect;
    WindowType type;
    WindowMode mode;
    std::string name;
    RecordReason reason = RecordReason::NORMAL;
    struct tm recordTime = {0};
};

class WindowManagerRecorder : public RefBase {
public:
    WindowManagerRecorder() = default;
    ~WindowManagerRecorder() = default;

    void AddNodeRecord(WindowManagerRecordInfo record);
    void AddTreeRecord(const std::vector<sptr<WindowNode>> &windowNodes);
    void Clear();
    void DumpWindowRecord(std::vector<std::string>& records);
    std::string GetCurrentWindowTree();

private:
    std::string GetFormatString(std::string srcStr);

private:
    std::string curWindowTree_;
    std::vector<std::string> records_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_MANAGER_RECORD_H
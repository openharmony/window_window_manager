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
 
#ifndef OHOS_ROSEN_WINDOW_SCENE_FFRT_HELPER_H
#define OHOS_ROSEN_WINDOW_SCENE_FFRT_HELPER_H

#include <functional>
#include <string>

namespace OHOS::Rosen {
class TaskHandleMap;

/**
 * @brief Enumerates FFRT Qos translation
 */
enum class TaskQos {
    INHERIT = 0,
    BACKGROUND,
    UTILITY,
    DEFAULT,
    USER_INITIATED,
    DEADLINE_REQUEST,
    USER_INTERACTIVE
};

class WSFFRTHelper {
public:
    WSFFRTHelper();
    ~WSFFRTHelper();
    void SubmitTask(std::function<void()>&& task, const std::string& taskName, uint64_t delayTime = 0,
        TaskQos qos = TaskQos::USER_INTERACTIVE);
    void CancelTask(const std::string& taskName);
    bool IsTaskExisted(const std::string& taskName) const;
    std::size_t CountTask() const;

private:
    std::unique_ptr<TaskHandleMap> taskHandleMap_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_FFRT_HELPER_H
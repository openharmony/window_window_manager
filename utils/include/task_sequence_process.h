/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef TASKSEQUENCEPROCESS_H
#define TASKSEQUENCEPROCESS_H

#include <queue>
#include <mutex>
#include <atomic>

struct EventInfo {
    int taskId; // 示例任务ID，可添加其他字段
};

class TaskSequenceProcess {
private:
    std::atomic<bool> taskRunningFlag;
    size_t maxQueueSize;
    std::queue<EventInfo> taskQueue;
    std::mutex queueMutex;

    // 任务执行完成后的回调处理
    void taskCallback(const EventInfo& node);

public:
    TaskSequenceProcess(size_t maxSize = 1);
    void push(const EventInfo& eventInfo);
    void notify();
    void exec(const EventInfo& eventInfo);
    size_t getQueueSize() const;
    size_t getMaxQueueSize() const;
    void setMaxQueueSize(size_t newSize);
};
#endif // TASKSEQUENCEPROCESS_H
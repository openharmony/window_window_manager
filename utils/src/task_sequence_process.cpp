/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "TaskSequenceProcess.h"
#include <thread>

namespace OHOS::Rosen {
TaskSequenceProcess::TaskSequenceProcess(size_t maxSize)
    : taskRunningFlag(false), maxQueueSize(maxSize) {}

// 推送任务到队列的实现
void TaskSequenceProcess::push(const EventInfo& eventInfo) {
    std::lock_guard<std::mutex> lock(queueMutex);
    
    // 队列超过最大长度时移除队首元素
    if (taskQueue.size() >= maxQueueSize) {
        taskQueue.pop();
    }
    
    taskQueue.push(eventInfo);
}

// 通知执行任务的实现
void TaskSequenceProcess::notify() {
    std::lock_guard<std::mutex> lock(queueMutex);
    
    // 有任务执行或队列为空时直接返回
    if (taskRunningFlag || taskQueue.empty()) {
        return;
    }

    // 取出队首任务并执行
    EventInfo task = taskQueue.front();
    taskQueue.pop();
    taskRunningFlag = true;
    exec(task);
}

// 任务执行的实现
void TaskSequenceProcess::exec(const EventInfo& eventInfo) {
    // 异步执行任务（实际场景可替换为线程池、异步框架等）
    std::thread([this, eventInfo]() {
        // 此处添加具体的任务执行逻辑
        // 示例：模拟任务执行耗时
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // 任务完成后调用回调
        taskCallback(eventInfo);
    }).detach();
}

// 任务回调的实现
void TaskSequenceProcess::taskCallback(const EventInfo& node) {
    taskRunningFlag = false; // 重置运行标志
    notify();                // 触发下一个任务执行
}

// 获取当前队列大小的实现
size_t TaskSequenceProcess::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return taskQueue.size();
}

// 获取最大队列大小的实现
size_t TaskSequenceProcess::getMaxQueueSize() const {
    return maxQueueSize;
}

// 设置最大队列大小的实现
void TaskSequenceProcess::setMaxQueueSize(size_t newSize) {
    std::lock_guard<std::mutex> lock(queueMutex);
    maxQueueSize = newSize;
    // 若新大小小于当前队列大小，截断队列
    while (taskQueue.size() > maxQueueSize) {
        taskQueue.pop();
    }
}
} // namespace OHOS::Rosen
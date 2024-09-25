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

#include "publish/scb_dump_subscriber.h"

#include <sstream>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {

void ScbDumpSubscriber::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    std::ostringstream oss;
    oss << data.GetData() << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    dumpInfo_ = oss.str();
    valueReady_ = true;
    cv_.notify_all();
}

std::string ScbDumpSubscriber::GetDebugDumpInfo(const std::chrono::milliseconds& time)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (cv_.wait_for(lock, time, [&] { return valueReady_; })) {
        return dumpInfo_;
    }
    return "timeout";
}

WSError ScbDumpSubscriber::Publish(const std::string& cmd)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        valueReady_ = false;
    }

    AAFwk::Want want;
    want.SetAction("com.ohos.sceneboard.debug.event.listener");
    EventFwk::CommonEventData commonEventData;
    commonEventData.SetWant(want);
    commonEventData.SetCode(0);
    commonEventData.SetData(cmd);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetSticky(false);
    publishInfo.SetOrdered(false);
    publishInfo.SetBundleName("com.ohos.sceneboard");

    // publish the common event
    bool ret = EventFwk::CommonEventManager::PublishCommonEvent(commonEventData, publishInfo, nullptr);
    if (!ret) {
        TLOGE(WmsLogTag::DEFAULT, "publish scene debug event error.");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    return WSError::WS_OK;
}

std::shared_ptr<ScbDumpSubscriber> ScbDumpSubscriber::Subscribe()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent("com.ohos.sceneboard.debug.event.response");
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPublisherBundleName("com.ohos.sceneboard");
    auto scbSubscriber = std::make_shared<ScbDumpSubscriber>(subscribeInfo);
    EventFwk::CommonEventManager::SubscribeCommonEvent(scbSubscriber);
    return scbSubscriber;
}

void ScbDumpSubscriber::UnSubscribe(const std::shared_ptr<ScbDumpSubscriber>& scbSubscriber)
{
    if (scbSubscriber) {
        EventFwk::CommonEventManager::UnSubscribeCommonEvent(scbSubscriber);
    }
}

} // namespace OHOS::Rosen
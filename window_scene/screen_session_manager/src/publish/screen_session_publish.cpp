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

#include "publish/screen_session_publish.h"

#include <common_event_manager.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
const std::string CAST_PLUG_IN_FLAG_DATA = "1";
const std::string CAST_PLUG_OUT_FLAG_DATA = "0";
const std::string COMMON_EVENT_DISPLAY_ROTATION_CHANGED = "usual.event.dms.rotation_changed";
const std::string COMMON_EVENT_CAST_PLUGGED_CHANGED = "usual.event.dms.cast_plugged_changed";
const std::string COMMON_EVENT_SMART_NOTIFICATION = "hicare.event.SMART_NOTIFICATION";
const std::string COMMON_EVENT_LOW_TEMP_WARNING = "usual.event.thermal.LOW_TEMP_WARNING";
constexpr int32_t PUBLISH_SUCCESS = 0;
constexpr int32_t PUBLISH_FAILURE = -1;
constexpr int32_t TRANS_CODE_CAST_PLUGGED_CHANGED = 0;
constexpr int32_t TRANS_CODE_ROTATION_CHANGED = 1005;
constexpr int32_t lOW_TEMP_UID = 1096;
constexpr int32_t RECEIVE_FAILURE = -1;

std::map<std::string, sptr<EventFwk::Want>> ScreenSessionPublish::cesWantMap_ = {
    {COMMON_EVENT_CAST_PLUGGED_CHANGED, nullptr},
    {COMMON_EVENT_DISPLAY_ROTATION_CHANGED, nullptr},
    {COMMON_EVENT_SMART_NOTIFICATION, nullptr}
};

ScreenSessionPublish::~ScreenSessionPublish()
{
    TLOGI(WmsLogTag::DMS_SSM, "destory");
    UnRegisterLowTempSubscriber();
}

ScreenSessionPublish &ScreenSessionPublish::GetInstance()
{
    static ScreenSessionPublish screenSessionPublish;
    return screenSessionPublish;
}

void ScreenSessionPublish::InitPublishEvents()
{
    if (publishInfo_ != nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "ScreenSessionPublish has been initialized");
        return;
    }
    publishInfo_ = new (std::nothrow) EventFwk::CommonEventPublishInfo();
    if (publishInfo_ == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "publishInfo new failed");
        return;
    }
    publishInfo_->SetOrdered(false);
    for (auto &[event, want] : cesWantMap_) {
        want = new (std::nothrow) EventFwk::Want();
        if (want == nullptr) {
            TLOGE(WmsLogTag::DMS_SSM, "common event: %{publish}s new want failed", event.c_str());
            continue;
        }
        want->SetAction(event);
    }
}

int32_t ScreenSessionPublish::PublishEvents(
    const EventFwk::CommonEventData& eventData, std::string bundleName)
{
    if (publishInfo_ == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "publishInfo is nullptr");
        return PUBLISH_FAILURE;
    }
    if (bundleName != "") {
        publishInfo_->SetBundleName(bundleName);
    }
    bool ret = EventFwk::CommonEventManager::PublishCommonEvent(eventData, *publishInfo_, nullptr);
    if (!ret) {
        TLOGE(WmsLogTag::DMS_SSM, "PublishCommonEvent failed");
        return PUBLISH_FAILURE;
    }
    TLOGI(WmsLogTag::DMS_SSM, "PublishCommonEvent succeed");
    return PUBLISH_SUCCESS;
}

void ScreenSessionPublish::PublishCastPluggedEvent(const bool& isEnable)
{
    TLOGI(WmsLogTag::DMS_SSM, "start to publish cast plugged event");
    EventFwk::CommonEventData eventData;
    eventData.SetCode(TRANS_CODE_CAST_PLUGGED_CHANGED);
    if (isEnable) {
        eventData.SetData(CAST_PLUG_IN_FLAG_DATA);
    } else {
        eventData.SetData(CAST_PLUG_OUT_FLAG_DATA);
    }
    auto want = cesWantMap_[COMMON_EVENT_CAST_PLUGGED_CHANGED];
    if (want == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "want is nullptr");
        return;
    }
    eventData.SetWant(*want);
    int32_t ret = PublishEvents(eventData);
    if (ret != PUBLISH_SUCCESS) {
        TLOGE(WmsLogTag::DMS_SSM, "PublishEvents failed");
        return;
    }
    TLOGI(WmsLogTag::DMS_SSM, "end of publish cast plugged event");
}

void ScreenSessionPublish::PublishCastPlugInEvent()
{
    TLOGI(WmsLogTag::DMS_SSM, "start to publish cast plug in event");
    PublishCastPluggedEvent(true);
}

void ScreenSessionPublish::PublishCastPlugOutEvent()
{
    TLOGI(WmsLogTag::DMS_SSM, "start to publish cast plug out event");
    PublishCastPluggedEvent(false);
}

void ScreenSessionPublish::PublishDisplayRotationEvent(
    const ScreenId& screenId, const Rotation& displayRotation)
{
    TLOGI(WmsLogTag::DMS,
        "start to publish display rotation event, screenId: %{public}d, displayRotation: %{public}d",
        static_cast<int32_t>(screenId), static_cast<int32_t>(displayRotation));
    EventFwk::CommonEventData eventData;
    eventData.SetCode(TRANS_CODE_ROTATION_CHANGED);
    auto want = cesWantMap_[COMMON_EVENT_DISPLAY_ROTATION_CHANGED];
    if (want == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "want is nullptr");
        return;
    }
    want->SetParam("screenid", static_cast<int32_t>(screenId));
    want->SetParam("rotation", static_cast<int32_t>(displayRotation));
    eventData.SetWant(*want);
    int32_t ret = PublishEvents(eventData);
    if (ret != PUBLISH_SUCCESS) {
        TLOGE(WmsLogTag::DMS_SSM, "PublishEvents failed");
        return;
    }
    TLOGI(WmsLogTag::DMS_SSM, "end of publish display rotation event");
}

void ScreenSessionPublish::PublishSmartNotificationEvent(const std::string& faultDesc, const std::string& faultSuggest)
{
    EventFwk::CommonEventData eventData;
    auto want = cesWantMap_[COMMON_EVENT_SMART_NOTIFICATION];
    if (want == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "want is nullptr");
        return;
    }
    eventData.SetWant(*want);
    std::string eventDataStr = "{\"faultDescription\":\"" + faultDesc + "\"," +
        "\"faultSuggestion\":\"" + faultSuggest + "\"}";
    eventData.SetData(eventDataStr);
    int32_t ret = PublishEvents(eventData);
    if (ret != PUBLISH_SUCCESS) {
        TLOGE(WmsLogTag::DMS_SSM, "PublishEvents failed");
        return;
    }
    TLOGI(WmsLogTag::DMS_SSM, "end event, send massage is %{public}s", eventDataStr.c_str());
}

bool ScreenSessionPublish::RegisterLowTempSubscriber()
{
    if (subscriber_ != nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "low temp is registered");
        return false;
    }
    EventFwk::MatchingSkills lowTempSkills = EventFwk::MatchingSkills();
    lowTempSkills.AddEvent(COMMON_EVENT_LOW_TEMP_WARNING);
    EventFwk::CommonEventSubscribeInfo lowTempInfo(lowTempSkills);
    lowTempInfo.SetPublisherUid(lOW_TEMP_UID);
    lowTempInfo.SetThreadMode(EventFwk::CommonEventSubscribeInfo::COMMON);
    subscriber_ = std::make_shared<EventSubscriber>(lowTempInfo);
    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_)) {
        TLOGE(WmsLogTag::DMS_SSM, "subscribe common event:%{public}s failed!", COMMON_EVENT_LOW_TEMP_WARNING.c_str());
        return false;
    }
    TLOGI(WmsLogTag::DMS_SSM, "subscribe common event:%{public}s success.", COMMON_EVENT_LOW_TEMP_WARNING.c_str());
    return true;
}

bool ScreenSessionPublish::UnRegisterLowTempSubscriber()
{
    if (subscriber_ == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM, "subscriber_ is nullptr");
        return false;
    }
    EventFwk::MatchingSkills lowTempSkills = EventFwk::MatchingSkills();
    lowTempSkills.RemoveEvent(COMMON_EVENT_LOW_TEMP_WARNING);
    if (!EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_)) {
        TLOGE(WmsLogTag::DMS_SSM, "unsubscribe common event:%{public}s failed!", COMMON_EVENT_LOW_TEMP_WARNING.c_str());
        return false;
    }
    TLOGI(WmsLogTag::DMS_SSM, "unsubscribe common event:%{public}s success.", COMMON_EVENT_LOW_TEMP_WARNING.c_str());
    return true;
}

void ScreenSessionPublish::EventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    std::string action = data.GetWant().GetAction();
    if (action == COMMON_EVENT_LOW_TEMP_WARNING) {
        int32_t lowTemp = data.GetWant().GetIntParam("lowTempWarning", RECEIVE_FAILURE);
        ScreenSessionManager::GetInstance().SetLowTemp(static_cast<LowTempMode>(lowTemp));
        TLOGI(WmsLogTag::DMS_SSM, "receive common event:%{public}s sucess, lowTempWarning is:%{public}d",
            COMMON_EVENT_LOW_TEMP_WARNING.c_str(), lowTemp);
    }
}
} // namespace OHOS::Rosen
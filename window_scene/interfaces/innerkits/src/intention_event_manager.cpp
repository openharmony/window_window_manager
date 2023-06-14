/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "intention_event_manger.h"

#include "intention_event_impl.h"

namespace OHOS {
namespace Rosen {

IntentionEventManager *IntentionEventManager::instance_ = new (std::nothrow) IntentionEventManager();

IntentionEventManager *IntentionEventManager::GetInstance()
{
    return instance_;
}

void IntentionEventManager::SetAnrObserver(sptr<IAnrObserver> observer)
{
    INTENTION_EV_IMPL->SetAnrObserver(observer);
}
} // namespace Rosen
} // namespace OHOS

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

#include "session/host/include/sub_session.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SubSession" };
} // namespace

SubSession::SubSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SceneSession(info, specificCallback)
{
    WLOGFD("Create SubSession");
}

SubSession::~SubSession()
{
    WLOGD("~SubSession, id: %{public}d", GetPersistentId());
}
} // namespace OHOS::Rosen

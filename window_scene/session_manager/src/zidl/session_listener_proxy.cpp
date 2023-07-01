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

#include "session_listener_proxy.h"

namespace OHOS::Rosen {

void SessionListenerProxy::OnSessionLabelChange(int persistentId, const std::string &label) {}
void SessionListenerProxy::OnSessionIconChange(int persistentId, const std::shared_ptr<Media::PixelMap> &icon) {}

} // namespace OHOS::Rosen
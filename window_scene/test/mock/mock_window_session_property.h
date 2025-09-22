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
#ifndef MOCK_WINDOW_SESSION_PROPERTY_H
#define MOCK_WINDOW_SESSION_PROPERTY_H

#include "interfaces/include/ws_common.h"
#include "common/include/window_session_property.h"
#include <gmock/gmock.h>
 
namespace OHOS {
namespace Rosen {
class WindowSessionPropertyMocker : public WindowSessionProperty {
public:
    MOCK_METHOD0(GetDisplayId, uint64_t());
};
} // namespace Rosen
} // namespace OHOS
#endif // MOCK_WINDOW_SESSION_PROPERTY_H
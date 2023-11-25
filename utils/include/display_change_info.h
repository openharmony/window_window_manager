/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef OHOS_ROSEN_DISPLAY_CHANGFE_INFO_H
#define OHOS_ROSEN_DISPLAY_CHANGFE_INFO_H

#include <parcel.h>

#include "dm_common.h"

namespace OHOS {
namespace Rosen {

enum class ActionType : uint32_t {
    SINGLE_START,
    SINGLE_CLOSE,
    MOVE_DISPLAY,
};

class DisplayChangeInfo : public Parcelable {
public:
    DisplayChangeInfo() = default;
    ~DisplayChangeInfo() = default;
    virtual bool Marshalling(Parcel& parcel) const override;
    static DisplayChangeInfo* Unmarshalling(Parcel& parcel);

    ActionType action_;
    ScreenId toScreenId_ = SCREEN_ID_INVALID;
    ScreenId fromScreenId_ = SCREEN_ID_INVALID;
    std::string abilityName_;
    std::string bundleName_;
};
}
}
#endif // OHOS_ROSEN_DISPLAY_CHANGFE_INFO_H
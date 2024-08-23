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

#ifndef OHOS_ROSEN_WINDOW_PID_VISIBILITY_INFO_H
#define OHOS_ROSEN_WINDOW_PID_VISIBILITY_INFO_H

#include "wm_common.h"

namespace OHOS::Rosen {
/**
 * @enum WindowVisibilityState
 *
 * @brief Visibility state of a window
 */
enum WindowPidVisibilityState : uint32_t {
    VISIBILITY_STATE,
    INVISIBILITY_STATE,
};

class WindowPidVisibilityInfo : public Parcelable {
public:
    WindowPidVisibilityInfo() = default;
    /**
     * @brief Construct of WindowPidVisibilityInfo.
     *
     * @param pid Process id.
     * @param visibility True means window is visible, false means the opposite.
     */
    WindowPidVisibilityInfo(int32_t pid, WindowPidVisibilityState visibilityState) : pid_(pid),
        visibilityState_(visibilityState) {};

    ~WindowPidVisibilityInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;

    static WindowPidVisibilityInfo* Unmarshalling(Parcel& parcel);

    int32_t pid_ { 0 };
    WindowPidVisibilityState visibilityState_ = INVISIBILITY_STATE;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_PID_VISIBILITY_INFO_H
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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_INFO_H
#define OHOS_ROSEN_WINDOW_LAYOUT_INFO_H

#include "wm_common.h"

namespace OHOS::Rosen {
    /*
     * @class WindowLayoutInfo
     *
     * @brief Info for all windos on the screen.
     */
class WindowLayoutInfo : public Parcelable {
public:
    /*
     * @brief Default construct of WindowLayoutInfo.
     */
    WindowLayoutInfo() = default;

    /*
     * @brief Construct of WindowLayoutInfo.
     *
     * @param rect rect of the window, { posX, posY, width, height }
     */
    WindowLayoutInfo(Rect rect) : rect_(rect) {};

    ~WindowLayoutInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;

    static WindowLayoutInfo* Unmarshalling(Parcel& parcel);

    Rect rect_ = { 0, 0, 0, 0 };
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_LAYOUT_INFO_H
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

#ifndef OHOS_ROSEN_WINDOW_DRAWING_CONTENT_INFO_H
#define OHOS_ROSEN_WINDOW_DRAWING_CONTENT_INFO_H

#include "wm_common.h"

namespace OHOS::Rosen {
/**
 * @class WindowDrawingContentInfo
 *
 * @brief DrawingContent info of window.
 */
class WindowDrawingContentInfo : public Parcelable {
public:
    /**
     * @brief Default construct of WindowDrawingContentInfo.
     */
    WindowDrawingContentInfo() = default;
    /**
     * @brief Construct of WindowDrawingContentInfo.
     *
     * @param winId Window id.
     * @param pid Process id.
     * @param uid User id.
     * @param visibility True means window is visible, false means the opposite.
     * @param winType Type of window.
     */
    WindowDrawingContentInfo(uint32_t winId, int32_t pid, int32_t uid, bool drawingContentState,
        WindowType winType) : windowId_(winId), pid_(pid), uid_(uid),
        drawingContentState_(drawingContentState), windowType_(winType) {};
    /**
     * @brief Deconstruct of WindowDrawingContentInfo.
     */
    ~WindowDrawingContentInfo() = default;

    /**
     * @brief Marshalling WindowDrawingContentInfo.
     *
     * @param parcel Package of WindowDrawingContentInfo.
     * @return True means marshall success, false means marshall failed.
     */
    virtual bool Marshalling(Parcel& parcel) const override;
    /**
     * @brief Unmarshalling WindowDrawingContentInfo.
     *
     * @param parcel Package of WindowDrawingContentInfo.
     * @return WindowDrawingContentInfo object.
     */
    static WindowDrawingContentInfo* Unmarshalling(Parcel& parcel);

    uint32_t windowId_ { INVALID_WINDOW_ID };
    int32_t pid_ { 0 };
    int32_t uid_ { 0 };
    bool drawingContentState_ {false};
    WindowType windowType_ { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW };
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_VISIBILITY_INFO_H
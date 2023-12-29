/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at,
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software,
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_DM_DISPLAY_LITE_H
#define FOUNDATION_DM_DISPLAY_LITE_H

#include <string>
#include "dm_common.h"
#include "noncopyable.h"

namespace OHOS::Rosen {
class DisplayInfo;
class CutoutInfo;

class DisplayLite : public RefBase {
friend class DisplayManagerLite;
public:
    ~DisplayLite();
    DisplayLite(const DisplayLite&) = delete;
    DisplayLite(DisplayLite&&) = delete;
    DisplayLite& operator=(const DisplayLite&) = delete;
    DisplayLite& operator=(DisplayLite&&) = delete;

    /**
     * @brief Get id of the display.
     *
     * @return Display id.
     */
    DisplayId GetId() const;

    /**
     * @brief Get info of the display.
     *
     * @return Info of the display.
     */
    sptr<DisplayInfo> GetDisplayInfo() const;

    /**
     * @brief Get width of the display.
     *
     * @return Width of the display.
     */
    int32_t GetWidth() const;

    /**
     * @brief Get height of the display.
     *
     * @return Height of the display.
     */
    int32_t GetHeight() const;

    /**
     * @brief Get cutout info of the display.
     *
     * @return Cutout info of the display.
     */
    sptr<CutoutInfo> GetCutoutInfo() const;
protected:
    // No more methods or variables can be defined here.
    DisplayLite(const std::string& name, sptr<DisplayInfo> info);
private:
    // No more methods or variables can be defined here.
    void UpdateDisplayInfo(sptr<DisplayInfo>) const;
    void UpdateDisplayInfo() const;
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_LITE_H
/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DM_SCREEN_H
#define FOUNDATION_DM_SCREEN_H

#include <screen_manager/screen_types.h>
#include <string>
#include <surface.h>
#include <vector>

#include "dm_common.h"
#include "noncopyable.h"

namespace OHOS::Rosen {
class ScreenInfo;

struct Point {
    int32_t posX_;
    int32_t posY_;
    Point() : posX_(0), posY_(0) {};
    Point(int32_t posX, int32_t posY) : posX_(posX), posY_(posY) {};
};

struct SupportedScreenModes : public RefBase {
    uint32_t width_;
    uint32_t height_;
    uint32_t refreshRate_;
};

struct VirtualScreenOption {
    std::string name_;
    uint32_t width_;
    uint32_t height_;
    float density_;
    sptr<Surface> surface_;
    int32_t flags_;
    bool isForShot_ {true};
};

struct ExpandOption {
    ScreenId screenId_;
    uint32_t startX_;
    uint32_t startY_;
};

class Screen : public RefBase {
friend class ScreenManager;
public:
    ~Screen();
    Screen(const Screen&) = delete;
    Screen(Screen&&) = delete;
    Screen& operator=(const Screen&) = delete;
    Screen& operator=(Screen&&) = delete;
    bool IsGroup() const;
    std::string GetName() const;
    
    /**
     * @brief Get screen id.
     */
    ScreenId GetId() const;

    /**
     * @brief Get width of the screen.
     */
    uint32_t GetWidth() const;
    
    /**
     * @brief Get height of the screen.
     */
    uint32_t GetHeight() const;

    /**
     * @brief Get virtual width of the screen.
     */
    uint32_t GetVirtualWidth() const;

    /**
     * @brief Get virtual height of the screen.
     */
    uint32_t GetVirtualHeight() const;

    /**
     * @brief Get virtual pixel ratio of the screen.
     */
    float GetVirtualPixelRatio() const;

    /**
     * @brief Get the Rotation of the screen.
     */
    Rotation GetRotation() const;

    /**
     * @brief Get the orientation of the screen.
     */
    Orientation GetOrientation() const;

    /**
     * @brief Is a real screen.
     */
    bool IsReal() const;

    /**
     * @brief Get screen parent id.
     */
    ScreenId GetParentId() const;

    /**
     * @brief Get screen mode id.
     */
    uint32_t GetModeId() const;

    /**
     * @brief Get supported modes of the screen.
     */
    std::vector<sptr<SupportedScreenModes>> GetSupportedModes() const;

    /**
     * @brief Set screen active mode.
     */
    DMError SetScreenActiveMode(uint32_t modeId);

    /**
     * @brief Set orientation for the screen.
     */
    DMError SetOrientation(Orientation orientation) const;

    /**
     * @brief Set the density dpi of the screen.
     */
    DMError SetDensityDpi(uint32_t dpi) const;

    /**
     * @brief Get the screen info.
     */
    sptr<ScreenInfo> GetScreenInfo() const;

    // colorspace, gamut
    /**
     * @brief Get the supported color gamuts of the screen.
     */
    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts) const;

    /**
     * @brief Get the color gamut of the screen.
     */
    DMError GetScreenColorGamut(ScreenColorGamut& colorGamut) const;

    /**
     * @brief Set the color gamut of the screen.
     */
    DMError SetScreenColorGamut(int32_t colorGamutIdx);

    /**
     * @brief Get the gamut map of the screen.
     */
    DMError GetScreenGamutMap(ScreenGamutMap& gamutMap) const;

    /**
     * @brief Set the gamut map of the screen.
     */
    DMError SetScreenGamutMap(ScreenGamutMap gamutMap);

    /**
     * @brief Set color transform for the screen.
     */
    DMError SetScreenColorTransform();
protected:
    // No more methods or variables can be defined here.
    explicit Screen(sptr<ScreenInfo> info);
    void UpdateScreenInfo() const;
    void UpdateScreenInfo(sptr<ScreenInfo> info) const;
private:
    // No more methods or variables can be defined here.
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_H
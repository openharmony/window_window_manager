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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H

#include "common/rs_rect.h"
#include "dm_common.h"
#include "class_var_definition.h"
#include "screen_info.h"

namespace OHOS::Rosen {

enum class ScreenPropertyChangeReason : uint32_t {
    UNDEFINED = 0,
    ROTATION,
    CHANGE_MODE,
    FOLD_SCREEN_EXPAND,
    SCREEN_CONNECT,
    SCREEN_DISCONNECT,
    FOLD_SCREEN_FOLDING,
};
class ScreenProperty {
public:
    ScreenProperty() = default;
    ~ScreenProperty() = default;

    void SetRotation(float rotation);
    float GetRotation() const;

    void SetBounds(const RRect& bounds);
    RRect GetBounds() const;

    void SetPhyBounds(const RRect& phyBounds);
    RRect GetPhyBounds() const;

    float GetDensity();
    float GetDefaultDensity();
    void SetDefaultDensity(float defaultDensity);

    void SetPhyWidth(uint32_t phyWidth);
    int32_t GetPhyWidth() const;

    void SetPhyHeight(uint32_t phyHeight);
    int32_t GetPhyHeight() const;

    void SetRefreshRate(uint32_t refreshRate);
    uint32_t GetRefreshRate() const;

    void UpdateVirtualPixelRatio(const RRect& bounds);
    void SetVirtualPixelRatio(float virtualPixelRatio);
    float GetVirtualPixelRatio() const;

    void SetScreenRotation(Rotation rotation);
    Rotation GetScreenRotation() const;
    void UpdateScreenRotation(Rotation rotation);

    void SetOrientation(Orientation orientation);
    Orientation GetOrientation() const;

    void SetDisplayOrientation(DisplayOrientation displayOrientation);
    DisplayOrientation GetDisplayOrientation() const;
    void CalcDefaultDisplayOrientation();

    float GetXDpi();
    float GetYDpi();

    void SetOffsetX(int32_t offsetX);
    int32_t GetOffsetX() const;

    void SetOffsetY(int32_t offsetY);
    int32_t GetOffsetY() const;

    void SetOffset(int32_t offsetX, int32_t offsetY);

    void SetScreenType(ScreenType type);
    ScreenType GetScreenType() const;

    void SetScreenRequestedOrientation(Orientation orientation);
    Orientation GetScreenRequestedOrientation() const;

private:
    static inline bool IsVertical(Rotation rotation)
    {
        return (rotation == Rotation::ROTATION_0 || rotation == Rotation::ROTATION_180);
    }
    float rotation_ { 0.0f };
    RRect bounds_;
    RRect phyBounds_;

    uint32_t phyWidth_ { UINT32_MAX };
    uint32_t phyHeight_ { UINT32_MAX };

    uint32_t refreshRate_ { 0 };
    float virtualPixelRatio_ { 1.0f };
    float defaultDensity_ { 1.0f };

    Orientation orientation_ { Orientation::UNSPECIFIED };
    DisplayOrientation displayOrientation_ { DisplayOrientation::UNKNOWN };
    Rotation screenRotation_ { Rotation::ROTATION_0 };
    Orientation screenRequestedOrientation_ { Orientation::UNSPECIFIED };

    float xDpi_ { 0.0f };
    float yDpi_ { 0.0f };

    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };

    ScreenType type_ { ScreenType::REAL };

    void UpdateXDpi();
    void UpdateYDpi();
    void CalculateXYDpi(uint32_t phyWidth, uint32_t phyHeight);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H

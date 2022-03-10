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

#ifndef FOUNDATION_DMSERVER_SCREEN_INFO_H
#define FOUNDATION_DMSERVER_SCREEN_INFO_H

#include <parcel.h>

#include "class_var_definition.h"
#include "screen.h"

namespace OHOS::Rosen {
enum class ScreenType : uint32_t {
    UNDEFINE,
    REAL,
    VIRTUAL
};
class ScreenInfo : public Parcelable {
friend class AbstractScreen;
public:
    ~ScreenInfo() = default;
    WM_DISALLOW_COPY_AND_MOVE(ScreenInfo);

    virtual bool Marshalling(Parcel& parcel) const override;
    static ScreenInfo* Unmarshalling(Parcel& parcel);

    DEFINE_VAR_FUNC_GET(std::string, Name, name);
    DEFINE_VAR_DEFAULT_FUNC_GET(ScreenId, ScreenId, id, SCREEN_ID_INVALID);
    DEFINE_VAR_DEFAULT_FUNC_GET(uint32_t, VirtualWidth, virtualWidth, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET(uint32_t, VirtualHeight, virtualHeight, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET(float, VirtualPixelRatio, virtualPixelRatio, 1.0f);
    DEFINE_VAR_DEFAULT_FUNC_GET(ScreenId, ParentId, parent, SCREEN_ID_INVALID);
    DEFINE_VAR_DEFAULT_FUNC_GET(bool, IsScreenGroup, isScreenGroup, false);
    DEFINE_VAR_DEFAULT_FUNC_GET(Rotation, Rotation, rotation, Rotation::ROTATION_0);
    DEFINE_VAR_DEFAULT_FUNC_GET(Orientation, Orientation, orientation, Orientation::UNSPECIFIED);
    DEFINE_VAR_DEFAULT_FUNC_GET(ScreenType, Type, type, ScreenType::UNDEFINE);
    DEFINE_VAR_DEFAULT_FUNC_GET(uint32_t, ModeId, modeId, 0);
    DEFINE_VAR_FUNC_GET(std::vector<sptr<SupportedScreenModes>>, Modes, modes);
protected:
    ScreenInfo() = default;
    bool InnerUnmarshalling(Parcel& parcel);
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_DISPLAY_INFO_H

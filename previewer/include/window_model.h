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

#ifndef INTERFACES_INNERKITS_WINDOW_MODEL_H
#define INTERFACES_INNERKITS_WINDOW_MODEL_H

#include "window.h"

namespace OHOS {
namespace Previewer {
enum class Orientation : int32_t {
    PORTRAIT,
    LANDSCAPE,
    ORIENTATION_UNDEFINED,
};

enum class DeviceType {
    PHONE,
    TV,
    WATCH,
    CAR,
    TABLET,
    UNKNOWN,
};

enum class ColorMode : int32_t {
    LIGHT = 0,
    DARK,
    COLOR_MODE_UNDEFINED,
};

struct PreviewerWindowModel {
    bool isRound = false; // shape rect(false) circle(true)
    int32_t originWidth = 0; // or width
    int32_t originHeight = 0; // or height
    int32_t compressWidth = 0; // cr width
    int32_t compressHeight = 0; // cr height
    Orientation orientation = Orientation::PORTRAIT; // orientation
    double density = 1.0; // dpi with calculate
    DeviceType deviceType = DeviceType::PHONE; // device type
    ColorMode colorMode = ColorMode::LIGHT; // color mode
};

class WINDOW_EXPORT PreviewerWindow {
public:
    PreviewerWindow(const PreviewerWindow&) = delete;
    PreviewerWindow& operator=(const PreviewerWindow&) = delete;
    ~PreviewerWindow() = default;

    static PreviewerWindow& GetInstance();
    static Rosen::Orientation TransOrientation(Previewer::Orientation orientation);
    void SetWindowParams(const PreviewerWindowModel& windowModel);
    PreviewerWindowModel& GetWindowParams();
    
    void SetWindowObject(const Rosen::Window* window);
    Rosen::Window* GetWindowObject();

private:
    PreviewerWindow();
    PreviewerWindowModel windowModel_;
    Rosen::Window* window_ = nullptr;
};
} // namespace Previewer
} // namespace OHOS
#endif // INTERFACES_INNERKITS_WINDOW_MODEL_H

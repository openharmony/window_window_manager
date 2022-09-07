/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_TEST_COMMON_MOCK_IREMOTE_OBJECT_MOCKER
#define OHOS_ROSEN_TEST_COMMON_MOCK_IREMOTE_OBJECT_MOCKER

#include "ui_content.h"
#include "native_engine/native_value.h"
#include "native_engine/native_engine.h"
namespace OHOS {
namespace AppExecFwk {
    class Ability;
    class Configuration;
} // namespace AppExecFwk
namespace AAFwk {
    class Want;
} // namespace AAFwk
namespace MMI {
    class PointerEvent;
    class AxisEvent;
} // namespace MMI
namespace Media {
    class PixelMap;
} // namespace Media
} // namespace OHOS
namespace OHOS {
namespace Ace {
class UIContentMocker : public UIContent {
public:
    UIContentMocker() 
    {
    }

    ~UIContentMocker()
    {
    }

    void Initialize(OHOS::Rosen::Window* window, const std::string& url, NativeValue* storage)
    {
        return;
    }

    void Foreground()
    {
        return;
    }

    void Background()
    {
        return;
    }

    void Focus()
    {
        return;
    }

    void UnFocus()
    {
        return;
    }

    void Destroy()
    {
        return;
    }

    void OnNewWant(const OHOS::AAFwk::Want& want)
    {
        return;
    }

    void Restore(OHOS::Rosen::Window* window, const std::string& contentInfo, NativeValue* storage)
    {
        return;
    }

    std::string GetContentInfo() const
    {
        return " ";
    }

    void DestroyUIDirector()
    {
        return;
    }

    bool ProcessBackPressed()
    {
        return false;
    }

    bool ProcessPointerEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent)
    {
        return false;
    }

    bool ProcessKeyEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent)
    {
        return false;
    }
    bool ProcessAxisEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent)
    {
        return false;
    }
    bool ProcessVsyncEvent(uint64_t timeStampNanos)
    {
        return false;
    }
    void UpdateConfiguration(const std::shared_ptr<OHOS::AppExecFwk::Configuration>& config)
    {
        return;
    }
    void UpdateViewportConfig(const ViewportConfig& config, OHOS::Rosen::WindowSizeChangeReason reason)
    {
        return;
    }
    void UpdateWindowMode(OHOS::Rosen::WindowMode mode)
    {
        return;
    }
    void HideWindowTitleButton(bool hideSplit, bool hideMaximize, bool hideMinimize)
    {
        return;
    }

    uint32_t GetBackgroundColor()
    {
        return 0;
    }
    void SetBackgroundColor(uint32_t color)
    {
        return;
    }
    void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info)
    {
        return;
    }
    void SetNextFrameLayoutCallback(std::function<void()>&& callback)
    {
        return;
    }
    void NotifyMemoryLevel(int32_t level)
    {
        return;
    }

    void SetAppWindowTitle(const std::string& title)
    {
        return;
    }
    void SetAppWindowIcon(const std::shared_ptr<Media::PixelMap>& pixelMap)
    {
        return;
    }
};
} // namespace Ace
} // namespace OHOS

#endif
/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_WM_TEST_ST_WINDOW_TEST_UTILS_H
#define FRAMEWORKS_WM_TEST_ST_WINDOW_TEST_UTILS_H

#include "display_manager.h"
#include "window.h"
#include "window_layout_policy.h"
#include "window_life_cycle_interface.h"
#include "window_option.h"
#include "window_scene.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
struct SplitRects {
    Rect primaryRect;
    Rect secondaryRect;
    Rect dividerRect;
};

class WindowTestUtils {
public:
    struct TestWindowInfo {
        std::string name;
        Rect rect;
        WindowType type;
        WindowMode mode;
        bool needAvoid;
        bool parentLimit;
        std::string parentName;
    };
    static Rect displayRect_;
    static Rect limitDisplayRect_;
    static Rect statusBarRect_;
    static Rect naviBarRect_;
    static Rect defaultAppRect_;
    static SplitRects splitRects_;
    static bool isVerticalDisplay_;

    static void InitByDisplayRect(const Rect& screenRect);
    static sptr<Window> CreateTestWindow(const TestWindowInfo& info);
    static sptr<Window> CreateStatusBarWindow();
    static sptr<Window> CreateNavigationBarWindow();
    static sptr<WindowScene> CreateWindowScene();
    static bool RectEqualTo(const sptr<Window>& window, const Rect& r);
    static void InitSplitRects();
    static void UpdateSplitRects(const sptr<Window>& window);

private:
    void UpdateLimitDisplayRect(Rect& avoidRect);
    void UpdateLimitSplitRects(int32_t divPos);
    void UpdateLimitSplitRect(Rect& limitSplitRect);
    AvoidPosType GetAvoidPosType(const Rect& rect);
    AvoidArea avoidArea_;
};
} // namespace ROSEN
} // namespace OHOS
#endif // FRAMEWORKS_WM_TEST_ST_WINDOW_TEST_UTILS_H

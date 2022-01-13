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

#include "window_test_utils.h"

namespace OHOS {
namespace Rosen {
Rect WindowTestUtils::screenRect_ = {0, 0, 0, 0};
Rect WindowTestUtils::statusBarRect_ = {0, 0, 0, 0};
Rect WindowTestUtils::naviBarRect_ = {0, 0, 0, 0};
Rect WindowTestUtils::defaultAppRect_ = {0, 0, 0, 0};

sptr<Window> WindowTestUtils::CreateTestWindow(const TestWindowInfo& info)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowRect(info.rect);
    option->SetWindowType(info.type);
    option->SetWindowMode(info.mode);
    if (info.parentName != "") {
        option->SetParentName(info.parentName);
    }
    if (info.needAvoid) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    }
    if (info.parentLimit) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    }
    sptr<Window> window = Window::Create(info.name, option);
    return window;
}

sptr<Window> WindowTestUtils::CreateStatusBarWindow()
{
    TestWindowInfo info = {
        .name = "statusBar",
        .rect = statusBarRect_,
        .type = WindowType::WINDOW_TYPE_STATUS_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
    return CreateTestWindow(info);
}

sptr<Window> WindowTestUtils::CreateNavigationBarWindow()
{
    TestWindowInfo info = {
        .name = "naviBar",
        .rect = naviBarRect_,
        .type = WindowType::WINDOW_TYPE_NAVIGATION_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
    return CreateTestWindow(info);
}

sptr<WindowScene> WindowTestUtils::CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, abilityContext, listener);
    return scene;
}

void WindowTestUtils::InitByScreenRect(const Rect& screenRect)
{
    const float barRatio = 0.07;
    const float appRation = 0.4;
    screenRect_ = screenRect;
    statusBarRect_ = {0, 0, screenRect_.width_, screenRect_.height_ * barRatio};
    naviBarRect_ = {0, screenRect_.height_ * (1 - barRatio), screenRect_.width_, screenRect_.height_ * barRatio};
    defaultAppRect_ = {0, 0, screenRect_.width_ * appRation, screenRect_.height_ * appRation};
}

bool WindowTestUtils::RectEqualTo(const sptr<Window>& window, const Rect& r)
{
    Rect l = window->GetRect();
    bool res = ((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
    if (!res) {
        printf("GetLayoutRect: %d %d %d %d, Expect: %d %d %d %d\n", l.posX_, l.posY_, l.width_, l.height_,
            r.posX_, r.posY_, r.width_, r.height_);
    }
    return res;
}
} // namespace ROSEN
} // namespace OHOS

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
#include <gtest/gtest.h>
#include "display_test_utils.h"
#include "screen.h"
#include "test_utils.h"
#include "wm_common.h"
#include "window.h"
#include "window_option.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace  {
    const int WAIT_FOR_SYNC_US = 1000 * 500;  // 500ms
}

class DisplayManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    sptr<Window> CreateWindow(std::string name, WindowMode mode, Rect rect);
};

void DisplayManagerTest::SetUpTestCase()
{
}

void DisplayManagerTest::TearDownTestCase()
{
}

void DisplayManagerTest::SetUp()
{
}

void DisplayManagerTest::TearDown()
{
}

sptr<Window> DisplayManagerTest::CreateWindow(std::string name, WindowMode mode, Rect rect)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetDisplayId(DisplayManager::GetInstance().GetDefaultDisplayId());
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
        option->SetWindowRect(rect);
    }
    option->SetWindowMode(mode);
    option->SetWindowName(name);
    sptr<Window> window = Window::Create(option->GetWindowName(), option);
    window->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    return window;
}

namespace {
/**
 * @tc.name: HasPrivateWindow
 * @tc.desc: Check whether there is a private window in the current display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, HasPrivateWindow, Function | SmallTest | Level2)
{
    sptr<Window> window = CreateWindow("test", WindowMode::WINDOW_MODE_FULLSCREEN, Rect {0, 0, 0, 0});
    window->SetPrivacyMode(true);
    window->Show();
    usleep(WAIT_FOR_SYNC_US);
    bool hasPrivateWindow = false;
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    DisplayManager::GetInstance().HasPrivateWindow(id, hasPrivateWindow);
    ASSERT_TRUE(hasPrivateWindow);

    window->SetPrivacyMode(false);
    usleep(WAIT_FOR_SYNC_US);
    DisplayManager::GetInstance().HasPrivateWindow(id, hasPrivateWindow);
    ASSERT_TRUE(!hasPrivateWindow);
    window->Destroy();
}

/**
 * @tc.name: HasPrivateWindowCovered
 * @tc.desc: The private window is covered
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, HasPrivateWindowCovered, Function | SmallTest | Level2)
{
    sptr<Window> window1 = CreateWindow("test", WindowMode::WINDOW_MODE_FULLSCREEN, Rect {0, 0, 0, 0});
    window1->Show();
    // 20:rect.posX_, 120:rect.posY_, 360:rect.width, 480:rect.height
    sptr<Window> window2 = CreateWindow("private", WindowMode::WINDOW_MODE_FLOATING, Rect {20, 120, 360, 480});
    window2->SetPrivacyMode(true);
    window2->Show();
    // 10:rect.posX_, 110:rect.posY_, 400:rect.width, 500:rect.height
    sptr<Window> window3 = CreateWindow("covered", WindowMode::WINDOW_MODE_FLOATING, Rect {10, 110, 400, 500});
    window3->Show();
    usleep(WAIT_FOR_SYNC_US);
    bool hasPrivateWindow = false;
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    DisplayManager::GetInstance().HasPrivateWindow(id, hasPrivateWindow);
    ASSERT_TRUE(!hasPrivateWindow);
    window1->Destroy();
    window2->Destroy();
    window3->Destroy();
}

/**
 * @tc.name: HasPrivateWindowCovered01
 * @tc.desc: The private window is partially covered
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, HasPrivateWindowCovered01, Function | SmallTest | Level2)
{
    sptr<Window> window1 = CreateWindow("test", WindowMode::WINDOW_MODE_FULLSCREEN, Rect {0, 0, 0, 0});
    window1->Show();
    // 20:rect.posX_, 120:rect.posY_, 360:rect.width, 480:rect.height
    sptr<Window> window2 = CreateWindow("private", WindowMode::WINDOW_MODE_FLOATING, Rect {20, 120, 360, 480});
    window2->SetPrivacyMode(true);
    window2->Show();
    // 10:rect.posX_, 110:rect.posY_, 360:rect.width, 480:rect.height
    sptr<Window> window3 = CreateWindow("covered", WindowMode::WINDOW_MODE_FLOATING, Rect {10, 110, 360, 480});
    window3->Show();
    usleep(WAIT_FOR_SYNC_US);
    bool hasPrivateWindow = false;
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    DisplayManager::GetInstance().HasPrivateWindow(id, hasPrivateWindow);
    ASSERT_TRUE(hasPrivateWindow);
    window1->Destroy();
    window2->Destroy();
    window3->Destroy();
}

/**
 * @tc.name: HasPrivateWindowCovered02
 * @tc.desc: The private window is covered
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, HasPrivateWindowCovered02, Function | SmallTest | Level2)
{
    sptr<Window> window1 = CreateWindow("test", WindowMode::WINDOW_MODE_FULLSCREEN, Rect {0, 0, 0, 0});
    window1->Show();
    // 20:rect.posX_, 120:rect.posY_, 360:rect.width, 480:rect.height
    sptr<Window> window2 = CreateWindow("private", WindowMode::WINDOW_MODE_FLOATING, Rect {20, 120, 360, 480});
    window2->SetPrivacyMode(true);
    window2->Show();
    // 10:rect.posX_, 110:rect.posY_, 380:rect.width, 480:rect.height
    sptr<Window> window3 = CreateWindow("covered1", WindowMode::WINDOW_MODE_FLOATING, Rect {10, 110, 380, 480});
    window3->Show();
    // 10:rect.posX_, 300:rect.posY_, 380:rect.width, 480:rect.height
    sptr<Window> window4 = CreateWindow("covered2", WindowMode::WINDOW_MODE_FLOATING, Rect {10, 300, 380, 480});
    window4->Show();
    usleep(WAIT_FOR_SYNC_US);
    bool hasPrivateWindow = false;
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    DisplayManager::GetInstance().HasPrivateWindow(id, hasPrivateWindow);
    ASSERT_TRUE(!hasPrivateWindow);
    window1->Destroy();
    window2->Destroy();
    window3->Destroy();
    window4->Destroy();
}

/**
 * @tc.name: HasPrivateWindowCovered03
 * @tc.desc: The private window is partially covered
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, HasPrivateWindowCovered03, Function | SmallTest | Level2)
{
    sptr<Window> window1 = CreateWindow("test", WindowMode::WINDOW_MODE_FULLSCREEN, Rect {0, 0, 0, 0});
    window1->Show();
    // 20:rect.posX_, 120:rect.pos_Y, rect.width_:360, rect.height_:700
    sptr<Window> window2 = CreateWindow("private", WindowMode::WINDOW_MODE_FLOATING, Rect {20, 120, 360, 700});
    window2->SetPrivacyMode(true);
    window2->Show();
    // 10:rect.posX_, 110:rect.pos_Y, rect.width_:380, rect.height_:480
    sptr<Window> window3 = CreateWindow("covered1", WindowMode::WINDOW_MODE_FLOATING, Rect {10, 110, 380, 480});
    window3->Show();
    // 10:rect.posX_, 600:rect.pos_Y, rect.width_:380, rect.height_:480
    sptr<Window> window4 = CreateWindow("covered2", WindowMode::WINDOW_MODE_FLOATING, Rect {10, 600, 380, 480});
    window4->Show();
    usleep(WAIT_FOR_SYNC_US);
    bool hasPrivateWindow = false;
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    DisplayManager::GetInstance().HasPrivateWindow(id, hasPrivateWindow);
    ASSERT_TRUE(hasPrivateWindow);
    window1->Destroy();
    window2->Destroy();
    window3->Destroy();
    window4->Destroy();
}

/**
 * @tc.name: HasPrivateWindowSkipSnapShot
 * @tc.desc: set snap shot skip
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, HasPrivateWindowSkipSnapShot, Function | SmallTest | Level2)
{
    sptr<Window> window1 = CreateWindow("test", WindowMode::WINDOW_MODE_FULLSCREEN, Rect {0, 0, 0, 0});
    window1->Show();
    // 20:rect.posX_, 120:rect.posY_, 360:rect.width, 480:rect.height
    sptr<Window> window2 = CreateWindow("private", WindowMode::WINDOW_MODE_FLOATING, Rect {20, 120, 360, 480});
    window2->SetSnapshotSkip(true);
    window2->Show();
    usleep(WAIT_FOR_SYNC_US);
    bool hasPrivateWindow = false;
    DisplayId id = DisplayManager::GetInstance().GetDefaultDisplayId();
    DisplayManager::GetInstance().HasPrivateWindow(id, hasPrivateWindow);
    ASSERT_TRUE(!hasPrivateWindow);
    window1->Destroy();
    window2->Destroy();
}
}
} // namespace OHOS::Rosen
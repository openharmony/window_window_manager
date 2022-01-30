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

// gtest
#include <gtest/gtest.h>
#include "wm_common.h"
#include "window_manager.h"
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowFocusTest"};
}

using utils = WindowTestUtils;
const int WAIT_ASYNC_US = 100000;  // 100000us

class TestFocusChangedListener : public IFocusChangedListener {
public:
    uint32_t focusedWindow_ = INVALID_WINDOW_ID;
    uint32_t unfocusedWindow_ = INVALID_WINDOW_ID;
    void OnFocused(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType,
                   DisplayId displayId) override;

    void OnUnfocused(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType,
                     DisplayId displayId) override;
};

class WindowFocusTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static sptr<TestFocusChangedListener> testFocusChangedListener_;
    utils::TestWindowInfo fullScreenAppInfo_;
    utils::TestWindowInfo floatAppInfo_;
    utils::TestWindowInfo subAppInfo_;
};

sptr<TestFocusChangedListener> WindowFocusTest::testFocusChangedListener_ =
    new TestFocusChangedListener();

void TestFocusChangedListener::OnFocused(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType,
                                         DisplayId displayId)
{
    WLOGFI("TestFocusChangedListener Focused ID: %{public}u", windowId);
    focusedWindow_ = windowId;
}

void TestFocusChangedListener::OnUnfocused(uint32_t windowId, sptr<IRemoteObject> abilityToken, WindowType windowType,
                                           DisplayId displayId)
{
    WLOGFI("TestFocusChangedListener Unfocused ID: %{public}u", windowId);
    unfocusedWindow_ = windowId;
}

void WindowFocusTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        WLOGFE("GetDefaultDisplay: failed!");
    } else {
        WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
            display->GetId(), display->GetWidth(), display->GetHeight(), display->GetFreshRate());
    }
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByDisplayRect(displayRect);
}

void WindowFocusTest::TearDownTestCase()
{
}

void WindowFocusTest::SetUp()
{
    fullScreenAppInfo_ = {
            .name = "FullWindow",
            .rect = utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentName = "",
    };
    floatAppInfo_ = {
            .name = "ParentWindow",
            .rect = utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FLOATING,
            .needAvoid = false,
            .parentLimit = false,
            .parentName = "",
    };
    subAppInfo_ = {
            .name = "SubWindow",
            .rect = utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FLOATING,
            .needAvoid = false,
            .parentLimit = false,
            .parentName = "",
    };
    WindowManager::GetInstance().RegisterFocusChangedListener(testFocusChangedListener_);
}

void WindowFocusTest::TearDown()
{
    WindowManager::GetInstance().UnregisterFocusChangedListener(testFocusChangedListener_);
}

namespace {
/**
 * @tc.name: FocusChangedTest01
 * @tc.desc: add main window and sub window and show it to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest01, Function | MediumTest | Level3)
{
    fullScreenAppInfo_.name = "window1";
    fullScreenAppInfo_.focusable_ = false;
    const sptr<Window>& window1 = utils::CreateTestWindow(fullScreenAppInfo_);

    floatAppInfo_.name = "window2";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& window2 = utils::CreateTestWindow(floatAppInfo_);

    floatAppInfo_.name = "window3";
    floatAppInfo_.rect = { 250, 150, 300, 500 };
    const sptr<Window>& window3 = utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentName = window3->GetWindowName();
    const sptr<Window>& subWindow = utils::CreateTestWindow(subAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window3->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window3->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window3->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, window2->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window3->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(window2->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, subWindow->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window2->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(subWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    window1->Destroy();
    window2->Destroy();
    window3->Destroy();
    subWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest02
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest02, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "mainWindow";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());

    subAppInfo_.name = "subWindow";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentName = mainWindow->GetWindowName();
    const sptr<Window>& subWindow = utils::CreateTestWindow(subAppInfo_);
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());
    ASSERT_EQ(subWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(subWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow->Destroy();
    subWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest03
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest03, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "mainWindow1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    subAppInfo_.name = "aboveSubWindow";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentName = mainWindow1->GetWindowName();
    const sptr<Window>& aboveSubWindow = utils::CreateTestWindow(subAppInfo_);
    ASSERT_EQ(WMError::WM_OK, aboveSubWindow->Show());

    floatAppInfo_.name = "mainWindow2";
    floatAppInfo_.rect = { 200, 200, 100, 100 };
    const sptr<Window>& mainWindow2 = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow2->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow1->Destroy();
    mainWindow2->Destroy();
    aboveSubWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest04
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest04, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "mainWindow1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    subAppInfo_.name = "belowSubWindow";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentName = mainWindow1->GetWindowName();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow = utils::CreateTestWindow(subAppInfo_);
    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());

    floatAppInfo_.name = "mainWindow2";
    floatAppInfo_.rect = { 200, 200, 100, 100 };
    const sptr<Window>& mainWindow2 = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow2->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(mainWindow1->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow1->Destroy();
    mainWindow2->Destroy();
    belowSubWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest05
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest05, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "mainWindow1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    floatAppInfo_.name = "mainWindow2";
    floatAppInfo_.rect = { 200, 200, 100, 100 };
    const sptr<Window>& mainWindow2 = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());

    subAppInfo_.name = "belowSubWindow";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentName = mainWindow2->GetWindowName();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow = utils::CreateTestWindow(subAppInfo_);
    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(mainWindow1->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow1->Destroy();
    mainWindow2->Destroy();
    belowSubWindow->Destroy();
}

/**
* @tc.name: FocusChangedTest06
* @tc.desc: hide unfocused window to test focus
* @tc.type: FUNC
*/
HWTEST_F(WindowFocusTest, FocusChangedTest06, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "mainWindow";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow = utils::CreateTestWindow(floatAppInfo_);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());

    subAppInfo_.name = "belowSubWindow";
    subAppInfo_.rect = { 100, 200, 100, 100 };
    subAppInfo_.parentName = mainWindow->GetWindowName();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow = utils::CreateTestWindow(subAppInfo_);
    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());

    subAppInfo_.name = "aboveSubWindow";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentName = mainWindow->GetWindowName();
    const sptr<Window>& aboveSubWindow = utils::CreateTestWindow(subAppInfo_);
    ASSERT_EQ(WMError::WM_OK, aboveSubWindow->Show());
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow->Destroy();
    belowSubWindow->Destroy();
    aboveSubWindow->Destroy();
}
}
} // namespace Rosen
} // namespace OHOS

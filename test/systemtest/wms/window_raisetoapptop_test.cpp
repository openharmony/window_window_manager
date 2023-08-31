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

// gtest
#include <gtest/gtest.h>
#include "window_test_utils.h"

#include "window_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowRaiseToAppTopTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::vector<sptr<Window>> activeWindows_;
    Utils::TestWindowInfo fullInfo_;
private:
    static constexpr uint32_t TEST_SLEEP_S = 1;
};

void WindowRaiseToAppTopTest::SetUpTestCase()
{
}

void WindowRaiseToAppTopTest::TearDownTestCase()
{
}

void WindowRaiseToAppTopTest::SetUp()
{
    fullInfo_ = {
            .name = "",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .parentId = INVALID_WINDOW_ID,
    };
    activeWindows_.clear();
}

void WindowRaiseToAppTopTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
/**
* @tc.name: WindowRaiseToAppTopTest1
* @tc.desc: to raise to app top, normal
* @tc.type: FUNC
*/
HWTEST_F(WindowRaiseToAppTopTest, NormalRaise1, Function | MediumTest | Level3)
{
    fullInfo_.name  = "mainWindow.1";
    sptr<Window> mainWindow = Utils::CreateTestWindow(fullInfo_);
    if (mainWindow == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, mainWindow);
    activeWindows_.push_back(mainWindow);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "subWindow.1";
    fullInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    fullInfo_.parentId  = mainWindow->GetWindowId();
    sptr<Window> subWindow1 = Utils::CreateTestWindow(fullInfo_);
    ASSERT_NE(nullptr, subWindow1);
    activeWindows_.push_back(subWindow1);
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "subWindow.2";
    fullInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    fullInfo_.parentId  = mainWindow->GetWindowId();
    sptr<Window> subWindow2 = Utils::CreateTestWindow(fullInfo_);
    ASSERT_NE(nullptr, subWindow2);
    activeWindows_.push_back(subWindow2);
    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());
    sleep(TEST_SLEEP_S);

    auto result1 = mainWindow->RaiseToAppTop();
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_PARENT, result1);
    auto result2 = subWindow1->RaiseToAppTop();
    ASSERT_EQ(WmErrorCode::WM_OK, result2);
}

/**
* @tc.name: WindowRaiseToAppTopTest2
* @tc.desc: to raise to app top, with dialog
* @tc.type: FUNC
*/
HWTEST_F(WindowRaiseToAppTopTest, RaiseWithDialog1, Function | MediumTest | Level3)
{
    fullInfo_.name  = "mainWindow.1";
    sptr<Window> mainWindow = Utils::CreateTestWindow(fullInfo_);
    if (mainWindow == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, mainWindow);
    activeWindows_.push_back(mainWindow);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "subWindow.1";
    fullInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    fullInfo_.parentId  = mainWindow->GetWindowId();
    sptr<Window> subWindow1 = Utils::CreateTestWindow(fullInfo_);
    ASSERT_NE(nullptr, subWindow1);
    activeWindows_.push_back(subWindow1);
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "subWindow.2";
    fullInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    fullInfo_.parentId  = mainWindow->GetWindowId();
    sptr<Window> subWindow2 = Utils::CreateTestWindow(fullInfo_);
    ASSERT_NE(nullptr, subWindow2);
    activeWindows_.push_back(subWindow2);
    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "dialog.2";
    fullInfo_.type = WindowType::WINDOW_TYPE_DIALOG;
    fullInfo_.parentId  = INVALID_WINDOW_ID;
    sptr<Window> dialog = Utils::CreateTestWindow(fullInfo_);
    ASSERT_NE(nullptr, dialog);
    activeWindows_.push_back(dialog);
    ASSERT_EQ(WMError::WM_OK, dialog->Show());
    sleep(TEST_SLEEP_S);

    auto result = subWindow1->RaiseToAppTop();
    ASSERT_EQ(WmErrorCode::WM_OK, result);
}

/**
* @tc.name: WindowRaiseToAppTopTest3
* @tc.desc: to raise to app top, in hide
* @tc.type: FUNC
*/
HWTEST_F(WindowRaiseToAppTopTest, RaiseWhenHide, Function | MediumTest | Level3)
{
    fullInfo_.name  = "mainWindow.1";
    sptr<Window> mainWindow = Utils::CreateTestWindow(fullInfo_);
    if (mainWindow == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, mainWindow);
    activeWindows_.push_back(mainWindow);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "subWindow.1";
    fullInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    fullInfo_.parentId  = mainWindow->GetWindowId();
    sptr<Window> subWindow1 = Utils::CreateTestWindow(fullInfo_);
    if (subWindow1 == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, subWindow1);
    activeWindows_.push_back(subWindow1);
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "subWindow.2";
    fullInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    fullInfo_.parentId  = mainWindow->GetWindowId();
    sptr<Window> subWindow2 = Utils::CreateTestWindow(fullInfo_);
    ASSERT_NE(nullptr, subWindow2);
    activeWindows_.push_back(subWindow2);
    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());
    sleep(TEST_SLEEP_S);

    ASSERT_EQ(WMError::WM_OK, mainWindow->Hide());
    sleep(TEST_SLEEP_S);

    auto result = subWindow1->RaiseToAppTop();
    ASSERT_EQ(WmErrorCode::WM_OK, result);

    ASSERT_EQ(WMError::WM_OK, subWindow1->Hide());
    sleep(TEST_SLEEP_S);

    result = subWindow1->RaiseToAppTop();
    ASSERT_EQ(WmErrorCode::WM_ERROR_STATE_ABNORMALLY, result);
}

/**
* @tc.name: WindowRaiseToAppTopTest3
* @tc.desc: to raise to app top, not app subwindow
* @tc.type: FUNC
*/
HWTEST_F(WindowRaiseToAppTopTest, NotAppSubWindow, Function | MediumTest | Level3)
{
    fullInfo_.name  = "mainWindow.1";
    fullInfo_.type = WindowType::WINDOW_TYPE_FLOAT;
    sptr<Window> mainWindow = Utils::CreateTestWindow(fullInfo_);
    if (mainWindow == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, mainWindow);
    activeWindows_.push_back(mainWindow);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());
    sleep(TEST_SLEEP_S);

    fullInfo_.name  = "subWindow.1";
    fullInfo_.type = WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW;
    fullInfo_.parentId  = mainWindow->GetWindowId();
    sptr<Window> subWindow1 = Utils::CreateTestWindow(fullInfo_);
    if (subWindow1 == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, subWindow1);
    activeWindows_.push_back(subWindow1);
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    sleep(TEST_SLEEP_S);

    auto result = subWindow1->RaiseToAppTop();
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_CALLING, result);
}
}
} // namespace Rosen
} // namespace OHOS

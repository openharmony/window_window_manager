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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowVisibilityInfoTest"};
}

using utils = WindowTestUtils;
constexpr int WAIT_ASYNC_US = 200000; // 200ms

class VisibilityChangedListenerImpl : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override;
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos_;
};

void VisibilityChangedListenerImpl::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    WLOGFI("size:%{public}zu", windowVisibilityInfo.size());
    windowVisibilityInfos_ = std::move(windowVisibilityInfo);
    for (auto& info : windowVisibilityInfos_) {
        WLOGFI("windowId:%{public}u, covered:%{public}d, pid:%{public}d, uid:%{public}d", info->windowId_,
            info->isVisible_, info->pid_, info->uid_);
    }
}

class WindowVisibilityInfoTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    virtual void SetUp() override;

    virtual void TearDown() override;

    static inline sptr<VisibilityChangedListenerImpl> visibilityChangedListener_ = new VisibilityChangedListenerImpl();
    utils::TestWindowInfo fullScreenAppInfo_;
    utils::TestWindowInfo floatAppInfo_;
    utils::TestWindowInfo subAppInfo_;
};

void WindowVisibilityInfoTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        WLOGFE("GetDefaultDisplay: failed!");
    } else {
        WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
            display->GetId(), display->GetWidth(), display->GetHeight(), display->GetFreshRate());
    }
    Rect displayRect = {0, 0,
                        static_cast<uint32_t>(display->GetWidth()), static_cast<uint32_t>(display->GetHeight())};
    utils::InitByDisplayRect(displayRect);
    WindowManager::GetInstance().RegisterVisibilityChangedListener(visibilityChangedListener_);
}

void WindowVisibilityInfoTest::TearDownTestCase()
{
    WindowManager::GetInstance().UnregisterVisibilityChangedListener(visibilityChangedListener_);
}

void WindowVisibilityInfoTest::SetUp()
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
}

void WindowVisibilityInfoTest::TearDown()
{
}

namespace {
/**
* @tc.name: WindowVisibilityInfoTest01
* @tc.desc: add main window and sub window, show and hide and test callback
* @tc.type: FUNC
*/
HWTEST_F(WindowVisibilityInfoTest, WindowVisibilityInfoTest01, Function | MediumTest | Level1)
{
    floatAppInfo_.name = "window1";
    floatAppInfo_.rect = {250, 150, 300, 500};
    sptr<Window> window1 = utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow1";
    subAppInfo_.rect = {400, 200, 100, 100};
    subAppInfo_.parentName = window1->GetWindowName();
    sptr<Window> subWindow1 = utils::CreateTestWindow(subAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, subWindow1->Hide());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());

    window1->Destroy();
    subWindow1->Destroy();
}

/**
* @tc.name: WindowVisibilityInfoTest02
* @tc.desc: add two fullscreen main window, test callback
* @tc.type: FUNC
*/
HWTEST_F(WindowVisibilityInfoTest, WindowVisibilityInfoTest02, Function | MediumTest | Level1)
{
    fullScreenAppInfo_.name = "window1";
    sptr<Window> window1 = utils::CreateTestWindow(fullScreenAppInfo_);

    fullScreenAppInfo_.name = "window2";
    sptr<Window> window2 = utils::CreateTestWindow(fullScreenAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, window2->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());

    window1->Destroy();
    window2->Destroy();
}

/**
* @tc.name: WindowVisibilityInfoTest03
* @tc.desc: add two main window and sub windows and test callback
* @tc.type: FUNC
*/
HWTEST_F(WindowVisibilityInfoTest, WindowVisibilityInfoTest03, Function | MediumTest | Level1)
{
    floatAppInfo_.name = "window1";
    floatAppInfo_.rect = {0, 0, 300, 600};
    sptr<Window> window1 = utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow1";
    subAppInfo_.rect = {400, 200, 100, 100};
    subAppInfo_.parentName = window1->GetWindowName();
    sptr<Window> subWindow1 = utils::CreateTestWindow(subAppInfo_);

    floatAppInfo_.name = "window2";
    floatAppInfo_.rect = {50, 150, 240, 426};
    sptr<Window> window2 = utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow2";
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    subAppInfo_.parentName = window2->GetWindowName();
    sptr<Window> subWindow2 = utils::CreateTestWindow(subAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window2->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());

    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());

    window1->Destroy();
    subWindow1->Destroy();
    window2->Destroy();
    subWindow2->Destroy();
}
}
} // namespace Rosen
} // namespace OHOS


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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "display_manager.h"
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

using Utils = WindowTestUtils;
constexpr int WAIT_ASYNC_MS_TIME_OUT = 2000; // 2000ms

#define CHECK_DISPLAY_POWER_STATE_RETURN()                              \
    do {                                                                \
        if (!displayPowerEventListener_->isDisplayPowerValid.load()) {  \
            WLOGFE("isDisplayPowerValid false!");                       \
            goto end;                                                   \
        }                                                               \
    } while (false)

class VisibilityChangedListenerImpl : public IVisibilityChangedListener {
public:
    VisibilityChangedListenerImpl(std::mutex& mutex, std::condition_variable& cv) : mutex_(mutex), cv_(cv) {}
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override;
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos_;
    bool isCallbackCalled_ { false };
private:
    std::mutex& mutex_;
    std::condition_variable& cv_;
};

class DisplayPowerEventListenerImpl : public IDisplayPowerEventListener {
public:
    DisplayPowerEventListenerImpl()
    {
        auto displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
        auto state = DisplayManager::GetInstance().GetDisplayState(displayId);
        isDisplayPowerValid = state == DisplayState::ON;
    }
    void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) override
    {
        isDisplayPowerValid.store(false);
    }
    std::atomic_bool isDisplayPowerValid;
};

void VisibilityChangedListenerImpl::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    std::unique_lock<std::mutex> lock(mutex_);
    isCallbackCalled_ = true;
    WLOGFI("size:%{public}zu", windowVisibilityInfo.size());
    windowVisibilityInfos_ = std::move(windowVisibilityInfo);
    for (auto& info : windowVisibilityInfos_) {
        WLOGFI("windowId:%{public}u, covered:%{public}d, pid:%{public}d, uid:%{public}d, windowType:%{public}u",
            info->windowId_, info->isVisible_, info->pid_, info->uid_, static_cast<uint32_t>(info->windowType_));
    }
    cv_.notify_all();
}

class WindowVisibilityInfoTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    virtual void SetUp() override;

    virtual void TearDown() override;

    static inline std::mutex mutex_;
    static inline std::condition_variable cv_;
    static inline sptr<VisibilityChangedListenerImpl> visibilityChangedListener_ =
        new VisibilityChangedListenerImpl(mutex_, cv_);
    static inline sptr<DisplayPowerEventListenerImpl> displayPowerEventListener_ =
        new DisplayPowerEventListenerImpl();

    static inline void ResetCallbackCalledFLag()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        visibilityChangedListener_->isCallbackCalled_ = false;
    }

    static void WaitForCallback();
    Utils::TestWindowInfo fullScreenAppInfo_;
    Utils::TestWindowInfo floatAppInfo_;
    Utils::TestWindowInfo subAppInfo_;
};

void WindowVisibilityInfoTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    Rect displayRect = {0, 0,
                        static_cast<uint32_t>(display->GetWidth()), static_cast<uint32_t>(display->GetHeight())};
    Utils::InitByDisplayRect(displayRect);
    WindowManager::GetInstance().RegisterVisibilityChangedListener(visibilityChangedListener_);
    DisplayManager::GetInstance().RegisterDisplayPowerEventListener(displayPowerEventListener_);
}

void WindowVisibilityInfoTest::TearDownTestCase()
{
    WindowManager::GetInstance().UnregisterVisibilityChangedListener(visibilityChangedListener_);
    DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(displayPowerEventListener_);
}

void WindowVisibilityInfoTest::SetUp()
{
    fullScreenAppInfo_ = {
        .name = "FullWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
    floatAppInfo_ = {
        .name = "ParentWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
    subAppInfo_ = {
        .name = "SubWindow",
        .rect = Utils::customAppRect_,
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

void WindowVisibilityInfoTest::WaitForCallback()
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto now = std::chrono::system_clock::now();
    if (!cv_.wait_until(lock, now + std::chrono::milliseconds(WAIT_ASYNC_MS_TIME_OUT),
        []() { return visibilityChangedListener_->isCallbackCalled_; })) {
        WLOGFI("wait_until time out");
    }
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
    sptr<Window> window1 = Utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow1";
    subAppInfo_.rect = {400, 200, 100, 100};
    subAppInfo_.parentName = window1->GetWindowName();
    sptr<Window> subWindow1 = Utils::CreateTestWindow(subAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, subWindow1->Hide());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

end:
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
    sptr<Window> window1 = Utils::CreateTestWindow(fullScreenAppInfo_);

    fullScreenAppInfo_.name = "window2";
    sptr<Window> window2 = Utils::CreateTestWindow(fullScreenAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window2->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

end:
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
    sptr<Window> window1 = Utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow1";
    subAppInfo_.rect = {400, 200, 100, 100};
    subAppInfo_.parentName = window1->GetWindowName();
    sptr<Window> subWindow1 = Utils::CreateTestWindow(subAppInfo_);

    floatAppInfo_.name = "window2";
    sptr<Window> window2 = Utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow2";
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    subAppInfo_.parentName = window2->GetWindowName();
    sptr<Window> subWindow2 = Utils::CreateTestWindow(subAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window2->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

end:
    window1->Destroy();
    subWindow1->Destroy();
    window2->Destroy();
    subWindow2->Destroy();
}

/**
* @tc.name: WindowVisibilityInfoTest04
* @tc.desc: add two main window and sub windows and test callback
* @tc.type: FUNC
*/
HWTEST_F(WindowVisibilityInfoTest, WindowVisibilityInfoTest04, Function | MediumTest | Level1)
{
    floatAppInfo_.name = "window1";
    floatAppInfo_.rect = {0, 0, 300, 600};
    sptr<Window> window1 = Utils::CreateTestWindow(floatAppInfo_);

    floatAppInfo_.name = "window2";
    floatAppInfo_.rect = {0, 0, 300, 300};
    sptr<Window> window2 = Utils::CreateTestWindow(floatAppInfo_);

    floatAppInfo_.name = "window3";
    floatAppInfo_.rect = {0, 300, 300, 300};
    sptr<Window> window3 = Utils::CreateTestWindow(floatAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window2->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(1, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window3->Show());
    WaitForCallback();
    CHECK_DISPLAY_POWER_STATE_RETURN();
    ASSERT_EQ(2, visibilityChangedListener_->windowVisibilityInfos_.size());
    ResetCallbackCalledFLag();

end:
    window1->Destroy();
    window2->Destroy();
    window3->Destroy();
}
}
} // namespace Rosen
} // namespace OHOS


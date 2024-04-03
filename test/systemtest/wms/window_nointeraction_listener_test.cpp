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

#include <transaction/rs_transaction.h>
#include "display_manager.h"
#include "window_manager.h"
#include "window_test_utils.h"
#include "pointer_event.h"
#include "key_event.h"
#include "wm_common.h"
#include "window_scene_session_impl.h"
#include "window_session_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNoInteractionTest"};
}

using Utils = WindowTestUtils;
constexpr int64_t NO_INTERACTION_LISTENER_TEST_TIMEOUT = 3000;
constexpr int64_t NO_INTERACTION_LISTENER_TEST_WATCHER_TIMEOUT = 4000;

class NoInteractionTesterListener : public IWindowNoInteractionListener {
public:
    NoInteractionTesterListener(std::mutex& mutex, std::condition_variable& cv) : mutex_(mutex), cv_(cv)
    {
        timeout_ = NO_INTERACTION_LISTENER_TEST_TIMEOUT;
    }
    void OnWindowNoInteractionCallback() override;
    void SetTimeout(int64_t timeout) override;
    int64_t GetTimeout() const override;
    bool isCallbackCalled_ { false };

private:
    std::mutex& mutex_;
    std::condition_variable& cv_;
    int64_t timeout_ { 0 }; // ms
};

void NoInteractionTesterListener::OnWindowNoInteractionCallback()
{
    WLOGI("OnWindowNoInteractionCallback trigged");
    std::unique_lock<std::mutex> lock(mutex_);
    WLOGI("OnWindowNoInteractionCallback trigged GET LOCK");
    isCallbackCalled_ = true;
    cv_.notify_all();
}

void NoInteractionTesterListener::SetTimeout(int64_t timeout)
{
    timeout_ = timeout;
}

int64_t NoInteractionTesterListener::GetTimeout() const
{
    return timeout_;
}

class WindowNoInteractionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;

    static inline std::mutex mutex_;
    static inline std::condition_variable cv_;

    static inline void ResetCallbackCalledFLag(sptr<NoInteractionTesterListener>& listener)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        listener->isCallbackCalled_ = false;
    }

    static void WaitForCallback(sptr<NoInteractionTesterListener>& listener);
};

void WindowNoInteractionTest::SetUpTestCase()
{
}

void WindowNoInteractionTest::TearDownTestCase()
{
}

void WindowNoInteractionTest::SetUp()
{
}

void WindowNoInteractionTest::TearDown()
{
}

void WindowNoInteractionTest::WaitForCallback(sptr<NoInteractionTesterListener>& listener)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (listener->isCallbackCalled_ == false) {
        auto now = std::chrono::system_clock::now();
        if (!cv_.wait_until(lock, now + std::chrono::milliseconds(NO_INTERACTION_LISTENER_TEST_WATCHER_TIMEOUT),
            [&listener]() { return listener->isCallbackCalled_; })) {
                WLOGI("wait_until time out");
        }
    }
}

namespace {

/**
* @tc.name: RegisterUnregisterNormal
* @tc.type: FUNC
*/
HWTEST_F(WindowNoInteractionTest, RegisterUnregisterNormal, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
    
    // unregister listener
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

HWTEST_F(WindowNoInteractionTest, RegisterUnregisterNull, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    sptr<NoInteractionTesterListener> nullTester = nullptr;
    WMError result = window->RegisterWindowNoInteractionListener(nullTester);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, result);

    result = window->UnregisterWindowNoInteractionListener(nullTester);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, result);
}

HWTEST_F(WindowNoInteractionTest, UnregisterNotReg, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
* @tc.name: KeyEventDownWindowShow
* @tc.type: FUNC
*/
HWTEST_F(WindowNoInteractionTest, KeyEventDownWindowShow, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);

    std::thread t([sessionWptr = wptr(window)]() {
        sleep(1500);
        auto windowTemp = sessionWptr.promote();
        auto keyEvent = MMI::KeyEvent::Create();
        keyEvent->SetId(100);
        keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_ESCAPE);
        keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
        windowTemp->ConsumeKeyEvent(keyEvent);
    });
    t.detach();

    WaitForCallback(noInteractionTesterListener);
    ASSERT_EQ(false, noInteractionTesterListener->isCallbackCalled_);
    ResetCallbackCalledFLag(noInteractionTesterListener);
    
    // unregister listener
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
* @tc.name: KeyEventDownWindowHide
* @tc.type: FUNC
*/
HWTEST_F(WindowNoInteractionTest, KeyEventDownWindowHide, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_HIDDEN;

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);

    std::thread t([&]() {
        sleep(1500);
        auto keyEvent = MMI::KeyEvent::Create();
        keyEvent->SetId(101);
        keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_ESCAPE);
        keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
        window->ConsumeKeyEvent(keyEvent);
    });
    t.detach();
    
    WaitForCallback(noInteractionTesterListener);
    ASSERT_EQ(false, noInteractionTesterListener->isCallbackCalled_);
    ResetCallbackCalledFLag(noInteractionTesterListener);
    
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
* @tc.name: KeyEventUpWindowShow
* @tc.type: FUNC
*/
HWTEST_F(WindowNoInteractionTest, KeyEventUpWindowShow, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);

    std::thread t([&]() {
        sleep(1500);
        auto keyEvent = MMI::KeyEvent::Create();
        keyEvent->SetId(102);
        keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_ESCAPE);
        keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
        window->ConsumeKeyEvent(keyEvent);
    });
    t.detach();
    
    WaitForCallback(noInteractionTesterListener);
    ASSERT_EQ(false, noInteractionTesterListener->isCallbackCalled_);
    ResetCallbackCalledFLag(noInteractionTesterListener);
    
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
* @tc.name: KeyEventUpWindowHide
* @tc.type: FUNC
*/
HWTEST_F(WindowNoInteractionTest, KeyEventUpWindowHide, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_HIDDEN;

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);

    std::thread t([&]() {
        sleep(1500);
        auto keyEvent = MMI::KeyEvent::Create();
        keyEvent->SetId(103);
        keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_ESCAPE);
        keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
        window->ConsumeKeyEvent(keyEvent);
    });
    t.detach();

    WaitForCallback(noInteractionTesterListener);
    ASSERT_EQ(false, noInteractionTesterListener->isCallbackCalled_);
    ResetCallbackCalledFLag(noInteractionTesterListener);
    
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
* @tc.name: PointerEventDown
* @tc.type: FUNC
*/
HWTEST_F(WindowNoInteractionTest, PointerEventDown, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    // show window.
    window->state_ = WindowState::STATE_SHOWN;

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);

    std::thread t([&]() {
        sleep(1500);
        auto pointerEvent = MMI::PointerEvent::Create();
        pointerEvent->SetId(104);
        pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
        window->ConsumePointerEvent(pointerEvent);
    });
    t.detach();
    
    WaitForCallback(noInteractionTesterListener);
    ASSERT_EQ(false, noInteractionTesterListener->isCallbackCalled_);
    ResetCallbackCalledFLag(noInteractionTesterListener);
    
    // unregister listener
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
* @tc.name: PointerEventUp
* @tc.type: FUNC
*/
HWTEST_F(WindowNoInteractionTest, PointerEventUp, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);

    std::thread t([&]() {
        sleep(1500);
        auto pointerEvent = MMI::PointerEvent::Create();
        pointerEvent->SetId(105);
        pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
        window->ConsumePointerEvent(pointerEvent);
    });
    t.detach();

    WaitForCallback(noInteractionTesterListener);
    ASSERT_EQ(false, noInteractionTesterListener->isCallbackCalled_);
    ResetCallbackCalledFLag(noInteractionTesterListener);
    
    // unregister listener
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

HWTEST_F(WindowNoInteractionTest, NoInteraction, Function | MediumTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;

    sptr<NoInteractionTesterListener> noInteractionTesterListener = new NoInteractionTesterListener(mutex_, cv_);
    WMError result = window->RegisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);

    window->NotifyNoInteractionTimeout(noInteractionTesterListener);

    WaitForCallback(noInteractionTesterListener);
    ASSERT_EQ(true, noInteractionTesterListener->isCallbackCalled_);
    ResetCallbackCalledFLag(noInteractionTesterListener);
    
    // unregister listener
    result = window->UnregisterWindowNoInteractionListener(noInteractionTesterListener);
    ASSERT_EQ(WMError::WM_OK, result);
}

}
} // namespace Rosen
} // namespace OHOS


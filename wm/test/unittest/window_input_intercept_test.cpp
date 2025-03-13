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

#include <gtest/gtest.h>
#include <cstdint>
#include "window_input_intercept.h"
#include "window_input_intercept_consumer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const int32_t DEVICE_ID = 12;
}
class WindowInputInterceptConsumer : public IInputEventInterceptConsumer {
public:
    void OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;

    void OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
public:
    std::shared_ptr<MMI::KeyEvent> keyEventResult_ = nullptr;
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ = nullptr;
};

void WindowInputInterceptConsumer::OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    keyEventResult_ = keyEvent;
}

void WindowInputInterceptConsumer::OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    pointerEvent_ = pointerEvent;
}

class WindowInputInterceptTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
};

void WindowInputInterceptTest::SetUp()
{
    WindowInputIntercept::GetInstance().UnRegisterInputEventIntercept(DEVICE_ID);
}

void WindowInputInterceptTest::TearDown() {}

/**
 * @tc.name: IsInputIntercept01
 * @tc.desc: when call IsInputIntercept(KeyEvent) and the callback of the deviceId is exist, return true
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputInterceptTest, IsInputIntercept01, Function | SmallTest | Level2)
{
    std::shared_ptr<WindowInputInterceptConsumer> consumer = std::make_shared<WindowInputInterceptConsumer>();
    WindowInputIntercept::GetInstance().RegisterInputEventIntercept(DEVICE_ID, consumer);
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetDeviceId(DEVICE_ID);
    EXPECT_TRUE(WindowInputIntercept::GetInstance().IsInputIntercept(keyEvent));
    EXPECT_EQ(DEVICE_ID, consumer->keyEventResult_->GetDeviceId());

    WindowInputIntercept::GetInstance().UnRegisterInputEventIntercept(DEVICE_ID);
    EXPECT_FALSE(WindowInputIntercept::GetInstance().IsInputIntercept(keyEvent));
}

/**
 * @tc.name: IsInputIntercept02
 * @tc.desc: when call IsInputIntercept(KeyEvent) and the callback of the deviceId is not exist, return false
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputInterceptTest, IsInputIntercept02, Function | SmallTest | Level2)
{
    std::shared_ptr<WindowInputInterceptConsumer> consumer = std::make_shared<WindowInputInterceptConsumer>();
    WindowInputIntercept::GetInstance().RegisterInputEventIntercept(DEVICE_ID, consumer);
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetDeviceId(0);
    EXPECT_FALSE(WindowInputIntercept::GetInstance().IsInputIntercept(keyEvent));
    EXPECT_TRUE(consumer->keyEventResult_ == nullptr);
}

/**
 * @tc.name: IsInputIntercept03
 * @tc.desc: when call IsInputIntercept(PointerEvent) and the callback of the deviceId is exist, return true
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputInterceptTest, IsInputIntercept03, Function | SmallTest | Level2)
{
    std::shared_ptr<WindowInputInterceptConsumer> consumer = std::make_shared<WindowInputInterceptConsumer>();
    WindowInputIntercept::GetInstance().RegisterInputEventIntercept(DEVICE_ID, consumer);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetDeviceId(DEVICE_ID);
    EXPECT_TRUE(WindowInputIntercept::GetInstance().IsInputIntercept(pointerEvent));
    EXPECT_EQ(DEVICE_ID, consumer->pointerEvent_->GetDeviceId());
}

/**
 * @tc.name: IsInputIntercept04
 * @tc.desc: when call IsInputIntercept(PointerEvent) and the callback of the deviceId is not exist, return false
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputInterceptTest, IsInputIntercept04, Function | SmallTest | Level2)
{
    std::shared_ptr<WindowInputInterceptConsumer> consumer = std::make_shared<WindowInputInterceptConsumer>();
    WindowInputIntercept::GetInstance().RegisterInputEventIntercept(DEVICE_ID, consumer);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetDeviceId(0);
    EXPECT_FALSE(WindowInputIntercept::GetInstance().IsInputIntercept(pointerEvent));
    EXPECT_TRUE(consumer->pointerEvent_ == nullptr);
}
}
}
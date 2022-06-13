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

#include "window_input_channel_test.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using WindowMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
void WindowInputChannelTest::SetUpTestCase()
{
}

void WindowInputChannelTest::TearDownTestCase()
{
}

void WindowInputChannelTest::SetUp()
{
    std::unique_ptr<WindowMocker> m = std::make_unique<WindowMocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("window");
    window_ = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Create("");
}

void WindowInputChannelTest::TearDown()
{
    window_->Destroy();
    window_ = nullptr;
}

namespace {
/**
 * @tc.name: HandlePointerEvent
 * @tc.desc: consume pointer event when receive callback from input
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, HandlePointerEvent, Function | SmallTest | Level2)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    window_->ConsumePointerEvent(pointerEvent);
    inputChannel->HandlePointerEvent(pointerEvent);
}

/**
 * @tc.name: HandleKeyEvent
 * @tc.desc: consume key event when receive callback from input
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, HandleKeyEvent, Function | SmallTest | Level2)
{
    auto keyEvent = MMI::KeyEvent::Create();
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    window_->ConsumeKeyEvent(keyEvent);
    inputChannel->HandleKeyEvent(keyEvent);
}

/**
 * @tc.name: SetInputListener
 * @tc.desc: set input listener when create window
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, SetInputListener, Function | SmallTest | Level2)
{
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    std::shared_ptr<MMI::IInputEventConsumer> listener = std::make_shared<InputEventListener>(InputEventListener());
    inputChannel->SetInputListener(listener);
}
}
} // namespace Rosen
} // namespace OHOS

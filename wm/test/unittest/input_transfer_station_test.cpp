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

#include "input_transfer_station_test.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using WindowMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
void InputTransferStationTest::SetUpTestCase()
{
    std::unique_ptr<WindowMocker> m = std::make_unique<WindowMocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("inputwindow");
    window_ = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Create("");
}

void InputTransferStationTest::TearDownTestCase()
{
}

void InputTransferStationTest::SetUp()
{
}

void InputTransferStationTest::TearDown()
{
}

namespace {
/**
 * @tc.name: AddInputWindow
 * @tc.desc: add input window in station.
 * @tc.type: FUNC
 * @tc.require: AR000GGTUV
 */
HWTEST_F(InputTransferStationTest, AddInputWindow, Function | SmallTest | Level2)
{
    std::shared_ptr<MMI::IInputEventConsumer> listener = std::make_shared<InputEventListener>(InputEventListener());
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener);
    InputTransferStation::GetInstance().AddInputWindow(window_);
}

/**
 * @tc.name: RemoveInputWindow
 * @tc.desc: remove input window in station.
 * @tc.type: FUNC
 * @tc.require: AR000GGTUV
 */
HWTEST_F(InputTransferStationTest, RemoveInputWindow, Function | SmallTest | Level2)
{
    InputTransferStation::GetInstance().RemoveInputWindow(window_);
}

/**
 * @tc.name: SetInputListener
 * @tc.desc: set input listener for inner window
 * @tc.type: FUNC
 * @tc.require: AR000GGTUV
 */
HWTEST_F(InputTransferStationTest, SetInputListener, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    std::shared_ptr<MMI::IInputEventConsumer> listener = std::make_shared<InputEventListener>(InputEventListener());
    InputTransferStation::GetInstance().SetInputListener(windowId, listener);
}
}
} // namespace Rosen
} // namespace OHOS

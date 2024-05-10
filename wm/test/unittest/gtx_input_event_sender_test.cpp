/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "gtx_input_event_sender.h"
#include "window_impl.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using WindowMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class GtxInputEventSenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void GtxInputEventSenderTest::SetUpTestCase()
{
}

void GtxInputEventSenderTest::TearDownTestCase()
{
}

void GtxInputEventSenderTest::SetUp()
{
}

void GtxInputEventSenderTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetTouchEvent
 * @tc.desc: GetTouchEvent.
 * @tc.type: FUNC
 */
HWTEST_F(GtxInputEventSenderTest, GetTouchEvent, Function | SmallTest | Level2)
{
    int32_t testPointerId = 1234;
    auto pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetPointerId(testPointerId);
    Rect testRect{};

    GtxTouchEventInfo touchEvent{};
    GtxInputEventSender::GetInstance().SetOpt(true);
    GtxInputEventSender::GetInstance().SetTouchEvent(testRect, pointerEvent);
    GtxInputEventSender::GetInstance().GetTouchEvent(touchEvent);
    GtxInputEventSender::GetInstance().SetOpt(false);

    EXPECT_EQ(testPointerId, touchEvent.pointerId);
}

}
} // namespace Rosen
} // namespace OHOS
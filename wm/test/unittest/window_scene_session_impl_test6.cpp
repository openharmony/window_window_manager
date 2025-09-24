/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <gtest/gtest.h>
#include <parameters.h>
 
#include "application_context.h"
#include "ability_context_impl.h"
#include "common_test_utils.h"
#include "display_info.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "mock_window_adapter.h"
#include "pointer_event.h"
#include "scene_board_judgement.h"
#include "singleton_mocker.h"
#include "wm_common_inner.h"
#include "window_scene_session_impl.h"
#include "window_session_impl.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace Rosen {
namespace {
using WindowAdapterMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
}
class WindowSceneSessionImplTest6 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void WindowSceneSessionImplTest6::SetUpTestCase()
{
}
 
void WindowSceneSessionImplTest6::TearDownTestCase()
{
}
 
void WindowSceneSessionImplTest6::SetUp()
{
}
 
void WindowSceneSessionImplTest6::TearDown()
{
}
 
namespace {
/**
 * @tc.name: MaximizeEvent
 * @tc.desc: MaximizeEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest6, MaximizeEvent, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MaximizeEvent");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    auto property = window->GetProperty();
    property->SetPersistentId(123);
    SessionInfo sessionInfo;
    sptr<SessionMocker> mockHostSession = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = mockHostSession;
 
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.freeMultiWindowEnable_ = false;
    system::SetParameter("persist.sceneboard.ispcmode", "false");
    window->property_->SetIsPcAppInPad(true);
    window->MaximizeEvent(window->hostSession_);
    EXPECT_EQ(window->property_->GetIsShowDecorInFreeMultiWindow(), true);
 
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.freeMultiWindowEnable_ = true;
    system::SetParameter("persist.sceneboard.ispcmode", "false");
    window->property_->SetIsPcAppInPad(true);
    window->MaximizeEvent(window->hostSession_);
    EXPECT_EQ(window->property_->GetIsShowDecorInFreeMultiWindow(), true);
 
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.freeMultiWindowEnable_ = true;
    system::SetParameter("persist.sceneboard.ispcmode", "true");
    window->property_->SetIsPcAppInPad(false);
    window->MaximizeEvent(window->hostSession_);
    EXPECT_EQ(window->property_->GetIsShowDecorInFreeMultiWindow(), true);
 
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.freeMultiWindowEnable_ = true;
    system::SetParameter("persist.sceneboard.ispcmode", "true");
    window->property_->SetIsPcAppInPad(true);
    window->MaximizeEvent(window->hostSession_);
    EXPECT_EQ(window->property_->GetIsShowDecorInFreeMultiWindow(), true);
 
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.freeMultiWindowEnable_ = true;
    system::SetParameter("persist.sceneboard.ispcmode", "false");
    window->property_->SetIsPcAppInPad(false);
    window->MaximizeEvent(window->hostSession_);
    EXPECT_EQ(window->property_->GetIsShowDecorInFreeMultiWindow(), false);
}
}
} // namespace Rosen
} // namespace OHOS
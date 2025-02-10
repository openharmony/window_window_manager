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

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "ability_context_impl.h"
#include "accessibility_event_info.h"
#include "color_parser.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "window_helper.h"
#include "window_session_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 50000;
};

void WindowSessionImplLayoutTest::SetUpTestCase() {}

void WindowSessionImplLayoutTest::TearDownTestCase() {}

void WindowSessionImplLayoutTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSessionImplLayoutTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: NotifySingleHandTransformChange_TestUIContent
 * @tc.desc: NotifySingleHandTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplLayoutTest, NotifySingleHandTransformChange_TestUIContent, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifySingleHandTransformChange_TestUIContent start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifySingleHandTransformChange_TestUIContent");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(2025);
    std::string url = "";
    window->SetUIContentInner(
        url, nullptr, nullptr, WindowSetUIContentType::DEFAULT, BackupAndRestoreType::NONE, nullptr);
    SingleHandTransform testTransform;
    testTransform.posX = 100;
    window->NotifySingleHandTransformChange(testTransform);
    ASSERT_EQ(testTransform.posX, window->singleHandTransform_.posX);
    GTEST_LOG_(INFO) << "WindowSessionImplLayoutTest: NotifySingleHandTransformChange_TestUIContent end";
}
}
} // namespace Rosen
} // namespace OHOS

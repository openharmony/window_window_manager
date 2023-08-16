/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <cinttypes>
#include <gtest/gtest.h>

#include "display_manager.h"
#include "window_manager_hilog.h"
#include "window.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "PrivateWindowTest"};
}
class PrivateWindowListener : public DisplayManager::IPrivateWindowListener {
public:
    virtual void OnPrivateWindow(bool hasPrivate)
    {
        WLOGFD("private window appeared.");
    };
};

class PrivateWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PrivateWindowTest::SetUpTestCase()
{
}

void PrivateWindowTest::TearDownTestCase()
{
}

void PrivateWindowTest::SetUp()
{
}

void PrivateWindowTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RegisterPrivateWindowListener
 * @tc.desc: Register private window listener test
 * @tc.type: FUNC
 */
HWTEST_F(PrivateWindowTest, RegisterPrivateWindowListener, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    sptr<PrivateWindowListener> listener_ = new PrivateWindowListener();
    if (dm == nullptr) {
        return;
    }
    dm.RegisterPrivateWindowListener(listener_);
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("private", option);
    if (window == nullptr) {
        return;
    }
    window->SetPrivacyMode(true);
    window->Show();
    dm.UnregisterPrivateWindowListener(listener_);
    window->Destroy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS
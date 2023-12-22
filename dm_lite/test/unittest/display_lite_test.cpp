/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "display_manager_lite.h"
#include "mock_display_manager_adapter_lite.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapterLite, MockDisplayManagerAdapterLite>;
class DisplayTestLite : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayTestLite::SetUpTestCase()
{
}

void DisplayTestLite::TearDownTestCase()
{
}

void DisplayTestLite::SetUp()
{
}

void DisplayTestLite::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateDisplayInfo01
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, UpdateDisplayInfo01, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);
    display->UpdateDisplayInfo(displayInfo);
    ASSERT_EQ(displayInfo, nullptr);
}
}
} // namespace Rosen
} // namespace OHOS
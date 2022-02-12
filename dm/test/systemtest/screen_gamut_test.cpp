/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "dm_common.h"
#include "screen_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenGamutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static sptr<Screen> defaultScreen_;
};

sptr<Screen> ScreenGamutTest::defaultScreen_ = nullptr;

void ScreenGamutTest::SetUpTestCase()
{
    auto screens = ScreenManager::GetInstance().GetAllScreens();
    if (screens.size() > 0) {
        defaultScreen_ = screens[0];
    }
}

void ScreenGamutTest::TearDownTestCase()
{
    defaultScreen_ = nullptr;
}

void ScreenGamutTest::SetUp()
{
}

void ScreenGamutTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ScreenGamut01
 * @tc.desc: gamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, ScreenGamut01, Function | MediumTest | Level1)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    std::vector<ScreenColorGamut> colorGamuts;
    ret = defaultScreen_->GetScreenSupportedColorGamuts(colorGamuts);
    ASSERT_EQ(ret, DMError::DM_OK);

    ScreenColorGamut colorGamut;
    ret = defaultScreen_->GetScreenColorGamut(colorGamut);
    ASSERT_EQ(ret, DMError::DM_OK);

    ret = defaultScreen_->SetScreenColorGamut(0);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: ScreenGamut02
 * @tc.desc: gamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, ScreenGamut02, Function | MediumTest | Level1)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    ScreenGamutMap gamutMap;

    ret = defaultScreen_->GetScreenGamutMap(gamutMap);
    ASSERT_EQ(ret, DMError::DM_OK);

    ret = defaultScreen_->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: ScreenGamut03
 * @tc.desc: gamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, ScreenGamut03, Function | MediumTest | Level1)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;

    ret = defaultScreen_->SetScreenColorTransform();
    ASSERT_EQ(ret, DMError::DM_OK);
}
}
} // namespace Rosen
} // namespace OHOS

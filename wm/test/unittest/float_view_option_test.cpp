/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include "float_view_option.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FloatViewOptionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FloatViewOptionTest::SetUpTestCase() {}
void FloatViewOptionTest::TearDownTestCase() {}
void FloatViewOptionTest::SetUp() {}
void FloatViewOptionTest::TearDown() {}

namespace {

/**
 * @tc.name: GetterSetterAndValidity
 * @tc.desc: Cover FvOption setters/getters + validity branches
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewOptionTest, GetterSetterAndValidity, TestSize.Level1)
{
    FvOption opt;

    // default values
    EXPECT_EQ(0u, opt.GetTemplate());
    EXPECT_TRUE(opt.GetUIPath().empty());
    EXPECT_EQ(nullptr, opt.GetStorage());
    EXPECT_EQ(nullptr, opt.GetContext());
    EXPECT_TRUE(opt.GetVisibilityInApp());
    EXPECT_TRUE(opt.GetShowWhenCreate());

    // UIPath validity branches
    EXPECT_FALSE(opt.IsUIPathValid());
    opt.SetUIPath("/data/test/ui.abc");
    EXPECT_TRUE(opt.IsUIPathValid());
    EXPECT_EQ(std::string("/data/test/ui.abc"), opt.GetUIPath());

    // Rect validity branches
    Rect r;
    r.posX_ = 0;
    r.posY_ = 0;
    r.width_ = 0;
    r.height_ = 0;
    opt.SetRect(r);
    EXPECT_FALSE(opt.IsRectValid());

    r.width_ = 1;
    r.height_ = 1;
    opt.SetRect(r);
    EXPECT_TRUE(opt.IsRectValid());
    auto r2 = opt.GetRect();
    EXPECT_EQ(1, r2.width_);
    EXPECT_EQ(1, r2.height_);

    // other setters/getters
    opt.SetTemplate(123);
    EXPECT_EQ(123u, opt.GetTemplate());

    opt.SetVisibilityInApp(false);
    EXPECT_FALSE(opt.GetVisibilityInApp());

    opt.SetShowWhenCreate(false);
    EXPECT_FALSE(opt.GetShowWhenCreate());

    int dummy = 0;
    opt.SetContext(&dummy);
    EXPECT_EQ(&dummy, opt.GetContext());

    // Storage (nullptr/non-null)
    std::shared_ptr<NativeReference> storage;
    opt.SetStorage(storage);
    EXPECT_EQ(nullptr, opt.GetStorage());
}

/**
 * @tc.name: GetFvTemplateInfo
 * @tc.desc: Cover GetFvTemplateInfo assignment
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewOptionTest, GetFvTemplateInfo, TestSize.Level1)
{
    FvOption opt;
    opt.SetTemplate(7);
    opt.SetVisibilityInApp(false);
    opt.SetShowWhenCreate(false);

    Rect r;
    r.posX_ = 10;
    r.posY_ = 20;
    r.width_ = 300;
    r.height_ = 400;
    opt.SetRect(r);

    FloatViewTemplateInfo info;
    opt.GetFvTemplateInfo(info);

    EXPECT_EQ(7u, info.template_);
    EXPECT_FALSE(info.visibleInApp_);
    EXPECT_FALSE(info.showWhenCreate_);
    EXPECT_EQ(10, info.rect_.posX_);
    EXPECT_EQ(20, info.rect_.posY_);
    EXPECT_EQ(300, info.rect_.width_);
    EXPECT_EQ(400, info.rect_.height_);
}

} // namespace
} // namespace Rosen
} // namespace OHOS

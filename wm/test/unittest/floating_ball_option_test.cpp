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

#include <gmock/gmock.h>
#include "parameters.h"
#include "floating_ball_option.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class FloatingBallOptionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FloatingBallOptionTest::SetUpTestCase()
{
}

void FloatingBallOptionTest::TearDownTestCase()
{
}

void FloatingBallOptionTest::SetUp()
{
}

void FloatingBallOptionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: TestParamSetAndGet
 * @tc.desc: TestParamSetAndGet
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionTest, TestParamSetAndGet, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    uint32_t templateType = 1;
    std::string title = "title";
    std::string content = "content";
    std::string color = "#RRGGBB";
    std::shared_ptr<Media::PixelMap> icon = nullptr;
    option->SetTemplate(templateType);
    option->SetTitle(title);
    option->SetContent(content);
    option->SetBackgroundColor(color);
    option->SetIcon(icon);
    EXPECT_EQ(templateType, option->GetTemplate());
    EXPECT_EQ(title, option->GetTitle());
    EXPECT_EQ(content, option->GetContent());
    EXPECT_EQ(nullptr, option->GetIcon());
    EXPECT_EQ(color, option->GetBackgroundColor());
    FloatingBallTemplateBaseInfo fbTemplateBaseInfo;
    option->GetFbTemplateBaseInfo(fbTemplateBaseInfo);
    EXPECT_EQ(title, fbTemplateBaseInfo.title_);
}
}
}
}
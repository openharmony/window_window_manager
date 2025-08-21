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

class FloatingBallOptionNewTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FloatingBallOptionNewTest::SetUpTestCase()
{
}

void FloatingBallOptionNewTest::TearDownTestCase()
{
}

void FloatingBallOptionNewTest::SetUp()
{
}

void FloatingBallOptionNewTest::TearDown()
{
}

namespace {
/**
 * @tc.name: TestParamSetAndGetTemplateType
 * @tc.desc: TestParamSetAndGetTemplateType
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetTemplateType, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    uint32_t templateType = 1;
    option->SetTemplate(templateType);
    EXPECT_EQ(templateType, option->GetTemplate());
}

/**
 * @tc.name: TestParamSetAndGetTitle
 * @tc.desc: TestParamSetAndGetTitle
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetTitle, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    std::string title = "title";
    option->SetTitle(title);
    EXPECT_EQ(title, option->GetTitle());
}

/**
 * @tc.name: TestParamSetAndGetContent
 * @tc.desc: TestParamSetAndGetContent
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetContent, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    std::string content = "content";
    option->SetContent(content);
    EXPECT_EQ(content, option->GetContent());
}

/**
 * @tc.name: TestParamSetAndGetColor
 * @tc.desc: TestParamSetAndGetColor
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetColor, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    std::string color = "#RRGGBB";
    option->SetBackgroundColor(color);
    EXPECT_EQ(color, option->GetBackgroundColor());
}

/**
 * @tc.name: TestParamSetAndGetIcon
 * @tc.desc: TestParamSetAndGetIcon
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetIcon, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    std::shared_ptr<Media::PixelMap> icon = nullptr;
    option->SetIcon(icon);
    EXPECT_EQ(nullptr, option->GetIcon());
}

/**
 * @tc.name: TestParamSetAndGetAssignmentTitle
 * @tc.desc: TestParamSetAndGetAssignmentTitle
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetAssignmentTitle, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    std::string title = "title";
    option->SetTitle(title);

    FloatingBallTemplateBaseInfo fbTemplateBaseInfo;
    option->GetFbTemplateBaseInfo(fbTemplateBaseInfo);
    EXPECT_EQ(title, fbTemplateBaseInfo.title_);
}

/**
 * @tc.name: TestParamSetAndGetAssignmentTemplateType
 * @tc.desc: TestParamSetAndGetAssignmentTemplateType
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetAssignmentTemplateType, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    uint32_t templateType = 1;
    option->SetTemplate(templateType);

    FloatingBallTemplateBaseInfo fbTemplateBaseInfo;
    option->GetFbTemplateBaseInfo(fbTemplateBaseInfo);
    EXPECT_EQ(templateType, fbTemplateBaseInfo.template_);
}

/**
 * @tc.name: TestParamSetAndGetAssignmentContent
 * @tc.desc: TestParamSetAndGetAssignmentContent
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetAssignmentContent, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    std::string content = "content";
    option->SetContent(content);

    FloatingBallTemplateBaseInfo fbTemplateBaseInfo;
    option->GetFbTemplateBaseInfo(fbTemplateBaseInfo);
    EXPECT_EQ(content, fbTemplateBaseInfo.content_);
}

/**
 * @tc.name: TestParamSetAndGetAssignmentColor
 * @tc.desc: TestParamSetAndGetAssignmentColor
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallOptionNewTest, TestParamSetAndGetAssignmentColor, TestSize.Level1)
{
    auto option = sptr<FbOption>::MakeSptr();
    std::string color = "#RRGGBB";
    option->SetBackgroundColor(color);
    EXPECT_EQ(color, option->GetBackgroundColor());

    FloatingBallTemplateBaseInfo fbTemplateBaseInfo;
    option->GetFbTemplateBaseInfo(fbTemplateBaseInfo);
    EXPECT_EQ(color, fbTemplateBaseInfo.backgroundColor_);
}
}
}
}
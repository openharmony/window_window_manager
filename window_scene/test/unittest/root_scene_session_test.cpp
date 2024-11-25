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

#include "root_scene_session.h"
#include <gtest/gtest.h>
#include "session_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RootSceneSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void LoadContentFuncTest(const std::string&, napi_env, napi_value, AbilityRuntime::Context*)
{
}

void RootSceneSessionTest::SetUpTestCase()
{
}

void RootSceneSessionTest::TearDownTestCase()
{
}

void RootSceneSessionTest::SetUp()
{
}

void RootSceneSessionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetLoadContentFunc
 * @tc.desc: test function : SetLoadContentFunc
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneSessionTest, SetLoadContentFunc, Function | SmallTest | Level1)
{
    RootSceneSession rootSceneSession;
    RootSceneSession::LoadContentFunc loadContentFunc_ = LoadContentFuncTest;
    rootSceneSession.SetLoadContentFunc(loadContentFunc_);
    ASSERT_FALSE(rootSceneSession.IsVisible());
}

/**
 * @tc.name: LoadContentFunc
 * @tc.desc: test function : LoadContentFunc
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneSessionTest, LoadContentFunc, Function | SmallTest | Level1)
{
    RootSceneSession rootSceneSession;
    std::string strTest("LoadContentFuncTest");
    napi_env nativeEnv_ = nullptr;
    napi_value nativeValue_ = nullptr;
    AbilityRuntime::Context* conText_ = nullptr;
    rootSceneSession.LoadContent(strTest, nativeEnv_, nativeValue_, conText_);
    ASSERT_FALSE(rootSceneSession.IsVisible());
}

/**
 * @tc.name: LoadContent
 * @tc.desc: test function : LoadContent
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneSessionTest, LoadContent, Function | SmallTest | Level1)
{
    RootSceneSession rootSceneSession;
    std::string strTest("LoadContentFuncTest");
    napi_env nativeEnv_ = nullptr;
    napi_value nativeValue_ = nullptr;
    AbilityRuntime::Context* conText_ = nullptr;
    RootSceneSession::LoadContentFunc loadContentFunc_ = LoadContentFuncTest;
    rootSceneSession.SetLoadContentFunc(loadContentFunc_);
    rootSceneSession.LoadContent(strTest, nativeEnv_, nativeValue_, conText_);
    ASSERT_FALSE(rootSceneSession.IsVisible());
}

/**
 * @tc.name: GetSystemAvoidAreaForRoot
 * @tc.desc: test function : GetSystemAvoidAreaForRoot
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneSessionTest, GetSystemAvoidAreaForRoot, Function | SmallTest | Level1)
{
    RootSceneSession rootSceneSession;
    AvoidArea avoidArea;
    WSRect rect;
    rootSceneSession.GetSystemAvoidAreaForRoot(rect, avoidArea);
    EXPECT_TRUE(avoidArea.isEmptyAvoidArea());
}

/**
 * @tc.name: GetKeyboardAvoidAreaForRoot
 * @tc.desc: test function : GetKeyboardAvoidAreaForRoot
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneSessionTest, GetKeyboardAvoidAreaForRoot, Function | SmallTest | Level1)
{
    RootSceneSession rootSceneSession;
    AvoidArea avoidArea;
    WSRect rect;
    rootSceneSession.GetKeyboardAvoidAreaForRoot(rect, avoidArea);
    EXPECT_TRUE(avoidArea.isEmptyAvoidArea());
}

/**
 * @tc.name: GetAINavigationBarAreaForRoot
 * @tc.desc: test function : GetAINavigationBarAreaForRoot
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneSessionTest, GetAINavigationBarAreaForRoot, Function | SmallTest | Level1)
{
    RootSceneSession rootSceneSession;
    AvoidArea avoidArea;
    WSRect rect;
    rootSceneSession.GetAINavigationBarAreaForRoot(rect, avoidArea);
    EXPECT_TRUE(avoidArea.isEmptyAvoidArea());
}

/**
 * @tc.name: GetCutoutAvoidAreaForRoot
 * @tc.desc: test function : GetCutoutAvoidAreaForRoot
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneSessionTest, GetCutoutAvoidAreaForRoot, Function | SmallTest | Level1)
{
    RootSceneSession rootSceneSession;
    AvoidArea avoidArea;
    WSRect rect;
    rootSceneSession.GetCutoutAvoidAreaForRoot(rect, avoidArea);
    EXPECT_TRUE(avoidArea.isEmptyAvoidArea());
}
}
}
}
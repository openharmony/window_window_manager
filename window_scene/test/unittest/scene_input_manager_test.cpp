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

#include "scene_input_manager.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneInputManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneInputManagerTest::SetUpTestCase()
{
}

void SceneInputManagerTest::TearDownTestCase()
{
}

void SceneInputManagerTest::SetUp()
{
}

void SceneInputManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: FlushDisplayInfoToMMI
 * @tc.desc: check func FlushDisplayInfoToMMI
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, FlushDisplayInfoToMMI, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "SceneInputManagerTest: FlushDisplayInfoToMMI start";
    int ret = 0;
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "SceneInputManagerTest: FlushDisplayInfoToMMI end";
}

/**
* @tc.name: NotifyWindowInfoChange
* @tc.desc: check func NotifyWindowInfoChange
* @tc.type: FUNC
*/
HWTEST_F(SceneInputManagerTest, NotifyWindowInfoChange, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "SceneInputManagerTest: NotifyWindowInfoChange start";
    SessionInfo info;
    info.abilityName_ = "BindDialogSessionTarget";
    info.bundleName_ = "BindDialogSessionTarget";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_
            = new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new SceneSession(info, specificCallback_);

    SceneInputManager::GetInstance()
    .NotifyWindowInfoChange(sceneSession, WindowUpdateType::WINDOW_UPDATE_ADDED);
    GTEST_LOG_(INFO) << "SceneInputManagerTest: NotifyWindowInfoChange end";
}

/**
* @tc.name: NotifyWindowInfoChangeFromSession
* @tc.desc: check func NotifyWindowInfoChangeFromSession
* @tc.type: FUNC
*/
HWTEST_F(SceneInputManagerTest, NotifyWindowInfoChangeFromSession, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "SceneInputManagerTest: NotifyWindowInfoChangeFromSession start";
    SessionInfo info;
    info.abilityName_ = "BindDialogSessionTarget";
    info.bundleName_ = "BindDialogSessionTarget";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_
            = new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new SceneSession(info, specificCallback_);

    SceneInputManager::GetInstance().NotifyWindowInfoChangeFromSession(sceneSession);
    GTEST_LOG_(INFO) << "SceneInputManagerTest: NotifyWindowInfoChangeFromSession end";
}

/**
* @tc.name: NotifyMMIWindowPidChange
* @tc.desc: check func NotifyMMIWindowPidChange
* @tc.type: FUNC
*/
HWTEST_F(SceneInputManagerTest, NotifyMMIWindowPidChange, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "SceneInputManagerTest: NotifyMMIWindowPidChange start";
    SessionInfo info;
    info.abilityName_ = "BindDialogSessionTarget";
    info.bundleName_ = "BindDialogSessionTarget";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_
            = new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new SceneSession(info, specificCallback_);

    SceneInputManager::GetInstance().NotifyMMIWindowPidChange(sceneSession, true);
    GTEST_LOG_(INFO) << "SceneInputManagerTest: NotifyMMIWindowPidChange end";
}

}
}
}
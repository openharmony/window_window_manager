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
#include "session_manager/include/scene_session_manager.h"

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
    static sptr<SceneSessionManager> ssm_;
};

sptr<SceneSessionManager> SceneInputManagerTest::ssm_ = nullptr;

void SceneInputManagerTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneInputManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneInputManagerTest::SetUp()
{
}

void SceneInputManagerTest::TearDown()
{
}

namespace {
void CheckNeedUpdateTest()
{
    SceneInputManager::GetInstance().lastFocusId_ = -1;
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();

    SceneInputManager::GetInstance().lastWindowInfoList_.clear();
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    SceneInputManager::GetInstance().lastDisplayInfos_.clear();
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    SceneInputManager::GetInstance().lastWindowInfoList_.clear();
    SceneInputManager::GetInstance().lastDisplayInfos_.clear();
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();

    if (SceneInputManager::GetInstance().lastDisplayInfos_.size() != 0) {
        MMI::DisplayInfo displayInfo;
        SceneInputManager::GetInstance().lastDisplayInfos_[0] = displayInfo;
        SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    }

    if (SceneInputManager::GetInstance().lastWindowInfoList_.size() != 0) {
        MMI::WindowInfo windowInfo;
        SceneInputManager::GetInstance().lastWindowInfoList_[0] = windowInfo;
        SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    }
}


void WindowInfoListZeroTest(sptr<SceneSessionManager> ssm_)
{
    const auto sceneSessionMap = ssm_->GetSceneSessionMap();
    for (auto sceneSession : sceneSessionMap) {
        ssm_->DestroyDialogWithMainWindow(sceneSession.second);
    }
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();

    for (auto sceneSession : sceneSessionMap) {
        sptr<WindowSessionProperty> windowSessionProperty = new WindowSessionProperty();
        windowSessionProperty->SetWindowType(sceneSession.second->GetWindowType());
        ssm_->RequestSceneSession(sceneSession.second->GetSessionInfo(), windowSessionProperty);
    }
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
}

void MaxWindowInfoTest(sptr<SceneSessionManager> ssm_)
{
    std::vector<sptr<SceneSession>> sessionList;
    int maxWindowInfoNum = 20;
    int32_t idStart = 1000;
    for (int i = 0; i < maxWindowInfoNum; i++) {
        SessionInfo info;
        info.abilityName_ = "test" + std::to_string(i);
        info.bundleName_ = "test" + std::to_string(i);
        info.appIndex_ = idStart + i;
        sptr<WindowSessionProperty> windowSessionProperty = new WindowSessionProperty();
        ASSERT_NE(windowSessionProperty, nullptr);
        windowSessionProperty->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
        auto sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
        if (sceneSession != nullptr) {
            sessionList.push_back(sceneSession);
        }
    }
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();

    for (auto session : sessionList) {
        ssm_->DestroyDialogWithMainWindow(session);
    }
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
}

/**
 * @tc.name: FlushDisplayInfoToMMI
 * @tc.desc: check func FlushDisplayInfoToMMI
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, FlushDisplayInfoToMMI, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "SceneInputManagerTest: FlushDisplayInfoToMMI start";
    int ret = 0;
    // sceneSessionDirty_ = nullptr
    auto oldDirty = SceneInputManager::GetInstance().sceneSessionDirty_;
    SceneInputManager::GetInstance().sceneSessionDirty_ = nullptr;
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    SceneInputManager::GetInstance().sceneSessionDirty_ = oldDirty;

    // NotNeedUpdate
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();

    CheckNeedUpdateTest();
    WindowInfoListZeroTest(ssm_);
    MaxWindowInfoTest(ssm_);

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
    info.abilityName_ = "NotifyWindowInfoChange";
    info.bundleName_ = "NotifyWindowInfoChange";
    info.appIndex_ = 10;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_
            = new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new SceneSession(info, specificCallback_);

    // sceneSessionDirty_ = nullptr
    auto oldDirty = SceneInputManager::GetInstance().sceneSessionDirty_;
    SceneInputManager::GetInstance().sceneSessionDirty_ = nullptr;
    SceneInputManager::GetInstance()
    .NotifyWindowInfoChange(sceneSession, WindowUpdateType::WINDOW_UPDATE_ADDED);
    SceneInputManager::GetInstance().sceneSessionDirty_ = oldDirty;

    SceneInputManager::GetInstance()
    .NotifyWindowInfoChange(sceneSession, WindowUpdateType::WINDOW_UPDATE_ADDED);
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
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
    info.abilityName_ = "NotifyWindowInfoChangeFromSession";
    info.bundleName_ = "NotifyWindowInfoChangeFromSession";
    info.appIndex_ = 100;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_
            = new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new SceneSession(info, specificCallback_);

    // sceneSessionDirty_ = nullptr
    auto oldDirty = SceneInputManager::GetInstance().sceneSessionDirty_;
    SceneInputManager::GetInstance().sceneSessionDirty_ = nullptr;
    SceneInputManager::GetInstance().NotifyWindowInfoChangeFromSession(sceneSession);
    SceneInputManager::GetInstance().sceneSessionDirty_ = oldDirty;

    SceneInputManager::GetInstance().NotifyWindowInfoChangeFromSession(sceneSession);
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
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
    info.abilityName_ = "NotifyMMIWindowPidChange";
    info.bundleName_ = "NotifyMMIWindowPidChange";
    info.appIndex_ = 1000;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_
            = new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new SceneSession(info, specificCallback_);

    // sceneSessionDirty_ = nullptr
    auto oldDirty = SceneInputManager::GetInstance().sceneSessionDirty_;
    SceneInputManager::GetInstance().sceneSessionDirty_ = nullptr;
    SceneInputManager::GetInstance().NotifyMMIWindowPidChange(sceneSession, true);
    SceneInputManager::GetInstance().sceneSessionDirty_ = oldDirty;

    SceneInputManager::GetInstance().NotifyMMIWindowPidChange(sceneSession, true);
    EXPECT_TRUE(sceneSession->IsStartMoving());
    SceneInputManager::GetInstance().NotifyMMIWindowPidChange(sceneSession, false);
    EXPECT_FALSE(sceneSession->IsStartMoving());
    SceneInputManager::GetInstance().NotifyMMIWindowPidChange(nullptr, false);
    EXPECT_FALSE(sceneSession->IsStartMoving());
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    GTEST_LOG_(INFO) << "SceneInputManagerTest: NotifyMMIWindowPidChange end";
}

}
}
}
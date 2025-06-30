/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include <gtest/gtest.h>

#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string EMPTY_DEVICE_ID = "";
constexpr float SINGLE_HAND_SCALE = 0.75f;
constexpr float SINGLE_HAND_DEFAULT_SCALE = 1.0f;
} // namespace
class SceneSessionManagerLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<SceneSessionManager> ssm_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerLayoutTest::ssm_ = nullptr;

void SceneSessionManagerLayoutTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerLayoutTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerLayoutTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerLayoutTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: GetNormalSingleHandTransform
 * @tc.desc: test function : GetNormalSingleHandTransform
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLayoutTest, GetNormalSingleHandTransform, TestSize.Level1)
{
    SingleHandTransform preTransform = ssm_->singleHandTransform_;
    ssm_->singleHandTransform_.posX = 100;
    EXPECT_EQ(100, ssm_->GetNormalSingleHandTransform().posX);
    ssm_->singleHandTransform_ = preTransform;
}

/**
 * @tc.name: NotifySingleHandInfoChange_TestUIType
 * @tc.desc: test function : NotifySingleHandInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLayoutTest, NotifySingleHandInfoChange_TestUIType, TestSize.Level1)
{
    SingleHandTransform singleHandTransform;
    ssm_->singleHandTransform_ = singleHandTransform;
    SingleHandScreenInfo singleHandScreenInfo;
    WSRect originRect, singleHandRect;
    singleHandScreenInfo.scaleRatio = SINGLE_HAND_SCALE;
    singleHandScreenInfo.mode = SingleHandMode::LEFT;
    originRect = { 0, 0, 400, 600 };
    singleHandRect = { 0, 100, 200, 300 };
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(0, screenSession));

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_NE(singleHandScreenInfo.scaleRatio, ssm_->singleHandTransform_.scaleX);

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_EQ(singleHandScreenInfo.scaleRatio, ssm_->singleHandTransform_.scaleX);
    ssm_->singleHandTransform_ = singleHandTransform;
}

/**
 * @tc.name: NotifySingleHandInfoChange_TestWindowName
 * @tc.desc: test function : NotifySingleHandInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLayoutTest, NotifySingleHandInfoChange_TestWindowName, TestSize.Level1)
{
    SingleHandTransform singleHandTransform;
    ssm_->singleHandTransform_ = singleHandTransform;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "OneHandModeBackground_testWindow";
    sessionInfo.abilityName_ = "OneHandModeBackground_testWindow";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->property_->SetWindowName("OneHandModeBackground_testWindow");
    ssm_->sceneSessionMap_.insert({ sceneSession->GetPersistentId(), sceneSession });
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(0, screenSession));
    SingleHandScreenInfo singleHandScreenInfo;
    WSRect originRect, singleHandRect;
    originRect = { 0, 0, 400, 600 };
    singleHandRect = { 0, 100, 200, 300 };
    singleHandScreenInfo.scaleRatio = SINGLE_HAND_SCALE;
    singleHandScreenInfo.mode = SingleHandMode::LEFT;
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_NE(singleHandScreenInfo.scaleRatio, sceneSession->singleHandTransform_.scaleX);
}

/**
 * @tc.name: NotifySingleHandInfoChange_TestDisplayId
 * @tc.desc: test function : NotifySingleHandInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLayoutTest, NotifySingleHandInfoChange_TestDisplayId, TestSize.Level1)
{
    SingleHandTransform singleHandTransform;
    ssm_->singleHandTransform_ = singleHandTransform;
    SingleHandScreenInfo singleHandScreenInfo;
    WSRect originRect, singleHandRect;
    originRect = { 0, 0, 400, 600 };
    singleHandRect = { 0, 100, 200, 300 };
    singleHandScreenInfo.scaleRatio = SINGLE_HAND_SCALE;
    singleHandScreenInfo.mode = SingleHandMode::LEFT;
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(0, screenSession));
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "NotifySingleHandInfoChange_TestDisplayId";
    sessionInfo.abilityName_ = "NotifySingleHandInfoChange_TestDisplayId";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->GetSessionProperty()->SetDisplayId(2025);
    ssm_->sceneSessionMap_.insert({ sceneSession->GetPersistentId(), sceneSession });
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_NE(singleHandScreenInfo.scaleRatio, sceneSession->singleHandTransform_.scaleX);

    sceneSession->GetSessionProperty()->SetDisplayId(0);
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_EQ(singleHandScreenInfo.scaleRatio, sceneSession->singleHandTransform_.scaleY);
    ssm_->singleHandTransform_ = singleHandTransform;
}

/**
 * @tc.name: NotifySingleHandInfoChange_TestMode
 * @tc.desc: test function : NotifySingleHandInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLayoutTest, NotifySingleHandInfoChange_TestMode, TestSize.Level1)
{
    SingleHandTransform singleHandTransform;
    ssm_->singleHandTransform_ = singleHandTransform;
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(0, screenSession));

    SingleHandScreenInfo singleHandScreenInfo;
    WSRect originRect, singleHandRect;
    originRect = { 0, 0, 400, 600 };
    singleHandRect = { 0, 100, 200, 300 };
    singleHandScreenInfo.scaleRatio = SINGLE_HAND_SCALE;
    singleHandScreenInfo.mode = SingleHandMode::LEFT;
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_EQ(100, ssm_->singleHandTransform_.posY);
    EXPECT_EQ(0, ssm_->singleHandTransform_.posX);
    ssm_->singleHandTransform_ = singleHandTransform;

    singleHandScreenInfo.mode = SingleHandMode::RIGHT;
    singleHandRect = { 50, 100, 200, 300 };
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_EQ(100, ssm_->singleHandTransform_.posY);
    EXPECT_EQ(50, ssm_->singleHandTransform_.posX);
    ssm_->singleHandTransform_ = singleHandTransform;

    singleHandScreenInfo.scaleRatio = SINGLE_HAND_DEFAULT_SCALE;
    singleHandScreenInfo.mode = SingleHandMode::MIDDLE;
    singleHandRect = { 0, 0, 200, 300 };
    ssm_->NotifySingleHandInfoChange(singleHandScreenInfo, originRect, singleHandRect);
    usleep(WAIT_SYNC_IN_NS);
    EXPECT_EQ(0, ssm_->singleHandTransform_.posY);
    EXPECT_EQ(0, ssm_->singleHandTransform_.posX);
    ssm_->singleHandTransform_ = singleHandTransform;
}

/**
 * @tc.name: GetDisplaySizeById_TestDisplayId
 * @tc.desc: test function : GetDisplaySizeById
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLayoutTest, GetDisplaySizeById_TestDisplayId, TestSize.Level1)
{
    DisplayId displayId = 2025;
    int32_t displayWidth = 0;
    int32_t displayHeight = 0;
    EXPECT_EQ(false, ssm_->GetDisplaySizeById(displayId, displayWidth, displayHeight));

    displayId = 0;
    EXPECT_EQ(true, ssm_->GetDisplaySizeById(displayId, displayWidth, displayHeight));
}

/**
 * @tc.name: UpdateWindowModeByIdForUITest01
 * @tc.desc: test function : UpdateWindowModeByIdForUITest
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLayoutTest, UpdateWindowModeByIdForUITest01, TestSize.Level1)
{
    const int32_t windowId = 0;
    const int32_t updateMode = 1;
    EXPECT_EQ(ssm_->UpdateWindowModeByIdForUITest(windowId, updateMode), WMError::WM_ERROR_INVALID_PERMISSION);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

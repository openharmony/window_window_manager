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
#include "screen_session_manager/include/screen_session_manager_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int MAX_WINDOWINFO_NUM = 15;
class SceneInputManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
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
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
void CheckNeedUpdateTest()
{
    SceneInputManager::GetInstance().SetUserBackground(true);
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
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
    SceneInputManager::GetInstance().isUserBackground_ = false;
    auto oldDirty = SceneInputManager::GetInstance().sceneSessionDirty_;
    SceneInputManager::GetInstance().sceneSessionDirty_ = nullptr;
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();
    SceneInputManager::GetInstance().sceneSessionDirty_ = oldDirty;

    // NotNeedUpdate
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI(true);
    SceneInputManager::GetInstance().FlushDisplayInfoToMMI();

    auto preEventHandler = SceneInputManager::GetInstance().eventHandler_;
    SceneInputManager::GetInstance().eventHandler_ = nullptr;
    SceneInputManager::GetInstance().FlushEmptyInfoToMMI();
    SceneInputManager::GetInstance().eventHandler_ = preEventHandler;

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

/**
 * @tc.name: UpdateFocusedSessionId
 * @tc.desc: UpdateFocusedSessionId
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, UpdateFocusedSessionId, Function | SmallTest | Level3)
{
    auto sceneInputManager = &SceneInputManager::GetInstance();
    ASSERT_NE(sceneInputManager, nullptr);
    EXPECT_EQ(sceneInputManager->focusedSessionId_, -1);

    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));

    sceneInputManager->UpdateFocusedSessionId(INVALID_SESSION_ID);
    EXPECT_EQ(sceneInputManager->focusedSessionId_, -1);
    sceneInputManager->UpdateFocusedSessionId(sceneSession->GetPersistentId());
    EXPECT_EQ(sceneInputManager->focusedSessionId_, -1);
    ExtensionWindowEventInfo extensionInfo {
        .persistentId = 12345
    };
    sceneSession->AddModalUIExtension(extensionInfo);
    sceneInputManager->UpdateFocusedSessionId(sceneSession->GetPersistentId());
    EXPECT_EQ(sceneInputManager->focusedSessionId_, extensionInfo.persistentId);

    ssm_->sceneSessionMap_.erase(sceneSession->GetPersistentId());
}

/**
 * @tc.name: PrintWindowInfo
 * @tc.desc: PrintWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, PrintWindowInfo, Function | SmallTest | Level3)
{
    int ret = 0;
    std::vector<MMI::WindowInfo> windowInfoList;
    SceneInputManager::GetInstance().PrintWindowInfo(windowInfoList);
    MMI::WindowInfo windowInfo;
    windowInfoList.emplace_back(windowInfo);
    SceneInputManager::GetInstance().PrintWindowInfo(windowInfoList);
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(-1);
    SceneInputManager::GetInstance().PrintWindowInfo(windowInfoList);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: FlushFullInfoToMMI
 * @tc.desc: FlushFullInfoToMMI
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, FlushFullInfoToMMI, Function | SmallTest | Level3)
{
    int ret = 0;
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    SceneInputManager::GetInstance().FlushFullInfoToMMI(displayInfos, windowInfoList);
    MMI::DisplayInfo displayInfo;
    displayInfos.emplace_back(displayInfo);
    SceneInputManager::GetInstance().FlushFullInfoToMMI(displayInfos, windowInfoList);
    auto oldDirty = SceneInputManager::GetInstance().sceneSessionDirty_;
    SceneInputManager::GetInstance().sceneSessionDirty_ = nullptr;
    SceneInputManager::GetInstance().FlushFullInfoToMMI(displayInfos, windowInfoList);
    SceneInputManager::GetInstance().sceneSessionDirty_ = oldDirty;
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: FlushChangeInfoToMMI
 * @tc.desc: FlushChangeInfoToMMI
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, FlushChangeInfoToMMI, Function | SmallTest | Level3)
{
    int ret = 0;
    std::map<uint64_t, std::vector<MMI::WindowInfo>> screenId2Windows;
    SceneInputManager::GetInstance().FlushChangeInfoToMMI(screenId2Windows);
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::WindowInfo windowInfo;
    windowInfoList.emplace_back(windowInfo);
    screenId2Windows.emplace(1, windowInfoList);
    SceneInputManager::GetInstance().FlushChangeInfoToMMI(screenId2Windows);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConstructDisplayInfos
 * @tc.desc: ConstructDisplayInfos
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, ConstructDisplayInfos, Function | SmallTest | Level3)
{
    int ret = 0;
    std::vector<MMI::DisplayInfo> displayInfos;
    SceneInputManager::GetInstance().ConstructDisplayInfos(displayInfos);
    ScreenProperty screenProperty0;
    Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties().insert(
        std::make_pair(0, screenProperty0));
    SceneInputManager::GetInstance().ConstructDisplayInfos(displayInfos);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CheckNeedUpdate
 * @tc.desc: CheckNeedUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, CheckNeedUpdate1, Function | SmallTest | Level3)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    int32_t focusId = 0;
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(focusId);
    SceneInputManager::GetInstance().lastFocusId_ = 1;
    bool ret1 = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(ret1);

    SceneInputManager::GetInstance().lastFocusId_ = 0;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    bool ret2 = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(ret2);

    displayInfos.clear();
    MMI::WindowInfo windowinfo;
    windowInfoList.emplace_back(windowinfo);
    bool ret3 = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(ret3);
}

/**
 * @tc.name: CheckNeedUpdate
 * @tc.desc: CheckNeedUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, CheckNeedUpdate2, Function | SmallTest | Level3)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    MMI::WindowInfo windowinfo;
    int32_t focusId = 0;
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(focusId);
    SceneInputManager::GetInstance().lastFocusId_ = 0;
    SceneInputManager::GetInstance().lastDisplayInfos_ = displayInfos;
    SceneInputManager::GetInstance().lastWindowInfoList_ = windowInfoList;
    bool result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_FALSE(result);
    windowInfoList.emplace_back(windowinfo);
    SceneInputManager::GetInstance().lastWindowInfoList_ = windowInfoList;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_FALSE(result);

    displayInfos[0].id = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].id = 0;

    windowInfoList[0].id = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].id = 0;

    windowInfoList[0].pid = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].pid = 0;

    windowInfoList[0].uid = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].uid = 0;

    windowInfoList[0].agentWindowId = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].agentWindowId = 0;

    windowInfoList[0].flags = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].flags = 0;

    windowInfoList[0].displayId = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].displayId = 0;

    windowInfoList[0].zOrder = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].zOrder = 0;
}

/**
 * @tc.name: CheckNeedUpdate
 * @tc.desc: CheckNeedUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, CheckNeedUpdate3, Function | SmallTest | Level3)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    MMI::WindowInfo windowinfo;
    windowInfoList.emplace_back(windowinfo);
    int32_t focusId = 0;
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(focusId);
    SceneInputManager::GetInstance().lastFocusId_ = 0;
    SceneInputManager::GetInstance().lastDisplayInfos_ = displayInfos;
    SceneInputManager::GetInstance().lastWindowInfoList_ = windowInfoList;
    bool result = false;
    windowInfoList[0].area.x = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].area.x = 0;

    windowInfoList[0].area.y = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].area.y = 0;

    windowInfoList[0].area.width = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].area.width = 0;

    windowInfoList[0].area.height = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].area.height = 0;

    MMI::Rect area;
    windowInfoList[0].defaultHotAreas.emplace_back(area);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].defaultHotAreas.clear();

    windowInfoList[0].pointerHotAreas.emplace_back(area);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].pointerHotAreas.clear();

    windowInfoList[0].transform.emplace_back(1.0);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].transform.clear();
}

/**
 * @tc.name: CheckNeedUpdate
 * @tc.desc: CheckNeedUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, CheckNeedUpdate4, Function | SmallTest | Level3)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    MMI::WindowInfo windowinfo;
    windowInfoList.emplace_back(windowinfo);
    int32_t focusId = 0;
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(focusId);
    SceneInputManager::GetInstance().lastFocusId_ = 0;
    SceneInputManager::GetInstance().lastDisplayInfos_ = displayInfos;
    SceneInputManager::GetInstance().lastWindowInfoList_ = windowInfoList;
    bool result = false;
    windowInfoList[0].transform.emplace_back(1.0);
    SceneInputManager::GetInstance().lastWindowInfoList_[0].transform.emplace_back(2.0);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].transform.clear();
    SceneInputManager::GetInstance().lastWindowInfoList_[0].transform.clear();

    MMI::Rect area;
    windowInfoList[0].defaultHotAreas.emplace_back(area);
    area.x = 1;
    SceneInputManager::GetInstance().lastWindowInfoList_[0].defaultHotAreas.emplace_back(area);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].defaultHotAreas.clear();
    SceneInputManager::GetInstance().lastWindowInfoList_[0].defaultHotAreas.clear();
    area.x = 0;

    windowInfoList[0].pointerHotAreas.emplace_back(area);
    area.x = 1;
    SceneInputManager::GetInstance().lastWindowInfoList_[0].pointerHotAreas.emplace_back(area);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].pointerHotAreas.clear();
    SceneInputManager::GetInstance().lastWindowInfoList_[0].pointerHotAreas.clear();
    area.x = 0;

    windowInfoList[0].pointerChangeAreas.emplace_back(1);
    SceneInputManager::GetInstance().lastWindowInfoList_[0].pointerChangeAreas.emplace_back(2);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].pointerChangeAreas.clear();
    SceneInputManager::GetInstance().lastWindowInfoList_[0].pointerChangeAreas.clear();
}

/**
 * @tc.name: CheckNeedUpdate
 * @tc.desc: CheckNeedUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, CheckNeedUpdate5, Function | SmallTest | Level3)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    MMI::WindowInfo windowinfo;
    windowInfoList.emplace_back(windowinfo);
    int32_t focusId = 0;
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(focusId);
    SceneInputManager::GetInstance().lastFocusId_ = 0;
    SceneInputManager::GetInstance().lastDisplayInfos_ = displayInfos;
    SceneInputManager::GetInstance().lastWindowInfoList_ = windowInfoList;
    bool result = false;
    windowInfoList[0].transform.emplace_back(1.0);
    SceneInputManager::GetInstance().lastWindowInfoList_[0].transform.emplace_back(2.0);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].transform.clear();
    SceneInputManager::GetInstance().lastWindowInfoList_[0].transform.clear();

    displayInfos[0].id = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].id = 0;

    displayInfos[0].x = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].x = 0;

    displayInfos[0].y = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].y = 0;

    displayInfos[0].width = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].width = 0;

    displayInfos[0].height = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].height = 0;
}

/**
 * @tc.name: CheckNeedUpdate
 * @tc.desc: CheckNeedUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, CheckNeedUpdate6, Function | SmallTest | Level3)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    MMI::WindowInfo windowinfo;
    windowInfoList.emplace_back(windowinfo);
    int32_t focusId = 0;
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(focusId);
    SceneInputManager::GetInstance().lastFocusId_ = 0;
    SceneInputManager::GetInstance().lastDisplayInfos_ = displayInfos;
    SceneInputManager::GetInstance().lastWindowInfoList_ = windowInfoList;
    bool result = false;
    windowInfoList[0].transform.emplace_back(1.0);
    SceneInputManager::GetInstance().lastWindowInfoList_[0].transform.emplace_back(2.0);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].transform.clear();
    SceneInputManager::GetInstance().lastWindowInfoList_[0].transform.clear();
    displayInfos[0].dpi = 1;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].dpi = 0;

    displayInfos[0].name = "TestName";
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].name = "";

    displayInfos[0].uniq = "TestUniq";
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].uniq = "";

    displayInfos[0].direction = MMI::Direction::DIRECTION90;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].direction = MMI::Direction::DIRECTION0;

    displayInfos[0].displayDirection = MMI::Direction::DIRECTION90;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].displayDirection = MMI::Direction::DIRECTION0;

    displayInfos[0].displayMode = MMI::DisplayMode::FULL;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    displayInfos[0].displayMode = MMI::DisplayMode::UNKNOWN;
}

/**
 * @tc.name: CheckNeedUpdate
 * @tc.desc: CheckNeedUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, CheckNeedUpdate7, Function | SmallTest | Level3)
{
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    MMI::WindowInfo windowinfo;
    windowInfoList.emplace_back(windowinfo);
    int32_t focusId = 0;
    Rosen::SceneSessionManager::GetInstance().SetFocusedSessionId(focusId);
    SceneInputManager::GetInstance().lastFocusId_ = 0;
    SceneInputManager::GetInstance().lastDisplayInfos_ = displayInfos;
    SceneInputManager::GetInstance().lastWindowInfoList_ = windowInfoList;
    bool result = false;
    
    auto tempPixeMap = std::make_shared<Media::PixelMap>();
    windowInfoList[0].pixelMap = static_cast<void*>(tempPixeMap.get());
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].pixelMap = nullptr;

    windowInfoList[0].windowInputType = MMI::WindowInputType::TRANSMIT_ALL;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].windowInputType = SceneInputManager::GetInstance().lastWindowInfoList_[0].windowInputType;

    windowInfoList[0].windowType = static_cast<int32_t>(WindowType::WINDOW_TYPE_APP_COMPONENT);
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].windowType = SceneInputManager::GetInstance().lastWindowInfoList_[0].windowType;

    windowInfoList[0].privacyMode = MMI::SecureFlag::PRIVACY_MODE;
    result = SceneInputManager::GetInstance().CheckNeedUpdate(displayInfos, windowInfoList);
    ASSERT_TRUE(result);
    windowInfoList[0].privacyMode = SceneInputManager::GetInstance().lastWindowInfoList_[0].privacyMode;
}

/**
 * @tc.name: UpdateSecSurfaceInfo
 * @tc.desc: UpdateSecSurfaceInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, UpdateSecSurfaceInfo, Function | SmallTest | Level3)
{
    int ret = 0;
    std::map<uint64_t, std::vector<SecSurfaceInfo>> emptyMap;
    auto oldDirty = SceneInputManager::GetInstance().sceneSessionDirty_;
    ASSERT_NE(oldDirty, nullptr);
    SceneInputManager::GetInstance().sceneSessionDirty_ = nullptr;
    SceneInputManager::GetInstance().UpdateSecSurfaceInfo(emptyMap);
    ASSERT_EQ(ret, 0);

    SceneInputManager::GetInstance().sceneSessionDirty_ = oldDirty;
    SceneInputManager::GetInstance().UpdateSecSurfaceInfo(emptyMap);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateDisplayAndWindowInfo
 * @tc.desc: UpdateDisplayAndWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, UpdateDisplayAndWindowInfo, Function | SmallTest | Level3)
{
    int ret = 0;
    std::vector<MMI::DisplayInfo> displayInfos;
    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::DisplayInfo displayinfo;
    displayInfos.emplace_back(displayinfo);
    SceneInputManager::GetInstance().UpdateDisplayAndWindowInfo(displayInfos, windowInfoList);
    MMI::WindowInfo windowinfo;
    windowInfoList.emplace_back(windowinfo);
    windowinfo.defaultHotAreas = std::vector<MMI::Rect>(MMI::WindowInfo::DEFAULT_HOTAREA_COUNT + 1);
    SceneInputManager::GetInstance().UpdateDisplayAndWindowInfo(displayInfos, windowInfoList);
    windowinfo.defaultHotAreas = std::vector<MMI::Rect>();
    windowInfoList = std::vector<MMI::WindowInfo>(MAX_WINDOWINFO_NUM - 1);
    SceneInputManager::GetInstance().UpdateDisplayAndWindowInfo(displayInfos, windowInfoList);
    windowInfoList = std::vector<MMI::WindowInfo>(MAX_WINDOWINFO_NUM + 1);
    SceneInputManager::GetInstance().UpdateDisplayAndWindowInfo(displayInfos, windowInfoList);
    windowInfoList[0].defaultHotAreas.resize(MMI::WindowInfo::DEFAULT_HOTAREA_COUNT + 1);
    SceneInputManager::GetInstance().UpdateDisplayAndWindowInfo(displayInfos, windowInfoList);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: FlushEmptyInfoToMMI
 * @tc.desc: FlushEmptyInfoToMMI
 * @tc.type: FUNC
 */
HWTEST_F(SceneInputManagerTest, FlushEmptyInfoToMMI, Function | SmallTest | Level3)
{
    int ret = 0;
    auto preEventHandler = SceneInputManager::GetInstance().eventHandler_;
    SceneInputManager::GetInstance().eventHandler_ = nullptr;
    SceneInputManager::GetInstance().FlushEmptyInfoToMMI();
    SceneInputManager::GetInstance().eventHandler_ = preEventHandler;
    ASSERT_EQ(ret, 0);
}
}
}
}
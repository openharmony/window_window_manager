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

#include "multi_screen_manager.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "display_manager_agent_default.h"
#include "zidl/screen_session_manager_client_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class TestClient : public IScreenSessionManagerClient {
public:
    void SwitchUserCallback(std::vector<int32_t> oldScbPids, int32_t currentScbPid) override {};
    void OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
        ScreenId rsId, const std::string& name, bool isExtend) override {};
    void OnPropertyChanged(ScreenId screenId,
        const ScreenProperty& property, ScreenPropertyChangeReason reason) override {};
    void OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) override {};
    void OnSensorRotationChanged(ScreenId screenId, float sensorRotation) override {};
    void OnHoverStatusChanged(ScreenId screenId, int32_t hoverStatus) override {};
    void OnScreenOrientationChanged(ScreenId screenId, float screenOrientation) override {};
    void OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked) override {};
    void OnScreenExtendChanged(ScreenId mainScreenId, ScreenId extendScreenId) override {};

    void OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override {};
    void OnScreenshot(DisplayId displayId) override {};
    void OnImmersiveStateChanged(ScreenId screenId, bool& immersive) override {};
    void SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId) override {};
    void OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
        std::vector<uint64_t>& surfaceNodeIds, bool isBlackList = false) override {};
    void OnUpdateFoldDisplayMode(FoldDisplayMode displayMode) override {};
    void SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio) override {};
    void OnFoldStatusChangedReportUE(const std::vector<std::string>& screenFoldInfo) override {};
    void ScreenCaptureNotify(ScreenId mainScreenId, int32_t uid, const std::string& clientName) override {};
    sptr<IRemoteObject> AsObject() override {return testPtr;};
    sptr<IRemoteObject> testPtr;
};
class MultiScreenManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<TestClient> testClient_;
};

void MultiScreenManagerTest::SetUpTestCase()
{
}

void MultiScreenManagerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void MultiScreenManagerTest::SetUp()
{
}

void MultiScreenManagerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: FilterPhysicalAndVirtualScreen
 * @tc.desc: FilterPhysicalAndVirtualScreen func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, FilterPhysicalAndVirtualScreen, Function | SmallTest | Level1)
{
    std::vector<ScreenId> allScreenIds = {2000, 2001, 0};
    std::vector<ScreenId> physicalScreenIds;
    std::vector<ScreenId> virtualScreenIds;
    MultiScreenManager::GetInstance().FilterPhysicalAndVirtualScreen(allScreenIds,
        physicalScreenIds, virtualScreenIds);
    EXPECT_EQ(virtualScreenIds.size(), 0);
}

/**
 * @tc.name: VirtualScreenMirrorSwitch
 * @tc.desc: VirtualScreenMirrorSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenMirrorSwitch01, Function | SmallTest | Level1)
{
    ScreenId mainScreenId = 1;
    std::vector<ScreenId> ScreenIds = {2, 3};
    ScreenId screenGroupId;
    MultiScreenManager::GetInstance().VirtualScreenMirrorSwitch(mainScreenId, ScreenIds, screenGroupId);
    EXPECT_EQ(screenGroupId, 0);
}

/**
 * @tc.name: VirtualScreenMirrorSwitch
 * @tc.desc: mainScreen is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenMirrorSwitch02, Function | SmallTest | Level1)
{
    ScreenId mainScreenId = 1002;
    std::vector<ScreenId> ScreenIds = {2, 3};
    ScreenId screenGroupId;
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[mainScreenId] = session;
    auto ret = MultiScreenManager::GetInstance().VirtualScreenMirrorSwitch(mainScreenId, ScreenIds, screenGroupId);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: VirtualScreenMirrorSwitch
 * @tc.desc: ret != DMError::DM_OK
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenMirrorSwitch03, Function | SmallTest | Level1)
{
    ScreenId mainScreenId = 1002;
    std::vector<ScreenId> ScreenIds = {2, 3};
    ScreenId screenGroupId;
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[mainScreenId] = session;
    ScreenId rsId = 1002;
    std::string name = "ok";
    ScreenCombination combination =  ScreenCombination::SCREEN_ALONE;
    sptr<ScreenSessionGroup> sessiongroup = new ScreenSessionGroup(mainScreenId, rsId, name, combination);
    ScreenSessionManager::GetInstance().smsScreenGroupMap_[mainScreenId] = sessiongroup;
    auto ret = MultiScreenManager::GetInstance().VirtualScreenMirrorSwitch(mainScreenId, ScreenIds, screenGroupId);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: VirtualScreenMirrorSwitch04
 * @tc.desc: VirtualScreenMirrorSwitch func04
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenMirrorSwitch04, Function | SmallTest | Level1)
{
    ScreenId mainScreenId = 1003;
    std::vector<ScreenId> ScreenIds = {2, 3};
    ScreenId screenGroupId;
    MultiScreenManager::GetInstance().VirtualScreenMirrorSwitch(mainScreenId, ScreenIds, screenGroupId);
    EXPECT_EQ(screenGroupId, 0);
}

/**
 * @tc.name: VirtualScreenMirrorSwitch05
 * @tc.desc: VirtualScreenMirrorSwitch func05
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenMirrorSwitch05, Function | SmallTest | Level1)
{
    ScreenId mainScreenId = 101;
    std::vector<ScreenId> ScreenIds = {1, 2};
    ScreenId screenGroupId;
    MultiScreenManager::GetInstance().VirtualScreenMirrorSwitch(mainScreenId, ScreenIds, screenGroupId);
    EXPECT_EQ(screenGroupId, 0);
}

/**
 * @tc.name: VirtualScreenMirrorSwitch06
 * @tc.desc: VirtualScreenMirrorSwitch func06
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenMirrorSwitch06, Function | SmallTest | Level1)
{
    ScreenId mainScreenId = 102;
    std::vector<ScreenId> ScreenIds = {1, 3};
    ScreenId screenGroupId;
    MultiScreenManager::GetInstance().VirtualScreenMirrorSwitch(mainScreenId, ScreenIds, screenGroupId);
    EXPECT_EQ(screenGroupId, 0);
}

/**
 * @tc.name: PhysicalScreenMirrorSwitch
 * @tc.desc: PhysicalScreenMirrorSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenMirrorSwitch01, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {2, 3};
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenMirrorSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: PhysicalScreenMirrorSwitch
 * @tc.desc: defaultSession != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenMirrorSwitch02, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1002, 1003};
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
        ScreenSessionManager::GetInstance().defaultScreenId_] = session;
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenMirrorSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: PhysicalScreenMirrorSwitch
 * @tc.desc: defaultSession != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenMirrorSwitch03, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1002, 1003};
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
        ScreenSessionManager::GetInstance().defaultScreenId_] = session;
    sptr<ScreenSession> session1 = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[1002] = session1;
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenMirrorSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: PhysicalScreenMirrorSwitch04
 * @tc.desc: defaultSession != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenMirrorSwitch04, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {101, 102};
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
        ScreenSessionManager::GetInstance().defaultScreenId_] = session;
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenMirrorSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}


/**
 * @tc.name: PhysicalScreenMirrorSwitch05
 * @tc.desc: defaultSession != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenMirrorSwitch05, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {12, 13};
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
        ScreenSessionManager::GetInstance().defaultScreenId_] = session;
    sptr<ScreenSession> session1 = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[12] = session1;
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenMirrorSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: PhysicalScreenUniqueSwitch
 * @tc.desc: PhysicalScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenUniqueSwitch, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {2, 3};
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenUniqueSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: PhysicalScreenUniqueSwitch01
 * @tc.desc: PhysicalScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenUniqueSwitch01, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1002, 1003};
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenUniqueSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: PhysicalScreenUniqueSwitch02
 * @tc.desc: screenSession != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenUniqueSwitch02, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1002, 1003};
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
       1002] = session;
    sptr<ScreenSession> session1 = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
       1003] = session1;
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenUniqueSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: PhysicalScreenUniqueSwitch03
 * @tc.desc: screenSession != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenUniqueSwitch03, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1002, 1003};
    sptr<ScreenSession> session = nullptr;
    ScreenSessionManager::GetInstance().screenSessionMap_[
       1002] = session;
    sptr<ScreenSession> session1 = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
       1003] = session1;
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenUniqueSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: PhysicalScreenUniqueSwitch04
 * @tc.desc: screenSession != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, PhysicalScreenUniqueSwitch04, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {2, 3};
    sptr<ScreenSession> session = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
       1002] = session;
    sptr<ScreenSession> session1 = new ScreenSession();
    ScreenSessionManager::GetInstance().screenSessionMap_[
       1003] = session1;
    DMError ret = MultiScreenManager::GetInstance().PhysicalScreenUniqueSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch01
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch01, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =new ScreenSession();
    std::vector<ScreenId> screenIds = {2, 3};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch02
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch02, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {2, 3};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch03, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {1012, 1002};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch04, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {12, 1001};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch05, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {12, 1002};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch06, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {12, 1003};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch07, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {1003, 1003};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch08, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {1003, 1002};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch09, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {1003, 1001};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch010, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession =nullptr;
    std::vector<ScreenId> screenIds = {1004, 1001};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch11
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch11, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession = new ScreenSession();
    std::vector<ScreenId> screenIds = {1003, 11};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: VirtualScreenUniqueSwitch12
 * @tc.desc: VirtualScreenUniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, VirtualScreenUniqueSwitch12, Function | SmallTest | Level1)
{
    sptr<ScreenSession> screenSession = new ScreenSession();
    std::vector<ScreenId> screenIds = {1004, 1001};
    DMError ret = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(screenSession, screenIds);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: UniqueSwitch
 * @tc.desc: UniqueSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, UniqueSwitch01, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {};
    DMError ret = MultiScreenManager::GetInstance().UniqueSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: UniqueSwitch
 * @tc.desc: screenIds is not empty
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, UniqueSwitch02, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1001, 1002};
    DMError ret = MultiScreenManager::GetInstance().UniqueSwitch(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: MirrorSwitch
 * @tc.desc: MirrorSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, MirrorSwitch, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {};
    ScreenId screenGroupId = 0;
    DMError ret = MultiScreenManager::GetInstance().MirrorSwitch(1, screenIds, screenGroupId);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: MirrorSwitch01
 * @tc.desc: MirrorSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, MirrorSwitch01, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1001, 1002};
    ScreenId screenGroupId = 0;
    DMError ret = MultiScreenManager::GetInstance().MirrorSwitch(1, screenIds, screenGroupId);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: MirrorSwitch02
 * @tc.desc: MirrorSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, MirrorSwitch02, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {2, 3};
    ScreenId screenGroupId = 0;
    DMError ret = MultiScreenManager::GetInstance().MirrorSwitch(1, screenIds, screenGroupId);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: MirrorSwitch03
 * @tc.desc: MirrorSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, MirrorSwitch03, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1003, 1002};
    ScreenId screenGroupId = 0;
    DMError ret = MultiScreenManager::GetInstance().MirrorSwitch(1, screenIds, screenGroupId);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: MirrorSwitch04
 * @tc.desc: MirrorSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, MirrorSwitch04, Function | SmallTest | Level1)
{
    std::vector<ScreenId> screenIds = {1003, 2};
    ScreenId screenGroupId = 0;
    DMError ret = MultiScreenManager::GetInstance().MirrorSwitch(1, screenIds, screenGroupId);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: MultiScreenModeChange
 * @tc.desc: firstSession == nullptr,secondarySession == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, MultiScreenModeChange01, Function | SmallTest | Level1)
{
    sptr<ScreenSession> firstSession = nullptr;
    sptr<ScreenSession> secondarySession = nullptr;
    MultiScreenManager::GetInstance().MultiScreenModeChange(firstSession, secondarySession, "mirror");

    secondarySession = new ScreenSession();
    MultiScreenManager::GetInstance().MultiScreenModeChange(firstSession, secondarySession, "mirror");

    firstSession = new ScreenSession();
    MultiScreenManager::GetInstance().MultiScreenModeChange(firstSession, secondarySession, "mirror");
    EXPECT_NE(secondarySession, nullptr);
}

/**
 * @tc.name: MultiScreenModeChange
 * @tc.desc: firstSession == nullptr,secondarySession == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, MultiScreenModeChange02, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);

    testClient_ = new TestClient();
    ScreenSessionManager::GetInstance().SetClient(testClient_);
    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);

    firstSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    MultiScreenManager::GetInstance().MultiScreenModeChange(firstSession, secondarySession, "extend");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_EXTEND);

    firstSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    MultiScreenManager::GetInstance().MultiScreenModeChange(firstSession, secondarySession, "mirror");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);

    firstSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    MultiScreenManager::GetInstance().MultiScreenModeChange(firstSession, secondarySession, "extend");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_EXTEND);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMainChange
 * @tc.desc: scbClient null
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMainChange01, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    ScreenCombination secondaryCombination = secondarySession->GetScreenCombination();

    ScreenSessionManager::GetInstance().clientProxy_ = nullptr;
    MultiScreenManager::GetInstance().DoFirstMainChange(firstSession, secondarySession, "unknown");
    ASSERT_EQ(secondarySession->GetScreenCombination(), secondaryCombination);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMainChange
 * @tc.desc: param error
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMainChange02, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    ScreenCombination secondaryCombination = secondarySession->GetScreenCombination();

    testClient_ = new TestClient();
    ScreenSessionManager::GetInstance().SetClient(testClient_);
    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMainChange(firstSession, secondarySession, "unknown");
    ASSERT_EQ(secondarySession->GetScreenCombination(), secondaryCombination);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMainChange
 * @tc.desc: DoFirstMainChangeExtend
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMainChange03, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);

    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMainChange(firstSession, secondarySession, "extend");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_EXTEND);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMainChange
 * @tc.desc: main change extend, no need to change
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMainChange04, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_ALONE);

    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMainChange(firstSession, secondarySession, "extend");
    ASSERT_NE(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_EXTEND);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMainChange
 * @tc.desc: DoFirstMainChangeMirror
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMainChange05, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);

    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMainChange(firstSession, secondarySession, "mirror");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMainChange
 * @tc.desc: main change mirror, no need to change
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMainChange06, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_ALONE);

    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMainChange(firstSession, secondarySession, "mirror");
    ASSERT_NE(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMirrorChange
 * @tc.desc: scbClient null
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMirrorChange01, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    ScreenCombination secondaryCombination = secondarySession->GetScreenCombination();

    ScreenSessionManager::GetInstance().clientProxy_ = nullptr;
    MultiScreenManager::GetInstance().DoFirstMirrorChange(firstSession, secondarySession, "unknown");
    ASSERT_EQ(secondarySession->GetScreenCombination(), secondaryCombination);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMirrorChange
 * @tc.desc: param error
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMirrorChange02, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    ScreenCombination secondaryCombination = secondarySession->GetScreenCombination();

    testClient_ = new TestClient();
    ScreenSessionManager::GetInstance().SetClient(testClient_);
    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMirrorChange(firstSession, secondarySession, "unknown");
    ASSERT_EQ(secondarySession->GetScreenCombination(), secondaryCombination);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}


/**
 * @tc.name: DoFirstMirrorChange
 * @tc.desc: DoFirstMirrorChangeExtend
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMirrorChange03, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);

    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMirrorChange(firstSession, secondarySession, "extend");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_EXTEND);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstMirrorChange
 * @tc.desc: DoFirstMirrorChangeMirror
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstMirrorChange04, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);

    ASSERT_NE(ScreenSessionManager::GetInstance().GetClientProxy(), nullptr);
    MultiScreenManager::GetInstance().DoFirstMirrorChange(firstSession, secondarySession, "mirror");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}


/**
 * @tc.name: DoFirstExtendChange
 * @tc.desc: param error
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstExtendChange01, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    ScreenCombination secondaryCombination = secondarySession->GetScreenCombination();

    MultiScreenManager::GetInstance().DoFirstExtendChange(firstSession, secondarySession, "unknown");
    ASSERT_EQ(secondarySession->GetScreenCombination(), secondaryCombination);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstExtendChange
 * @tc.desc: extend change extend
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstExtendChange02, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);

    MultiScreenManager::GetInstance().DoFirstExtendChange(firstSession, secondarySession, "extend");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_EXTEND);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: DoFirstExtendChange
 * @tc.desc: extend change mirror
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenManagerTest, DoFirstExtendChange03, Function | SmallTest | Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto firstSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);

    MultiScreenManager::GetInstance().DoFirstExtendChange(firstSession, secondarySession, "mirror");
    ASSERT_EQ(secondarySession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);

    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}
}
} // namespace Rosen
} // namespace OHOS
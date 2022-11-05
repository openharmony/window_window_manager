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

#include <gtest/gtest.h>

#include "abstract_screen_controller.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class AbstractScreenControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::recursive_mutex mutex;
    sptr<AbstractScreenController> absController_ = new AbstractScreenController(mutex);
    std::string name = "AbstractScreenController";
    std::vector<sptr<AbstractScreen>> screen_vec;
    std::vector<sptr<AbstractScreenGroup>> screenGroup_vec;
};

void AbstractScreenControllerTest::SetUpTestCase()
{
}

void AbstractScreenControllerTest::TearDownTestCase()
{
}

void AbstractScreenControllerTest::SetUp()
{
    for(int i = 0; i < 5; ++i) {
        ScreenId dmsId = i;
        ScreenId rsId = i;
        sptr<AbstractScreen> absScreen = new AbstractScreen(absController_, name, dmsId, rsId);
        sptr<AbstractScreenGroup> absScreenGroup = new AbstractScreenGroup(absController_, dmsId, rsId, name, ScreenCombination::SCREEN_ALONE);
        screen_vec.emplace_back(absScreen);
        screenGroup_vec.emplace_back(absScreenGroup);
        // init dmsScreenMap_
        absController_->dmsScreenMap_.insert(std::make_pair(dmsId, absScreen));
        // init screenIdManager_
        absController_->screenIdManager_.rs2DmsScreenIdMap_.insert(std::make_pair(rsId, dmsId));
        absController_->screenIdManager_.dms2RsScreenIdMap_.insert(std::make_pair(dmsId, rsId));
        // init dmsScreenGroupMap_
        absController_->dmsScreenGroupMap_.insert(std::make_pair(rsId, absScreenGroup));
    }
    screen_vec[4]->type_ = ScreenType::UNDEFINED;
    absController_->dmsScreenMap_.insert(std::make_pair(static_cast<ScreenId>(5), nullptr));
    absController_->screenIdManager_.dms2RsScreenIdMap_.insert(std::make_pair(5, SCREEN_ID_INVALID));
    absController_->dmsScreenGroupMap_.insert(std::make_pair(5, nullptr));
}

void AbstractScreenControllerTest::TearDown()
{
    screen_vec.clear();
}

namespace {
/**
 * @tc.name: GetAllValidScreenIds
 * @tc.desc: GetAllValidScreenIds test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, GetAllValidScreenIds, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screenIds {0, 1, 1, 2, 2, 3, 4};
    std::vector<ScreenId> valid {0, 1, 2, 3};
    ASSERT_EQ(valid, absController_->GetAllValidScreenIds(screenIds));
}
/**
 * @tc.name: GetRSDisplayNodeByScreenId
 * @tc.desc: GetRSDisplayNodeByScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, GetRSDisplayNodeByScreenId, Function | SmallTest | Level3)
{
    ScreenId id = 6;
    std::shared_ptr<RSDisplayNode> node = nullptr;
    ASSERT_EQ(node, absController_->GetRSDisplayNodeByScreenId(id));
}
/**
 * @tc.name: UpdateRSTree
 * @tc.desc: UpdateRSTree test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, UpdateRSTree01, Function | SmallTest | Level3)
{
    ScreenId id = 8;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    absController_->UpdateRSTree(id, id, node, true, true);
    ASSERT_EQ(nullptr, absController_->GetAbstractScreen(id));
}
/**
 * @tc.name: UpdateRSTree
 * @tc.desc: UpdateRSTree test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, UpdateRSTree02, Function | SmallTest | Level3)
{
    ScreenId id = 1;
    ScreenId p_id = 8;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    absController_->UpdateRSTree(id, p_id, node, true, true);
    ASSERT_NE(nullptr, absController_->GetAbstractScreen(id));
    ASSERT_EQ(nullptr, absController_->GetAbstractScreen(p_id));
}
/**
 * @tc.name: UpdateRSTree
 * @tc.desc: UpdateRSTree test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, UpdateRSTree03, Function | SmallTest | Level3)
{
    ScreenId id = 1;
    ScreenId p_id = 2;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    ASSERT_NE(nullptr, absController_->GetAbstractScreen(id));
    sptr<AbstractScreen> parentScreen = absController_->GetAbstractScreen(p_id);
    ASSERT_NE(nullptr, absController_->GetAbstractScreen(p_id));
    parentScreen->rsDisplayNode_ = nullptr;
    absController_->UpdateRSTree(id, p_id, node, true, true);
}
/**
 * @tc.name: RegisterAbstractScreenCallback
 * @tc.desc: RegisterAbstractScreenCallback test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, RegisterAbstractScreenCallback, Function | SmallTest | Level3)
{
    sptr<AbstractScreenController::AbstractScreenCallback> cb = nullptr;
    absController_->RegisterAbstractScreenCallback(cb);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: OnRsScreenConnectionChange
 * @tc.desc: OnRsScreenConnectionChange test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, OnRsScreenConnectionChange01, Function | SmallTest | Level3)
{
    ScreenEvent event = ScreenEvent::DISCONNECTED;
    ScreenId rsScreenId = 1;
    absController_->OnRsScreenConnectionChange(rsScreenId, event);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: OnRsScreenConnectionChange
 * @tc.desc: OnRsScreenConnectionChange test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, OnRsScreenConnectionChange02, Function | SmallTest | Level3)
{
    ScreenEvent event = ScreenEvent::UNKNOWN;
    ScreenId rsScreenId = 1;
    absController_->OnRsScreenConnectionChange(rsScreenId, event);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: ProcessScreenConnected
 * @tc.desc: ProcessScreenConnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenConnected01, Function | SmallTest | Level3)
{
    ScreenId id = 0;
    absController_->ProcessScreenConnected(id);
    ASSERT_EQ(true, absController_->screenIdManager_.HasRsScreenId(id));
}
/**
 * @tc.name: ProcessScreenDisconnected
 * @tc.desc: ProcessScreenDisconnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenDisconnected01, Function | SmallTest | Level3)
{
    ScreenId rs_id = 6;
    ScreenId dms_id;
    absController_->ProcessScreenDisconnected(rs_id);
    ASSERT_EQ(false, absController_->screenIdManager_.ConvertToDmsScreenId(rs_id, dms_id));
}
/**
 * @tc.name: ProcessScreenDisconnected
 * @tc.desc: ProcessScreenDisconnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenDisconnected02, Function | SmallTest | Level3)
{
    ScreenId rs_id = 2;
    ScreenId dms_id;
    absController_->ProcessScreenDisconnected(rs_id);
    absController_->screenIdManager_.ConvertToDmsScreenId(rs_id, dms_id);
    absController_->dmsScreenMap_.erase(dms_id);
    ASSERT_EQ(false, absController_->screenIdManager_.ConvertToDmsScreenId(rs_id, dms_id));
    ASSERT_EQ(absController_->dmsScreenMap_.end(), absController_->dmsScreenMap_.find(dms_id));
}
/**
 * @tc.name: ProcessScreenDisconnected
 * @tc.desc: ProcessScreenDisconnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenDisconnected03, Function | SmallTest | Level3)
{
    ScreenId rs_id = 2;
    ScreenId dms_id;
    absController_->abstractScreenCallback_ = nullptr;
    absController_->ProcessScreenDisconnected(rs_id);
    ASSERT_EQ(false, absController_->screenIdManager_.ConvertToDmsScreenId(rs_id, dms_id));
    ASSERT_NE(absController_->dmsScreenMap_.end(), absController_->dmsScreenMap_.find(dms_id));
}
/**
 * @tc.name: AddToGroupLocked
 * @tc.desc: AddToGroupLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, AddToGroupLocked, Function | SmallTest | Level3)
{
    ASSERT_EQ(false, absController_->dmsScreenGroupMap_.empty());
    ASSERT_EQ(nullptr, absController_->AddToGroupLocked(screen_vec[0]));
}
/**
 * @tc.name: RemoveFromGroupLocked
 * @tc.desc: RemoveFromGroupLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, RemoveFromGroupLocked01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screen_vec[0];
    screen->groupDmsId_ = 0;
    ASSERT_EQ(nullptr, absController_->RemoveFromGroupLocked(screen));
}
/**
 * @tc.name: RemoveChildFromGroup
 * @tc.desc: RemoveChildFromGroup test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, RemoveChildFromGroup01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screen_vec[0];
    ScreenId dmsId = screen->dmsId_;
    Point point;
    auto p = std::make_pair(screen, point);
    sptr<AbstractScreenGroup> screenGroup = absController_->dmsScreenGroupMap_[0];
    screenGroup->abstractScreenMap_.insert(std::make_pair(dmsId, p));
    ASSERT_EQ(true, absController_->RemoveChildFromGroup(screen, screenGroup));
}
/**
 * @tc.name: AddAsSuccedentScreenLocked
 * @tc.desc: AddAsSuccedentScreenLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, AddAsSuccedentScreenLocked01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screen_vec[0];
    absController_->dmsScreenMap_.erase(absController_->GetDefaultAbstractScreenId());
    ASSERT_EQ(nullptr, absController_->AddAsSuccedentScreenLocked(screen));
}
/**
 * @tc.name: AddAsSuccedentScreenLocked
 * @tc.desc: AddAsSuccedentScreenLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, AddAsSuccedentScreenLocked02, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screen_vec[0];
    ASSERT_EQ(nullptr, absController_->AddAsSuccedentScreenLocked(screen));
}
/**
 * @tc.name: CreateVirtualScreen
 * @tc.desc: CreateVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, CreateVirtualScreen01, Function | SmallTest | Level3)
{
    VirtualScreenOption option;
    sptr<IRemoteObject> displayManagerAgent = new IRemoteObjectMocker();
    ASSERT_EQ(0, absController_->CreateVirtualScreen(option, displayManagerAgent));
}
/**
 * @tc.name: InitVirtualScreen
 * @tc.desc: InitVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, InitVirtualScreen01, Function | SmallTest | Level3)
{
    VirtualScreenOption option;
    absController_->dmsScreenMap_.erase(absController_->GetDefaultAbstractScreenId());
    sptr<AbstractScreen> screen = absController_->InitVirtualScreen(0, 0, option);
    ASSERT_EQ(0, screen->activeIdx_);
}
/**
 * @tc.name: InitVirtualScreen
 * @tc.desc: InitVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, InitVirtualScreen02, Function | SmallTest | Level3)
{
    VirtualScreenOption option;
    sptr<AbstractScreen> defaultScreen = absController_->dmsScreenMap_[absController_->GetDefaultAbstractScreenId()];
    sptr<SupportedScreenModes> modes;
    defaultScreen->modes_.emplace_back(modes);
    defaultScreen->activeIdx_ = 0;
    ASSERT_EQ(nullptr, defaultScreen->GetActiveScreenMode());
    sptr<AbstractScreen> screen = absController_->InitVirtualScreen(0, 0, option);
    ASSERT_EQ(ScreenType::VIRTUAL, screen->type_);
}
/**
 * @tc.name: DestroyVirtualScreen
 * @tc.desc: DestroyVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, DestroyVirtualScreen01, Function | SmallTest | Level3)
{
    ScreenId id = 5;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, absController_->DestroyVirtualScreen(id));
}
/**
 * @tc.name: DestroyVirtualScreen
 * @tc.desc: DestroyVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, DestroyVirtualScreen02, Function | SmallTest | Level3)
{
    ScreenId id = 1;
    ASSERT_EQ(DMError::DM_OK, absController_->DestroyVirtualScreen(id));
}
/**
 * @tc.name: SetVirtualScreenSurface
 * @tc.desc: SetVirtualScreenSurface test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetVirtualScreenSurface01, Function | SmallTest | Level3)
{
    ScreenId id = 6;
    sptr<Surface> surface = nullptr;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absController_->SetVirtualScreenSurface(id, surface));
}
/**
 * @tc.name: SetBuildInDefaultOrientation
 * @tc.desc: SetBuildInDefaultOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetBuildInDefaultOrientation, Function | SmallTest | Level3)
{
    Orientation orientation = Orientation::BEGIN;
    absController_->SetBuildInDefaultOrientation(orientation);
    ASSERT_EQ(orientation, absController_->buildInDefaultOrientation_);
}
/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetOrientation01, Function | SmallTest | Level3)
{
    absController_->dmsScreenMap_[1]->isScreenGroup_ = true;
    Orientation orientation = Orientation::BEGIN;
    ASSERT_EQ(false, absController_->SetOrientation(1, orientation, true));
}
/**
 * @tc.name: SetScreenRotateAnimation
 * @tc.desc: SetScreenRotateAnimation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetScreenRotateAnimation01, Function | SmallTest | Level3)
{
    RSDisplayNodeConfig config;
    absController_->dmsScreenMap_[1]->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    sptr<AbstractScreen> screen = screen_vec[0];
    screen->rotation_ = Rotation::ROTATION_270;
    absController_->SetScreenRotateAnimation(screen, 1, Rotation::ROTATION_0);
    ASSERT_EQ(Rotation::ROTATION_270, screen->rotation_);
}
/**
 * @tc.name: SetRotation
 * @tc.desc: SetRotation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetRotation01, Function | SmallTest | Level3)
{
    absController_->dmsScreenMap_[1]->rotation_ = Rotation::ROTATION_180;
    absController_->screenIdManager_.dms2RsScreenIdMap_.erase(1);
    ASSERT_EQ(false, absController_->SetRotation(1, Rotation::ROTATION_0, true));
}
/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: SetScreenActiveMode test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetScreenActiveMode01, Function | SmallTest | Level3)
{
    ASSERT_EQ(false, absController_->SetScreenActiveMode(5, 0));
}
/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: SetScreenActiveMode test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetScreenActiveMode02, Function | SmallTest | Level3)
{
    absController_->screenIdManager_.dms2RsScreenIdMap_.erase(1);
    ASSERT_EQ(false, absController_->SetScreenActiveMode(1, 0));
}
/**
 * @tc.name: ProcessScreenModeChanged
 * @tc.desc: ProcessScreenModeChanged test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenModeChanged01, Function | SmallTest | Level3)
{
    absController_->ProcessScreenModeChanged(7);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: ProcessScreenModeChanged
 * @tc.desc: ProcessScreenModeChanged test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenModeChanged02, Function | SmallTest | Level3)
{
    absController_->ProcessScreenModeChanged(5);
    ASSERT_EQ(nullptr, absController_->dmsScreenMap_[5]);
}
/**
 * @tc.name: ProcessScreenModeChanged
 * @tc.desc: ProcessScreenModeChanged test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenModeChanged03, Function | SmallTest | Level3)
{
    absController_->ProcessScreenModeChanged(2);
    ASSERT_NE(nullptr, absController_->dmsScreenMap_[2]);
}
/**
 * @tc.name: MakeMirror
 * @tc.desc: MakeMirror test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, MakeMirror01, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screens;
    ASSERT_EQ(false, absController_->MakeMirror(5, screens));
}
/**
 * @tc.name: MakeMirror
 * @tc.desc: MakeMirror test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, MakeMirror02, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screens;
    absController_->dmsScreenMap_[2]->type_ = ScreenType::UNDEFINED;
    ASSERT_EQ(false, absController_->MakeMirror(2, screens));
}
/**
 * @tc.name: MakeMirror
 * @tc.desc: MakeMirror test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, MakeMirror03, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screens;
    absController_->dmsScreenMap_[2]->type_ = ScreenType::REAL;
    absController_->dmsScreenMap_[2]->groupDmsId_ = 5;
    absController_->abstractScreenCallback_ = nullptr;
    ASSERT_EQ(false, absController_->MakeMirror(2, screens));
}
/**
 * @tc.name: MakeMirror
 * @tc.desc: MakeMirror test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, MakeMirror04, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screens;
    absController_->dmsScreenMap_[2]->type_ = ScreenType::REAL;
    absController_->dmsScreenMap_[2]->groupDmsId_ = 5;
    absController_->abstractScreenCallback_ = new AbstractScreenController::AbstractScreenCallback;
    ASSERT_EQ(false, absController_->MakeMirror(2, screens));
}
/**
 * @tc.name: ChangeScreenGroup
 * @tc.desc: ChangeScreenGroup test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ChangeScreenGroup01, Function | SmallTest | Level3)
{
    sptr<AbstractScreenGroup> group = screenGroup_vec[0];
    Point point;
    auto abs2point_pair = std::make_pair(screen_vec[0],point);
    group->abstractScreenMap_.insert(std::make_pair(0, abs2point_pair));
    group->abstractScreenMap_.insert(std::make_pair(1, abs2point_pair));
    std::vector<Point> startPoints;
    std::vector<ScreenId> screens;
    for(ScreenId i = 0; i < 7; ++i) {
        screens.emplace_back(i);
        startPoints.emplace_back(point);
        if (i < absController_->dmsScreenMap_.size() && absController_->dmsScreenMap_[i] != nullptr) {
            absController_->dmsScreenMap_[i]->groupDmsId_ = 1;
        }
    }
    absController_->ChangeScreenGroup(group, screens, startPoints, true, ScreenCombination::SCREEN_ALONE);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
}
} // namespace Rosen
} // namespace OHOS

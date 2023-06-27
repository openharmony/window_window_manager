/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include <fcntl.h>
#include <gtest/gtest.h>
#include "common_test_utils.h"
#include "iremote_object_mocker.h"
#include "mock_IWindow.h"
#include "mock_RSIWindowAnimationController.h"
#include "window_manager_service.h"

#include <thread>

#include <ability_manager_client.h>
#include <cinttypes>
#include <chrono>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <parameters.h>
#include <rs_iwindow_animation_controller.h>
#include <system_ability_definition.h>
#include <sstream>
#include "xcollie/watchdog.h"

#include "color_parser.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "drag_controller.h"
#include "minimize_app.h"
#include "permission.h"
#include "remote_animation.h"
#include "singleton_container.h"
#include "ui/rs_ui_director.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void SetAceessTokenPermission(const std::string processName);
    sptr<WindowManagerService> wms = new WindowManagerService();
};

void WindowManagerServiceTest::SetUpTestCase()
{
}

void WindowManagerServiceTest::TearDownTestCase()
{
}

void WindowManagerServiceTest::SetUp()
{
    CommonTestUtils::SetAceessTokenPermission("WindowManagerServiceTest");
}

void WindowManagerServiceTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnAddSystemAbility
 * @tc.desc: OnAddSystemAbility test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, OnAddSystemAbility01, Function | SmallTest | Level2)
{
    std::string str = "OnAddSystemAbility";
    wms->OnAddSystemAbility(0, str);
    ASSERT_EQ(nullptr, wms->windowCommonEvent_->subscriber_);
}
/**
 * @tc.name: WindowVisibilityChangeCallback
 * @tc.desc: WindowVisibilityChangeCallback test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, WindowVisibilityChangeCallback01, Function | SmallTest | Level2)
{
    std::shared_ptr<RSOcclusionData> occlusionData = nullptr;
    wms->WindowVisibilityChangeCallback(occlusionData);
    ASSERT_EQ(nullptr, occlusionData);
}
/**
 * @tc.name: InitWithAbilityManagerServiceAdded
 * @tc.desc: Init with ability manager service added.
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, InitWithAbilityManagerServiceAdded01, Function | SmallTest | Level2)
{
    wms->wmsHandler_ = new WindowManagerServiceHandler;
    wms->InitWithAbilityManagerServiceAdded();
    ASSERT_NE(nullptr, wms->wmsHandler_);
}
/**
 * @tc.name: Dump
 * @tc.desc: Dump info
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, Dump01, Function | SmallTest | Level2)
{
    wms->windowDumper_ = nullptr;
    std::vector<std::u16string> args;
    const std::string dumpFile = "data/window_dump_test.txt";
    int fd = open(dumpFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        return;
    }
    ASSERT_EQ(static_cast<int>(WMError::WM_ERROR_INVALID_PARAM), wms->Dump(-1, args));
    ASSERT_EQ(static_cast<int>(WMError::WM_OK), wms->Dump(fd, args));
    close(fd);
    unlink(dumpFile.c_str());
}
/**
 * @tc.name: NotifyWindowTransition
 * @tc.desc: NotifyWindowTransition test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, NotifyWindowTransition01, Function | SmallTest | Level2)
{
    sptr<WindowTransitionInfo> fromInfo = nullptr;
    sptr<WindowTransitionInfo> toInfo = nullptr;
    ASSERT_EQ(WMError::WM_OK, wms->NotifyWindowTransition(fromInfo, toInfo, false));
    ASSERT_EQ(WMError::WM_ERROR_NO_REMOTE_ANIMATION, wms->NotifyWindowTransition(fromInfo, toInfo, true));
}
/**
 * @tc.name: StartingWindow
 * @tc.desc: StartingWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, StartingWindow01, Function | SmallTest | Level2)
{
    wms->startingOpen_ = false;
    wms->StartingWindow(nullptr, nullptr, false, 0);
    ASSERT_EQ(false, wms->startingOpen_);
    wms->CancelStartingWindow(nullptr);
    wms->startingOpen_ = true;
    wms->StartingWindow(nullptr, nullptr, false, 0);
    ASSERT_EQ(true, wms->startingOpen_);
    wms->CancelStartingWindow(nullptr);
}

/**
 * @tc.name: MoveMissionsToForeground
 * @tc.desc: MoveMissionsToForont test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, MoveMissionsToForeground01, Function | SmallTest | Level2)
{
    WMError rs = wms->MoveMissionsToForeground({}, -1);
    ASSERT_EQ(WMError::WM_OK, rs);
}


/**
 * @tc.name: MoveMissionsToBackground
 * @tc.desc: MoveMissionsToBackground test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, MoveMissionsToBackground01, Function | SmallTest | Level2)
{
    std::vector<int32_t> moveRs;
    WMError rs = wms->MoveMissionsToBackground({}, moveRs);
    ASSERT_EQ(WMError::WM_OK, rs);
}

/**
 * @tc.name: CreateWindow
 * @tc.desc: CreateWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, CreateWindow01, Function | SmallTest | Level2)
{
    sptr<IWindow> window = nullptr;
    uint32_t id = 2;
    std::shared_ptr<RSSurfaceNode> RS_node = nullptr;
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_WALLPAPER);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->CreateWindow(window, property, RS_node, id, nullptr));
    wms->DestroyWindow(id, true);
}
/**
 * @tc.name: AddWindow
 * @tc.desc: AddWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, AddWindow01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->AddWindow(property));
}
/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: RegisterWindowManagerAgent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, RegisterWindowManagerAgent01, Function | SmallTest | Level2)
{
    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->UnregisterWindowManagerAgent(type, windowManagerAgent));
}
/**
 * @tc.name: SetWindowAnimationController
 * @tc.desc: SetWindowAnimationController test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, SetWindowAnimationController01, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> controller = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->SetWindowAnimationController(controller));
    controller = new RSIWindowAnimationControllerMocker;
    ASSERT_EQ(WMError::WM_OK, wms->SetWindowAnimationController(controller));
}
/**
 * @tc.name: OnWindowEvent
 * @tc.desc: OnWindowEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, OnWindowEvent01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new IRemoteObjectMocker;
    wms->OnWindowEvent(static_cast<Event>(1), remoteObject);
    wms->OnWindowEvent(Event::REMOTE_DIED, remoteObject);
    ASSERT_EQ(INVALID_WINDOW_ID, wms->windowRoot_->GetWindowIdByObject(remoteObject));
}
/**
 * @tc.name: UpdateProperty
 * @tc.desc: UpdateProperty test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, UpdateProperty01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> windowProperty = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->UpdateProperty(windowProperty,
        PropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG, true));
}
/**
 * @tc.name: GetModeChangeHotZones
 * @tc.desc: GetModeChangeHotZones test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, GetModeChangeHotZones01, Function | SmallTest | Level2)
{
    ModeChangeHotZonesConfig config = {false, 0, 0, 0};
    DisplayId displayId = 0;
    ModeChangeHotZones hotZone;
    wms->hotZonesConfig_ = config;
    ASSERT_EQ(WMError::WM_DO_NOTHING, wms->GetModeChangeHotZones(displayId, hotZone));
    config.isModeChangeHotZoneConfigured_ = true;
    wms->hotZonesConfig_ = config;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->GetModeChangeHotZones(displayId, hotZone));
}
/**
 * @tc.name: UpdateAvoidAreaListener
 * @tc.desc: UpdateAvoidAreaListener test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, UpdateAvoidAreaListener01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = new WindowProperty();
    sptr<WindowNode> node = new WindowNode(property);
    wms->windowRoot_->windowNodeMap_.insert(std::make_pair(0, node));
    ASSERT_EQ(WMError::WM_DO_NOTHING, wms->UpdateAvoidAreaListener(0, true));
}
/**
 * @tc.name: BindDialogTarget
 * @tc.desc: BindDialogTarget test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, BindDialogTarget01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> targetToken = new IRemoteObjectMocker();
    uint32_t id = 0;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->BindDialogTarget(id, targetToken));
}
/**
 * @tc.name: DispatchKeyEvent01
 * @tc.desc: Dispatch KeyEvent for app window bellow the component window.
 * @tc.require: issueI6RMUY
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, DispatchKeyEvent01, Function | SmallTest | Level2)
{
    wms = new WindowManagerService();
    sptr<WindowNode> compNode = new WindowNode();
    compNode->property_->type_ = WindowType::WINDOW_TYPE_APP_COMPONENT;
    compNode->SetWindowToken(new IWindowMocker);
    compNode->property_->windowId_ = 1U;
    sptr<WindowNode> appNode = new WindowNode();
    appNode->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    IWindowMocker* token = new IWindowMocker;
    appNode->SetWindowToken(token);
    appNode->property_->windowId_ = 2U;
    auto event = MMI::KeyEvent::Create();
    wms->windowController_->windowRoot_->windowNodeMap_[appNode->GetWindowId()] = appNode;
    wms->windowController_->windowRoot_->windowNodeMap_[compNode->GetWindowId()] = compNode;
    sptr<WindowNodeContainer> container = new WindowNodeContainer(new DisplayInfo, 0);
    wms->windowController_->windowRoot_->windowNodeContainerMap_[compNode->GetDisplayId()] = container;
    wms->windowController_->windowRoot_->displayIdMap_[compNode->GetDisplayId()] = { compNode->GetDisplayId() };
    container->appWindowNode_->children_.push_back(appNode);
    container->appWindowNode_->children_.push_back(compNode);
    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);
    ASSERT_TRUE(windowNodes[0] == compNode);
    ASSERT_TRUE(windowNodes[1] == appNode);
    EXPECT_CALL(*token, ConsumeKeyEvent(_));
    wms->DispatchKeyEvent(compNode->GetWindowId(), event);
    testing::Mock::AllowLeak(token);
}
/**
 * @tc.name: DispatchKeyEvent02
 * @tc.desc: Dispatch KeyEvent for app window bellow the app window.
 * @tc.require: issueI6RMUY
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, DispatchKeyEvent02, Function | SmallTest | Level2)
{
    wms = new WindowManagerService();
    sptr<WindowNode> appNode1 = new WindowNode();
    appNode1->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    appNode1->SetWindowToken(new IWindowMocker);
    appNode1->property_->windowId_ = 1U;
    sptr<WindowNode> appNode2 = new WindowNode();
    appNode2->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    appNode2->SetWindowToken(new IWindowMocker);
    appNode2->property_->windowId_ = 2U;
    auto event = MMI::KeyEvent::Create();
    wms->windowController_->windowRoot_->windowNodeMap_[appNode2->GetWindowId()] = appNode2;
    wms->windowController_->windowRoot_->windowNodeMap_[appNode1->GetWindowId()] = appNode1;
    sptr<WindowNodeContainer> container = new WindowNodeContainer(new DisplayInfo, 0);
    wms->windowController_->windowRoot_->windowNodeContainerMap_[appNode1->GetDisplayId()] = container;
    wms->windowController_->windowRoot_->displayIdMap_[appNode1->GetDisplayId()] = { appNode1->GetDisplayId() };
    container->appWindowNode_->children_.push_back(appNode2);
    container->appWindowNode_->children_.push_back(appNode1);
    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);
    ASSERT_TRUE(windowNodes[0] == appNode1);
    ASSERT_TRUE(windowNodes[1] == appNode2);
    wms->DispatchKeyEvent(appNode1->GetWindowId(), event);
}
/**
 * @tc.name: DispatchKeyEvent03
 * @tc.desc: Dispatch KeyEvent for app window bellow two component window.
 * @tc.require: issueI6RMUY
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, DispatchKeyEvent03, Function | SmallTest | Level2)
{
    wms = new WindowManagerService();
    sptr<WindowNode> compNode1 = new WindowNode();
    compNode1->property_->type_ = WindowType::WINDOW_TYPE_APP_COMPONENT;
    compNode1->SetWindowToken(new IWindowMocker);
    compNode1->property_->windowId_ = 1U;
    sptr<WindowNode> compNode2 = new WindowNode();
    compNode2->property_->type_ = WindowType::WINDOW_TYPE_APP_COMPONENT;
    compNode2->SetWindowToken(new IWindowMocker);
    compNode2->property_->windowId_ = 2U;
    sptr<WindowNode> appNode = new WindowNode();
    appNode->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    IWindowMocker* token = new IWindowMocker;
    appNode->SetWindowToken(token);
    appNode->property_->windowId_ = 3U;
    auto event = MMI::KeyEvent::Create();
    wms->windowController_->windowRoot_->windowNodeMap_[appNode->GetWindowId()] = appNode;
    wms->windowController_->windowRoot_->windowNodeMap_[compNode1->GetWindowId()] = compNode1;
    wms->windowController_->windowRoot_->windowNodeMap_[compNode2->GetWindowId()] = compNode2;
    sptr<WindowNodeContainer> container = new WindowNodeContainer(new DisplayInfo, 0);
    wms->windowController_->windowRoot_->windowNodeContainerMap_[compNode1->GetDisplayId()] = container;
    wms->windowController_->windowRoot_->displayIdMap_[compNode1->GetDisplayId()] = { compNode1->GetDisplayId() };
    container->appWindowNode_->children_.push_back(appNode);
    container->appWindowNode_->children_.push_back(compNode1);
    container->appWindowNode_->children_.push_back(compNode2);
    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);
    ASSERT_TRUE(windowNodes[0] == compNode2);
    ASSERT_TRUE(windowNodes[1] == compNode1);
    ASSERT_TRUE(windowNodes[2] == appNode);
    EXPECT_CALL(*token, ConsumeKeyEvent(_));
    wms->DispatchKeyEvent(compNode2->GetWindowId(), event);
    testing::Mock::AllowLeak(token);
}
/**
 * @tc.name: SetWindowGravity
 * @tc.desc: SetWindowGravity test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, SetWindowGravity01, Function | SmallTest | Level2)
{
    uint32_t id = 0;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->SetWindowGravity(id, WindowGravity::WINDOW_GRAVITY_BOTTOM, 0));
}
/*
 * @tc.name: GetWindowAnimationTargets01
 * @tc.desc: get window animation targets
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, GetWindowAnimationTargets01, Function | SmallTest | Level2)
{
    wms = new WindowManagerService();
    sptr<WindowNode> compNode = new WindowNode();
    compNode->property_->type_ = WindowType::WINDOW_TYPE_APP_COMPONENT;
    compNode->SetWindowToken(new IWindowMocker);
    compNode->property_->windowId_ = 1U;
    compNode->abilityInfo_.missionId_ = 1;
    sptr<WindowNode> appNode = new WindowNode();
    appNode->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    IWindowMocker* token = new IWindowMocker;
    appNode->SetWindowToken(token);
    appNode->property_->windowId_ = 2U;
    appNode->abilityInfo_.missionId_ = 2;
    wms->windowController_->windowRoot_->windowNodeMap_[appNode->GetWindowId()] = appNode;
    wms->windowController_->windowRoot_->windowNodeMap_[compNode->GetWindowId()] = compNode;
    sptr<WindowNodeContainer> container = new WindowNodeContainer(new DisplayInfo, 0);
    wms->windowController_->windowRoot_->windowNodeContainerMap_[compNode->GetDisplayId()] = container;
    wms->windowController_->windowRoot_->displayIdMap_[compNode->GetDisplayId()] = { compNode->GetDisplayId() };
    container->appWindowNode_->children_.push_back(appNode);
    container->appWindowNode_->children_.push_back(compNode);
    RemoteAnimation::SetWindowControllerAndRoot(wms->windowController_, wms->windowController_->windowRoot_);
    std::vector<uint32_t> missionIds;
    missionIds.push_back(1);
    missionIds.push_back(2);
    std::vector<sptr<RSWindowAnimationTarget>> targets;
    ASSERT_EQ(WMError::WM_OK, wms->GetWindowAnimationTargets(missionIds, targets));
    ASSERT_EQ(0, targets.size());
}
}
}
}

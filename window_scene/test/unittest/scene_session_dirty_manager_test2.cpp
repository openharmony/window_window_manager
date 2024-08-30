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
#include "transaction/rs_uiextension_data.h"
#include "input_manager.h"
#include "session_manager/include/scene_session_dirty_manager.h"
#include <gtest/gtest.h>
#include <parameter.h>
#include <parameters.h>
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "scene_input_manager.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "transaction/rs_uiextension_data.h"
#include "window_helper.h"
#include "common/include/window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionDirtyManagerTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
};
std::shared_ptr<SceneSessionDirtyManager> manager_;
sptr<SceneSessionManager> ssm_;
std::shared_ptr<SceneInputManager> sim_;
void SceneSessionDirtyManagerTest2::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
    ssm_->sceneSessionMap_.clear();
    sim_ = std::make_shared<SceneInputManager>();
}

void SceneSessionDirtyManagerTest2::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionDirtyManagerTest2::SetUp()
{
    manager_ = std::make_shared<SceneSessionDirtyManager>();
}

void SceneSessionDirtyManagerTest2::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    manager_ = nullptr;
}
namespace {

void InitSessionInfo(MMI::DisplayInfo& displayedInfo, MMI::WindowInfo& windowInfo)
{
    displayedInfo = {
        .id = 42,
        .x = 0,
        .y = 0,
        .width = 1270,
        .height = 2240
    };

    windowInfo = {
        .id = 43,
        .pid = 433,
        .displayId = 42,
        .zOrder = 30.0,
        .area = {0, 0, 1000, 1200}
    };
}

/**
 * @tc.name: Init sceneSession
 * @tc.desc: Init sceneSession
 * @tc.type: FUNC
 */
void InitSceneSession(sptr<SceneSession> &sceneSession, int32_t pid, int windowId, WindowType propertyType)
{
    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(windowSessionProperty, nullptr);
    uint64_t displayId = 1;
    windowSessionProperty->SetDisplayId(displayId);
    windowSessionProperty->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    windowSessionProperty->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    windowSessionProperty->SetWindowType(propertyType);
    windowSessionProperty->topmost_ = false;
    windowSessionProperty->SetPersistentId(windowId);
    sceneSession->SetSessionProperty(windowSessionProperty);

    WSRect windowRect = {0, 0, 1270, 2700};
    sceneSession->SetSessionRect(windowRect);

    sceneSession->SetCallingPid(pid);
    int32_t uid = 1315;
    sceneSession->SetCallingUid(uid);
}

/**
 * @tc.name: GetWindowInfoWithoutHotArea
 * @tc.desc: windowInfo without hotAreas information
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithoutHotArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TestWithoutHotArea";
    info.bundleName_ = "TestWithoutHotArea";
    sptr<SceneSession> sceneSession =  new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(windowSessionProperty, nullptr);
    windowSessionProperty->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    sceneSession->InitSessionPropertyWhenConnect(windowSessionProperty);
    WSRect windowRect = {0, 0, 1270, 2700};
    sceneSession->SetSessionRect(windowRect);
    sceneSession->globalRect_ = windowRect;
    //set hotArea without info
    std::vector<MMI::Rect> touchHotAreas;
    std::vector<MMI::Rect> pointerHotAreas;
    manager_->UpdateHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    bool touchHotResult = touchHotAreas[0].x == 0 && touchHotAreas[0].y == 0 &&
                          touchHotAreas[0].width == 1270 && touchHotAreas[0].height == 2700;
    ASSERT_EQ(touchHotResult, true);
    bool pointerHotResult = pointerHotAreas[0].x == 0 && pointerHotAreas[0].y == 0 &&
                          pointerHotAreas[0].width == 1270 && pointerHotAreas[0].height == 2700;
    ASSERT_EQ(pointerHotResult, true);
}

/**
 * @tc.name: GetWindowInfoWithHotArea
 * @tc.desc: windowInfo with hotAreas information
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithHotArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TestWithHotArea";
    info.bundleName_ = "TestWithHotArea";
    sptr<SceneSession> sceneSession =  new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(windowSessionProperty, nullptr);
    windowSessionProperty->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    Rect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.width_ = 300;
    rect.height_ = 500;
    std::vector<Rect> rects;
    rects.emplace_back(rect);
    // set touchHotArea and pointerHotArea info
    windowSessionProperty->SetTouchHotAreas(rects);
    sceneSession->InitSessionPropertyWhenConnect(windowSessionProperty);
    WSRect windowRect = {0, 0, 1270, 2700};
    sceneSession->SetSessionRect(windowRect);
    std::vector<MMI::Rect> touchHotAreas;
    std::vector<MMI::Rect> pointerHotAreas;
    manager_->UpdateHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    bool touchHotResult = touchHotAreas[0].x == 0 && touchHotAreas[0].y == 0 &&
                          touchHotAreas[0].width == 300 && touchHotAreas[0].height == 500;
    ASSERT_EQ(touchHotResult, true);
    bool pointerHotResult = pointerHotAreas[0].x == 0 && pointerHotAreas[0].y == 0 &&
                          pointerHotAreas[0].width == 300 && pointerHotAreas[0].height == 500;
    ASSERT_EQ(pointerHotResult, true);
}

/**
 * @tc.name: GetWindowInfoWithWindowTypeDialog
 * @tc.desc: windowInfo with windowType dialog
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithWindowTypeDialog, Function | SmallTest | Level2)
{
    // init main window info
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 30;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->zOrder_ = 55.0f;
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->isVisible_ = true;
    int32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    int32_t mainWindowPid = 50;
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    // init sub window with window type dialog
    SessionInfo dialogWindowInfo;
    dialogWindowInfo.abilityName_ = "TestDialogWithType";
    dialogWindowInfo.bundleName_ = "TestDialogWithType";
    int32_t dialogWindowId = 31;
    sptr<SceneSession> sceneSessionDialogWindow =  new (std::nothrow) SceneSession(dialogWindowInfo, nullptr);
    ASSERT_NE(sceneSessionDialogWindow, nullptr);
    sceneSessionDialogWindow->zOrder_ = 56.0f;
    sceneSessionDialogWindow->persistentId_ = dialogWindowId;
    sceneSessionDialogWindow->isVisible_ = true;
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL);
    sceneSessionDialogWindow->property_->SetWindowFlags(flags);
    int32_t dialogWindowPid = 51;
    InitSceneSession(sceneSessionDialogWindow, dialogWindowPid, dialogWindowId, WindowType::WINDOW_TYPE_DIALOG);
    sceneSessionDialogWindow->SetParentSession(sceneSessionMainWindow);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    retSceneSessionMap.insert(std::make_pair(dialogWindowPid, sceneSessionDialogWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    auto [windowInfoList, pixelMapList] = manager_->GetFullWindowInfoList();
    ASSERT_EQ(windowInfoList.size(), 2);
    bool windowTypeDialogResult = false;
    for (MMI::WindowInfo windowInfo : windowInfoList) {
        if (windowInfo.id == mainWindowId && windowInfo.agentWindowId == dialogWindowId &&
            windowInfo.pid == dialogWindowPid) {
                windowTypeDialogResult = true;
            }
    }
    ASSERT_EQ(windowTypeDialogResult, true);
}

/**
 * @tc.name: GetWindowInfoWithWindowTypeAppSub
 * @tc.desc: windowInfo with window_Type_app_sub
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithWindowTypeAppSub, Function | SmallTest | Level2)
{
    // init main window info
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 32;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->isVisible_ = true;
    int32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    int32_t mainWindowPid = 52;
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    // init sub window with window type float
    SessionInfo subWindowInfo;
    subWindowInfo.abilityName_ = "TestSubWithType";
    subWindowInfo.bundleName_ = "TestSubWithType";
    int32_t subWindowId = 33;
    sptr<SceneSession> sceneSessionSubWindow =  new (std::nothrow) SceneSession(subWindowInfo, nullptr);
    ASSERT_NE(sceneSessionSubWindow, nullptr);
    sceneSessionSubWindow->persistentId_ = subWindowId;
    sceneSessionSubWindow->isVisible_ = true;
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL);
    sceneSessionSubWindow->property_->SetWindowFlags(flags);
    int32_t subWindowPid = 53;
    InitSceneSession(sceneSessionSubWindow, subWindowPid, subWindowId, WindowType::WINDOW_TYPE_FLOAT);
    sceneSessionSubWindow->SetParentSession(sceneSessionMainWindow);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    retSceneSessionMap.insert(std::make_pair(subWindowPid, sceneSessionSubWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    auto [windowInfoList, pixelMapList] = manager_->GetFullWindowInfoList();
    ASSERT_EQ(windowInfoList.size(), 2);
    bool windowTypeDialogResult = false;
    for (MMI::WindowInfo windowInfo : windowInfoList) {
        if (windowInfo.id == mainWindowId && windowInfo.agentWindowId == subWindowId &&
            windowInfo.pid == subWindowPid) {
                windowTypeDialogResult = true;
            }
    }
    ASSERT_EQ(windowTypeDialogResult, false);
}

/**
 * @tc.name: GetWindowInfoWithTypeKeyboardPanel
 * @tc.desc: windowInfo with window_Type_keyboard panel
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithTypeKeyboardPanel, Function | SmallTest | Level2)
{
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 34;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->isVisible_ = false;
    uint32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    int32_t mainWindowPid = 54;
    // init sceneSession with windowType is window_type_keyBoard_panel
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMap =
        Rosen::SceneSessionManager::GetInstance().GetSceneSessionMap();
    int32_t windowInfoSize = sceneSessionMap.size();
    ASSERT_EQ(windowInfoSize, 0);
}

/**
 * @tc.name: GetWindowInfoWithoutParentWindow
 * @tc.desc: windowInfo without parent window
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithoutParentWindow, Function | SmallTest | Level2)
{
    SessionInfo subWindowInfo;
    subWindowInfo.abilityName_ = "TestSubWithType";
    subWindowInfo.bundleName_ = "TestSubWithType";
    int32_t subWindowId = 35;
    sptr<SceneSession> sceneSessionSubWindow =  new (std::nothrow) SceneSession(subWindowInfo, nullptr);
    ASSERT_NE(sceneSessionSubWindow, nullptr);
    sceneSessionSubWindow->persistentId_ = subWindowId;
    sceneSessionSubWindow->isVisible_ = false;
    uint32_t subFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionSubWindow->property_->SetWindowFlags(subFlags);
    int32_t subWindowPid = 55;
    InitSceneSession(sceneSessionSubWindow, subWindowPid, subWindowId, WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSessionSubWindow->SetParentSession(nullptr);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(subWindowPid, sceneSessionSubWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMap =
        Rosen::SceneSessionManager::GetInstance().GetSceneSessionMap();
    int32_t windowInfoSize = sceneSessionMap.size();
    ASSERT_EQ(windowInfoSize, 0);
}

/**
 * @tc.name: GetWindowInfoWithoutParentWindowAndStateActive
 * @tc.desc: windowInfo without parent window and state active
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithoutParentWindowAndStateActive, Function | SmallTest | Level2)
{
    SessionInfo subWindowInfo;
    subWindowInfo.abilityName_ = "TestSubWithType";
    subWindowInfo.bundleName_ = "TestSubWithType";
    int32_t subWindowId = 36;
    sptr<SceneSession> sceneSessionSubWindow =  new (std::nothrow) SceneSession(subWindowInfo, nullptr);
    ASSERT_NE(sceneSessionSubWindow, nullptr);
    sceneSessionSubWindow->persistentId_ = subWindowId;
    sceneSessionSubWindow->isVisible_ = false;
    uint32_t subFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionSubWindow->property_->SetWindowFlags(subFlags);
    int32_t subWindowPid = 56;
    InitSceneSession(sceneSessionSubWindow, subWindowPid, subWindowId, WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSessionSubWindow->SetParentSession(nullptr);
    sceneSessionSubWindow->SetSessionState(SessionState::STATE_ACTIVE);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(subWindowPid, sceneSessionSubWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMap =
        Rosen::SceneSessionManager::GetInstance().GetSceneSessionMap();
    int32_t windowInfoSize = sceneSessionMap.size();
    ASSERT_EQ(windowInfoSize, 0);
}

/**
 * @tc.name: GetWindowInfoWithNotSystemTouchable
 * @tc.desc: windowInfo with systemtouchable is false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithNotSystemTouchable, Function | SmallTest | Level2)
{
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 37;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->isVisible_ = true;
    uint32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    bool isTouchable = false;
    // set systemTouchable false;
    sceneSessionMainWindow->SetSystemTouchable(isTouchable);
    int32_t mainWindowPid = 57;
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    MMI::WindowInfo windowInfo;
    ScreenId screenId = 37;
    ScreenSessionConfig config;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    manager_->UpdateWindowFlags(screenId, sceneSessionMainWindow, windowInfo);
    bool windowFlagResult = false;
    if (windowInfo.flags == 1) {
        windowFlagResult = true;
    }
    ASSERT_EQ(windowFlagResult, true);
}

/**
 * @tc.name: GetWindowInfoWithNotIsTouchEnable
 * @tc.desc: windowInfo with isTouchenable is false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithNotIsTouchEnable, Function | SmallTest | Level2)
{
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 38;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->isVisible_ = true;
    uint32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    bool isTouchable = true;
    sceneSessionMainWindow->SetSystemTouchable(isTouchable);
    int32_t mainWindowPid = 58;
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    MMI::WindowInfo windowInfo;
    ScreenId screenId = 38;
    ScreenSessionConfig config;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    // set screenSession touchenable_ true
    screenSession->touchEnabled_.store(false);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    manager_->UpdateWindowFlags(screenId, sceneSessionMainWindow, windowInfo);
    bool windowFlagResult = false;
    if (windowInfo.flags == MMI::WindowInfo::FLAG_BIT_UNTOUCHABLE) {
        windowFlagResult = true;
    }
    ASSERT_EQ(windowFlagResult, true);
}

/**
 * @tc.name: GetWindowInfoWithNotIsForceTouchEnable
 * @tc.desc: windowInfo with isForeTouchenable is false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithNotIsForceTouchEnable, Function | SmallTest | Level2)
{
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 39;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->isVisible_ = true;
    uint32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    // set isForceTouchable false
    bool isForceTouchable = false;
    sceneSessionMainWindow->SetForceTouchable(isForceTouchable);
    int32_t mainWindowPid = 59;
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    MMI::WindowInfo windowInfo;
    ScreenId screenId = 39;
    ScreenSessionConfig config;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    screenSession->touchEnabled_.store(true);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    manager_->UpdateWindowFlags(screenId, sceneSessionMainWindow, windowInfo);
    bool windowFlagResult = false;
    if (windowInfo.flags == 1) {
        windowFlagResult = true;
    }
    ASSERT_EQ(windowFlagResult, true);
}

/**
 * @tc.name: GetWindowInfoWithNotForegroundInteractiveStatus
 * @tc.desc: windowInfo with foregroundInteractiveStatus_ is false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithNotForegroundInteractiveStatus, Function | SmallTest | Level2)
{
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 40;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->isVisible_ = true;
    // set foregroundInterativeStatus_ false
    sceneSessionMainWindow->foregroundInteractiveStatus_.store(false);
    uint32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    int32_t mainWindowPid = 60;
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    MMI::WindowInfo windowInfo;
    ScreenId screenId = 40;
    ScreenSessionConfig config;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    manager_->UpdateWindowFlags(screenId, sceneSessionMainWindow, windowInfo);
    bool windowFlagResult = false;
    if (windowInfo.flags == 1) {
        windowFlagResult = true;
    }
    ASSERT_EQ(windowFlagResult, true);
}

/**
 * @tc.name: GetWindowInfoWithNotPropertyTouchable
 * @tc.desc: windowInfo with property touchable false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithNotPropertyTouchable, Function | SmallTest | Level2)
{
    SessionInfo mainWindowInfo;
    mainWindowInfo.abilityName_ = "TestMainWithType";
    mainWindowInfo.bundleName_ = "TestMainWithType";
    int32_t mainWindowId = 41;
    sptr<SceneSession> sceneSessionMainWindow =  new (std::nothrow) SceneSession(mainWindowInfo, nullptr);
    ASSERT_NE(sceneSessionMainWindow, nullptr);
    sceneSessionMainWindow->persistentId_ = mainWindowId;
    sceneSessionMainWindow->foregroundInteractiveStatus_.store(false);
    uint32_t mainFlags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sceneSessionMainWindow->property_->SetWindowFlags(mainFlags);
    // set property_.touchEnable_ false
    sceneSessionMainWindow->property_->touchable_ = false;
    int32_t mainWindowPid = 61;
    InitSceneSession(sceneSessionMainWindow, mainWindowPid, mainWindowId, WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    retSceneSessionMap.insert(std::make_pair(mainWindowPid, sceneSessionMainWindow));
    ssm_->sceneSessionMap_ = retSceneSessionMap;
    MMI::WindowInfo windowInfo;
    ScreenId screenId = 41;
    ScreenSessionConfig config;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    manager_->UpdateWindowFlags(screenId, sceneSessionMainWindow, windowInfo);
    bool windowFlagResult = false;
    if (windowInfo.flags == 1) {
        windowFlagResult = true;
    }
    ASSERT_EQ(windowFlagResult, true);
}

/**
 * @tc.name: GetWindowInfoWithSameInfo
 * @tc.desc: windowInfo with same displayInfos and windowInfos
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithSameInfo, Function | SmallTest | Level2)
{
    MMI::DisplayInfo lastDisplayedInfo;
    MMI::WindowInfo lastWindowInfo;
    InitSessionInfo(lastDisplayedInfo, lastWindowInfo);
    int32_t focusedSession = 42;
    sim_->focusedSessionId_ = focusedSession;
    int32_t lastFocusedSession = 42;
    sim_->lastFocusId_ = lastFocusedSession;
    std::vector<MMI::DisplayInfo> lastDisplayInfos;
    std::vector<MMI::WindowInfo> lastWindowInfoList;
    lastDisplayInfos.emplace_back(lastDisplayedInfo);
    lastWindowInfoList.emplace_back(lastWindowInfo);
    sim_->lastDisplayInfos_ = lastDisplayInfos;
    sim_->lastWindowInfoList_ = lastWindowInfoList;
    MMI::DisplayInfo currDisplayedInfo;
    MMI::WindowInfo currWindowInfo;
    InitSessionInfo(currDisplayedInfo, currWindowInfo);
    std::vector<MMI::DisplayInfo> currDisplayInfos;
    std::vector<MMI::WindowInfo> currWindowInfoList;
    currDisplayInfos.emplace_back(currDisplayedInfo);
    currWindowInfoList.emplace_back(currWindowInfo);
    ssm_->focusedSessionId_ = focusedSession;
    bool checkNeedUpdateFlag = true;
    // check the same information
    checkNeedUpdateFlag = sim_->CheckNeedUpdate(currDisplayInfos, currWindowInfoList);
    ASSERT_EQ(checkNeedUpdateFlag, false);
}

/**
 * @tc.name: GetWindowInfoWithPidDiff
 * @tc.desc: windowInfo with different pid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithPidDiff, Function | SmallTest | Level2)
{
    MMI::DisplayInfo lastDisplayedInfo;
    MMI::WindowInfo lastWindowInfo;
    InitSessionInfo(lastDisplayedInfo, lastWindowInfo);
    int32_t focusedSession = 43;
    sim_->focusedSessionId_ = focusedSession;
    int32_t lastFocusedSession = 43;
    sim_->lastFocusId_ = lastFocusedSession;
    std::vector<MMI::DisplayInfo> lastDisplayInfos;
    std::vector<MMI::WindowInfo> lastWindowInfoList;
    lastDisplayInfos.emplace_back(lastDisplayedInfo);
    lastWindowInfoList.emplace_back(lastWindowInfo);
    sim_->lastDisplayInfos_ = lastDisplayInfos;
    sim_->lastWindowInfoList_ = lastWindowInfoList;
    MMI::DisplayInfo currDisplayedInfo;
    MMI::WindowInfo currWindowInfo;
    InitSessionInfo(currDisplayedInfo, currWindowInfo);
    // set different pid number
    currWindowInfo.pid = 434;
    std::vector<MMI::DisplayInfo> currDisplayInfos;
    std::vector<MMI::WindowInfo> currWindowInfoList;
    currDisplayInfos.emplace_back(currDisplayedInfo);
    currWindowInfoList.emplace_back(currWindowInfo);
    ssm_->focusedSessionId_ = focusedSession;
    bool checkNeedUpdateFlag = false;
    checkNeedUpdateFlag = sim_->CheckNeedUpdate(currDisplayInfos, currWindowInfoList);
    ASSERT_EQ(checkNeedUpdateFlag, true);
}

/**
 * @tc.name: GetWindowInfoWithAreaDiff
 * @tc.desc: windowInfo with different area
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithAreaDiff, Function | SmallTest | Level2)
{
    MMI::DisplayInfo lastDisplayedInfo;
    MMI::WindowInfo lastWindowInfo;
    InitSessionInfo(lastDisplayedInfo, lastWindowInfo);
    int32_t focusedSession = 44;
    sim_->focusedSessionId_ = focusedSession;
    int32_t lastFocusedSession = 44;
    sim_->lastFocusId_ = lastFocusedSession;
    std::vector<MMI::DisplayInfo> lastDisplayInfos;
    std::vector<MMI::WindowInfo> lastWindowInfoList;
    lastDisplayInfos.emplace_back(lastDisplayedInfo);
    lastWindowInfoList.emplace_back(lastWindowInfo);
    sim_->lastDisplayInfos_ = lastDisplayInfos;
    sim_->lastWindowInfoList_ = lastWindowInfoList;
    MMI::DisplayInfo currDisplayedInfo;
    MMI::WindowInfo currWindowInfo;
    InitSessionInfo(currDisplayedInfo, currWindowInfo);
    // set different area number
    currWindowInfo.area = {0, 0, 500, 600};
    std::vector<MMI::DisplayInfo> currDisplayInfos;
    std::vector<MMI::WindowInfo> currWindowInfoList;
    currDisplayInfos.emplace_back(currDisplayedInfo);
    currWindowInfoList.emplace_back(currWindowInfo);
    ssm_->focusedSessionId_ = focusedSession;
    bool checkNeedUpdateFlag = false;
    checkNeedUpdateFlag = sim_->CheckNeedUpdate(currDisplayInfos, currWindowInfoList);
    ASSERT_EQ(checkNeedUpdateFlag, true);
}

/**
 * @tc.name: GetWindowInfoWithzOrderDiff
 * @tc.desc: windowInfo with different zOrder
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest2, GetWindowInfoWithzOrderDiff, Function | SmallTest | Level2)
{
    MMI::DisplayInfo lastDisplayedInfo;
    MMI::WindowInfo lastWindowInfo;
    InitSessionInfo(lastDisplayedInfo, lastWindowInfo);
    int32_t focusedSession = 45;
    sim_->focusedSessionId_ = focusedSession;
    int32_t lastFocusedSession = 45;
    sim_->lastFocusId_ = lastFocusedSession;
    std::vector<MMI::DisplayInfo> lastDisplayInfos;
    std::vector<MMI::WindowInfo> lastWindowInfoList;
    lastDisplayInfos.emplace_back(lastDisplayedInfo);
    lastWindowInfoList.emplace_back(lastWindowInfo);
    sim_->lastDisplayInfos_ = lastDisplayInfos;
    sim_->lastWindowInfoList_ = lastWindowInfoList;
    MMI::DisplayInfo currDisplayedInfo;
    MMI::WindowInfo currWindowInfo;
    InitSessionInfo(currDisplayedInfo, currWindowInfo);
    // set different zOrder number
    currWindowInfo.zOrder = 40.0;
    std::vector<MMI::DisplayInfo> currDisplayInfos;
    std::vector<MMI::WindowInfo> currWindowInfoList;
    currDisplayInfos.emplace_back(currDisplayedInfo);
    currWindowInfoList.emplace_back(currWindowInfo);
    ssm_->focusedSessionId_ = focusedSession;
    bool checkNeedUpdateFlag = false;
    checkNeedUpdateFlag = sim_->CheckNeedUpdate(currDisplayInfos, currWindowInfoList);
    ASSERT_EQ(checkNeedUpdateFlag, true);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
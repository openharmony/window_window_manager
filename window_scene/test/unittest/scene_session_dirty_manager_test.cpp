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
#include "transaction/rs_uiextension_data.h"
#include "input_manager.h"
#include "session_manager/include/scene_session_dirty_manager.h"
#include <gtest/gtest.h>
#include <parameter.h>
#include <parameters.h>
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "transaction/rs_uiextension_data.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int POINTER_CHANGE_AREA_SIXTEEN = 16;
constexpr int POINTER_CHANGE_AREA_DEFAULT = 0;
constexpr int POINTER_CHANGE_AREA_FIVE = 5;
static int32_t g_screenRotationOffset = system::GetIntParameter<int32_t>("const.fold.screen_rotation.offset", 0);
class SceneSessionDirtyManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
};
SceneSessionDirtyManager *manager_;
SceneSessionManager *ssm_;
void SceneSessionDirtyManagerTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionDirtyManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionDirtyManagerTest::SetUp()
{
    manager_ = new SceneSessionDirtyManager();
}

void SceneSessionDirtyManagerTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    delete manager_;
    manager_ = nullptr;
}
namespace {

/**
 * @tc.name: NotifyWindowInfoChange
 * @tc.desc: NotifyWindowInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, NotifyWindowInfoChange, Function | SmallTest | Level2)
{
    int ret = 0;
    manager_->NotifyWindowInfoChange(nullptr, WindowUpdateType::WINDOW_UPDATE_ADDED, true);
    SessionInfo info;
    info.abilityName_ = "TestAbilityName";
    info.bundleName_ = "TestBundleName";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    manager_->NotifyWindowInfoChange(sceneSession, WindowUpdateType::WINDOW_UPDATE_ADDED, true);
    manager_->NotifyWindowInfoChange(sceneSession, WindowUpdateType::WINDOW_UPDATE_REMOVED, true);
    manager_->NotifyWindowInfoChange(sceneSession, WindowUpdateType::WINDOW_UPDATE_ACTIVE, true);
    manager_->NotifyWindowInfoChange(sceneSession, WindowUpdateType::WINDOW_UPDATE_FOCUSED, true);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetFullWindowInfoList
 * @tc.desc: GetFullWindowInfoList
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, GetFullWindowInfoList, Function | SmallTest | Level2)
{
    int ret = 0;
    SessionInfo info;
    info.abilityName_ = "TestAbilityName";
    info.bundleName_ = "TestBundleName";
    {
        sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
        if (sceneSession == nullptr) {
            return;
        }
        ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    }
    {
        ssm_->sceneSessionMap_.insert({111, nullptr});
    }
    {
        sptr<SceneSession> sceneSessionDialog1 = new (std::nothrow) SceneSession(info, nullptr);
        if (sceneSessionDialog1 == nullptr) {
            return;
        }
        sptr<WindowSessionProperty> propertyDialog1 = sceneSessionDialog1->GetSessionProperty();
        propertyDialog1->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
        ssm_->sceneSessionMap_.insert({sceneSessionDialog1->GetPersistentId(), sceneSessionDialog1});
    }
    {
        sptr<SceneSession> sceneSessionModal1 = new (std::nothrow) SceneSession(info, nullptr);
        if (sceneSessionModal1 == nullptr) {
            return;
        }
        sptr<WindowSessionProperty> propertyModal1 = sceneSessionModal1->GetSessionProperty();
        propertyModal1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL));
        ssm_->sceneSessionMap_.insert({sceneSessionModal1->GetPersistentId(), sceneSessionModal1});
    }
    manager_->GetFullWindowInfoList();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: IsFilterSession
 * @tc.desc: IsFilterSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, IsFilterSession, Function | SmallTest | Level2)
{
    int ret = 0;
    manager_->IsFilterSession(nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.sceneType_ = SceneType::INPUT_SCENE;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    sptr<WindowSessionProperty> property = sceneSession->GetSessionProperty();
    info.sceneType_ = SceneType::INPUT_SCENE;
    manager_->IsFilterSession(sceneSession);
    info.sceneType_ = SceneType::WINDOW_SCENE;
    info.isSystem_ = false;
    sceneSession->isVisible_ = false;
    sceneSession->isSystemActive_ = false;
    manager_->IsFilterSession(sceneSession);
    info.isSystem_ = true;
    manager_->IsFilterSession(sceneSession);
    sceneSession->isVisible_ = true;
    manager_->IsFilterSession(sceneSession);
    sceneSession->isSystemActive_ = true;
    manager_->IsFilterSession(sceneSession);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetWindowInfo
 * @tc.desc: GetWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, GetWindowInfo, Function | SmallTest | Level2)
{
    int ret = 0;
    manager_->GetWindowInfo(nullptr, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    SessionInfo info;
    info.abilityName_ = "111";
    info.bundleName_ = "111";
    sptr<SceneSession> session = new (std::nothrow) SceneSession(info, nullptr);
    if (session == nullptr) {
        return;
    }
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    session = new (std::nothrow) SceneSession(info, nullptr);
    if (session == nullptr) {
        return;
    }
    sptr<WindowSessionProperty> windowSessionProperty = session->GetSessionProperty();
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    windowSessionProperty->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    windowSessionProperty->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    windowSessionProperty->SetMaximizeMode(Rosen::MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    info.isSetPointerAreas_ = true;
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    windowSessionProperty->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_HANDWRITING));
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    windowSessionProperty->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL));
    manager_->GetWindowInfo(session, SceneSessionDirtyManager::WindowAction::WINDOW_ADD);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CalNotRotateTransform
 * @tc.desc: CalNotRotateTransform
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, CalNotRotateTransform, Function | SmallTest | Level2)
{
    int ret = 0;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "CalNotRotateTransform";
    sessionInfo.moduleName_ = "sessionInfo";
    Matrix3f transform;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    manager_->CalNotRotateTransform(nullptr, transform);
    auto screenId = 0;
    sceneSession->GetSessionProperty()->SetDisplayId(screenId);
    manager_->CalNotRotateTransform(sceneSession, transform);
    ScreenProperty screenProperty0;
    screenProperty0.SetRotation(0.0f);
    ScreenSessionConfig config;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    ASSERT_NE(screenSession, nullptr);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::UNKNOWN);
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;
    Rosen::ScreenSessionManagerClient::GetInstance().screenSessionMap_.emplace(screenId, screenSession);
    Rosen::ScreenSessionManagerClient::GetInstance().OnPropertyChanged(screenId, screenProperty0, reason);
    manager_->CalNotRotateTransform(sceneSession, transform);

    screenProperty0.SetRotation(90.0f);
    Rosen::ScreenSessionManagerClient::GetInstance().OnPropertyChanged(screenId, screenProperty0, reason);
    manager_->CalNotRotateTransform(sceneSession, transform);

    screenProperty0.SetRotation(180.0f);
    Rosen::ScreenSessionManagerClient::GetInstance().OnPropertyChanged(screenId, screenProperty0, reason);
    manager_->CalNotRotateTransform(sceneSession, transform);

    screenProperty0.SetRotation(270.0f);
    Rosen::ScreenSessionManagerClient::GetInstance().OnPropertyChanged(screenId, screenProperty0, reason);
    manager_->CalNotRotateTransform(sceneSession, transform);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CalTransform
 * @tc.desc: CalTransform
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, CalTransform, Function | SmallTest | Level2)
{
    int ret = 0;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "CalTransform";
    sessionInfo.moduleName_ = "CalTransform";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    Matrix3f transform;
    manager_->CalTransform(nullptr, transform);
    sessionInfo.isRotable_ = true;
    manager_->CalTransform(sceneSession, transform);
    sessionInfo.isSystem_ = true;
    manager_->CalTransform(sceneSession, transform);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateHotAreas
 * @tc.desc: UpdateHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, UpdateHotAreas, Function | SmallTest | Level2)
{
    int ret = 0;
    std::vector<MMI::Rect> empty(0);
    manager_->UpdateHotAreas(nullptr, empty, empty);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "UpdateHotAreas";
    sessionInfo.moduleName_ = "UpdateHotAreas";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    std::vector<MMI::Rect> touchHotAreas(0);
    std::vector<MMI::Rect> pointerHotAreas(0);
    std::vector<OHOS::Rosen::Rect> touchHotAreasInSceneSession(0);
    sceneSession->GetSessionProperty()->SetTouchHotAreas(touchHotAreasInSceneSession);
    sceneSession->persistentId_ = 1;
    for (int i = 0; i < 2 ; i++) {
        OHOS::Rosen::Rect area;
        area.posX_ = i * 10;
        area.posY_ = i * 10;
        area.width_ = 10;
        area.height_ = 10;
        touchHotAreasInSceneSession.emplace_back(area);
    }
    manager_->UpdateHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    for (int i = 2; i < 10 ; i++) {
        OHOS::Rosen::Rect area;
        area.posX_ = i * 10;
        area.posY_ = i * 10;
        area.width_ = 10;
        area.height_ = 10;
        touchHotAreasInSceneSession.emplace_back(area);
    }
    manager_->UpdateHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateDefaultHotAreas
 * @tc.desc: UpdateDefaultHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, UpdateDefaultHotAreas, Function | SmallTest | Level2)
{
    int ret = 0;
    std::vector<MMI::Rect> empty(0);
    manager_->UpdateDefaultHotAreas(nullptr, empty, empty);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "UpdateDefaultHotAreas";
    sessionInfo.moduleName_ = "UpdateDefaultHotAreas";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    WSRect rect = {0, 0, 320, 240};
    sceneSession->SetSessionRect(rect);
    manager_->UpdateDefaultHotAreas(sceneSession, empty, empty);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    manager_->UpdateDefaultHotAreas(sceneSession, empty, empty);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    manager_->UpdateDefaultHotAreas(sceneSession, empty, empty);
    sceneSession->SetSessionProperty(nullptr);
    manager_->UpdateDefaultHotAreas(sceneSession, empty, empty);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConvertDegreeToMMIRotation
 * @tc.desc: ConvertDegreeToMMIRotation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, ConvertDegreeToMMIRotation, Function | SmallTest | Level2)
{
    MMI::Direction dirction = MMI::DIRECTION0;
    dirction = ConvertDegreeToMMIRotation(0.0, MMI::DisplayMode::UNKNOWN);
    ASSERT_EQ(dirction, MMI::DIRECTION0);
    dirction = ConvertDegreeToMMIRotation(90.0, MMI::DisplayMode::UNKNOWN);
    ASSERT_EQ(dirction, MMI::DIRECTION90);
    dirction = ConvertDegreeToMMIRotation(180.0, MMI::DisplayMode::UNKNOWN);
    ASSERT_EQ(dirction, MMI::DIRECTION180);
    dirction = ConvertDegreeToMMIRotation(270.0, MMI::DisplayMode::UNKNOWN);
    ASSERT_EQ(dirction, MMI::DIRECTION270);
    if (g_screenRotationOffset != 0) {
        dirction = ConvertDegreeToMMIRotation(0.0, MMI::DisplayMode::FULL);
        ASSERT_EQ(dirction, MMI::DIRECTION90);
        dirction = ConvertDegreeToMMIRotation(90.0, MMI::DisplayMode::FULL);
        ASSERT_EQ(dirction, MMI::DIRECTION180);
        dirction = ConvertDegreeToMMIRotation(180.0, MMI::DisplayMode::FULL);
        ASSERT_EQ(dirction, MMI::DIRECTION270);
        dirction = ConvertDegreeToMMIRotation(270.0, MMI::DisplayMode::FULL);
        ASSERT_EQ(dirction, MMI::DIRECTION0);
        dirction = ConvertDegreeToMMIRotation(30.0, MMI::DisplayMode::FULL);
        ASSERT_EQ(dirction, MMI::DIRECTION90);
    }
    dirction = ConvertDegreeToMMIRotation(30.0, MMI::DisplayMode::UNKNOWN);
    ASSERT_EQ(dirction, MMI::DIRECTION0);
}

/**
 * @tc.name: GetDialogSessionMap
 * @tc.desc: GetDialogSessionMap
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, GetDialogSessionMap, Function | SmallTest | Level2)
{
    std::map<int32_t, sptr<SceneSession>> sessionMap;
    SessionInfo info;
    info.abilityName_ = "TestAbilityName";
    info.bundleName_ = "TestBundleName";
    sessionMap.emplace(1, nullptr);
    auto sessionList = manager_->GetDialogSessionMap(sessionMap);
    ASSERT_EQ(0, sessionList.size());
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (!sceneSession) {
        GTEST_LOG_(INFO) << "sceneSession is nullptr";
        return;
    }
    sessionMap.emplace(2, sceneSession);
    auto sessionList2 = manager_->GetDialogSessionMap(sessionMap);
    ASSERT_EQ(0, sessionList2.size());
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    if (!property) {
        GTEST_LOG_(INFO) << "property is nullptr";
        return;
    }
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sceneSession->SetSessionProperty(property);
    sptr<Session> session = new (std::nothrow) Session(info);
    if (!session) {
        GTEST_LOG_(INFO) << "session is nullptr";
        return;
    }
    sceneSession->SetParentSession(session);
    auto sessionList3 = manager_->GetDialogSessionMap(sessionMap);
    ASSERT_EQ(1, sessionList3.size());
}

/**
 * @tc.name: UpdatePointerAreas
 * @tc.desc: UpdatePointerAreas
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, UpdatePointerAreas, Function | SmallTest | Level2)
{
    std::vector<int32_t> pointerChangeAreas;
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    if (!sceneSession || !property) {
        return;
    }
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    property->SetDragEnabled(false);
    sceneSession->SetSessionProperty(property);
    manager_->UpdatePointerAreas(sceneSession, pointerChangeAreas);
    ASSERT_EQ(0, pointerChangeAreas.size());
    property->SetDragEnabled(true);
    float vpr = 1.5f;
    property->SetDisplayId(100);
    int32_t pointerAreaFivePx = static_cast<int32_t>(POINTER_CHANGE_AREA_FIVE * vpr);
    int32_t pointerAreaSixteenPx = static_cast<int32_t>(POINTER_CHANGE_AREA_SIXTEEN * vpr);
    WindowLimits limits;
    limits.maxHeight_ = 1;
    limits.minHeight_ = 0;
    limits.maxWidth_ = 0;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    manager_->UpdatePointerAreas(sceneSession, pointerChangeAreas);
    std::vector<int32_t> compare2 = {POINTER_CHANGE_AREA_DEFAULT, pointerAreaFivePx,
        POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT,
        pointerAreaFivePx, POINTER_CHANGE_AREA_DEFAULT,  POINTER_CHANGE_AREA_DEFAULT};
    ASSERT_EQ(compare2, pointerChangeAreas);
    limits.maxHeight_ = 0;
    limits.minHeight_ = 0;
    limits.maxWidth_ = 1;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    manager_->UpdatePointerAreas(sceneSession, pointerChangeAreas);
    std::vector<int32_t> compare3 = {POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT,
        POINTER_CHANGE_AREA_DEFAULT, pointerAreaFivePx, POINTER_CHANGE_AREA_DEFAULT,
        POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT, pointerAreaFivePx};
    ASSERT_EQ(compare3, pointerChangeAreas);
    limits.maxHeight_ = 1;
    limits.minHeight_ = 0;
    limits.maxWidth_ = 1;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    manager_->UpdatePointerAreas(sceneSession, pointerChangeAreas);
    std::vector<int32_t> compare4 = pointerChangeAreas = {pointerAreaSixteenPx, pointerAreaFivePx,
        pointerAreaSixteenPx, pointerAreaFivePx, pointerAreaSixteenPx,
        pointerAreaFivePx, pointerAreaSixteenPx, pointerAreaFivePx};
    ASSERT_EQ(compare4, pointerChangeAreas);
}

/**
 * @tc.name: UpdatePrivacyMode
 * @tc.desc: UpdatePrivacyMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, UpdatePrivacyMode, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    MMI::WindowInfo windowinfo;
    auto tempProperty = sceneSession->GetSessionProperty();
    sceneSession->property_ = nullptr;
    manager_->UpdatePrivacyMode(sceneSession, windowinfo);

    sceneSession->property_ = tempProperty;
    sceneSession->property_->isPrivacyMode_ = true;
    sceneSession->property_->isSystemPrivacyMode_ = false;
    manager_->UpdatePrivacyMode(sceneSession, windowinfo);

    sceneSession->property_->isPrivacyMode_ = false;
    sceneSession->property_->isSystemPrivacyMode_ = true;
    manager_->UpdatePrivacyMode(sceneSession, windowinfo);

    sceneSession->property_->isPrivacyMode_ = false;
    sceneSession->property_->isSystemPrivacyMode_ = false;
    manager_->UpdatePrivacyMode(sceneSession, windowinfo);
}

/**
 * @tc.name: UpdateWindowFlags
 * @tc.desc: UpdateWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, UpdateWindowFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    MMI::WindowInfo windowinfo;
    auto screenId = 0;
    manager_->UpdateWindowFlags(screenId, sceneSession, windowinfo);

    ScreenProperty screenProperty0;
    screenProperty0.SetRotation(0.0f);
    ScreenSessionConfig config;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    ASSERT_NE(screenSession, nullptr);
    screenSession->SetTouchEnabledFromJs(true);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::UNKNOWN);
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;
    Rosen::ScreenSessionManagerClient::GetInstance().screenSessionMap_.emplace(screenId, screenSession);
    Rosen::ScreenSessionManagerClient::GetInstance().OnPropertyChanged(screenId, screenProperty0, reason);
    manager_->UpdateWindowFlags(screenId, sceneSession, windowinfo);

    screenSession->SetTouchEnabledFromJs(false);
    manager_->UpdateWindowFlags(screenId, sceneSession, windowinfo);
}

/**
 * @tc.name: AddModalExtensionWindowInfo
 * @tc.desc: AddModalExtensionWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, AddModalExtensionWindowInfo, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    std::vector<MMI::WindowInfo> windowInfoList;
    MMI::WindowInfo windowInfo;
    windowInfoList.emplace_back(windowInfo);
    manager_->AddModalExtensionWindowInfo(windowInfoList, windowInfo, nullptr);
    EXPECT_EQ(windowInfoList.size(), 1);

    ExtensionWindowEventInfo extensionInfo = {
        .persistentId = 12345,
        .pid = 1234
    };
    sceneSession->AddModalUIExtension(extensionInfo);
    manager_->AddModalExtensionWindowInfo(windowInfoList, windowInfo, sceneSession);
    ASSERT_EQ(windowInfoList.size(), 2);
    EXPECT_TRUE(windowInfoList[1].defaultHotAreas.empty());

    Rect windowRect {1, 1, 7, 8};
    extensionInfo.windowRect = windowRect;
    sceneSession->UpdateModalUIExtension(extensionInfo);
    manager_->AddModalExtensionWindowInfo(windowInfoList, windowInfo, sceneSession);
    ASSERT_EQ(windowInfoList.size(), 3);
    EXPECT_EQ(windowInfoList[2].defaultHotAreas.size(), 1);
}

/**
 * @tc.name: GetHostComponentWindowInfo
 * @tc.desc: GetHostComponentWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, GetHostComponentWindowInfo, Function | SmallTest | Level2)
{
    MMI::WindowInfo hostWindowinfo;
    SecSurfaceInfo secSurfaceInfo;
    Matrix3f transform;
    MMI::WindowInfo ret;
    MMI::WindowInfo windowInfo;
    ret = manager_->GetHostComponentWindowInfo(secSurfaceInfo, hostWindowinfo, transform);
    ASSERT_EQ(ret.id, windowInfo.id);

    SecRectInfo secRectInfo;
    secSurfaceInfo.upperNodes.emplace_back(secRectInfo);
    secSurfaceInfo.hostPid = 1;
    windowInfo.pid = 1;
    ret = manager_->GetHostComponentWindowInfo(secSurfaceInfo, hostWindowinfo, transform);
    ASSERT_EQ(ret.pid, windowInfo.pid);

    ret = manager_->GetHostComponentWindowInfo(secSurfaceInfo, hostWindowinfo, transform);
    ASSERT_EQ(ret.defaultHotAreas.size(), 1);
}

/**
 * @tc.name: GetSecComponentWindowInfo
 * @tc.desc: GetSecComponentWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, GetSecComponentWindowInfo, Function | SmallTest | Level2)
{
    MMI::WindowInfo hostWindowinfo;
    SecSurfaceInfo secSurfaceInfo;
    Matrix3f transform;
    MMI::WindowInfo ret;
    MMI::WindowInfo windowInfo;
    ret = manager_->GetSecComponentWindowInfo(secSurfaceInfo, hostWindowinfo, nullptr, transform);
    ASSERT_EQ(ret.id, windowInfo.id);

    SessionInfo info;
    info.abilityName_ = "TestAbilityName";
    info.bundleName_ = "TestBundleName";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    ret = manager_->GetSecComponentWindowInfo(secSurfaceInfo, hostWindowinfo, sceneSession, transform);
    ASSERT_EQ(ret.id, windowInfo.id);

    secSurfaceInfo.uiExtensionNodeId = 1;
    sceneSession->AddUIExtSurfaceNodeId(1, 1);
    windowInfo.privacyUIFlag = true;
    ret = manager_->GetSecComponentWindowInfo(secSurfaceInfo, hostWindowinfo, sceneSession, transform);
    ASSERT_EQ(ret.privacyUIFlag, windowInfo.privacyUIFlag);

    windowInfo.id = 1;
    ret = manager_->GetSecComponentWindowInfo(secSurfaceInfo, hostWindowinfo, sceneSession, transform);
    ASSERT_EQ(ret.id, windowInfo.id);
}

/**
 * @tc.name: GetSecSurfaceWindowinfoList
 * @tc.desc: GetSecSurfaceWindowinfoList
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, GetSecSurfaceWindowinfoList, Function | SmallTest | Level2)
{
    MMI::WindowInfo hostWindowinfo;
    SecSurfaceInfo secSurfaceInfo;
    Matrix3f transform;
    MMI::WindowInfo windowInfo;
    auto ret = manager_->GetSecSurfaceWindowinfoList(nullptr, hostWindowinfo, transform);
    ASSERT_EQ(ret.size(), 0);

    std::vector<SecSurfaceInfo> secSurfaceInfoList;
    secSurfaceInfoList.emplace_back(secSurfaceInfo);
    manager_->secSurfaceInfoMap_.emplace(1, secSurfaceInfoList);
    ret = manager_->GetSecSurfaceWindowinfoList(nullptr, hostWindowinfo, transform);
    ASSERT_EQ(ret.size(), 0);

    SessionInfo info;
    info.abilityName_ = "TestAbilityName";
    info.bundleName_ = "TestBundleName";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->id_ = 1;
    sceneSession->surfaceNode_ = surfaceNode;
    ret = manager_->GetSecSurfaceWindowinfoList(sceneSession, hostWindowinfo, transform);
    ASSERT_EQ(ret.size(), 2);

    manager_->secSurfaceInfoMap_.emplace(0, secSurfaceInfoList);
    ret = manager_->GetSecSurfaceWindowinfoList(sceneSession, hostWindowinfo, transform);
    ASSERT_EQ(ret.size(), 2);
}

/**
 * @tc.name: UpdateSecSurfaceInfo
 * @tc.desc: UpdateSecSurfaceInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, UpdateSecSurfaceInfo, Function | SmallTest | Level2)
{
    std::map<uint64_t, std::vector<SecSurfaceInfo>> secSurfaceInfoMap;
    SecRectInfo secRectInfo1;
    SecRectInfo secRectInfo2;
    SecSurfaceInfo secSurfaceInfo1;
    secSurfaceInfo1.upperNodes.emplace_back(secRectInfo1);
    SecSurfaceInfo secSurfaceInfo2;
    secSurfaceInfo2.upperNodes.emplace_back(secRectInfo2);
    std::vector<SecSurfaceInfo> secSurfaceInfoList1;
    std::vector<SecSurfaceInfo> secSurfaceInfoList2;
    secSurfaceInfoList1.emplace_back(secSurfaceInfo1);
    secSurfaceInfoList2.emplace_back(secSurfaceInfo2);
    manager_->secSurfaceInfoMap_.emplace(1, secSurfaceInfoList1);
    secSurfaceInfoMap.emplace(1, secSurfaceInfoList2);
    manager_->UpdateSecSurfaceInfo(secSurfaceInfoMap);
    ASSERT_EQ(secSurfaceInfoMap.size(), manager_->secSurfaceInfoMap_.size());

    secSurfaceInfoMap.emplace(2, secSurfaceInfoList2);
    manager_->UpdateSecSurfaceInfo(secSurfaceInfoMap);
    ASSERT_EQ(secSurfaceInfoMap.size(), manager_->secSurfaceInfoMap_.size());

    secSurfaceInfoMap.clear();
    manager_->secSurfaceInfoMap_.clear();
    secSurfaceInfoList1.clear();
    secSurfaceInfoList2.clear();
    secSurfaceInfo1.uiExtensionRectInfo.scale[0] = 1;
    secSurfaceInfoList1.emplace_back(secSurfaceInfo1);
    secSurfaceInfoList2.emplace_back(secSurfaceInfo2);
    secSurfaceInfoMap.emplace(1, secSurfaceInfoList1);
    manager_->secSurfaceInfoMap_.emplace(1, secSurfaceInfoList2);
    ASSERT_EQ(secSurfaceInfoMap.size(), manager_->secSurfaceInfoMap_.size());
}

} // namespace
} // namespace Rosen
} // namespace OHOS

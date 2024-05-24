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

#include "input_manager.h"
#include "session_manager/include/scene_session_dirty_manager.h"
#include <gtest/gtest.h>
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int POINTER_CHANGE_AREA_SIXTEEN = 16;
constexpr int POINTER_CHANGE_AREA_DEFAULT = 0;
constexpr int POINTER_CHANGE_AREA_FIVE = 5;
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
    info.isSystemInput_ = true;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    sptr<WindowSessionProperty> property = sceneSession->GetSessionProperty();
    info.isSystemInput_ = true;
    manager_->IsFilterSession(sceneSession);
    info.isSystemInput_ = false;
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
 * @tc.name: CalNotRotateTramform
 * @tc.desc: CalNotRotateTramform
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, CalNotRotateTramform, Function | SmallTest | Level2)
{
    int ret = 0;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "CalNotRotateTramform";
    sessionInfo.moduleName_ = "sessionInfo";
    Matrix3f tranform;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, nullptr);
    manager_->CalNotRotateTramform(nullptr, tranform);
    sceneSession->GetSessionProperty()->SetDisplayId(0);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    std::map<ScreenId, ScreenProperty> screensProperties =
        Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    ScreenProperty screenProperty0;
    screenProperty0.SetRotation(0.0f);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::UNKNOWN);
    Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties().insert(
        std::make_pair(0, screenProperty0));
    manager_->CalNotRotateTramform(sceneSession, tranform);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::FULL);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    ScreenProperty screenProperty1;
    screenProperty1.SetRotation(90.0f);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::UNKNOWN);
    Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties().insert(
        std::make_pair(1, screenProperty1));
    sceneSession->GetSessionProperty()->SetDisplayId(1);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::FULL);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    ScreenProperty screenProperty2;
    screenProperty2.SetRotation(180.0f);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::UNKNOWN);
    Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties().insert(
        std::make_pair(2, screenProperty2));
    sceneSession->GetSessionProperty()->SetDisplayId(2);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::FULL);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    ScreenProperty screenProperty3;
    screenProperty3.SetRotation(270.0f);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::UNKNOWN);
    Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties().insert(
        std::make_pair(3, screenProperty3));
    sceneSession->GetSessionProperty()->SetDisplayId(3);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    Rosen::ScreenSessionManagerClient::GetInstance().OnUpdateFoldDisplayMode(FoldDisplayMode::FULL);
    manager_->CalNotRotateTramform(sceneSession, tranform);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CalTramform
 * @tc.desc: CalTramform
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionDirtyManagerTest, CalTramform, Function | SmallTest | Level2)
{
    int ret = 0;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "CalTramform";
    sessionInfo.moduleName_ = "CalTramform";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    Matrix3f tranform;
    manager_->CalTramform(nullptr, tranform);
    sessionInfo.isRotable_ = true;
    manager_->CalTramform(sceneSession, tranform);
    sessionInfo.isSystem_ = true;
    manager_->CalTramform(sceneSession, tranform);
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
    dirction = ConvertDegreeToMMIRotation(0.0, MMI::DisplayMode::FULL);
    ASSERT_EQ(dirction, MMI::DIRECTION90);
    dirction = ConvertDegreeToMMIRotation(90.0, MMI::DisplayMode::FULL);
    ASSERT_EQ(dirction, MMI::DIRECTION180);
    dirction = ConvertDegreeToMMIRotation(180.0, MMI::DisplayMode::FULL);
    ASSERT_EQ(dirction, MMI::DIRECTION270);
    dirction = ConvertDegreeToMMIRotation(270.0, MMI::DisplayMode::FULL);
    ASSERT_EQ(dirction, MMI::DIRECTION0);
    dirction = ConvertDegreeToMMIRotation(30.0, MMI::DisplayMode::UNKNOWN);
    ASSERT_EQ(dirction, MMI::DIRECTION0);
    dirction = ConvertDegreeToMMIRotation(30.0, MMI::DisplayMode::FULL);
    ASSERT_EQ(dirction, MMI::DIRECTION90);
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
    if (!sceneSession) {
        GTEST_LOG_(INFO) << "sceneSession is nullptr";
        return;
    }
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    if (!property) {
        GTEST_LOG_(INFO) << "property is nullptr";
        return;
    }
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    property->SetDragEnabled(false);
    sceneSession->SetSessionProperty(property);
    manager_->UpdatePointerAreas(sceneSession, pointerChangeAreas);
    ASSERT_EQ(0, pointerChangeAreas.size());
    property->SetDragEnabled(true);
    float vpr = 1.5f;
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


} // namespace
} // namespace Rosen
} // namespace OHOS

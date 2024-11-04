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
#include "window_helper.h"
#include "display_manager.h"
#include "pointer_event.h"

#include <gtest/gtest.h>
#include "key_event.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "session/host/include/main_session.h"
#include "wm_common.h"
#include "mock/mock_session_stage.h"
#include "input_event.h"
#include <pointer_event.h>
#include <ui/rs_surface_node.h>

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest::SetUpTestCase()
{
}

void SceneSessionTest::TearDownTestCase()
{
}

void SceneSessionTest::SetUp()
{
}

void SceneSessionTest::TearDown()
{
}

namespace {

/**
 * @tc.name: SetGlobalMaximizeMode01
 * @tc.desc: SetGlobalMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetGlobalMaximizeMode01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    auto result = sceneSession->SetGlobalMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: GetGlobalMaximizeMode01
 * @tc.desc: GetGlobalMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetGlobalMaximizeMode01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    MaximizeMode mode;
    auto result = sceneSession->GetGlobalMaximizeMode(mode);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetAndGetPipTemplateInfo
 * @tc.desc: SetAndGetPipTemplateInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAndGetPipTemplateInfo, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    PiPTemplateInfo pipTemplateInfo;
    pipTemplateInfo.pipTemplateType = static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL);
    sceneSession->SetPiPTemplateInfo(pipTemplateInfo);
    ASSERT_EQ(sceneSession->GetPiPTemplateInfo().pipTemplateType,
        static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL));
}

/**
 * @tc.name: UpdateWindowSceneAfterCustomAnimation01
 * @tc.desc: UpdateWindowSceneAfterCustomAnimation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateWindowSceneAfterCustomAnimation01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    auto result = sceneSession->UpdateWindowSceneAfterCustomAnimation(false);
    ASSERT_EQ(result, WSError::WS_OK);
    result = sceneSession->UpdateWindowSceneAfterCustomAnimation(true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetZOrder01
 * @tc.desc: SetZOrder
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetZOrder01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    int resultValue = 0;
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetZOrder(2);
    ASSERT_EQ(0, resultValue);
}

/**
 * @tc.name: GetTouchHotAreas01
 * @tc.desc: GetTouchHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetTouchHotAreas01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "GetTouchHotAreas01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    Rect windowRect = {1, 1, 1, 1};
    std::vector<Rect> rects;
    uint32_t hotAreasNum = 10;
    uint32_t hotAreaWidth = windowRect.width_ / hotAreasNum;
    uint32_t hotAreaHeight = windowRect.height_ / hotAreasNum;
    for (uint32_t i = 0; i < hotAreasNum; ++i) {
        rects.emplace_back(Rect{hotAreaWidth * i, hotAreaHeight * i, hotAreaWidth, hotAreaHeight});
    }
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();

    ASSERT_NE(nullptr, property);
    property->SetTouchHotAreas(rects);
    ASSERT_NE(rects, sceneSession->GetTouchHotAreas());
}

/**
 * @tc.name: SetTurnScreenOn01
 * @tc.desc: SetTurnScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetTurnScreenOn01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetTurnScreenOn(false));
    ASSERT_EQ(false, sceneSession->IsTurnScreenOn());
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetTurnScreenOn(true));
    ASSERT_EQ(true, sceneSession->IsTurnScreenOn());
}

/**
 * @tc.name: UpdateWindowAnimationFlag01
 * @tc.desc: UpdateWindowAnimationFlag
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateWindowAnimationFlag01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    ASSERT_EQ(WSError::WS_OK, sceneSession->UpdateWindowAnimationFlag(false));
}

/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetRequestedOrientation01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "SetRequestedOrientation";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    Orientation ori = Orientation::UNSPECIFIED;
    sceneSession->SetRequestedOrientation(ori);
    Orientation ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ori, ret);

    sceneSession->SetRequestedOrientation(Orientation::AUTO_ROTATION_UNSPECIFIED);
    Orientation ret1 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret1, Orientation::AUTO_ROTATION_UNSPECIFIED);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    Orientation ret2 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret2, Orientation::USER_ROTATION_PORTRAIT);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE);
    Orientation ret3 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret3, Orientation::USER_ROTATION_LANDSCAPE);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED);
    Orientation ret4 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret4, Orientation::USER_ROTATION_PORTRAIT_INVERTED);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED);
    Orientation ret5 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret5, Orientation::USER_ROTATION_LANDSCAPE_INVERTED);

    sceneSession->SetRequestedOrientation(Orientation::FOLLOW_DESKTOP);
    Orientation ret6 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret6, Orientation::FOLLOW_DESKTOP);
}

/**
 * @tc.name: GetRequestedOrientation
 * @tc.desc: GetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetRequestedOrientation, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "GetRequestedOrientation";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    Orientation ori = Orientation::HORIZONTAL;
    sceneSession->SetRequestedOrientation(ori);
    Orientation ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ori, ret);

    sceneSession->SetRequestedOrientation(Orientation::AUTO_ROTATION_UNSPECIFIED);
    Orientation ret1 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret1, Orientation::AUTO_ROTATION_UNSPECIFIED);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    Orientation ret2 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret2, Orientation::USER_ROTATION_PORTRAIT);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE);
    Orientation ret3 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret3, Orientation::USER_ROTATION_LANDSCAPE);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED);
    Orientation ret4 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret4, Orientation::USER_ROTATION_PORTRAIT_INVERTED);

    sceneSession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED);
    Orientation ret5 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret5, Orientation::USER_ROTATION_LANDSCAPE_INVERTED);

    sceneSession->SetRequestedOrientation(Orientation::FOLLOW_DESKTOP);
    Orientation ret6 = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(ret6, Orientation::FOLLOW_DESKTOP);
}

/**
 * @tc.name: SetDefaultRequestedOrientation
 * @tc.desc: SetDefaultRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetDefaultRequestedOrientation, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "SetDefaultRequestedOrientation";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    Orientation orientation = Orientation::AUTO_ROTATION_UNSPECIFIED;
    sceneSession->SetDefaultRequestedOrientation(orientation);
    Orientation ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(orientation, ret);
}


/**
 * @tc.name: IsKeepScreenOn
 * @tc.desc: IsKeepScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsKeepScreenOn, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsKeepScreenOn";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    ASSERT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetKeepScreenOn(true));
    ASSERT_EQ(true, sceneSession->IsKeepScreenOn());
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetKeepScreenOn(false));
    ASSERT_EQ(false, sceneSession->IsKeepScreenOn());
}

/**
 * @tc.name: IsAppSession
 * @tc.desc: IsAppSession true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsAppSession01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsAppSession";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(true, sceneSession->IsAppSession());
}

/**
 * @tc.name: IsAppSession
 * @tc.desc: IsAppSession false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsAppSession02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsAppSession";
    info.windowType_ = 2106;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(false, sceneSession->IsAppSession());

    SessionInfo parentInfo;
    parentInfo.abilityName_ = "testSession1";
    parentInfo.moduleName_ = "testSession2";
    parentInfo.bundleName_ = "testSession3";
    sptr<Session> parentSession = sptr<Session>::MakeSptr(parentInfo);
    ASSERT_NE(parentSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    parentSession->SetSessionProperty(property);
    sceneSession->SetParentSession(parentSession);
    ASSERT_EQ(false, sceneSession->IsAppSession());

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    parentSession->SetSessionProperty(property);
    sceneSession->SetParentSession(parentSession);
    ASSERT_EQ(true, sceneSession->IsAppSession());
}

/**
 * @tc.name: IsAppOrLowerSystemSession
 * @tc.desc: IsAppOrLowerSystemSession true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsAppOrLowerSystemSession01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsAppOrLowerSystemSession01";
    info.windowType_ = 2126;

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(true, sceneSession->IsAppOrLowerSystemSession());
}

/**
 * @tc.name: IsAppOrLowerSystemSession
 * @tc.desc: IsAppOrLowerSystemSession false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsAppOrLowerSystemSession02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background02";
    info.bundleName_ = "IsAppOrLowerSystemSession02";
    info.windowType_ = 2106;

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(false, sceneSession->IsAppOrLowerSystemSession());

    SessionInfo parentInfo;
    parentInfo.abilityName_ = "testSession1";
    parentInfo.moduleName_ = "testSession2";
    parentInfo.bundleName_ = "testSession3";
    sptr<Session> parentSession = sptr<Session>::MakeSptr(parentInfo);
    ASSERT_NE(parentSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    parentSession->SetSessionProperty(property);
    sceneSession->SetParentSession(parentSession);
    ASSERT_EQ(false, sceneSession->IsAppOrLowerSystemSession());

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    parentSession->SetSessionProperty(property);
    sceneSession->SetParentSession(parentSession);
    ASSERT_EQ(true, sceneSession->IsAppOrLowerSystemSession());
}

/**
 * @tc.name: IsSystemSessionAboveApp
 * @tc.desc: IsSystemSessionAboveApp true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsSystemSessionAboveApp01, Function | SmallTest | Level2)
{
    SessionInfo info1;
    info1.abilityName_ = "HighZOrder01";
    info1.bundleName_ = "IsSystemSessionAboveApp01";
    info1.windowType_ = 2122;

    sptr<SceneSession> sceneSession1;
    sceneSession1 = new (std::nothrow) SceneSession(info1, nullptr);
    EXPECT_NE(sceneSession1, nullptr);
    ASSERT_EQ(true, sceneSession1->IsSystemSessionAboveApp());

    SessionInfo info2;
    info2.abilityName_ = "HighZOrder02";
    info2.bundleName_ = "IsSystemSessionAboveApp02";
    info2.windowType_ = 2104;

    sptr<SceneSession> sceneSession2;
    sceneSession2 = new (std::nothrow) SceneSession(info2, nullptr);
    EXPECT_NE(sceneSession2, nullptr);
    ASSERT_EQ(true, sceneSession2->IsSystemSessionAboveApp());

    SessionInfo info3;
    info3.abilityName_ = "HighZOrder03";
    info3.bundleName_ = "SCBDropdownPanel13";
    info3.windowType_ = 2109;

    sptr<SceneSession> sceneSession3;
    sceneSession3 = new (std::nothrow) SceneSession(info3, nullptr);
    EXPECT_NE(sceneSession3, nullptr);
    ASSERT_EQ(true, sceneSession3->IsSystemSessionAboveApp());

    SessionInfo info4;
    info4.abilityName_ = "HighZOrder04";
    info4.bundleName_ = "IsSystemSessionAboveApp04";
    info4.windowType_ = 2109;

    sptr<SceneSession> sceneSession4;
    sceneSession4 = new (std::nothrow) SceneSession(info4, nullptr);
    EXPECT_NE(sceneSession4, nullptr);
    ASSERT_EQ(false, sceneSession4->IsSystemSessionAboveApp());
}

/**
 * @tc.name: IsSystemSessionAboveApp
 * @tc.desc: IsSystemSessionAboveApp false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsSystemSessionAboveApp02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HighZOrder05";
    info.bundleName_ = "IsSystemSessionAboveApp05";
    info.windowType_ = 1;

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(false, sceneSession->IsSystemSessionAboveApp());
}

/**
 * @tc.name: GetWindowName
 * @tc.desc: GetWindowName
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetWindowName, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "GetWindowName";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_NE("ww", sceneSession->GetWindowName());
}

/**
 * @tc.name: IsDecorEnable
 * @tc.desc: IsDecorEnable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsDecorEnable, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsDecorEnable";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(true, sceneSession->IsDecorEnable());
    SessionInfo info_;
    info_.abilityName_ = "Background01";
    info_.bundleName_ = "IsDecorEnable";
    info_.windowType_ = 1000;
    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info_, nullptr);
    EXPECT_NE(sceneSession1, nullptr);
    ASSERT_EQ(false, sceneSession1->IsDecorEnable());
}

/**
 * @tc.name: IsDecorEnable01
 * @tc.desc: IsDecorEnable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsDecorEnable01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsDecorEnable01";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetDecorEnable(true);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->property_ = property;
    ASSERT_EQ(true, sceneSession->IsDecorEnable());

    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession1, nullptr);
    property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetDecorEnable(false);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(true, sceneSession1->IsDecorEnable());

    sceneSession1->SetSessionProperty(nullptr);
    ASSERT_EQ(false, sceneSession1->IsDecorEnable());
}

/**
 * @tc.name: UpdateNativeVisibility
 * @tc.desc: UpdateNativeVisibility
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateNativeVisibility, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "UpdateNativeVisibility";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->UpdateNativeVisibility(false);
    ASSERT_EQ(false, sceneSession->IsVisible());
    sceneSession->NotifyWindowVisibility();

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->NotifyWindowVisibility();
}

/**
 * @tc.name: SetPrivacyMode01
 * @tc.desc: Set PrivacyMode as false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetPrivacyMode01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "SetPrivacyMode";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    sceneSession->surfaceNode_ = surfaceNode;
    sceneSession->SetPrivacyMode(false);
    ASSERT_EQ(false, sceneSession->property_->GetPrivacyMode());
    ASSERT_EQ(false, sceneSession->property_->GetSystemPrivacyMode());
}

/**
 * @tc.name: SetPrivacyMode02
 * @tc.desc: Set PrivacyMode as true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetPrivacyMode02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background02";
    info.bundleName_ = "SetPrivacyMode";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    sceneSession->surfaceNode_ = surfaceNode;
    sceneSession->SetPrivacyMode(true);
    ASSERT_EQ(true, sceneSession->property_->GetPrivacyMode());
    ASSERT_EQ(true, sceneSession->property_->GetSystemPrivacyMode());
}

/**
 * @tc.name: IsFloatingWindowAppType
 * @tc.desc: IsFloatingWindowAppType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, IsFloatingWindowAppType, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(false, sceneSession->IsFloatingWindowAppType());

    sceneSession->SetSessionProperty(nullptr);
    ASSERT_EQ(false, sceneSession->IsFloatingWindowAppType());
}

/**
 * @tc.name: DumpSessionElementInfo01
 * @tc.desc: DumpSessionElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, DumpSessionElementInfo, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    std::vector<std::string> params;
    sceneSession->DumpSessionElementInfo(params);
    int ret = 1;
    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->DumpSessionElementInfo(params);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: SaveAspectRatio
 * @tc.desc: SaveAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SaveAspectRatio, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(true, sceneSession->SaveAspectRatio(0.1));

    sceneSession->sessionInfo_.bundleName_ = "";
    sceneSession->sessionInfo_.moduleName_ = "";
    sceneSession->sessionInfo_.abilityName_ = "";
    ASSERT_EQ(false, sceneSession->SaveAspectRatio(0.1));
}

/**
 * @tc.name: NotifyIsCustomAnimationPlaying
 * @tc.desc: NotifyIsCustomAnimationPlaying
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, NotifyIsCustomAnimationPlaying, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->NotifyIsCustomAnimationPlaying(false);

    sceneSession->onIsCustomAnimationPlaying_ = [](bool status) {};
    sceneSession->NotifyIsCustomAnimationPlaying(false);
}

/**
 * @tc.name: ModalUIExtension
 * @tc.desc: ModalUIExtension
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, ModalUIExtension, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ModalUIExtension";
    info.bundleName_ = "ModalUIExtension";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    EXPECT_FALSE(sceneSession->HasModalUIExtension());
    ExtensionWindowEventInfo extensionInfo;
    extensionInfo.persistentId = 12345;
    extensionInfo.pid = 1234;
    extensionInfo.windowRect = { 1, 2, 3, 4 };
    sceneSession->AddModalUIExtension(extensionInfo);
    EXPECT_TRUE(sceneSession->HasModalUIExtension());

    auto getInfo = sceneSession->GetLastModalUIExtensionEventInfo();
    EXPECT_EQ(getInfo.persistentId, extensionInfo.persistentId);
    EXPECT_EQ(getInfo.pid, extensionInfo.pid);
    EXPECT_EQ(getInfo.windowRect, extensionInfo.windowRect);

    Rect windowRect = { 5, 6, 7, 8 };
    extensionInfo.windowRect = windowRect;
    sceneSession->UpdateModalUIExtension(extensionInfo);
    getInfo = sceneSession->GetLastModalUIExtensionEventInfo();
    EXPECT_EQ(getInfo.windowRect, windowRect);

    sceneSession->RemoveModalUIExtension(extensionInfo.persistentId);
    EXPECT_FALSE(sceneSession->HasModalUIExtension());
}

/**
 * @tc.name: NotifySessionRectChange
 * @tc.desc: NotifySessionRectChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, NotifySessionRectChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    WSRect overlapRect = { 0, 0, 0, 0 };
    sceneSession->NotifySessionRectChange(overlapRect, SizeChangeReason::ROTATION, -1);
    sceneSession->NotifySessionRectChange(overlapRect, SizeChangeReason::ROTATION, 11);
    sceneSession->sessionRectChangeFunc_ = [](const WSRect& rect,
        const SizeChangeReason reason, DisplayId displayId) {
        return;
    };
    sceneSession->NotifySessionRectChange(overlapRect, SizeChangeReason::ROTATION, -1);
    sceneSession->NotifySessionRectChange(overlapRect, SizeChangeReason::ROTATION, 11);
}

/**
 * @tc.name: FixRectByAspectRatio
 * @tc.desc: FixRectByAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, FixRectByAspectRatio, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    WSRect originalRect_ = { 0, 0, 0, 0 };
    ASSERT_EQ(false, sceneSession->FixRectByAspectRatio(originalRect_));
}

/**
 * @tc.name: GetKeyboardAvoidArea
 * @tc.desc: GetKeyboardAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetKeyboardAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    specificCallback_->onGetSceneSessionVectorByType_ = [](WindowType type,
        uint64_t displayId) -> std::vector<sptr<SceneSession>> {
        std::vector<sptr<SceneSession>> backgroundSession;
        return backgroundSession;
    };

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    WSRect overlapRect = {0, 0, 0, 0};
    AvoidArea avoidArea;
    int ret = 1;
    sceneSession->GetKeyboardAvoidArea(overlapRect, avoidArea);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: GetCutoutAvoidArea
 * @tc.desc: GetCutoutAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetCutoutAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    WSRect overlapRect = { 0, 0, 0, 0 };
    AvoidArea avoidArea;
    int ret = 1;
    sceneSession->GetCutoutAvoidArea(overlapRect, avoidArea);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: SetSystemBarProperty
 * @tc.desc: SetSystemBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetSystemBarProperty, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->property_ = nullptr;
    SystemBarProperty statusBarProperty;
    sceneSession->SetSystemBarProperty(WindowType::WINDOW_TYPE_FLOAT_CAMERA, statusBarProperty);
    ASSERT_EQ(sceneSession->SetSystemBarProperty(WindowType::WINDOW_TYPE_FLOAT_CAMERA, statusBarProperty),
              WSError::WS_ERROR_NULLPTR);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->property_ = property;
    ASSERT_EQ(sceneSession->SetSystemBarProperty(WindowType::WINDOW_TYPE_FLOAT_CAMERA, statusBarProperty),
              WSError::WS_OK);
}

/**
 * @tc.name: OnShowWhenLocked
 * @tc.desc: OnShowWhenLocked
 * @tc.type: FUNC ok
 */
HWTEST_F(SceneSessionTest, OnShowWhenLocked, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    int ret = 0;
    sceneSession->OnShowWhenLocked(false);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: IsShowWhenLocked
 * @tc.desc: IsShowWhenLocked
 * @tc.type: FUNC ok
 */
HWTEST_F(SceneSessionTest, IsShowWhenLocked, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(sceneSession->IsShowWhenLocked(), false);
    sceneSession->property_ = property;
    sceneSession->SetTemporarilyShowWhenLocked(true);
    ASSERT_EQ(sceneSession->IsShowWhenLocked(), true);
    property->SetWindowFlags(4);
    sceneSession->SetTemporarilyShowWhenLocked(false);
    ASSERT_EQ(sceneSession->IsShowWhenLocked(), true);
    sceneSession->SetTemporarilyShowWhenLocked(true);
    ASSERT_EQ(sceneSession->IsShowWhenLocked(), true);
}

/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: GetAvoidAreaByType
 * @tc.type: FUNC ok
 */
HWTEST_F(SceneSessionTest, GetAvoidAreaByType, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
        specificCallback_->onGetSceneSessionVectorByType_ = [](WindowType type,
            uint64_t displayId)-> std::vector<sptr<SceneSession>>
    {
        SessionInfo info_;
        info_.abilityName_ = "Background01";
        info_.bundleName_ = "IsFloatingWindowAppType";
        std::vector<sptr<SceneSession>> backgroundSession;
        sptr<SceneSession> session2=new (std::nothrow) SceneSession(info_, nullptr);
        backgroundSession.push_back(session2);
        return backgroundSession;
    };
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    sceneSession->SetSessionRect(rect);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->property_ = property;
    AvoidArea avoidArea;
    sceneSession->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT);
    sceneSession->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM);
    sceneSession->GetAvoidAreaByType(AvoidAreaType::TYPE_KEYBOARD);
    sceneSession->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM_GESTURE);
    EXPECT_NE(sceneSession, nullptr);
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, TransferPointerEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent), WSError::WS_ERROR_NULLPTR);
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ =  MMI::PointerEvent::Create();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetPersistentId(11);
    sceneSession->property_ = property;
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent_), WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: TransferPointerEventDecorDialog
 * @tc.desc: TransferPointerEventDecorDialog
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, TransferPointerEventDecorDialog, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TransferPointerEventDecorDialog";
    info.bundleName_ = "TransferPointerEventDecorDialogBundle";
    info.windowType_ = 2122;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    sptr<SceneSession> sceneSession =
        new (std::nothrow) SceneSession(info, specificCallback_);
    sceneSession->moveDragController_ = new MoveDragController(12);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ =  MMI::PointerEvent::Create();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    property->SetDecorEnable(true);
    property->SetDragEnabled(true);
    property->SetPersistentId(12);
    sceneSession->property_ = property;
    EXPECT_NE(sceneSession, nullptr);
}

/**
 * @tc.name: TransferPointerEventSystemDialog
 * @tc.desc: TransferPointerEventSystemDialog
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, TransferPointerEventSystemDialog, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TransferPointerEventSystemDialog";
    info.bundleName_ = "TransferPointerEventSystemDialogBundle";
    info.windowType_ = 2123;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    sptr<SceneSession> sceneSession =
        new (std::nothrow) SceneSession(info, specificCallback_);
    sceneSession->moveDragController_ = new MoveDragController(12);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ =  MMI::PointerEvent::Create();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    property->SetDecorEnable(true);
    property->SetDragEnabled(true);
    property->SetPersistentId(13);
    sceneSession->property_ = property;
    EXPECT_NE(sceneSession, nullptr);
}

/**
 * @tc.name: CalculateAvoidAreaRect
 * @tc.desc: CalculateAvoidAreaRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, CalculateAvoidAreaRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    int ret = 0;
    WSRect overlapRect = { 0, 0, 0, 0 };
    WSRect avoidRect = { 0, 0, 0, 0 };
    AvoidArea avoidArea;
    sceneSession->CalculateAvoidAreaRect(overlapRect, avoidRect, avoidArea);
    WSRect overlapRect_ = { 1, 1, 1, 1 };
    WSRect avoidRect_ = { 1, 1, 1, 1 };
    sceneSession->CalculateAvoidAreaRect(overlapRect_, avoidRect_, avoidArea);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: OnNeedAvoid
 * @tc.desc: OnNeedAvoid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, OnNeedAvoid, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(sceneSession->OnNeedAvoid(false), WSError::WS_OK);

    sceneSession->onNeedAvoid_ = [](bool state) {};
    ASSERT_EQ(sceneSession->OnNeedAvoid(false), WSError::WS_OK);
}

/**
 * @tc.name: SetCollaboratorType
 * @tc.desc: SetCollaboratorType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetCollaboratorType, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetCollaboratorType(2);
    ASSERT_EQ(sceneSession->GetCollaboratorType(), 2);
}

/**
 * @tc.name: GetAbilityInfo
 * @tc.desc: GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetAbilityInfo, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo;
    sceneSession->SetAbilitySessionInfo(abilityInfo);
    ASSERT_EQ(sceneSession->GetAbilityInfo(), abilityInfo);
}

/**
 * @tc.name: UpdateCameraWindowStatus
 * @tc.desc: UpdateCameraWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateCameraWindowStatus, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "UpdateCameraWindowStatus";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SystemSession> sysSession;
    sysSession = new (std::nothrow) SystemSession(info, specificCallback_);
    EXPECT_NE(sysSession, nullptr);
    int ret = 1;
    specificCallback_->onCameraFloatSessionChange_ = [](uint32_t accessTokenId, bool isShowing) {};
    specificCallback_->onCameraSessionChange_ = [](uint32_t accessTokenId, bool isShowing) {};

    sysSession->UpdateCameraWindowStatus(false);
    sysSession = new (std::nothrow) SystemSession(info, specificCallback_);
    sysSession->UpdateCameraWindowStatus(false);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sysSession->property_ = property;
    sysSession->UpdateCameraWindowStatus(false);
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    sysSession->property_ = property;
    sysSession->UpdateCameraWindowStatus(false);
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    PiPTemplateInfo pipType;
    pipType.pipTemplateType = static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL);
    sysSession->SetPiPTemplateInfo(pipType);
    sysSession->property_ = property;
    sysSession->UpdateCameraWindowStatus(false);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: GetRatioPreferenceKey
 * @tc.desc: GetRatioPreferenceKey
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetRatioPreferenceKey, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ability";
    info.bundleName_ = "bundle";
    info.moduleName_ = "module";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    std::string key = info.bundleName_ + info.moduleName_ + info.abilityName_;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    ASSERT_EQ(key, sceneSession->GetRatioPreferenceKey());

    std::string key2(30, 'a');
    std::string key3(80, 'a');
    sceneSession->sessionInfo_.bundleName_ = key2;
    sceneSession->sessionInfo_.moduleName_ = key2;
    sceneSession->sessionInfo_.abilityName_ = key2;
    ASSERT_EQ(key3, sceneSession->GetRatioPreferenceKey());
}

/**
 * @tc.name: NotifyPropertyWhenConnect
 * @tc.desc: NotifyPropertyWhenConnect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, NotifyPropertyWhenConnect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ability";
    info.bundleName_ = "bundle";
    info.moduleName_ = "module";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    int ret = 1;
    std::string key = info.bundleName_ + info.moduleName_ + info.abilityName_;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    sceneSession->NotifyPropertyWhenConnect();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->property_ = property;
    sceneSession->NotifyPropertyWhenConnect();
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: DumpSessionInfo
 * @tc.desc: DumpSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, DumpSessionInfo, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.bundleName_ = "SceneSessionTest";
    info.abilityName_ = "DumpSessionInfo";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    std::vector<std::string> infos;
    sceneSession->DumpSessionInfo(infos);
    ASSERT_FALSE(infos.empty());
}

/**
 * @tc.name: OnSessionEvent
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, OnSessionEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnSessionEvent";
    info.bundleName_ = "OnSessionEvent";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->moveDragController_ = new MoveDragController(1);
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    sceneSession->OnSessionEvent(SessionEvent::EVENT_START_MOVE);
    sceneSession->moveDragController_->isStartDrag_ = true;
    sceneSession->moveDragController_->hasPointDown_ = true;
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(sceneSession->sessionChangeCallback_, nullptr);
    ASSERT_EQ(sceneSession->OnSessionEvent(SessionEvent::EVENT_START_MOVE), WSError::WS_OK);
    ASSERT_EQ(sceneSession->OnSessionEvent(SessionEvent::EVENT_END_MOVE), WSError::WS_OK);
}

/**
 * @tc.name: SyncSessionEvent
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SyncSessionEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SyncSessionEvent";
    info.bundleName_ = "SyncSessionEvent";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    property->isSystemCalling_ = true;
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = false;

    SessionEvent event = SessionEvent::EVENT_START_MOVE;
    auto result = sceneSession->SyncSessionEvent(event);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    property->isSystemCalling_ = false;
    result = sceneSession->SyncSessionEvent(event);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: OnTitleAndDockHoverShowChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, OnTitleAndDockHoverShowChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnTitleAndDockHoverShowChange";
    info.bundleName_ = "OnTitleAndDockHoverShowChange";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession> sceneSession = sptr<MainSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->OnTitleAndDockHoverShowChange(true, true);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetTopmost
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetTopmost, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetTopmost";
    info.bundleName_ = "SetTopmost";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession> sceneSession = new (std::nothrow) MainSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetTopmost(false);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_FALSE(sceneSession->IsTopmost());
}

/**
 * @tc.name: SetMainWindowTopmost
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetMainWindowTopmost, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetMainWindowTopmost";
    info.bundleName_ = "SetMainWindowTopmost";
    sptr<SceneSession> sceneSession = new (std::nothrow) MainSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetMainWindowTopmost(false);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_FALSE(sceneSession->IsMainWindowTopmost());
}

/**
 * @tc.name: SetAspectRatio2
 * @tc.desc: test for aspectRatio NearZero
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio2";
    info.bundleName_ = "SetAspectRatio2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.0001;
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession->GetAspectRatio(), ratio);
}

/**
 * @tc.name: SetAspectRatio3
 * @tc.desc: test for aspectRatio is smaller than minWidth/maxHeight
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio3, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio3";
    info.bundleName_ = "SetAspectRatio3";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 2.5;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxWidth_ = 3000;
    limits.maxHeight_ = 3000;
    limits.minWidth_ = 2000;
    limits.minHeight_ = 2000;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio4
 * @tc.desc: test for aspectRatio is smaller than minWidth/maxHeight
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio4, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio4";
    info.bundleName_ = "SetAspectRatio4";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxWidth_ = 3000;
    limits.maxHeight_ = 3000;
    limits.minWidth_ = 2000;
    limits.minHeight_ = 2000;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio5
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio5, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio5";
    info.bundleName_ = "SetAspectRatio5";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetAspectRatio(ratio);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio6
 * @tc.desc: test for sessionProperty is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio6, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio6";
    info.bundleName_ = "SetAspectRatio6";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;
    sceneSession->SetSessionProperty(nullptr);

    float ratio = 0.1;
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: SetAspectRatio7
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio7, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio7";
    info.bundleName_ = "SetAspectRatio7";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 10;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: UpdateRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect";
    info.bundleName_ = "UpdateRect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = sceneSession->UpdateRect(rect, reason, "SceneSessionTest");
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateInputMethodSessionRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateInputMethodSessionRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateInputMethodSessionRect";
    info.bundleName_ = "UpdateInputMethodSessionRect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    WSRect newWinRect;
    WSRect newRequestRect;

    sceneSession->UpdateInputMethodSessionRect(rect, newWinRect, newRequestRect);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->SetSessionProperty(nullptr);
    auto res = sceneSession->UpdateInputMethodSessionRect(rect, newWinRect, newRequestRect);
    ASSERT_EQ(res, false);

    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;
    sceneSession->SetSessionProperty(property);
    res = sceneSession->UpdateInputMethodSessionRect(rect, newWinRect, newRequestRect);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: UpdateSessionRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateSessionRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateSessionRect";
    info.bundleName_ = "UpdateSessionRect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::MOVE;
    WSError result = sceneSession->UpdateSessionRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateSessionRect1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateSessionRect1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateSessionRect";
    info.bundleName_ = "UpdateSessionRect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::RESIZE;
    WSError result = sceneSession->UpdateSessionRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateSessionRect2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateSessionRect2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateSessionRect";
    info.bundleName_ = "UpdateSessionRect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = sceneSession->UpdateSessionRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateSessionRect3
 * @tc.desc: test for isGlobal is true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, UpdateSessionRect3, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateSessionRect";
    info.bundleName_ = "UpdateSessionRect";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    sceneSession->SetSessionProperty(property);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSRect oldRect({1, 1, 1, 1});
    WSRect parentRect({10, 10, 1, 1});

    sptr<SceneSession> parentSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetParentSession(parentSession);
    EXPECT_NE(sceneSession->GetParentSession(), nullptr);
    parentSession->SetSessionRect(parentRect);
    sceneSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;

    bool isGlobal = true;
    WSError result = sceneSession->UpdateSessionRect(oldRect, reason, isGlobal);
    ASSERT_EQ(result, WSError::WS_OK);

    WSRect newRect = sceneSession->GetSessionRect();
    ASSERT_EQ(newRect.posX_, oldRect.posX_ - parentRect.posX_);
    ASSERT_EQ(newRect.posY_, oldRect.posY_ - parentRect.posY_);
}

/**
 * @tc.name: GetStatusBarHeight
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetStatusBarHeight, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetStatusBarHeight";
    info.bundleName_ = "GetStatusBarHeight";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    int32_t height = sceneSession->GetStatusBarHeight();
    ASSERT_EQ(height, 0);
    SystemBarProperty propertyHide;
    propertyHide.enable_ = false;
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
        propertyHide));
    ASSERT_EQ(height, 0);
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    height = sceneSession->GetStatusBarHeight();
    ASSERT_EQ(height, 0);
    WSRect rect({0, 0, 0, 1});
    sceneSession->winRect_ = rect;
    specificCallback_->onGetSceneSessionVectorByType_ = [&](WindowType type,
        uint64_t displayId)->std::vector<sptr<SceneSession>>
    {
        std::vector<sptr<SceneSession>> vec;
        vec.push_back(sceneSession);
        return vec;
    };
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    sceneSession->property_ = property;
    height = sceneSession->GetStatusBarHeight();
    ASSERT_EQ(height, 1);
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
        propertyHide));
    ASSERT_EQ(height, 1);
}

/**
 * @tc.name: GetAppForceLandscapeConfig
 * @tc.desc: GetAppForceLandscapeConfig
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetAppForceLandscapeConfig, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetAppForceLandscapeConfig";
    info.bundleName_ = "GetAppForceLandscapeConfig";
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    AppForceLandscapeConfig config = {};
    auto result = sceneSession->GetAppForceLandscapeConfig(config);
    ASSERT_EQ(result, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: HandleCompatibleModeMoveDrag
 * @tc.desc: HandleCompatibleModeMoveDrag
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, HandleCompatibleModeMoveDrag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleCompatibleModeMoveDrag";
    info.bundleName_ = "HandleCompatibleModeMoveDrag";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    WSRect rect = {1, 1, 1, 1};
    WSRect rect2 = {1, 1, 2, 1};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::HIDE, true);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::HIDE);

    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::HIDE, false);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::HIDE);

    rect2 = {1, 1, 1, 2};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::HIDE, true);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::HIDE);

    rect = {1, 1, 2000, 1};
    rect2 = {1, 1, 2, 1};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::HIDE, true);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::HIDE);

    rect = {1, 1, 2000, 1};
    rect2 = {1, 1, 1, 2};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::HIDE, true);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::HIDE);

    rect = {1, 1, 500, 1};
    rect2 = {1, 1, 1, 2};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::HIDE, true);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::HIDE);

    rect = {1, 1, 500, 1};
    rect2 = {1, 1, 1, 2};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::HIDE, false);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::HIDE);

    sceneSession->HandleCompatibleModeMoveDrag(rect, SizeChangeReason::MOVE, false);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::MOVE);
}

/**
 * @tc.name: HandleCompatibleModeDrag
 * @tc.desc: HandleCompatibleModeDrag
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, HandleCompatibleModeDrag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleCompatibleModeDrag";
    info.bundleName_ = "HandleCompatibleModeDrag";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    WSRect rect = {1, 1, 1, 1};
    WSRect rect2 = {2, 1, 1, 1};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeDrag(rect, SizeChangeReason::MOVE, false, false, false);
    ASSERT_EQ(sceneSession->winRect_, rect2);

    rect2 = {1, 2, 1, 1};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeDrag(rect, SizeChangeReason::MOVE, false, false, false);
    ASSERT_EQ(sceneSession->winRect_, rect2);

    rect2 = {1, 1, 2, 1};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeDrag(rect, SizeChangeReason::MOVE, false, false, false);
    ASSERT_EQ(sceneSession->winRect_, rect2);

    rect2 = {1, 1, 1, 2};
    sceneSession->winRect_ = rect2;
    sceneSession->HandleCompatibleModeDrag(rect, SizeChangeReason::MOVE, false, false, false);
    ASSERT_EQ(sceneSession->winRect_, rect2);
}

/**
 * @tc.name: SetMoveDragCallback
 * @tc.desc: SetMoveDragCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetMoveDragCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetMoveDragCallback";
    info.bundleName_ = "SetMoveDragCallback";
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->moveDragController_ = nullptr;
    sceneSession->SetMoveDragCallback();
}

/**
 * @tc.name: GetScreenWidthAndHeightFromServer
 * @tc.desc: GetScreenWidthAndHeightFromServer
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetScreenWidthAndHeightFromServer, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetScreenWidthAndHeightFromServer";
    info.bundleName_ = "GetScreenWidthAndHeightFromServer";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    sceneSession->SetSessionProperty(property);

    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    bool result = sceneSession->GetScreenWidthAndHeightFromServer(property, screenWidth, screenHeight);
    ASSERT_EQ(result, true);

    sceneSession->SetSessionProperty(nullptr);
    result = sceneSession->GetScreenWidthAndHeightFromServer(property, screenWidth, screenHeight);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: SetDefaultDisplayIdIfNeed
 * @tc.desc: SetDefaultDisplayIdIfNeed
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetDefaultDisplayIdIfNeed, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetDefaultDisplayIdIfNeed();

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetDisplayId(-99);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetDefaultDisplayIdIfNeed();
    EXPECT_EQ(property->GetDisplayId(), SCREEN_ID_INVALID);
}

/**
 * @tc.name: SetSessionGlobalRect/GetSessionGlobalRect
 * @tc.desc: SetSessionGlobalRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetSessionGlobalRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    WSRect test = { 100, 100, 100, 100 };
    sceneSession->SetSessionGlobalRect(test);
    sceneSession->SetScbCoreEnabled(true);
    EXPECT_EQ(test, sceneSession->GetSessionGlobalRect());
}

/**
 * @tc.name: SetSessionGlobalRect/GetSessionGlobalRect
 * @tc.desc: SetSessionGlobalRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetIsStatusBarVisibleInner01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetIsStatusBarVisibleInner01";
    info.bundleName_ = "SetIsStatusBarVisibleInner01";
    info.windowType_ = 1;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCallback);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isStatusBarVisible_ = true;
    EXPECT_EQ(sceneSession->SetIsStatusBarVisibleInner(true), WSError::WS_OK);
    EXPECT_EQ(sceneSession->SetIsStatusBarVisibleInner(false), WSError::WS_ERROR_NULLPTR);

    sceneSession->isLastFrameLayoutFinishedFunc_ = [](bool& isLayoutFinished) {
        return WSError::WS_ERROR_NULLPTR;
    };
    EXPECT_EQ(sceneSession->SetIsStatusBarVisibleInner(true), WSError::WS_ERROR_NULLPTR);

    sceneSession->isLastFrameLayoutFinishedFunc_ = [](bool& isLayoutFinished) {
        isLayoutFinished = false;
        return WSError::WS_OK;
    };
    EXPECT_EQ(sceneSession->SetIsStatusBarVisibleInner(false), WSError::WS_OK);

    sceneSession->isLastFrameLayoutFinishedFunc_ = [](bool& isLayoutFinished) {
        isLayoutFinished = true;
        return WSError::WS_OK;
    };
    sceneSession->specificCallback_->onUpdateAvoidAreaByType_ = [](int32_t persistentId, AvoidAreaType type) {};
    EXPECT_EQ(sceneSession->SetIsStatusBarVisibleInner(true), WSError::WS_OK);

    sceneSession->specificCallback_ = nullptr;
    EXPECT_EQ(sceneSession->SetIsStatusBarVisibleInner(false), WSError::WS_OK);
}
} // namespace
} // Rosen
} // OHOS
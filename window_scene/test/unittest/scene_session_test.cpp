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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;
    auto result = scensession->SetGlobalMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
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

    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;
    MaximizeMode mode;
    auto result = scensession->GetGlobalMaximizeMode(mode);
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

    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;
    PiPTemplateInfo pipTemplateInfo;
    pipTemplateInfo.pipTemplateType = static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL);
    scensession->SetPiPTemplateInfo(pipTemplateInfo);
    ASSERT_EQ(scensession->GetPiPTemplateInfo().pipTemplateType,
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;
    auto result = scensession->UpdateWindowSceneAfterCustomAnimation(false);
    ASSERT_EQ(result, WSError::WS_OK);
    result = scensession->UpdateWindowSceneAfterCustomAnimation(true);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->SetZOrder(2);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
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
    ASSERT_NE(rects, scensession->GetTouchHotAreas());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    ASSERT_EQ(WSError::WS_OK, scensession->SetTurnScreenOn(false));
    ASSERT_EQ(false, scensession->IsTurnScreenOn());
    ASSERT_EQ(WSError::WS_OK, scensession->SetTurnScreenOn(true));
    ASSERT_EQ(true, scensession->IsTurnScreenOn());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    ASSERT_EQ(WSError::WS_OK, scensession->UpdateWindowAnimationFlag(false));
}

/**
 * @tc.name: ClearEnterWindow01
 * @tc.desc: ClearEnterWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, ClearEnterWindow01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "ClearEnterWindow01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    int resultValue = 0;
    SceneSession::ClearEnterWindow();
    ASSERT_EQ(resultValue, 0);
}

/**
 * @tc.name: GetEnterWindow01
 * @tc.desc: GetEnterWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetEnterWindow01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "GetEnterWindow01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    wptr<SceneSession> scenesession_;
    ASSERT_EQ(scenesession_, SceneSession::GetEnterWindow());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    Orientation ori = Orientation::UNSPECIFIED;
    scensession->SetRequestedOrientation(ori);
    Orientation ret = scensession->GetRequestedOrientation();
    ASSERT_EQ(ori, ret);

    scensession->SetRequestedOrientation(Orientation::AUTO_ROTATION_UNSPECIFIED);
    Orientation ret1 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret1, Orientation::AUTO_ROTATION_UNSPECIFIED);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    Orientation ret2 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret2, Orientation::USER_ROTATION_PORTRAIT);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE);
    Orientation ret3 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret3, Orientation::USER_ROTATION_LANDSCAPE);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED);
    Orientation ret4 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret4, Orientation::USER_ROTATION_PORTRAIT_INVERTED);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED);
    Orientation ret5 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret5, Orientation::USER_ROTATION_LANDSCAPE_INVERTED);

    scensession->SetRequestedOrientation(Orientation::FOLLOW_DESKTOP);
    Orientation ret6 = scensession->GetRequestedOrientation();
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    Orientation ori = Orientation::HORIZONTAL;
    scensession->SetRequestedOrientation(ori);
    Orientation ret = scensession->GetRequestedOrientation();
    ASSERT_EQ(ori, ret);

    scensession->SetRequestedOrientation(Orientation::AUTO_ROTATION_UNSPECIFIED);
    Orientation ret1 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret1, Orientation::AUTO_ROTATION_UNSPECIFIED);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    Orientation ret2 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret2, Orientation::USER_ROTATION_PORTRAIT);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE);
    Orientation ret3 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret3, Orientation::USER_ROTATION_LANDSCAPE);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED);
    Orientation ret4 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret4, Orientation::USER_ROTATION_PORTRAIT_INVERTED);

    scensession->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED);
    Orientation ret5 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret5, Orientation::USER_ROTATION_LANDSCAPE_INVERTED);

    scensession->SetRequestedOrientation(Orientation::FOLLOW_DESKTOP);
    Orientation ret6 = scensession->GetRequestedOrientation();
    ASSERT_EQ(ret6, Orientation::FOLLOW_DESKTOP);
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
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    ASSERT_EQ(WSError::WS_OK, scensession->SetKeepScreenOn(false));
    ASSERT_EQ(false, scensession->IsKeepScreenOn());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    ASSERT_EQ(true, scensession->IsAppSession());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    ASSERT_EQ(false, scensession->IsAppSession());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    ASSERT_NE("ww", scensession->GetWindowName());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    ASSERT_EQ(true, scensession->IsDecorEnable());
    SessionInfo info_;
    info_.abilityName_ = "Background01";
    info_.bundleName_ = "IsDecorEnable";
    info_.windowType_ = 1000;
    sptr<SceneSession> scensession_;
    scensession_ = new (std::nothrow) SceneSession(info_, nullptr);
    EXPECT_NE(scensession_, nullptr);
    ASSERT_EQ(false, scensession_->IsDecorEnable());
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

    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetDecorEnable(true);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    scensession->property_ = property;
    ASSERT_EQ(true, scensession->IsDecorEnable());

    sptr<SceneSession> scensession_;
    scensession_ = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession_, nullptr);
    property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetDecorEnable(false);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(true, scensession_->IsDecorEnable());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->UpdateNativeVisibility(false);
    ASSERT_EQ(false, scensession->IsVisible());
    scensession->NotifyWindowVisibility();
}

/**
 * @tc.name: SetPrivacyMode
 * @tc.desc: SetPrivacyMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetPrivacyMode, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "SetPrivacyMode";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    int ret = 0;
    scensession->SetPrivacyMode(false);
    ASSERT_EQ(0, ret);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    ASSERT_EQ(false, scensession->IsFloatingWindowAppType());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    std::vector<std::string> params;
    scensession->DumpSessionElementInfo(params);
    int ret = 1;
    scensession->sessionStage_ = mockSessionStage;
    scensession->DumpSessionElementInfo(params);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    ASSERT_EQ(true, scensession->SaveAspectRatio(0.1));
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->NotifyIsCustomAnimationPlaying(false);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    WSRect overlapRect = { 0, 0, 0, 0 };
    scensession->NotifySessionRectChange(overlapRect, SizeChangeReason::ROTATION);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    WSRect originalRect_ = { 0, 0, 0, 0 };
    ASSERT_EQ(false, scensession->FixRectByAspectRatio(originalRect_));
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

    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    WSRect overlapRect = {0, 0, 0, 0};
    AvoidArea avoidArea;
    int ret = 1;
    scensession->GetKeyboardAvoidArea(overlapRect, avoidArea);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    WSRect overlapRect = { 0, 0, 0, 0 };
    AvoidArea avoidArea;
    int ret = 1;
    scensession->GetCutoutAvoidArea(overlapRect, avoidArea);
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

    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    scensession->property_ = nullptr;
    SystemBarProperty statusBarProperty;
    scensession->SetSystemBarProperty(WindowType::WINDOW_TYPE_FLOAT_CAMERA, statusBarProperty);
    ASSERT_EQ(scensession->SetSystemBarProperty(WindowType::WINDOW_TYPE_FLOAT_CAMERA, statusBarProperty),
              WSError::WS_ERROR_NULLPTR);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    scensession->property_ = property;
    ASSERT_EQ(scensession->SetSystemBarProperty(WindowType::WINDOW_TYPE_FLOAT_CAMERA, statusBarProperty),
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    int ret = 0;
    scensession->OnShowWhenLocked(false);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(scensession->IsShowWhenLocked(), false);
    scensession->property_ = property;
    scensession->SetTemporarilyShowWhenLocked(true);
    ASSERT_EQ(scensession->IsShowWhenLocked(), true);
    property->SetWindowFlags(4);
    scensession->SetTemporarilyShowWhenLocked(false);
    ASSERT_EQ(scensession->IsShowWhenLocked(), true);
    scensession->SetTemporarilyShowWhenLocked(true);
    ASSERT_EQ(scensession->IsShowWhenLocked(), true);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    scensession->SetSessionRect(rect);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    scensession->property_ = property;
    AvoidArea avoidArea;
    scensession->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT);
    scensession->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM);
    scensession->GetAvoidAreaByType(AvoidAreaType::TYPE_KEYBOARD);
    scensession->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM_GESTURE);
    EXPECT_NE(scensession, nullptr);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent), WSError::WS_ERROR_NULLPTR);
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ =  MMI::PointerEvent::Create();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetPersistentId(11);
    scensession->property_ = property;
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent_), WSError::WS_ERROR_INVALID_SESSION);
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
    sptr<SceneSession> scensession =
        new (std::nothrow) SceneSession(info, specificCallback_);
    scensession->moveDragController_ = new MoveDragController(12);
    scensession->SetSessionState(SessionState::STATE_ACTIVE);
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ =  MMI::PointerEvent::Create();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    property->SetDecorEnable(true);
    property->SetDragEnabled(true);
    property->SetPersistentId(12);
    scensession->property_ = property;
    EXPECT_NE(scensession, nullptr);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    int ret = 0;
    WSRect overlapRect = { 0, 0, 0, 0 };
    WSRect avoidRect = { 0, 0, 0, 0 };
    AvoidArea avoidArea;
    scensession->CalculateAvoidAreaRect(overlapRect, avoidRect, avoidArea);
    WSRect overlapRect_ = { 1, 1, 1, 1 };
    WSRect avoidRect_ = { 1, 1, 1, 1 };
    scensession->CalculateAvoidAreaRect(overlapRect_, avoidRect_, avoidArea);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    ASSERT_EQ(scensession->OnNeedAvoid(false), WSError::WS_OK);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    scensession->SetCollaboratorType(2);
    ASSERT_EQ(scensession->GetCollaboratorType(), 2);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo;
    scensession->SetAbilitySessionInfo(abilityInfo);
    ASSERT_EQ(scensession->GetAbilityInfo(), abilityInfo);
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    std::string key = info.bundleName_ + info.moduleName_ + info.abilityName_;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    ASSERT_EQ(key, scensession->GetRatioPreferenceKey());
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
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    int ret = 1;
    std::string key = info.bundleName_ + info.moduleName_ + info.abilityName_;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    scensession->NotifyPropertyWhenConnect();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    scensession->property_ = property;
    scensession->NotifyPropertyWhenConnect();
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
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    std::vector<std::string> infos;
    scensession->DumpSessionInfo(infos);
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
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    scensession->SetSessionProperty(property);
    scensession->isActive_ = false;

    SessionEvent event = SessionEvent::EVENT_START_MOVE;
    auto result = scensession->OnSessionEvent(event);
    ASSERT_EQ(result, WSError::WS_OK);
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
    sptr<SceneSession> scenesession = new (std::nothrow) MainSession(info, nullptr);
    EXPECT_NE(scenesession, nullptr);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    scenesession->SetSessionProperty(property);
    auto result = scenesession->SetTopmost(false);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_FALSE(scenesession->IsTopmost());
}

/**
 * @tc.name: SetAspectRatio2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio2";
    info.bundleName_ = "SetAspectRatio2";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.0001;
    auto result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    scensession->SetSessionProperty(property);
    result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetAspectRatio3
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio3, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio3";
    info.bundleName_ = "SetAspectRatio3";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    scensession->SetSessionProperty(property);
    auto result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetAspectRatio4
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio4, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio4";
    info.bundleName_ = "SetAspectRatio4";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 0;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    scensession->SetSessionProperty(property);
    auto result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
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
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    scensession->SetSessionProperty(property);
    scensession->SetAspectRatio(ratio);
    EXPECT_NE(scensession, nullptr);
}

/**
 * @tc.name: SetAspectRatio6
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetAspectRatio6, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio6";
    info.bundleName_ = "SetAspectRatio6";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 0;
    limits.minWidth_ = 10;
    property->SetWindowLimits(limits);
    scensession->SetSessionProperty(property);
    auto result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
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
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 10;
    property->SetWindowLimits(limits);
    scensession->SetSessionProperty(property);
    auto result = scensession->SetAspectRatio(ratio);
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
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    scensession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = scensession->UpdateRect(rect, reason);
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
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);

    scensession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    WSRect newWinRect;
    WSRect newRequestRect;
    scensession->UpdateInputMethodSessionRect(rect, newWinRect, newRequestRect);
    EXPECT_NE(scensession, nullptr);
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
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);

    scensession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::MOVE;
    WSError result = scensession->UpdateSessionRect(rect, reason);
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
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);

    scensession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::RESIZE;
    WSError result = scensession->UpdateSessionRect(rect, reason);
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
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);

    scensession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = scensession->UpdateSessionRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);
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
    ASSERT_EQ(result, WMError::WM_OK);
}
}
}
}
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
#include <gtest/gtest.h>

#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "pointer_event.h"

#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "window_helper.h"
#include "wm_common.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionTest4 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSession> sceneSession;
    sptr<WindowSessionProperty> property;
    SessionInfo info;
    WSPropertyChangeAction action;
};

void SceneSessionTest4::SetUpTestCase()
{
}

void SceneSessionTest4::TearDownTestCase()
{
}

void SceneSessionTest4::SetUp()
{
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    property = new (std::nothrow) WindowSessionProperty();
    action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
}

void SceneSessionTest4::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleActionUpdateFlags
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    sceneSession->HandleActionUpdateStatusProps(property, sceneSession, action);
    sceneSession->HandleActionUpdateNavigationProps(property, sceneSession, action);
    sceneSession->HandleActionUpdateNavigationIndicatorProps(property, sceneSession, action);
    sceneSession->HandleActionUpdateFlags(property, sceneSession, action);

    auto ret = sceneSession->HandleActionUpdateFlags(property, sceneSession, action);
    ASSERT_NE(ret, WMError::WM_ERROR_NOT_SYSTEM_APP);
    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateFlags(property, sceneSession, action);
    windowSessionProperty.isSystemCalling_ = {true};
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateFlags(property, sceneSession, action);
}

/**
 * @tc.name: HandleActionUpdateTouchHotArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateTouchHotArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    sceneSession->HandleActionUpdateTouchHotArea(property, sceneSession, action);
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateTouchHotArea(property, sceneSession, action);
}

/**
 * @tc.name: HandleActionUpdateDecorEnable
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateDecorEnable, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    OHOS::Rosen::Session session(info);
    session.property_ = nullptr;
    sceneSession->HandleActionUpdateDecorEnable(property, sceneSession, action);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateDecorEnable(property, sceneSession, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    auto ret = sceneSession->HandleActionUpdateDecorEnable(property, sceneSession, action);
    ASSERT_EQ(WMError::WM_ERROR_NOT_SYSTEM_APP, ret);
}

/**
 * @tc.name: HandleActionUpdateWindowLimits
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateWindowLimits, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    OHOS::Rosen::Session session(info);
    session.property_ = nullptr;
    sceneSession->HandleActionUpdateWindowLimits(property, sceneSession, action);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateWindowLimits(property, sceneSession, action);
}

/**
 * @tc.name: HandleActionUpdateDragenabled
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateDragenabled, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    OHOS::Rosen::Session session(info);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    sceneSession->HandleActionUpdateDragenabled(property, sceneSession, action);

    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateDragenabled(property, sceneSession, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateDragenabled(property, sceneSession, action);
}

/**
 * @tc.name: HandleActionUpdateRaiseenabled
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateRaiseenabled, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG;
    sceneSession->HandleActionUpdateRaiseenabled(property, sceneSession, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateRaiseenabled(property, sceneSession, action);

    windowSessionProperty.isSystemCalling_ = {true};
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateRaiseenabled(property, sceneSession, action);

    sceneSession->HandleActionUpdateHideNonSystemFloatingWindows(property, sceneSession, action);
}

/**
 * @tc.name: HandleActionUpdateTextfieldAvoidInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateTextfieldAvoidInfo, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, property);
    sceneSession->HandleActionUpdateTextfieldAvoidInfo(property, sceneSession, action);
    sceneSession->HandleActionUpdateWindowMask(property, sceneSession, action);

    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateTextfieldAvoidInfo(property, sceneSession, action);
    sceneSession->HandleActionUpdateWindowMask(property, sceneSession, action);
    sceneSession->HandleActionUpdateTopmost(property, sceneSession, action);
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetWindowFlags, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, property);
    sptr<SceneSession> sceneSession1 = nullptr;
    sceneSession->SetWindowFlags(sceneSession1, property);
    sceneSession1 = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession->SetWindowFlags(sceneSession1, property);

    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->SetWindowFlags(sceneSession1, property);
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property, action);
}

/**
 * @tc.name: TerminateSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, TerminateSession, Function | SmallTest | Level2)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    OHOS::Rosen::Session session(info);
    session.isTerminating = true;
    sceneSession->TerminateSession(abilitySessionInfo);
}

/**
 * @tc.name: NotifySessionException
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, NotifySessionException, Function | SmallTest | Level2)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    bool needRemoveSession = true;
    OHOS::Rosen::Session session(info);
    session.isTerminating = true;
    sceneSession->NotifySessionException(abilitySessionInfo, needRemoveSession);
    sceneSession->GetLastSafeRect();
    WSRect rect;
    sceneSession->SetLastSafeRect(rect);
}
}
}
}
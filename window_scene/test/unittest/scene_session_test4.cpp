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
};

void SceneSessionTest4::SetUpTestCase()
{
}

void SceneSessionTest4::TearDownTestCase()
{
}

void SceneSessionTest4::SetUp()
{
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
    sptr<SceneSession> sceneSession =new (std::nothrow) SceneSession(info,nullptr);
    ASSERT_NE(nullptr,sceneSession);
    WSPropertyChangeAction action =WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    sceneSession->HandleActionUpdateStatusProps(nullptr,sceneSession,action);
    sceneSession->HandleActionUpdateNavigationProps(nullptr,sceneSession,action);
    sceneSession->HandleActionUpdateNavigationIndicatorProps(nullptr,sceneSession,action);
    sceneSession->HandleActionUpdateFlags(nullptr,sceneSession,action);

    auto ret=sceneSession->HandleActionUpdateFlags(nullptr,sceneSession,action);
    ASSERT_EQ(ret,WMError::WM_ERROR_NOT_SYSTEM_APP);
    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_={true};
    sceneSession->HandleActionUpdateFlags(nullptr,sceneSession,action);
    windowSessionProperty.isSystemCalling_={true};
    OHOS::Rosen::Session session(info);
    session.property_=new WindowSessionProperty();
    sceneSession->HandleActionUpdateFlags(nullptr,sceneSession,action);
}

/**
 * @tc.name: HandleActionUpdateTouchHotArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateTouchHotArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession =new (std::nothrow) SceneSession(info,nullptr);
    ASSERT_NE(nullptr,sceneSession);
    WSPropertyChangeAction action=WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    sceneSession->HandleActionUpdateTouchHotArea(nullptr,sceneSession,action);
    OHOS::Rosen::Session session(info);
    session.property_=new WindowSessionProperty();
    sceneSession->HandleActionUpdateTouchHotArea(nullptr,sceneSession,action);
}

/**
 * @tc.name: HandleActionUpdateDecorEnable
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateDecorEnable, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession =new (std::nothrow) SceneSession(info,nullptr);
    ASSERT_NE(nullptr,sceneSession);
    WSPropertyChangeAction action=WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;

    sptr<WindowSessionProperty> property =nullptr;
    OHOS::Rosen::Session session(info);
    session.property_=nullptr;
    sceneSession->HandleActionUpdateDecorEnable(property,sceneSession,action);
    session.property_ =new WindowSessionProperty();
    sceneSession->HandleActionUpdateDecorEnable(property,sceneSession,action);

    property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    auto ret = sceneSession->HandleActionUpdateDecorEnable(property,sceneSession,action);
    ASSERT_EQ(WMError::WM_ERROR_NOT_SYSTEM_APP,ret);
}

/**
 * @tc.name: HandleActionUpdateWindowLimits
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateWindowLimits, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession =new (std::nothrow) SceneSession(info,nullptr);
    ASSERT_NE(nullptr,sceneSession);
    WSPropertyChangeAction action=WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    OHOS::Rosen::Session session(info);
    session.property_=nullptr;
    sceneSession->HandleActionUpdateWindowLimits(nullptr,sceneSession,action);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateWindowLimits(nullptr,sceneSession,action);
}

/**
 * @tc.name: HandleActionUpdateDragenabled
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateDragenabled, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession =new (std::nothrow) SceneSession(info,nullptr);
    ASSERT_NE(nullptr,sceneSession);
    WSPropertyChangeAction action=WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    OHOS::Rosen::Session session(info);
    sptr<WindowSessionProperty>  property=new WindowSessionProperty();
    sceneSession->HandleActionUpdateDragenabled(property,sceneSession,action);

    session.property_=new WindowSessionProperty();
    sceneSession->HandleActionUpdateDragenabled(property,sceneSession,action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_={true};
    sceneSession->HandleActionUpdateDragenabled(property,sceneSession,action);
}
}
}
}
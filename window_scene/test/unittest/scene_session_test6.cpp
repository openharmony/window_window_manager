/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <pointer_event.h>

#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"


#include "screen_manager.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/system_session.h"
#include <ui/rs_surface_node.h>
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class SceneSessionTest6 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest6::SetUpTestCase()
{
}

void SceneSessionTest6::TearDownTestCase()
{
}

void SceneSessionTest6::SetUp()
{
}

void SceneSessionTest6::TearDown()
{
}

namespace {

/**
 * @tc.name: RegisterNotifySurfaceBoundsChangeFunc
 * @tc.desc: RegisterNotifySurfaceBoundsChangeFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, RegisterNotifySurfaceBoundsChangeFunc01, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    constexpr int sessionId = 10001;
    sceneSession->RegisterNotifySurfaceBoundsChangeFunc(sessionId, nullptr);
    ASSERT_EQ(nullptr, sceneSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);

    auto task = [](const WSRect& rect, bool isGlobal, bool needFlush) {};
    sceneSession->RegisterNotifySurfaceBoundsChangeFunc(sessionId, std::move(task));
    ASSERT_NE(nullptr, sceneSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);

    sceneSession->UnregisterNotifySurfaceBoundsChangeFunc(sessionId);
    ASSERT_EQ(nullptr, sceneSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);
}

/**
 * @tc.name: NotifyUpdateGravity
 * @tc.desc: NotifyUpdateGravity
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, NotifyUpdateGravity01, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> subSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    int32_t subSessionId = subSession->GetPersistentId();
    
    sptr<MainSession> mainSession = sptr<MainSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, mainSession);

    constexpr int sessionId = 10001;
    auto task = [](const WSRect& rect, bool isGlobal, bool needFlush) {};
    mainSession->RegisterNotifySurfaceBoundsChangeFunc(sessionId, std::move(task));
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);

    subSession->isFollowParentLayout_ = false;
    mainSession->RegisterNotifySurfaceBoundsChangeFunc(subSessionId, std::move(task));
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    subSession->isFollowParentLayout_ = true;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    sptr<MoveDragController> followController =
        sptr<MoveDragController>::MakeSptr(subSessionId, subSession->GetWindowType());
    ASSERT_NE(nullptr, followController);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    ASSERT_NE(nullptr, surfaceNode);
    subSession->surfaceNode_ = nullptr;
    subSession->moveDragController_ = nullptr;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    subSession->surfaceNode_ = surfaceNode;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    subSession->moveDragController_ = followController;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);
}

/**
 * @tc.name: GetSceneSessionById
 * @tc.desc: GetSceneSessionById
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, GetSceneSessionById01, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> findSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    sceneSession->specificCallback_ = nullptr;
    sptr<SceneSession> ret = sceneSession->GetSceneSessionById(findSession->GetPersistentId());
    ASSERT_EQ(nullptr, ret);

    sptr<SceneSession::SpecificSessionCallback> callBack = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(nullptr, callBack);
    sceneSession->specificCallback_ = callBack;
    ret = sceneSession->GetSceneSessionById(findSession->GetPersistentId());
    ASSERT_EQ(nullptr, ret);

    auto task = [&findSession](int32_t persistentId) {
        return findSession;
    };
    callBack->onGetSceneSessionByIdCallback_ = task;
    ret = sceneSession->GetSceneSessionById(findSession->GetPersistentId());
    ASSERT_EQ(findSession->GetPersistentId(), ret->GetPersistentId());
}

/**
 * @tc.name: SetFollowParentRectFunc
 * @tc.desc: SetFollowParentRectFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentRectFunc01, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    
    sceneSession->SetFollowParentRectFunc(nullptr);
    ASSERT_EQ(nullptr, sceneSession->followParentRectFunc_);

    NotifyFollowParentRectFunc func = [](bool isFollow) {};
    sceneSession->SetFollowParentRectFunc(std::move(func));
    ASSERT_NE(nullptr, sceneSession->followParentRectFunc_);
}

/**
 * @tc.name: SetFollowParentWindowLayoutEnabled
 * @tc.desc: SetFollowParentWindowLayoutEnabled01, check the param
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentWindowLayoutEnabled01, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    WSError ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    sceneSession->property_ = property;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);

    property->subWindowLevel_ = 100;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);
    
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);

    property->subWindowLevel_ = 1;
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: SetFollowParentWindowLayoutEnabled
 * @tc.desc: SetFollowParentWindowLayoutEnabled02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentWindowLayoutEnabled02, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    property->subWindowLevel_ = 1;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->property_ = property;
    // test set isFollowParentLayout_
    sceneSession->isFollowParentLayout_ = false;
    sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_TRUE(sceneSession->isFollowParentLayout_);
    //test after set flag, call func
    bool isCall = false;
    NotifyFollowParentRectFunc func = [&isCall](bool isFollow) {
        isCall = true;
    };
    sceneSession->SetFollowParentRectFunc(std::move(func));
    ASSERT_NE(nullptr, sceneSession->followParentRectFunc_);
    sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_TRUE(isCall);
}

/**
 * @tc.name: SetFollowParentWindowLayoutEnabled
 * @tc.desc: SetFollowParentWindowLayoutEnabled03, test register callback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentWindowLayoutEnabled03, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->subWindowLevel_ = 1;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->property_ = property;

    sptr<MainSession> parentSession = sptr<MainSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, parentSession);

    sceneSession->parentSession_ = parentSession;
    sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_NE(nullptr, parentSession->notifySurfaceBoundsChangeFuncMap_[sceneSession->GetPersistentId()]);
    WSRect rect;
    parentSession->NotifySubAndDialogFollowRectChange(rect, false, false);

    sceneSession->SetFollowParentWindowLayoutEnabled(false);
    ASSERT_EQ(nullptr, parentSession->notifySurfaceBoundsChangeFuncMap_[sceneSession->GetPersistentId()]);
}

/**
 * @tc.name: GetSystemAvoidArea
 * @tc.desc: GetSystemAvoidArea function
 * @tc.type: FUNC
 */
 HWTEST_F(SceneSessionTest6, GetSystemAvoidArea, Function | SmallTest | Level1)
 {
    SessionInfo info;
    info.abilityName_ = "GetSystemAvoidArea";
    info.bundleName_ = "GetSystemAvoidArea";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(session, nullptr);
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    session->GetSessionProperty()->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_EQ(WindowMode::WINDOW_MODE_FLOATING, session->GetSessionProperty()->GetWindowMode());
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);

    SystemSessionConfig systemConfig;
    systemConfig.windowUIType_ = WindowUIType::PHONE_WINDOW;
    session->SetSystemConfig(systemConfig);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    session->GetSessionProperty()->SetDisplayId(2025);
    session->SetIsMidScene(false);
    EXPECT_EQ(session->GetIsMidScene(), false);

    WSRect rect;
    AvoidArea avoidArea;
    session->GetSystemAvoidArea(rect, avoidArea);
    int32_t height = session->GetStatusBarHeight();
    EXPECT_EQ(height, avoidArea.topRect_.height_);
}

/**
 * @tc.name: RegisterUpdateAppUseControlCallback Test
 * @tc.desc: RegisterUpdateAppUseControlCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, RegisterUpdateAppUseControlCallback, Function | SmallTest | Level3)
{
    EXPECT_EQ(0, SceneSession::GetAllAppUseControlMap().size());
    SceneSession::ControlInfo controlInfo = {
        .isNeedControl = true,
        .isControlRecentOnly = true
    };
    SessionInfo info;
    info.bundleName_ = "app";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    auto callback = [](ControlAppType type, bool isNeedControl, bool isControlRecentOnly) {};
    sceneSession->RegisterUpdateAppUseControlCallback(callback);
 
    std::unordered_map<std::string, std::unordered_map<ControlAppType, SceneSession::ControlInfo>>&
        allAppUseMap = sceneSession->GetAllAppUseControlMap();
    std::string key = "app#0";
    allAppUseMap[key][ControlAppType::APP_LOCK] = controlInfo;
    sceneSession->RegisterUpdateAppUseControlCallback(callback);
    ASSERT_NE(nullptr, sceneSession->onUpdateAppUseControlFunc_);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
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

#include <gtest/gtest.h>
#include <regex>
#include <pointer_event.h>
#include <ui/rs_surface_node.h>

#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "mock/mock_pattern_detach_callback.h"
#include "session/host/include/extension_session.h"
#include "session/host/include/move_drag_controller.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "session/host/include/session.h"
#include "session_info.h"
#include "key_event.h"
#include "wm_common.h"
#include "window_event_channel_base.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string UNDEFINED = "undefined";
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowSessionTest"};
}

class WindowSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    WSError TransferSearchElementInfo(bool isChannelNull);
    WSError TransferSearchElementInfosByText(bool isChannelNull);
    WSError TransferFindFocusedElementInfo(bool isChannelNull);
    WSError TransferFocusMoveSearch(bool isChannelNull);
    WSError TransferExecuteAction(bool isChannelNull);
    int32_t GetTaskCount();
    sptr<SceneSessionManager> ssm_;

private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    sptr<Session> session_ = nullptr;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
};

void WindowSessionTest::SetUpTestCase()
{
}

void WindowSessionTest::TearDownTestCase()
{
}

void WindowSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    session_ = new (std::nothrow) Session(info);
    session_->surfaceNode_ = CreateRSSurfaceNode();
    EXPECT_NE(nullptr, session_);
    ssm_ = new SceneSessionManager();
    session_->SetEventHandler(ssm_->taskScheduler_->GetEventHandler(), ssm_->eventHandler_);
    auto isScreenLockedCallback = [this]() {
        return ssm_->IsScreenLocked();
    };
    session_->RegisterIsScreenLockedCallback(isScreenLockedCallback);
}

void WindowSessionTest::TearDown()
{
    session_ = nullptr;
    usleep(WAIT_SYNC_IN_NS);
}

RSSurfaceNode::SharedPtr WindowSessionTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (surfaceNode == nullptr) {
        GTEST_LOG_(INFO) << "WindowSessionTest::CreateRSSurfaceNode surfaceNode is nullptr";
    }
    return surfaceNode;
}


WSError WindowSessionTest::TransferSearchElementInfo(bool isChannelNull)
{
    int64_t elementId = 0;
    int32_t mode = 0;
    int64_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;
    if (isChannelNull) {
        return session_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
    } else {
        session_->windowEventChannel_ = new TestWindowEventChannel();
        return session_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
    }
}

WSError WindowSessionTest::TransferSearchElementInfosByText(bool isChannelNull)
{
    int64_t elementId = 0;
    std::string text;
    int64_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;
    if (isChannelNull) {
        return session_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    } else {
        session_->windowEventChannel_ = new TestWindowEventChannel();
        return session_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    }
}

WSError WindowSessionTest::TransferFindFocusedElementInfo(bool isChannelNull)
{
    int64_t elementId = 0;
    int32_t focusType = 0;
    int64_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;
    if (isChannelNull) {
        return session_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    } else {
        session_->windowEventChannel_ = new TestWindowEventChannel();
        return session_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    }
}

WSError WindowSessionTest::TransferFocusMoveSearch(bool isChannelNull)
{
    int64_t elementId = 0;
    int32_t direction = 0;
    int64_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;
    if (isChannelNull) {
        return session_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
    } else {
        session_->windowEventChannel_ = new TestWindowEventChannel();
        return session_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
    }
}

WSError WindowSessionTest::TransferExecuteAction(bool isChannelNull)
{
    int64_t elementId = 0;
    std::map<std::string, std::string> actionArguments;
    int32_t action = 0;
    int64_t baseParent = 0;
    if (isChannelNull) {
        return session_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    } else {
        session_->windowEventChannel_ = new TestWindowEventChannel();
        return session_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    }
}

int32_t WindowSessionTest::GetTaskCount()
{
    std::string dumpInfo = session_->handler_->GetEventRunner()->GetEventQueue()->DumpCurrentQueueSize();
    std::regex pattern("\\d+");
    std::smatch matches;
    int32_t taskNum = 0;
    while (std::regex_search(dumpInfo, matches, pattern)) {
        taskNum += std::stoi(matches.str());
        dumpInfo = matches.suffix();
    }
    return taskNum;
}

namespace {
/**
 * @tc.name: TransferSearchElementInfo01
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferSearchElementInfo01, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfo01 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_OK, TransferSearchElementInfo(false));
    WLOGFI("TransferSearchElementInfo01 end!");
}

/**
 * @tc.name: TransferSearchElementInfo02
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferSearchElementInfo02, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfo02 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, TransferSearchElementInfo(true));
    WLOGFI("TransferSearchElementInfo02 end!");
}

/**
 * @tc.name: TransferSearchElementInfosByText01
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferSearchElementInfosByText01, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfosByText01 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_OK, TransferSearchElementInfosByText(false));
    WLOGFI("TransferSearchElementInfosByText01 end!");
}

/**
 * @tc.name: TransferSearchElementInfosByText02
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferSearchElementInfosByText02, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfosByText02 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, TransferSearchElementInfosByText(true));
    WLOGFI("TransferSearchElementInfosByText02 end!");
}

/**
 * @tc.name: TransferFindFocusedElementInfo01
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferFindFocusedElementInfo01, Function | SmallTest | Level2)
{
    WLOGFI("TransferFindFocusedElementInfo01 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_OK, TransferFindFocusedElementInfo(false));
    WLOGFI("TransferFindFocusedElementInfo01 end!");
}

/**
 * @tc.name: TransferFindFocusedElementInfo02
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferFindFocusedElementInfo02, Function | SmallTest | Level2)
{
    WLOGFI("TransferFindFocusedElementInfo02 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, TransferFindFocusedElementInfo(true));
    WLOGFI("TransferFindFocusedElementInfo02 end!");
}

/**
 * @tc.name: TransferFocusMoveSearch01
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferFocusMoveSearch01, Function | SmallTest | Level2)
{
    WLOGFI("TransferFocusMoveSearch01 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_OK, TransferFocusMoveSearch(false));
    WLOGFI("TransferFocusMoveSearch01 end!");
}

/**
 * @tc.name: TransferFocusMoveSearch02
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferFocusMoveSearch02, Function | SmallTest | Level2)
{
    WLOGFI("TransferFocusMoveSearch02 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, TransferFocusMoveSearch(true));
    WLOGFI("TransferFocusMoveSearch02 end!");
}

/**
 * @tc.name: TransferExecuteAction01
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferExecuteAction01, Function | SmallTest | Level2)
{
    WLOGFI("TransferExecuteAction01 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_OK, TransferExecuteAction(false));
    WLOGFI("TransferExecuteAction01 end!");
}

/**
 * @tc.name: TransferExecuteAction02
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferExecuteAction02, Function | SmallTest | Level2)
{
    WLOGFI("TransferExecuteAction02 begin!");
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, TransferExecuteAction(true));
    WLOGFI("TransferExecuteAction02 end!");
}

/**
 * @tc.name: SetForceTouchable
 * @tc.desc: SetForceTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetForceTouchable, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool touchable = false;
    session_->SetForceTouchable(touchable);
    ASSERT_EQ(session_->forceTouchable_, touchable);
}

/**
 * @tc.name: SetActive01
 * @tc.desc: set session active
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(WindowSessionTest, SetActive01, Function | SmallTest | Level2)
{
    sptr<ISession> sessionToken = nullptr;
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    EXPECT_CALL(*(mockSessionStage), SetActive(_)).WillOnce(Return(WSError::WS_OK));
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _, _)).Times(0).WillOnce(Return(WSError::WS_OK));
    session_->sessionStage_ = mockSessionStage;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->SetActive(true));

    sptr<WindowEventChannelMocker> mockEventChannel = new(std::nothrow) WindowEventChannelMocker(mockSessionStage);
    EXPECT_NE(nullptr, mockEventChannel);
    auto surfaceNode = CreateRSSurfaceNode();
    SystemSessionConfig sessionConfig;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    ASSERT_EQ(WSError::WS_OK, session_->Connect(mockSessionStage,
            mockEventChannel, surfaceNode, sessionConfig, property));
    ASSERT_EQ(WSError::WS_OK, session_->SetActive(true));
    ASSERT_EQ(false, session_->isActive_);

    session_->UpdateSessionState(SessionState::STATE_FOREGROUND);
    ASSERT_EQ(WSError::WS_OK, session_->SetActive(true));
    ASSERT_EQ(true, session_->isActive_);
}

/**
 * @tc.name: UpdateRect01
 * @tc.desc: update rect
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(WindowSessionTest, UpdateRect01, Function | SmallTest | Level2)
{
    sptr<ISession> sessionToken = nullptr;
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session_->sessionStage_ = mockSessionStage;
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _, _)).Times(0).WillOnce(Return(WSError::WS_OK));

    WSRect rect = {0, 0, 0, 0};
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->UpdateRect(rect, SizeChangeReason::UNDEFINED));
    sptr<WindowEventChannelMocker> mockEventChannel = new(std::nothrow) WindowEventChannelMocker(mockSessionStage);
    EXPECT_NE(nullptr, mockEventChannel);
    SystemSessionConfig sessionConfig;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    ASSERT_EQ(WSError::WS_OK, session_->Connect(mockSessionStage,
            mockEventChannel, nullptr, sessionConfig, property));

    rect = {0, 0, 100, 100};
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _, _)).Times(1).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, session_->UpdateRect(rect, SizeChangeReason::UNDEFINED));
    ASSERT_EQ(rect, session_->winRect_);
}

/**
 * @tc.name: IsSessionValid01
 * @tc.desc: check func IsSessionValid
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, IsSessionValid01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_FALSE(session_->IsSessionValid());
    session_->state_ = SessionState::STATE_CONNECT;
    ASSERT_TRUE(session_->IsSessionValid());
}

/**
 * @tc.name: SetSessionProperty01
 * @tc.desc: SetSessionProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetSessionProperty01, Function | SmallTest | Level2)
{
    ASSERT_EQ(session_->SetSessionProperty(nullptr), WSError::WS_OK);
}

/**
 * @tc.name: Connect01
 * @tc.desc: check func Connect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Connect01, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();
    session_->state_ = SessionState::STATE_CONNECT;
    SystemSessionConfig systemConfig;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    auto result = session_->Connect(nullptr, nullptr, nullptr, systemConfig, property);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    session_->state_ = SessionState::STATE_DISCONNECT;
    result = session_->Connect(nullptr, nullptr, nullptr, systemConfig, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    result = session_->Connect(mockSessionStage, nullptr, surfaceNode, systemConfig, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new(std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    result = session_->Connect(mockSessionStage, testWindowEventChannel, surfaceNode, systemConfig, property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Reconnect01
 * @tc.desc: check func Reconnect01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Reconnect01, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    auto result = session_->Reconnect(nullptr, nullptr, nullptr, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    result = session_->Reconnect(mockSessionStage, nullptr, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new (std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    result = session_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = session_->Reconnect(nullptr, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = session_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Foreground01
 * @tc.desc: check func Foreground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Foreground01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_DISCONNECT;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    auto result = session_->Foreground(property);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    session_->state_ = SessionState::STATE_CONNECT;
    session_->isActive_ = true;
    result = session_->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    session_->isActive_ = false;
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Background01
 * @tc.desc: check func Background
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Background01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_CONNECT;
    auto result = session_->Background();
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    session_->state_ = SessionState::STATE_INACTIVE;
    result = session_->Background();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(session_->state_, SessionState::STATE_BACKGROUND);
}

/**
 * @tc.name: Disconnect01
 * @tc.desc: check func Disconnect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Disconnect01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_CONNECT;
    auto result = session_->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(session_->state_, SessionState::STATE_DISCONNECT);

    session_->state_ = SessionState::STATE_BACKGROUND;
    result = session_->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(session_->state_, SessionState::STATE_DISCONNECT);
}

/**
 * @tc.name: TerminateSessionNew01
 * @tc.desc: check func TerminateSessionNew
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TerminateSessionNew01, Function | SmallTest | Level2)
{
    int resultValue = 0;
    NotifyTerminateSessionFuncNew callback =
        [&resultValue](const SessionInfo& info, bool needStartCaller, bool isFromBroker) {
        resultValue = 1;
    };

    bool needStartCaller = false;
    bool isFromBroker = false;
    sptr<AAFwk::SessionInfo> info = new (std::nothrow)AAFwk::SessionInfo();
    session_->terminateSessionFuncNew_ = nullptr;
    session_->TerminateSessionNew(info, needStartCaller, isFromBroker);
    ASSERT_EQ(resultValue, 0);

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION,
              session_->TerminateSessionNew(nullptr, needStartCaller, isFromBroker));
}

/**
 * @tc.name: TerminateSessionNew02
 * @tc.desc: terminateSessionFuncNew_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TerminateSessionNew02, Function | SmallTest | Level2)
{
    int res = 0;
    int resultValue = 0;
    NotifyTerminateSessionFuncNew callback =
        [&resultValue](const SessionInfo& info, bool needStartCaller, bool isFromBroker) {
        resultValue = 1;
    };

    bool needStartCaller = true;
    bool isFromBroker = true;
    sptr<AAFwk::SessionInfo> info = new (std::nothrow)AAFwk::SessionInfo();
    session_->SetTerminateSessionListenerNew(callback);
    session_->TerminateSessionNew(info, needStartCaller, isFromBroker);
    res++;
    ASSERT_EQ(res, 1);
}

/**
 * @tc.name: SetSessionRect
 * @tc.desc: check func SetSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetSessionRect, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    session_->SetSessionRect(rect);
    ASSERT_EQ(rect, session_->winRect_);
}

/**
 * @tc.name: GetSessionRect
 * @tc.desc: check func GetSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetSessionRect, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    session_->SetSessionRect(rect);
    ASSERT_EQ(rect, session_->GetSessionRect());
}

/**
 * @tc.name: CheckDialogOnForeground
 * @tc.desc: check func CheckDialogOnForeground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, CheckDialogOnForeground, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->dialogVec_.clear();
    ASSERT_EQ(false, session_->CheckDialogOnForeground());
    SessionInfo info;
    info.abilityName_ = "dialogAbilityName";
    info.moduleName_ = "dialogModuleName";
    info.bundleName_ = "dialogBundleName";
    sptr<Session> dialogSession = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession, nullptr);
    dialogSession->state_ = SessionState::STATE_INACTIVE;
    session_->dialogVec_.push_back(dialogSession);
    ASSERT_EQ(false, session_->CheckDialogOnForeground());
    session_->dialogVec_.clear();
}

/**
 * @tc.name: IsTopDialog
 * @tc.desc: check func IsTopDialog
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, IsTopDialog, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->dialogVec_.clear();
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";

    sptr<Session> dialogSession1 = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession1, nullptr);
    dialogSession1->persistentId_ = 33;
    dialogSession1->SetParentSession(session_);
    dialogSession1->state_ = SessionState::STATE_ACTIVE;
    session_->dialogVec_.push_back(dialogSession1);

    sptr<Session> dialogSession2 = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession2, nullptr);
    dialogSession2->persistentId_ = 34;
    dialogSession2->SetParentSession(session_);
    dialogSession2->state_ = SessionState::STATE_ACTIVE;
    session_->dialogVec_.push_back(dialogSession2);

    sptr<Session> dialogSession3 = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession3, nullptr);
    dialogSession3->persistentId_ = 35;
    dialogSession3->SetParentSession(session_);
    dialogSession3->state_ = SessionState::STATE_INACTIVE;
    session_->dialogVec_.push_back(dialogSession3);

    ASSERT_EQ(false, dialogSession3->IsTopDialog());
    ASSERT_EQ(true, dialogSession2->IsTopDialog());
    ASSERT_EQ(false, dialogSession1->IsTopDialog());
    session_->dialogVec_.clear();
}

/**
 * @tc.name: NotifyDestroy
 * @tc.desc: check func NotifyDestroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyDestroy, Function | SmallTest | Level2)
{
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    session_->sessionStage_ = mockSessionStage;
    EXPECT_CALL(*(mockSessionStage), NotifyDestroy()).Times(1).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, session_->NotifyDestroy());
    session_->sessionStage_ = nullptr;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->NotifyDestroy());
}

/**
 * @tc.name: RaiseToAppTop01
 * @tc.desc: RaiseToAppTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, RaiseToAppTop01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    auto result = scensession->RaiseToAppTop();
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<SceneSession::SessionChangeCallback> scensessionchangeCallBack =
        new (std::nothrow) SceneSession::SessionChangeCallback();
    EXPECT_NE(scensessionchangeCallBack, nullptr);
    scensession->RegisterSessionChangeCallback(scensessionchangeCallBack);
    result = scensession->RaiseToAppTop();
    ASSERT_EQ(result, WSError::WS_OK);

    NotifyRaiseToTopFunc onRaiseToTop_ = []() {};
    scensessionchangeCallBack->onRaiseToTop_ = onRaiseToTop_;
    result = scensession->RaiseToAppTop();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateSessionRect01
 * @tc.desc: UpdateSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateSessionRect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    WSRect rect = {0, 0, 320, 240}; // width: 320, height: 240
    auto result = scensession->UpdateSessionRect(rect, SizeChangeReason::RESIZE);
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<SceneSession::SessionChangeCallback> scensessionchangeCallBack =
        new (std::nothrow) SceneSession::SessionChangeCallback();
    EXPECT_NE(scensessionchangeCallBack, nullptr);
    scensession->RegisterSessionChangeCallback(scensessionchangeCallBack);
    result = scensession->UpdateSessionRect(rect, SizeChangeReason::RESIZE);
    ASSERT_EQ(result, WSError::WS_OK);

    int resultValue = 0;
    NotifySessionRectChangeFunc onRectChange_ = [&resultValue](const WSRect &rect, const SizeChangeReason& reason)
    { resultValue = 1; };
    scensessionchangeCallBack->onRectChange_ = onRectChange_;
    result = scensession->UpdateSessionRect(rect, SizeChangeReason::RESIZE);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: OnSessionEvent01
 * @tc.desc: OnSessionEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, OnSessionEvent01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    auto result = scensession->OnSessionEvent(SessionEvent::EVENT_MINIMIZE);
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<SceneSession::SessionChangeCallback> scensessionchangeCallBack =
        new (std::nothrow) SceneSession::SessionChangeCallback();
    EXPECT_NE(scensessionchangeCallBack, nullptr);
    scensession->RegisterSessionChangeCallback(scensessionchangeCallBack);
    result = scensession->OnSessionEvent(SessionEvent::EVENT_MINIMIZE);
    ASSERT_EQ(result, WSError::WS_OK);

    int resultValue = 0;
    NotifySessionEventFunc onSessionEvent_ = [&resultValue](int32_t eventId, SessionEventParam param)
    { resultValue = 1; };
    scensessionchangeCallBack->OnSessionEvent_ = onSessionEvent_;
    result = scensession->OnSessionEvent(SessionEvent::EVENT_MINIMIZE);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ConsumeMoveEvent01
 * @tc.desc: ConsumeMoveEvent, abnormal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, ConsumeMoveEvent01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->moveDragController_ = new MoveDragController(1);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_TRUE(sceneSession->moveDragController_);
    sceneSession->moveDragController_->InitMoveDragProperty();
    WSRect originalRect = { 100, 100, 1000, 1000 };

    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    auto result = sceneSession->moveDragController_->ConsumeMoveEvent(pointerEvent, originalRect);
    ASSERT_FALSE(result);

    pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetPointerId(1);
    sceneSession->moveDragController_->moveDragProperty_.pointerId_ = 0;
    result = sceneSession->moveDragController_->ConsumeMoveEvent(pointerEvent, originalRect);
    ASSERT_FALSE(result);

    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_RIGHT);
    result = sceneSession->moveDragController_->ConsumeMoveEvent(pointerEvent, originalRect);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: ConsumeMoveEvent02
 * @tc.desc: ConsumeMoveEvent, normal secne
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, ConsumeMoveEvent02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->moveDragController_ = new MoveDragController(1);
    ASSERT_TRUE(sceneSession->moveDragController_);
    sceneSession->moveDragController_->InitMoveDragProperty();
    WSRect originalRect = { 100, 100, 1000, 1000 };
    sceneSession->moveDragController_->isStartMove_ = true;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerItem.SetDisplayX(115);
    pointerItem.SetDisplayY(500);
    pointerItem.SetWindowX(15);
    pointerItem.SetWindowY(400);
    auto result = sceneSession->moveDragController_->ConsumeMoveEvent(pointerEvent, originalRect);
    ASSERT_EQ(result, true);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerItem.SetDisplayX(145);
    pointerItem.SetDisplayY(550);
    result = sceneSession->moveDragController_->ConsumeMoveEvent(pointerEvent, originalRect);
    ASSERT_EQ(result, true);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerItem.SetDisplayX(175);
    pointerItem.SetDisplayY(600);
    result = sceneSession->moveDragController_->ConsumeMoveEvent(pointerEvent, originalRect);
    ASSERT_EQ(result, true);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    pointerItem.SetDisplayX(205);
    pointerItem.SetDisplayY(650);
    result = sceneSession->moveDragController_->ConsumeMoveEvent(pointerEvent, originalRect);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: ConsumeDragEvent01
 * @tc.desc: ConsumeDragEvent, abnormal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, ConsumeDragEvent01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->moveDragController_ = new MoveDragController(1);
    ASSERT_TRUE(sceneSession->moveDragController_);
    sceneSession->moveDragController_->InitMoveDragProperty();
    WSRect originalRect = { 100, 100, 1000, 1000 };
    SystemSessionConfig sessionConfig;

    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    sptr<WindowSessionProperty> property = nullptr;
    auto result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property,
        sessionConfig);
    ASSERT_EQ(result, false);

    pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    property = new WindowSessionProperty();
    sceneSession->moveDragController_->isStartDrag_ = false;
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, false);

    pointerEvent->SetPointerId(1);
    sceneSession->moveDragController_->moveDragProperty_.pointerId_ = 0;
    sceneSession->moveDragController_->isStartDrag_ = true;
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, false);

    pointerEvent->SetPointerId(0);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: ConsumeDragEvent02
 * @tc.desc: ConsumeDragEvent, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, ConsumeDragEvent02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->moveDragController_ = new MoveDragController(1);
    ASSERT_TRUE(sceneSession->moveDragController_);
    sceneSession->moveDragController_->InitMoveDragProperty();
    WSRect originalRect = { 100, 100, 1000, 1000 };
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SystemSessionConfig sessionConfig;
    sessionConfig.isSystemDecorEnable_ = true;
    sessionConfig.backgroundswitch = true;
    sessionConfig.decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    pointerItem.SetDisplayX(100);
    pointerItem.SetDisplayY(100);
    pointerItem.SetWindowX(0);
    pointerItem.SetWindowY(0);
    auto result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property,
        sessionConfig);
    ASSERT_EQ(result, true);

    sceneSession->moveDragController_->aspectRatio_ = 0.0f;
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerItem.SetDisplayX(150);
    pointerItem.SetDisplayY(150);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);

    sceneSession->moveDragController_->aspectRatio_ = 1.0f;
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerItem.SetDisplayX(200);
    pointerItem.SetDisplayY(200);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    pointerItem.SetDisplayX(250);
    pointerItem.SetDisplayY(250);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: ConsumeDragEvent03
 * @tc.desc: ConsumeDragEvent, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, ConsumeDragEvent03, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->moveDragController_ = new MoveDragController(1);
    ASSERT_TRUE(sceneSession->moveDragController_);
    sceneSession->moveDragController_->InitMoveDragProperty();
    WSRect originalRect = { 100, 100, 1000, 1000 };
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SystemSessionConfig sessionConfig;
    sessionConfig.isSystemDecorEnable_ = true;
    sessionConfig.backgroundswitch = true;
    sessionConfig.decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);

    // LEFT_TOP
    pointerItem.SetWindowX(0);
    pointerItem.SetWindowY(0);
    auto result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property,
        sessionConfig);
    ASSERT_EQ(result, true);

    // RIGHT_TOP
    pointerItem.SetWindowX(1000);
    pointerItem.SetWindowY(0);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);

    // RIGHT_BOTTOM
    pointerItem.SetWindowX(1000);
    pointerItem.SetWindowY(1000);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);

    // LEFT_BOTTOM
    pointerItem.SetWindowX(0);
    pointerItem.SetWindowY(1000);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: ConsumeDragEvent04
 * @tc.desc: ConsumeDragEvent, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, ConsumeDragEvent04, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->moveDragController_ = new MoveDragController(1);
    ASSERT_TRUE(sceneSession->moveDragController_);
    sceneSession->moveDragController_->InitMoveDragProperty();
    WSRect originalRect = { 100, 100, 1000, 1000 };
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SystemSessionConfig sessionConfig;
    sessionConfig.isSystemDecorEnable_ = true;
    sessionConfig.backgroundswitch = true;
    sessionConfig.decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE(pointerEvent);
    pointerEvent->SetAgentWindowId(1);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(0);
    pointerEvent->AddPointerItem(pointerItem);

    // LEFT
    pointerItem.SetWindowX(0);
    pointerItem.SetWindowY(500);
    auto result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property,
        sessionConfig);
    ASSERT_EQ(result, true);

    // TOP
    pointerItem.SetWindowX(500);
    pointerItem.SetWindowY(0);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);

    // RIGHT
    pointerItem.SetWindowX(1000);
    pointerItem.SetWindowY(500);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);

    // BOTTOM
    pointerItem.SetWindowX(500);
    pointerItem.SetWindowY(1000);
    result = sceneSession->moveDragController_->ConsumeDragEvent(pointerEvent, originalRect, property, sessionConfig);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: GetWindowId01
 * @tc.desc: GetWindowId, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetWindowId, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(0, session_->GetWindowId());
}

/**
 * @tc.name: GetVisible01
 * @tc.desc: GetVisible, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetVisible, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_OK, session_->SetVisible(false));
    session_->state_ = SessionState::STATE_CONNECT;
    if (!session_->GetVisible()) {
        ASSERT_EQ(false, session_->GetVisible());
    }
}

/**
 * @tc.name: IsActive01
 * @tc.desc: IsActive, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, IsActive, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->isActive_ = false;
    if (!session_->IsActive()) {
        ASSERT_EQ(false, session_->IsActive());
    }
}

/**
 * @tc.name: IsSessionForeground01
 * @tc.desc: IsSessionForeground, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, IsSessionForeground, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_EQ(true, session_->IsSessionForeground());
    session_->state_ = SessionState::STATE_ACTIVE;
    ASSERT_EQ(true, session_->IsSessionForeground());
    session_->state_ = SessionState::STATE_INACTIVE;
    ASSERT_EQ(false, session_->IsSessionForeground());
    session_->state_ = SessionState::STATE_BACKGROUND;
    ASSERT_EQ(false, session_->IsSessionForeground());
    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(false, session_->IsSessionForeground());
    session_->state_ = SessionState::STATE_CONNECT;
    ASSERT_EQ(false, session_->IsSessionForeground());
}

/**
 * @tc.name: SetFocusable01
 * @tc.desc: SetFocusable, normal scene
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetFocusable, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetSnapshot, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    std::shared_ptr<Media::PixelMap> snapshot = session_->Snapshot();
    ASSERT_EQ(snapshot, session_->GetSnapshot());
}

/**
 * @tc.name: NotifyActivation
 * @tc.desc: NotifyActivation Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyActivation, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->NotifyActivation();

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: NotifyForeground
 * @tc.desc: NotifyForeground Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyForeground, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->NotifyForeground();

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: NotifyBackground
 * @tc.desc: NotifyBackground Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyBackground, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->NotifyBackground();

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: NotifyExtensionDied
 * @tc.desc: NotifyExtensionDied Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyExtensionDied, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->NotifyExtensionDied();

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: NotifyExtensionTimeout
 * @tc.desc: NotifyExtensionTimeout Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyExtensionTimeout, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->NotifyExtensionTimeout(3);

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetAspectRatio
 * @tc.desc: SetAspectRatio Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetAspectRatio, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(WSError::WS_OK, session_->SetAspectRatio(0.1f));
}

/**
 * @tc.name: UpdateSessionTouchable
 * @tc.desc: UpdateSessionTouchable Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateSessionTouchable, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->UpdateSessionTouchable(false);

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetFocusable02
 * @tc.desc: others
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetFocusable02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->state_ = SessionState::STATE_FOREGROUND;
    session_->sessionInfo_.isSystem_ = false;

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(true));
}

/**
 * @tc.name: GetFocusable01
 * @tc.desc: property_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetFocusable01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->property_ = new WindowSessionProperty();
    ASSERT_EQ(true, session_->GetFocusable());
}

/**
 * @tc.name: GetFocusable02
 * @tc.desc: property_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetFocusable02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->property_ = nullptr;
    ASSERT_EQ(true, session_->GetFocusable());
}

/**
 * @tc.name: SetNeedNotify
 * @tc.desc: SetNeedNotify Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetNeedNotify, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->SetNeedNotify(false);

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: NeedNotify
 * @tc.desc: NeedNotify Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NeedNotify, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->SetNeedNotify(true);
    ASSERT_EQ(true, session_->NeedNotify());
}

/**
 * @tc.name: SetFocusedOnShow
 * @tc.desc: SetFocusedOnShow Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetFocusedOnShow, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetFocusedOnShow(false);
    auto focusedOnShow = session_->IsFocusedOnShow();
    ASSERT_EQ(focusedOnShow, false);
    session_->SetFocusedOnShow(true);
    focusedOnShow = session_->IsFocusedOnShow();
    ASSERT_EQ(focusedOnShow, true);
}

/**
 * @tc.name: SetTouchable01
 * @tc.desc: IsSessionValid() return false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetTouchable01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->sessionInfo_.isSystem_ = true;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->SetTouchable(false));
}

/**
 * @tc.name: SetTouchable02
 * @tc.desc: IsSessionValid() return true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetTouchable02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_FOREGROUND;
    session_->sessionInfo_.isSystem_ = false;
    ASSERT_EQ(WSError::WS_OK, session_->SetTouchable(false));
}

/**
 * @tc.name: SetSessionInfoLockedState01
 * @tc.desc: IsSessionValid() return false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetSessionInfoLockedState01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetSessionInfoLockedState(false);
    ASSERT_EQ(false, session_->sessionInfo_.lockedState);
}

/**
 * @tc.name: SetSessionInfoLockedState02
 * @tc.desc: IsSessionValid() return true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetSessionInfoLockedState02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetSessionInfoLockedState(true);
    ASSERT_EQ(true, session_->sessionInfo_.lockedState);
}

/**
 * @tc.name: GetCallingPid
 * @tc.desc: GetCallingPid Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetCallingPid, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetCallingPid(111);
    ASSERT_EQ(111, session_->GetCallingPid());
}

/**
 * @tc.name: GetCallingUid
 * @tc.desc: GetCallingUid Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetCallingUid, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetCallingUid(111);
    ASSERT_EQ(111, session_->GetCallingUid());
}

/**
 * @tc.name: GetAbilityToken
 * @tc.desc: GetAbilityToken Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetAbilityToken, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetAbilityToken(nullptr);
    ASSERT_EQ(nullptr, session_->GetAbilityToken());
}

/**
 * @tc.name: SetBrightness01
 * @tc.desc: property_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetBrightness01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->property_ = nullptr;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->SetBrightness(0.1f));
}

/**
 * @tc.name: SetBrightness02
 * @tc.desc: property_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetBrightness02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->property_ = new WindowSessionProperty();
    ASSERT_EQ(WSError::WS_OK, session_->SetBrightness(0.1f));
}

/**
 * @tc.name: UpdateHotRect
 * @tc.desc: UpdateHotRect Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateHotRect, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    WSRect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.width_ = 0;
    rect.height_ = 0;

    WSRectF newRect;
    const float outsideBorder = 4.0f * 1.5f;
    const size_t outsideBorderCount = 2;
    newRect.posX_ = rect.posX_ - outsideBorder;
    newRect.posY_ = rect.posY_ - outsideBorder;
    newRect.width_ = rect.width_ + outsideBorder * outsideBorderCount;
    newRect.height_ = rect.height_ + outsideBorder * outsideBorderCount;

    ASSERT_EQ(newRect, session_->UpdateHotRect(rect));
}

/**
 * @tc.name: SetTerminateSessionListener
 * @tc.desc: SetTerminateSessionListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetTerminateSessionListener, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    NotifyTerminateSessionFunc func = nullptr;
    session_->SetTerminateSessionListener(func);

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: TerminateSessionTotal01
 * @tc.desc: abilitySessionInfo is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TerminateSessionTotal01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION,
            session_->TerminateSessionTotal(nullptr, TerminateType::CLOSE_AND_KEEP_MULTITASK));
}

/**
 * @tc.name: TerminateSessionTotal02
 * @tc.desc: abilitySessionInfo is not nullptr, isTerminating is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TerminateSessionTotal02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    session_->isTerminating = true;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_OPERATION,
            session_->TerminateSessionTotal(abilitySessionInfo, TerminateType::CLOSE_AND_KEEP_MULTITASK));
}

/**
 * @tc.name: TerminateSessionTotal03
 * @tc.desc: abilitySessionInfo is not nullptr, isTerminating is false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TerminateSessionTotal03, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    session_->isTerminating = false;
    ASSERT_EQ(WSError::WS_OK,
            session_->TerminateSessionTotal(abilitySessionInfo, TerminateType::CLOSE_AND_KEEP_MULTITASK));
}

/**
 * @tc.name: SetTerminateSessionListenerTotal
 * @tc.desc: SetTerminateSessionListenerTotal Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetTerminateSessionListenerTotal, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    NotifyTerminateSessionFuncTotal func = nullptr;
    session_->SetTerminateSessionListenerTotal(func);

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetSessionLabel
 * @tc.desc: SetSessionLabel Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetSessionLabel, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->updateSessionLabelFunc_ = nullptr;
    ASSERT_EQ(WSError::WS_OK, session_->SetSessionLabel("SetSessionLabel Test"));
}

/**
 * @tc.name: SetUpdateSessionLabelListener
 * @tc.desc: SetUpdateSessionLabelListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetUpdateSessionLabelListener, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    NofitySessionLabelUpdatedFunc func = nullptr;
    session_->SetUpdateSessionLabelListener(func);

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetPendingSessionToForegroundListener
 * @tc.desc: SetPendingSessionToForegroundListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetPendingSessionToForegroundListener, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    NotifyPendingSessionToForegroundFunc func = nullptr;
    session_->SetPendingSessionToForegroundListener(func);

    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: PendingSessionToBackgroundForDelegator
 * @tc.desc: PendingSessionToBackgroundForDelegator Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, PendingSessionToBackgroundForDelegator, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetPendingSessionToBackgroundForDelegatorListener(nullptr);
    ASSERT_EQ(WSError::WS_OK, session_->PendingSessionToBackgroundForDelegator());
}

/**
 * @tc.name: NotifyScreenshot
 * @tc.desc: NotifyScreenshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyScreenshot, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->sessionStage_ = nullptr;
    session_->NotifyScreenshot();

    session_->property_ = new WindowSessionProperty();
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: TransferBackPressedEventForConsumed02
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferBackPressedEventForConsumed02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_OK, session_->TransferBackPressedEventForConsumed(isConsumed));
}

/**
 * @tc.name: TransferKeyEventForConsumed01
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferKeyEventForConsumed01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = nullptr;

    auto keyEvent = MMI::KeyEvent::Create();
    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEventForConsumed(keyEvent, isConsumed));
}

/**
 * @tc.name: TransferKeyEventForConsumed02
 * @tc.desc: windowEventChannel_ is not nullptr, keyEvent is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferKeyEventForConsumed02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEventForConsumed(keyEvent, isConsumed));
}

/**
 * @tc.name: TransferKeyEventForConsumed03
 * @tc.desc: windowEventChannel_ is not nullptr, keyEvent is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferKeyEventForConsumed03, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_OK, session_->TransferKeyEventForConsumed(keyEvent, isConsumed));
}

/**
 * @tc.name: TransferFocusActiveEvent02
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferFocusActiveEvent02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    ASSERT_EQ(WSError::WS_OK, session_->TransferFocusActiveEvent(false));
}

/**
 * @tc.name: TransferFocusStateEvent02
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferFocusStateEvent02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    ASSERT_EQ(WSError::WS_OK, session_->TransferFocusStateEvent(false));
}
/**
 * @tc.name: CreateDetectStateTask001
 * @tc.desc: Create detection task when there are no pre_existing tasks.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, CreateDetectStateTask001, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::NO_TASK;
    int32_t beforeTaskNum = GetTaskCount();
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(false, WindowMode::WINDOW_MODE_FULLSCREEN);

    ASSERT_NE(beforeTaskNum + 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::DETACH_TASK, session_->GetDetectTaskInfo().taskState);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: CreateDetectStateTask002
 * @tc.desc: Detect state when window mode changed.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, CreateDetectStateTask002, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    session_->handler_->PostTask(task, taskName, delayTime);
    int32_t beforeTaskNum = GetTaskCount();

    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::DETACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(true, WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    ASSERT_NE(beforeTaskNum - 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::NO_TASK, session_->GetDetectTaskInfo().taskState);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, session_->GetDetectTaskInfo().taskWindowMode);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: CreateDetectStateTask003
 * @tc.desc: Detect sup and down tree tasks fo the same type.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, CreateDetectStateTask003, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::DETACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    int32_t beforeTaskNum = GetTaskCount();
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(false, WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    ASSERT_NE(beforeTaskNum + 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::DETACH_TASK, session_->GetDetectTaskInfo().taskState);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: CreateDetectStateTask004
 * @tc.desc: Detection tasks under the same window mode.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, CreateDetectStateTask004, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    DetectTaskInfo detectTaskInfo;
    int32_t beforeTaskNum = GetTaskCount();
    detectTaskInfo.taskState = DetectTaskState::DETACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(true, WindowMode::WINDOW_MODE_FULLSCREEN);

    ASSERT_NE(beforeTaskNum + 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::ATTACH_TASK, session_->GetDetectTaskInfo().taskState);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: PostExportTask02
 * @tc.desc: PostExportTask
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, PostExportTask02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    std::string name = "sessionExportTask";
    auto task = [](){};
    int64_t delayTime = 0;

    session_->PostExportTask(task, name, delayTime);
    auto result = session_->GetBufferAvailable();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: SetLeashWinSurfaceNode02
 * @tc.desc: SetLeashWinSurfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetLeashWinSurfaceNode02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->leashWinSurfaceNode_ = WindowSessionTest::CreateRSSurfaceNode();
    session_->SetLeashWinSurfaceNode(nullptr);

    session_->leashWinSurfaceNode_ = nullptr;
    session_->SetLeashWinSurfaceNode(nullptr);
    auto result = session_->GetBufferAvailable();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetCloseAbilityWantAndClean
 * @tc.desc: GetCloseAbilityWantAndClean
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetCloseAbilityWantAndClean, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    AAFwk::Want outWant;
    session_->sessionInfo_.closeAbilityWant = std::make_shared<AAFwk::Want>();
    session_->GetCloseAbilityWantAndClean(outWant);

    session_->sessionInfo_.closeAbilityWant = nullptr;
    session_->GetCloseAbilityWantAndClean(outWant);
    auto result = session_->GetBufferAvailable();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: SetScreenId02
 * @tc.desc: SetScreenId Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetScreenId02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    uint64_t screenId = 0;
    session_->sessionStage_ = new (std::nothrow) SessionStageMocker();
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: SetFocusable03
 * @tc.desc: SetFocusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetFocusable03, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->isFocused_ = true;
    session_->property_ = new (std::nothrow) WindowSessionProperty();
    session_->property_->focusable_ = false;
    bool isFocusable = true;

    auto result = session_->SetFocusable(isFocusable);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: GetFocused
 * @tc.desc: GetFocused Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetFocused, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool result = session_->GetFocused();
    ASSERT_EQ(result, false);

    session_->isFocused_ = true;
    bool result2 = session_->GetFocused();
    ASSERT_EQ(result2, true);
}

/**
 * @tc.name: UpdatePointerArea
 * @tc.desc: UpdatePointerArea Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdatePointerArea, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    WSRect rect = { 0, 0, 0, 0 };
    session_->preRect_ = rect;
    session_->UpdatePointerArea(rect);
    ASSERT_EQ(session_->GetFocused(), false);
}

/**
 * @tc.name: UpdateSizeChangeReason02
 * @tc.desc: UpdateSizeChangeReason Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateSizeChangeReason02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = session_->UpdateSizeChangeReason(reason);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: UpdateDensity
 * @tc.desc: UpdateDensity Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateDensity, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_FALSE(session_->IsSessionValid());
    WSError result = session_->UpdateDensity();
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    session_->state_ = SessionState::STATE_CONNECT;
    ASSERT_TRUE(session_->IsSessionValid());
    session_->sessionStage_ = nullptr;
    WSError result02 = session_->UpdateDensity();
    ASSERT_EQ(result02, WSError::WS_ERROR_NULLPTR);
}
}
} // namespace Rosen
} // namespace OHOS

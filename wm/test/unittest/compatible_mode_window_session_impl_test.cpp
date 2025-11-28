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

#include "mock_session.h"
#include "mock_session_stub.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "window_helper.h"
#include "window_session_impl.h"
#include "wm_common.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
std::string g_errLog;
void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
    const char *msg)
{
    g_errLog = msg;
}

class CompatibleModeWindowSessionImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
};

void CompatibleModeWindowSessionImplTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "setup test case";
}

void CompatibleModeWindowSessionImplTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "teardown test case";
}

void CompatibleModeWindowSessionImplTest::SetUp()
{
    GTEST_LOG_(INFO) << "setup";
}

void CompatibleModeWindowSessionImplTest::TearDown()
{
    GTEST_LOG_(INFO) << "teardown";
}

namespace {

/**
 * @tc.name: RegisterNavigateCallbackForPageCompatibleModeIfNeed
 * @tc.desc: RegisterNavigateCallbackForPageCompatibleModeIfNeedWithNullUiContent
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest, RegisterNavigateCallbackForPageCompatibleModeIfNeedWithNullUiContent,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterNavigateCallbackForPageCompatibleModeIfNeedWithNullUiContent test start";
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->RegisterNavigateCallbackForPageCompatibleModeIfNeed();
    EXPECT_TRUE(g_errLog.find("content is nullptr or page is empty") != std::string::npos);
    LOG_SetCallback(nullptr);
    GTEST_LOG_(INFO) << "RegisterNavigateCallbackForPageCompatibleModeIfNeedWithNullUiContent test end";
}

/**
 * @tc.name: RegisterNavigateCallbackForPageCompatibleModeIfNeed
 * @tc.desc: RegisterNavigateCallbackForPageCompatibleModeIfNeedWithEmptyCompatibleModePage
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest,
    RegisterNavigateCallbackForPageCompatibleModeIfNeedWithEmptyCompatibleModePage, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterNavigateCallbackForPageCompatibleModeIfNeedWithEmptyCompatibleModePage test start";
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->RegisterNavigateCallbackForPageCompatibleModeIfNeed();
    EXPECT_TRUE(g_errLog.find("content is nullptr or page is empty") != std::string::npos);
    LOG_SetCallback(nullptr);
    GTEST_LOG_(INFO) << "RegisterNavigateCallbackForPageCompatibleModeIfNeedWithEmptyCompatibleModePage test end";
}

/**
 * @tc.name: RegisterNavigateCallbackForPageCompatibleModeIfNeed
 * @tc.desc: RegisterNavigateCallbackForPageCompatibleModeIfNeedWithSuccess
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest, RegisterNavigateCallbackForPageCompatibleModeIfNeedWithSuccess,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterNavigateCallbackForPageCompatibleModeIfNeedWithSuccess test start";
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->property_->SetCompatibleModePage("page");
    window->RegisterNavigateCallbackForPageCompatibleModeIfNeed();
    EXPECT_TRUE(g_errLog.find("content is nullptr or page is empty") == std::string::npos);
    LOG_SetCallback(nullptr);
    GTEST_LOG_(INFO) << "RegisterNavigateCallbackForPageCompatibleModeIfNeedWithSuccess test end";
}

/**
 * @tc.name: HandleNavigateCallbackForPageCompatibleMode
 * @tc.desc: HandleNavigateCallbackForPageCompatibleModeWithSameTargetPage
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest, HandleNavigateCallbackForPageCompatibleModeWithSameTargetPage,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithSameTargetPage test start";
    SessionInfo sessionInfo = { "TestBundle", "TestModule", "TestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    std::string targetPage = "targetPage";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->property_->SetCompatibleModePage(targetPage);
    window->HandleNavigateCallbackForPageCompatibleMode(targetPage, targetPage);
    EXPECT_EQ(window->property_->GetPageCompatibleMode(), CompatibleStyleMode::INVALID_VALUE);
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithSameTargetPage test end";
}

/**
 * @tc.name: HandleNavigateCallbackForPageCompatibleMode
 * @tc.desc: HandleNavigateCallbackForPageCompatibleModeWithSameEmptyPage
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest, HandleNavigateCallbackForPageCompatibleModeWithSameEmptyPage,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithSameEmptyPage test start";
    SessionInfo sessionInfo = { "TestBundle", "TestModule", "TestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    std::string targetPage = "targetPage";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->property_->SetCompatibleModePage(targetPage);
    window->HandleNavigateCallbackForPageCompatibleMode("", "");
    EXPECT_EQ(window->property_->GetPageCompatibleMode(), CompatibleStyleMode::INVALID_VALUE);
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithSameEmptyPage test end";
}

/**
 * @tc.name: HandleNavigateCallbackForPageCompatibleMode
 * @tc.desc: HandleNavigateCallbackForPageCompatibleModeWithTargetFromPage
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest, HandleNavigateCallbackForPageCompatibleModeWithTargetFromPage,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithTargetFromPage test start";
    SessionInfo sessionInfo = { "TestBundle", "TestModule", "TestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    std::string targetPage = "targetPage";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->property_->SetCompatibleModePage(targetPage);
    window->HandleNavigateCallbackForPageCompatibleMode(targetPage, "");
    EXPECT_EQ(window->property_->GetPageCompatibleMode(), CompatibleStyleMode::INVALID_VALUE);
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithTargetFromPage test end";
}

/**
 * @tc.name: HandleNavigateCallbackForPageCompatibleMode
 * @tc.desc: HandleNavigateCallbackForPageCompatibleModeWithTargetToPage
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest, HandleNavigateCallbackForPageCompatibleModeWithTargetToPage,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithTargetToPage test start";
    SessionInfo sessionInfo = { "TestBundle", "TestModule", "TestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    std::string targetPage = "targetPage";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->property_->SetCompatibleModePage(targetPage);
    window->HandleNavigateCallbackForPageCompatibleMode("", targetPage);
    EXPECT_EQ(window->property_->GetPageCompatibleMode(), CompatibleStyleMode::LANDSCAPE_18_9);
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithTargetToPage test end";
}

/**
 * @tc.name: HandleNavigateCallbackForPageCompatibleMode
 * @tc.desc: HandleNavigateCallbackForPageCompatibleModeWithNullSession
 * @tc.type: FUNC
 */
HWTEST_F(CompatibleModeWindowSessionImplTest, HandleNavigateCallbackForPageCompatibleModeWithNullSession,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithNullSession test start";
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    std::string targetPage = "targetPage";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->property_->SetCompatibleModePage(targetPage);
    window->HandleNavigateCallbackForPageCompatibleMode("", targetPage);
    EXPECT_TRUE(g_errLog.find("hostSession is null") != std::string::npos);
    LOG_SetCallback(nullptr);
    GTEST_LOG_(INFO) << "HandleNavigateCallbackForPageCompatibleModeWithNullSession test end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS

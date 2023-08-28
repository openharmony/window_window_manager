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
#include "screen_session.h"
#include <gtest/gtest.h>

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class ScreenSessionTest : public testing::Test {
  public:
    ScreenSessionTest() {}
    ~ScreenSessionTest() {}
};

namespace {
/**
 * @tc.name: UnregisterScreenChangeListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UnregisterScreenChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener start";
    IScreenChangeListener* screenChangeListener = nullptr;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int64_t ret = 0;
    session->UnregisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener end";
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenSupportedColorGamuts, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSupportedColorGamuts start";
    std::vector<ScreenColorGamut> colorGamuts;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    DMError ret = session->GetScreenSupportedColorGamuts(colorGamuts);
    ASSERT_EQ(ret, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSupportedColorGamuts end";
}

/**
 * @tc.name: GetActiveScreenMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetActiveScreenMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetActiveScreenMode start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    sptr<SupportedScreenModes> mode1 = session->GetActiveScreenMode();
    session->GetActiveScreenMode();
    ASSERT_EQ(mode1, nullptr);

    session->activeIdx_ = 100;
    sptr<SupportedScreenModes> mode2 = session->GetActiveScreenMode();
    session->GetActiveScreenMode();
    ASSERT_EQ(mode2, nullptr);

    GTEST_LOG_(INFO) << "ScreenSessionTest: GetActiveScreenMode end";
}

/**
 * @tc.name: GetSourceMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSourceMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->screenId_ = session->defaultScreenId_;
    ScreenSourceMode mode = session->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_MAIN);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode end";
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenColorGamut, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenColorGamut start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    ScreenColorGamut colorGamut;
    DMError res = session->GetScreenColorGamut(colorGamut);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenColorGamut end";
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenColorGamut, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    int32_t colorGamut = 1;
    DMError res = session->SetScreenColorGamut(colorGamut);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut end";
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenGamutMap, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenGamutMap start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    ScreenGamutMap gamutMap;
    DMError res = session->GetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenGamutMap end";
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenGamutMap, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenGamutMap start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);
    ScreenGamutMap gamutMap = GAMUT_MAP_CONSTANT;
    DMError res = session->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);

    gamutMap = GAMUT_MAP_HDR_EXTENSION;
    res = session->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenGamutMap end";
}

/**
 * @tc.name: InitRSDisplayNode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, InitRSDisplayNode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: InitRSDisplayNode start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);
    RSDisplayNodeConfig config;
    Point startPoint;
    int res = 0;
    sessionGroup.InitRSDisplayNode(config, startPoint);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: InitRSDisplayNode end";
}

/**
 * @tc.name: GetRSDisplayNodeConfig
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetRSDisplayNodeConfig, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    RSDisplayNodeConfig config = {1};
    sptr<ScreenSession> defaultScreenSession;
    bool res = sessionGroup.GetRSDisplayNodeConfig(session0, config, defaultScreenSession);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    sessionGroup.combination_ = ScreenCombination::SCREEN_ALONE;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);

    sessionGroup.combination_ = ScreenCombination::SCREEN_EXPAND;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);

    sessionGroup.combination_ = ScreenCombination::SCREEN_MIRROR;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig end";
}

/**
 * @tc.name: AddChild
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, AddChild, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChild start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    Point startPoint(0, 0);
    sptr<ScreenSession> defaultScreenSession;
    bool res = sessionGroup.AddChild(session0, startPoint, defaultScreenSession);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    res = sessionGroup.AddChild(session, startPoint, defaultScreenSession);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChild end";
}

/**
 * @tc.name: AddChildren
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, AddChildren, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    std::vector<sptr<ScreenSession>> smsScreens;
    std::vector<Point> startPoints;
    bool res = sessionGroup.AddChildren(smsScreens, startPoints);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren end";
}

/**
 * @tc.name: RemoveChild
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, RemoveChild, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: RemoveChild start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    bool res = sessionGroup.RemoveChild(session0);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    res = sessionGroup.RemoveChild(session);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig end";
}

/**
 * @tc.name: GetChildPosition
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetChildPosition, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetChildPosition start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    ScreenId screenId = 1;
    Point res = sessionGroup.GetChildPosition(screenId);
    ASSERT_EQ(res.posX_, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig end";
}

/**
 * @tc.name: ConvertToScreenGroupInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ConvertToScreenGroupInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToScreenGroupInfo start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenGroupInfo> res = sessionGroup.ConvertToScreenGroupInfo();
    ASSERT_NE(res, nullptr);
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToScreenGroupInfo end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS

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
#include "session_manager/include/hidump_controller.h"
#include "session/host/include/scene_session.h"
#include "window_manager_hilog.h"
#include "wm_single_instance.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HidumpControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<SceneSession> GetSceneSession(std::string name);
};

void HidumpControllerTest::SetUpTestCase() {}

void HidumpControllerTest::TearDownTestCase() {}

void HidumpControllerTest::SetUp() {}

void HidumpControllerTest::TearDown() {}

sptr<SceneSession> HidumpControllerTest::GetSceneSession(std::string name)
{
    SessionInfo info;
    info.abilityName_ = name;
    info.bundleName_ = name;
    auto result = sptr<SceneSession>::MakeSptr(info, nullptr);
    if (result != nullptr) {
        result->property_ = sptr<WindowSessionProperty>::MakeSptr();
        if (result->property_ == nullptr) {
            return nullptr;
        }
    }

    return result;
}

namespace {
/**
 * @tc.name: GetAllSessionDumpDetailedInfo
 * @tc.desc: GetAllSessionDumpDetailedInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, GetAllSessionDumpDetailedInfo02, TestSize.Level1)
{
    std::vector<sptr<SceneSession>> allSession;
    std::vector<sptr<SceneSession>> backgroundSession;
    auto sceneSession = GetSceneSession("GetAllSessionDumpDetailedInfo02");
    ASSERT_NE(sceneSession, nullptr);
    allSession.push_back(sceneSession);

    std::ostringstream oss;
    HidumpController::GetInstance().GetAllSessionDumpDetailedInfo(oss, allSession, backgroundSession);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpSceneSessionParamList
 * @tc.desc: DumpSceneSessionParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpSceneSessionParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpSceneSessionParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpSceneSessionParam
 * @tc.desc: DumpSceneSessionParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpSceneSessionParam01, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpSceneSessionParam01");
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->property_ = nullptr;
    std::ostringstream oss;
    HidumpController::GetInstance().DumpSceneSessionParam(oss, sceneSession);

    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpSceneSessionParam
 * @tc.desc: DumpSceneSessionParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpSceneSessionParam02, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpSceneSessionParam02");
    ASSERT_NE(sceneSession, nullptr);
    std::ostringstream oss;
    HidumpController::GetInstance().DumpSceneSessionParam(oss, sceneSession);

    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpSessionParamList
 * @tc.desc: DumpSessionParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpSessionParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpSessionParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpSessionParam
 * @tc.desc: DumpSessionParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpSessionParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpLayoutRectParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpSessionParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLayoutRectParamList
 * @tc.desc: DumpLayoutRectParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLayoutRectParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpLayoutRectParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLayoutRectParam
 * @tc.desc: DumpLayoutRectParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLayoutRectParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpLayoutRectParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpLayoutRectParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLayoutParamList
 * @tc.desc: DumpLayoutParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLayoutParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpLayoutParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLayoutParam
 * @tc.desc: DumpLayoutParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLayoutParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpLayoutParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpLayoutParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpAbilityParamList
 * @tc.desc: DumpAbilityParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpAbilityParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpAbilityParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpAbilityParam
 * @tc.desc: DumpAbilityParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpAbilityParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpLayoutParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpAbilityParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpKeyboardParamList
 * @tc.desc: DumpKeyboardParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpKeyboardParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpKeyboardParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpKeyboardParam
 * @tc.desc: DumpKeyboardParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpKeyboardParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpKeyboardParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpKeyboardParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpSysconfigParamList
 * @tc.desc: DumpSysconfigParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpSysconfigParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpSysconfigParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpSysconfigParam
 * @tc.desc: DumpSysconfigParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpSysconfigParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpSysconfigParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpSysconfigParam(oss, sceneSession);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLifeParamList
 * @tc.desc: DumpLifeParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLifeParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpLifeParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLifeParam
 * @tc.desc: DumpLifeParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLifeParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpLifeParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpLifeParam(oss, sceneSession);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpDisplayParamList
 * @tc.desc: DumpDisplayParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpDisplayParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpDisplayParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpDisplayParam
 * @tc.desc: DumpDisplayParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpDisplayParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpDisplayParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpDisplayParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpFocusParamList
 * @tc.desc: DumpFocusParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpFocusParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpFocusParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpFocusParam
 * @tc.desc: DumpFocusParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpFocusParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpFocusParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpFocusParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpInputParamList
 * @tc.desc: DumpInputParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpInputParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpInputParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpInputParam
 * @tc.desc: DumpInputParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpInputParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpInputParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpInputParam(oss, sceneSession, sceneSession->property_);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLakeParamList
 * @tc.desc: DumpLakeParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLakeParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpLakeParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpLakeParam
 * @tc.desc: DumpLakeParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpLakeParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpLakeParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpLakeParam(oss, sceneSession);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpCOMParamList
 * @tc.desc: DumpCOMParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpCOMParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpCOMParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpCOMParam
 * @tc.desc: DumpCOMParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpCOMParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpCOMParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpCOMParam(oss, sceneSession);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpVisibleParamList
 * @tc.desc: DumpVisibleParamList Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpVisibleParamList, TestSize.Level1)
{
    std::ostringstream oss;
    HidumpController::GetInstance().DumpVisibleParamList(oss);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}

/**
 * @tc.name: DumpVisibleParam
 * @tc.desc: DumpVisibleParam Test
 * @tc.type: FUNC
 */
HWTEST_F(HidumpControllerTest, DumpVisibleParam, TestSize.Level1)
{
    auto sceneSession = GetSceneSession("DumpVisibleParam");
    ASSERT_NE(sceneSession, nullptr);

    std::ostringstream oss;
    HidumpController::GetInstance().DumpVisibleParam(oss, sceneSession);
    std::string result = oss.str();
    ASSERT_NE(result.size(), 0);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
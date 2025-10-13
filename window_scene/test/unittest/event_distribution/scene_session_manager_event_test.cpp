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
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "session_manager/include/scene_session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_accesstoken_kit.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    std::string g_logMsg;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
        const char *msg)
    {
        g_logMsg = msg;
    }
}
namespace {
const std::string EMPTY_DEVICE_ID = "";
constexpr int WAIT_SLEEP_TIME = 1;
using ConfigItem = WindowSceneConfig::ConfigItem;
ConfigItem ReadConfig(const std::string& xmlStr)
{
    ConfigItem config;
    xmlDocPtr docPtr = xmlParseMemory(xmlStr.c_str(), xmlStr.length());
    if (docPtr == nullptr) {
        return config;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return config;
    }

    std::map<std::string, ConfigItem> configMap;
    config.SetValue(configMap);
    WindowSceneConfig::ReadConfig(rootPtr, *config.mapValue_);
    xmlFreeDoc(docPtr);
    return config;
}
} // namespace
class SceneSessionManagerEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerEventTest::ssm_ = nullptr;

bool SceneSessionManagerEventTest::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerEventTest::callbackFunc_ =
    [](bool enable, const std::string& bundleName, GestureBackType type) { gestureNavigationEnabled_ = enable; };

void SceneSessionManagerEventTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerEventTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerEventTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerEventTest::TearDown()
{
    MockAccesstokenKit::ChangeMockStateToInit();
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: LockCursor
 * @tc.desc: LockCursor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerEventTest, LockCursor, TestSize.Level1)
{
    // test checkDatas: The format is incorrect(size<1).
    std::vector<int32_t> datas;
    auto ret = ssm_->LockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_ERROR_ILLEGAL_PARAM);
    
    // test checkDatas: The format is incorrect(length error).
    datas.emplace_back(LOCK_CURSOR_LENGTH + 1);
    ret = ssm_->LockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_ERROR_ILLEGAL_PARAM);

    // test checkDatas: The format is incorrect(size error).
    datas.clear();
    datas.emplace_back(LOCK_CURSOR_LENGTH);
    ret = ssm_->LockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_ERROR_ILLEGAL_PARAM);

    // test normal process
    datas.clear();
    datas.emplace_back(LOCK_CURSOR_LENGTH);
    datas.emplace_back(1);
    datas.emplace_back(1);
    ret = ssm_->LockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_OK);

    // test normal process
    datas.clear();
    datas.emplace_back(LOCK_CURSOR_LENGTH);
    datas.emplace_back(1);
    datas.emplace_back(0);
    ret = ssm_->LockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: UnLockCursor
 * @tc.desc: UnLockCursor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerEventTest, UnLockCursor, TestSize.Level1)
{
    std::vector<int32_t> datas;
    // test checkDatas: The format is incorrect(size<1).
    auto ret = ssm_->UnLockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_ERROR_ILLEGAL_PARAM);
    
    // test checkDatas: The format is incorrect(length error).
    datas.emplace_back(UNLOCK_CURSOR_LENGTH + 1);
    ret = ssm_->UnLockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_ERROR_ILLEGAL_PARAM);

    // test checkDatas: The format is incorrect(size error).
    datas.clear();
    datas.emplace_back(UNLOCK_CURSOR_LENGTH);
    ret = ssm_->UnLockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_ERROR_ILLEGAL_PARAM);

    // test normal process
    datas.clear();
    datas.emplace_back(UNLOCK_CURSOR_LENGTH);
    datas.emplace_back(1);
    ret = ssm_->UnLockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_OK);

    // test normal process(repeat)
    datas.clear();
    datas.emplace_back(UNLOCK_CURSOR_LENGTH);
    datas.emplace_back(1);
    ret = ssm_->UnLockCursor(datas);
    EXPECT_EQ(ret, WMError::WM_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

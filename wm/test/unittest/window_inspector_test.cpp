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

#include "window_inspector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowInspetorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowInspetorTest::SetUpTestCase() {}

void WindowInspetorTest::TearDownTestCase() {}

void WindowInspetorTest::SetUp() {}

void WindowInspetorTest::TearDown() {}

namespace {
/**
 * @tc.name: InitConnectServer01
 * @tc.desc: InitConnectServer
 * @tc.type: FUNC
 */
HWTEST_F(WindowInspetorTest, InitConnectServer, TestSize.Level1)
{
    WindowInspector::GetInstance().ConnectServer();
    EXPECT_EQ(true, WindowInspector::GetInstance().IsConnectServerSuccess());
}

/**
 * @tc.name: ProcessArkUIInspectorMessage01
 * @tc.desc: ProcessArkUIInspectorMessage
 * @tc.type: FUNC
 */
HWTEST_F(WindowInspetorTest, ProcessArkUIInspectorMessage, TestSize.Level1)
{
    std::string jsonStr;
    std::string message1 = "{method:WMS,params:{interface:getCurrentProcessWindowList}}";
    auto ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message1, jsonStr);
    EXPECT_EQ(false, ret);

    std::string message2 = "{method:WMS.windowList,params:{interface:get}}";
    ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message2, jsonStr);
    EXPECT_EQ(false, ret);

    std::string message3 = "{method:WMS.windowList,params:{interface:getCurrentProcessWindowList}}";
    ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message3, jsonStr);
    EXPECT_EQ(false, ret);

    std::string message4 = R"({"method":30,"params":{"interface":"getCurrentProcessWindowList"}})";
    ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message4, jsonStr);
    EXPECT_EQ(false, ret);

    std::string message5 = R"({"method":"WMS.windowList","params":{"command":30}})";
    ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message5, jsonStr);
    EXPECT_EQ(false, ret);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
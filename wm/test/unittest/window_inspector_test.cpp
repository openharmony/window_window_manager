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
HWTEST_F(WindowInspetorTest, InitConnectServer, Function | SmallTest | Level2)
{
    WindowInspector::GetInstance().ConnectServer();
    EXPECT_EQ(true, WindowInspector::GetInstance().IsConnectServerSuccess());
}

/**
 * @tc.name: CloseConnectServer01
 * @tc.desc: CloseConnectServer
 * @tc.type: FUNC
 */
HWTEST_F(WindowInspetorTest, CloseConnectServer01, Function | SmallTest | Level2)
{
    WindowInspector::GetInstance().ConnectServer();
    WindowInspector::GetInstance().CloseConnectServer();
    EXPECT_EQ(false, WindowInspector::GetInstance().IsConnectServerSuccess());
}

/**
 * @tc.name: ProcessArkUIInspectorMessage01
 * @tc.desc: ProcessArkUIInspectorMessage
 * @tc.type: FUNC
 */
HWTEST_F(WindowInspetorTest, ProcessArkUIInspectorMessage, Function | SmallTest | Level2)
{
    std::string message1 = "{method:WMS,params:{interface:getCurrentProcessWindowList}}";
    auto ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message1);
    EXPECT_EQ(false, ret);

    std::string message2 = "{method:WMS.windowList,params:{interface:get}}";
    ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message2);
    EXPECT_EQ(false, ret);

    std::string message3 = "{method:WMS.windowList,params:{interface:getCurrentProcessWindowList}}";
    ret = WindowInspector::GetInstance().ProcessArkUIInspectorMessage(message3);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: TransformDataToJson01
 * @tc.desc: TransformDataToJson
 * @tc.type: FUNC
 */
HWTEST_F(WindowInspetorTest, TransformDataToJson, Function | SmallTest | Level2)
{
    Rect windowRect = { 100, 100, 100, 100 };
    std::vector<WindowListsInfo> windowListsInfo;
    windowListsInfo.push_back({ "test01", 1, 1, windowRect});
    WindowInspector::GetInstance().TransformDataToJson(windowListsInfo);
    std::string ret = "{\"type\":\"window\",\"content\":[{\"windowName\":\"test01\",\"winId\":\"1\",\"type\":\"1\","
                      "\"rect\":{\"startX\":\"100\",\"startY\":\"100\",\"width\":\"100\",\"height\":\"100\"}}]}";
    EXPECT_EQ(ret, WindowInspector::GetInstance().jsonWindowListsInfoStr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
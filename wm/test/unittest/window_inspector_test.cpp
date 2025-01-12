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

#include "window_inspector.h"

namespace OHOS {
namespace Rosen {
class WindowInspetorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
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
    WindowInspector::GetInstance().InitConnectServer();
    EXPECT_EQ(true, WindowInspector::GetInstance().isInitConnectSuccess_);
}

/**
 * @tc.name: RegisterWMSConnectCallback01
 * @tc.desc: RegisterWMSConnectCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowInspetorTest, RegisterWMSConnectCallback, Function | SmallTest | Level2)
{
    auro ret = WindowInspector::GetInstance().RegisterWMSConnectCallback();
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: UnregisterCallback01
 * @tc.desc: UnregisterCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowInspetorTest, UnregisterCallback, Function | SmallTest | Level2)
{
    WindowInspector::GetInstance().UnregisterCallback();
    EXPECT_EQ(nullptr, WindowInspector::GetInstance().setWMSCallback_);
    EXPECT_EQ(nullptr, WindowInspector::GetInstance().sendMessage_);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
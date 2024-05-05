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

#include "session_manager/include/zidl/scene_session_manager_lite_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSessionManagerLiteStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManagerLiteStubTest> sceneSessionManagerLiteStub_ = nullptr;
};

void SceneSessionManagerLiteStubTest::SetUpTestCase()
{
}

void SceneSessionManagerLiteStubTest::TearDownTestCase()
{
}

void SceneSessionManagerLiteStubTest::SetUp()
{
    sceneSessionManagerLiteStub_ = new SceneSessionManagerLiteStubTest();
}

void SceneSessionManagerLiteStubTest::TearDown()
{
    sceneSessionManagerLiteStub_ = nullptr;
}

namespace {
/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: OnRemoteRequest test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, OnRemoteRequest, Function | SmallTest | Level1)
{
    uint32_t code = static_cast<uint32_t>(SceneSessionManagerLiteStub::SceneSessionManagerLiteMessage
        ::TRANS_ID_SET_SESSION_LABEL);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WritenInterfaceToken(u"OpenHameny");
}

}
}
}

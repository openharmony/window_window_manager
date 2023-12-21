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

#include "modal_system_ui_extension.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ModalSystemUiExtensionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ModalSystemUiExtensionTest::SetUpTestCase()
{
}

void ModalSystemUiExtensionTest::TearDownTestCase()
{
}

void ModalSystemUiExtensionTest::SetUp()
{
}

void ModalSystemUiExtensionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ModalSystemUiExtensionConnection01
 * @tc.desc: connect modal system ui_extension
 * @tc.type: FUNC
 */
HWTEST_F(ModalSystemUiExtensionTest, ModalSystemUiExtensionConnection01, Function | SmallTest | Level2)
{
    auto connection = new(std::nothrow)ModalSystemUiExtension();
    if (connection == nullptr) {
        return;
    }
    OHOS::AAFwk::Want want;
    ASSERT_TRUE(connection->CreateModalUIExtension(want) == ERR_OK);
    delete connection;
}
}
} // Rosen
} // OHOS
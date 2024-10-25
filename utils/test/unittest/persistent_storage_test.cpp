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
#include "persistent_storage.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PersistentStorageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PersistentStorageTest::SetUpTestCase()
{
}

void PersistentStorageTest::TearDownTestCase()
{
}

void PersistentStorageTest::SetUp()
{
}

void PersistentStorageTest::TearDown()
{
}

namespace {
/**
 * @tc.name: HasKey
 * @tc.desc: HasKey test
 * @tc.type: FUNC
 */
HWTEST_F(PersistentStorageTest, HasKey, Function | SmallTest | Level3)
{
    const std::string keyName = "name";
    auto result = PersistentStorage::HasKey(keyName, PersistentStorageType::UKNOWN);
    ASSERT_EQ(false, result);

    auto result2 = PersistentStorage::HasKey(keyName, PersistentStorageType::ASPECT_RATIO);
    ASSERT_EQ(false, result2);
}

/**
 * @tc.name: Insert/Get/Delete
 * @tc.desc: Insert/Get/Delete test
 * @tc.type: FUNC
 */
HWTEST_F(PersistentStorageTest, StorageOperate, Function | SmallTest | Level3)
{
    const std::string keyName = "/data/service/el1/public/window/window_aspect_ratio.xml";
    float ratio = 1;
    float ratioValue = 2;

    PersistentStorage::Delete(keyName, PersistentStorageType::UKNOWN);
    auto result0 = PersistentStorage::HasKey(keyName, PersistentStorageType::UKNOWN);
    EXPECT_EQ(false, result0);

    PersistentStorage::Insert(keyName, ratioValue, PersistentStorageType::UKNOWN);
    PersistentStorage::Get(keyName, ratio, PersistentStorageType::UKNOWN);
    EXPECT_EQ(1, ratio);

    PersistentStorage::Insert(keyName, ratioValue, PersistentStorageType::ASPECT_RATIO);
    PersistentStorage::Get(keyName, ratio, PersistentStorageType::ASPECT_RATIO);
    EXPECT_EQ(2, ratio);

    auto result = PersistentStorage::HasKey(keyName, PersistentStorageType::ASPECT_RATIO);
    EXPECT_EQ(true, result);

    PersistentStorage::Delete(keyName, PersistentStorageType::ASPECT_RATIO);
    auto result2 = PersistentStorage::HasKey(keyName, PersistentStorageType::ASPECT_RATIO);
    EXPECT_EQ(false, result2);

    PersistentStorage::Insert(keyName, ratioValue, PersistentStorageType::MAXIMIZE_STATE);
    PersistentStorage::Get(keyName, ratio, PersistentStorageType::MAXIMIZE_STATE);
    EXPECT_EQ(2, ratio);

    auto result3 = PersistentStorage::HasKey(keyName, PersistentStorageType::MAXIMIZE_STATE);
    EXPECT_EQ(true, result3);

    PersistentStorage::Delete(keyName, PersistentStorageType::MAXIMIZE_STATE);
    auto result4 = PersistentStorage::HasKey(keyName, PersistentStorageType::MAXIMIZE_STATE);
    EXPECT_EQ(false, result4);
}

}
} // namespace Rosen
} // namespace OHOS
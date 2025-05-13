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

#include "window_manager_lru.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
constexpr std::size_t TEST_CACHE_CAPACITY = 3;

class LruCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    std::unique_ptr<LruCache> testLruCache_;
};

void LruCacheTest::SetUpTestCase() {}

void LruCacheTest::TearDownTestCase() {}

void LruCacheTest::SetUp() {}

void LruCacheTest::TearDown() {}

/**
 * @tc.name: Visit
 * @tc.desc: test function Visit
 * @tc.type: FUNC
 */
HWTEST_F(LruCacheTest, Visit, TestSize.Level1)
{
    testLruCache_ = std::make_unique<LruCache>(TEST_CACHE_CAPACITY);
    bool res = testLruCache_->Visit(30);
    ASSERT_EQ(res, false);

    testLruCache_->Put(30);
    res = testLruCache_->Visit(30);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: Put
 * @tc.desc: test function Put
 * @tc.type: FUNC
 */
HWTEST_F(LruCacheTest, Put, TestSize.Level1)
{
    testLruCache_ = std::make_unique<LruCache>(TEST_CACHE_CAPACITY);
    int32_t res = testLruCache_->Put(30);
    ASSERT_EQ(res, -1);

    testLruCache_->Put(31);
    testLruCache_->Put(32);
    res = testLruCache_->Put(33);
    ASSERT_EQ(res, 30);
}

/**
 * @tc.name: Remove
 * @tc.desc: test function Remove
 * @tc.type: FUNC
 */
HWTEST_F(LruCacheTest, Remove, TestSize.Level1)
{
    testLruCache_ = std::make_unique<LruCache>(TEST_CACHE_CAPACITY);
    int32_t res = testLruCache_->Put(30);
    ASSERT_EQ(res, -1);

    testLruCache_->Put(31);
    testLruCache_->Put(32);
    testLruCache_->Remove(32);
    res = testLruCache_->Put(33);
    ASSERT_EQ(res, -1);
}
} // namespace Rosen
} // namespace OHOS
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

#include "scene_persistence.h"
#include "session.h"
#include <gtest/gtest.h>
#include "session_info.h"
#include "ws_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScenePersistenceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

std::string ScenePersistence::snapshotDirectory_;
std::string ScenePersistence::updatedIconDirectory_;
constexpr const char* UNDERLINE_SEPARATOR = "_";
constexpr const char* IMAGE_SUFFIX = ".jpg";

void ScenePersistenceTest::SetUpTestCase()
{
}

void ScenePersistenceTest::TearDownTestCase()
{
}

void ScenePersistenceTest::SetUp()
{
}

void ScenePersistenceTest::TearDown()
{
}

namespace {
/**
 * @tc.name: CreateSnapshotDir
 * @tc.desc: test function : CreateSnapshotDir
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, CreateSnapshotDir, Function | SmallTest | Level1)
{
    std::string directory = "0/Storage";
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    ScenePersistence scenPersistence(bundleName, persistentId);
    bool result = scenPersistence.CreateSnapshotDir(directory);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CreateUpdatedIconDir
 * @tc.desc: test function : CreateUpdatedIconDir
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, CreateUpdatedIconDir, Function | SmallTest | Level1)
{
    std::string directory = "0/Storage";
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    ScenePersistence scenPersistence(bundleName, persistentId);
    bool result = scenPersistence.CreateUpdatedIconDir(directory);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: SaveSnapshot
 * @tc.desc: test function : SaveSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, SaveSnapshot, Function | SmallTest | Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    std::string directory = "0/Storage";
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    ScenePersistence scenPersistence(bundleName, persistentId);
    scenPersistence.SaveSnapshot(pixelMap);
    SessionInfo info;
    info.bundleName_ = "bundleName";
    Session session(info);
    pixelMap = session.GetSnapshot();
    scenPersistence.SaveSnapshot(pixelMap);
    std::string result(scenPersistence.GetSnapshotFilePath());
    uint32_t fileID = static_cast<uint32_t>(persistentId) & 0x3fffffff;
    std::string test = ScenePersistence::snapshotDirectory_ +
        bundleName + UNDERLINE_SEPARATOR + std::to_string(fileID) + IMAGE_SUFFIX;
    EXPECT_EQ(result.compare(test), 0);
    std::pair<uint32_t, uint32_t> sizeResult = scenPersistence.GetSnapshotSize();
    EXPECT_EQ(sizeResult.first, 0);
    EXPECT_EQ(sizeResult.second, 0);
}

/**
 * @tc.name: SaveUpdatedIcon
 * @tc.desc: test function : SaveUpdatedIcon
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, SaveUpdatedIcon, Function | SmallTest | Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    std::string directory = "0/Storage";
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    ScenePersistence scenPersistence(bundleName, persistentId);
    scenPersistence.SaveUpdatedIcon(pixelMap);
    SessionInfo info;
    info.bundleName_ = "bundleName";
    Session session(info);
    pixelMap = session.GetSnapshot();
    scenPersistence.SaveUpdatedIcon(pixelMap);
    std::string result(scenPersistence.GetUpdatedIconPath());
    std::string test = ScenePersistence::updatedIconDirectory_ + bundleName + IMAGE_SUFFIX;
    EXPECT_EQ(result.compare(test), 0);
}

/**
 * @tc.name: IsSnapshotExisted
 * @tc.desc: test function : IsSnapshotExisted
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, IsSnapshotExisted, Function | SmallTest | Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    ScenePersistence scenPersistence(bundleName, persistentId);
    bool result = scenPersistence.IsSnapshotExisted();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetLocalSnapshotPixelMap
 * @tc.desc: test function : get local snapshot pixelmap
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, GetLocalSnapshotPixelMap, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    Session session(info);
    auto abilityInfo = session.GetSessionInfo();
    auto persistendId = abilityInfo.persistentId_;
    ScenePersistence::CreateSnapshotDir("storage");
    ScenePersistence scenePersistence(abilityInfo.bundleName_, persistendId);
    auto result = scenePersistence.GetLocalSnapshotPixelMap();
    EXPECT_EQ(result, nullptr);

    auto pixelMap = session.GetSnapshot();
    scenePersistence.SaveSnapshot(pixelMap);
    result = scenePersistence.GetLocalSnapshotPixelMap();
    remove(scenePersistence.GetSnapshotFilePath().c_str());
    EXPECT_EQ(result, nullptr);
}
}
}
}
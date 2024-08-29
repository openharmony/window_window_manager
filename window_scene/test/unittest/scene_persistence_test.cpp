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
#include "session/host/include/scene_session.h"
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
private:
    std::shared_ptr<Media::PixelMap> mPixelMap = std::make_shared<Media::PixelMap>();
};

constexpr const char* UNDERLINE_SEPARATOR = "_";
constexpr const char* IMAGE_SUFFIX = ".jpg";

static sptr<ScenePersistence> scenePersistence = new ScenePersistence("testBundleName", 1423);

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
    ASSERT_NE(nullptr, scenePersistence);
    bool result = scenePersistence->CreateSnapshotDir(directory);
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
    ASSERT_NE(nullptr, scenePersistence);
    bool result = scenePersistence->CreateUpdatedIconDir(directory);
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

    SessionInfo info;
    info.abilityName_ = bundleName;
    info.bundleName_ = bundleName;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);

    int32_t persistentId = 1423;
    ASSERT_NE(nullptr, scenePersistence);
    scenePersistence->SaveSnapshot(pixelMap);

    sptr<Session> session = new Session(info);
    ASSERT_NE(nullptr, session);
    scenePersistence->snapshotPath_ = "/data/1.png";

    scenePersistence->SaveSnapshot(mPixelMap);
    uint32_t fileID = static_cast<uint32_t>(persistentId) & 0x3fffffff;
    std::string test = ScenePersistence::snapshotDirectory_ +
        bundleName + UNDERLINE_SEPARATOR + std::to_string(fileID) + IMAGE_SUFFIX;
    std::pair<uint32_t, uint32_t> sizeResult = scenePersistence->GetSnapshotSize();
    EXPECT_EQ(sizeResult.first, 0);
    EXPECT_EQ(sizeResult.second, 0);
}

/**
 * @tc.name: RenameSnapshotFromOldPersistentId
 * @tc.desc: test function : RenameSnapshotFromOldPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, RenameSnapshotFromOldPersistentId, Function | SmallTest | Level3)
{
    int ret = 0;
    int32_t persistentId = 1424;
    std::string bundleName = "testBundleName";
    sptr<ScenePersistence> scenePersistence2 = new ScenePersistence(bundleName, persistentId);
    scenePersistence2->RenameSnapshotFromOldPersistentId(persistentId);
    ASSERT_EQ(ret, 0);

    sptr<ScenePersistence> scenePersistence3 = new ScenePersistence(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence3);
    scenePersistence3->snapshotPath_ = "/data/1.png";
    scenePersistence3->RenameSnapshotFromOldPersistentId(persistentId);
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
    ASSERT_NE(nullptr, scenePersistence);
    scenePersistence->SaveUpdatedIcon(pixelMap);
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sptr<Session> session = new Session(info);
    ASSERT_NE(nullptr, session);
    scenePersistence->SaveUpdatedIcon(mPixelMap);
    std::string result(scenePersistence->GetUpdatedIconPath());
    std::string test = ScenePersistence::updatedIconDirectory_ + bundleName + IMAGE_SUFFIX;
    EXPECT_NE(result.compare(test), 0);
}

/**
 * @tc.name: SaveUpdatedIcon02
 * @tc.desc: test function : SaveUpdatedIcon02
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, SaveUpdatedIcon02, Function | SmallTest | Level1)
{
    std::string directory = "0/Storage";
    std::string bundleName = "testBundleName";
    ASSERT_NE(nullptr, scenePersistence);
    scenePersistence->snapshotPath_ = "/data/1.png";
    ASSERT_NE(nullptr, mPixelMap);
    scenePersistence->SaveUpdatedIcon(mPixelMap);
    std::string result(scenePersistence->GetUpdatedIconPath());
    std::string test = ScenePersistence::updatedIconDirectory_ + bundleName + IMAGE_SUFFIX;
    EXPECT_EQ(result.compare(test), 1);
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
    sptr<ScenePersistence> scenePersistence = new ScenePersistence(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    bool result = scenePersistence->IsSnapshotExisted();
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
    sptr<Session> session = new Session(info);
    ASSERT_NE(nullptr, session);
    auto abilityInfo = session->GetSessionInfo();
    auto persistentId = abilityInfo.persistentId_;
    ScenePersistence::CreateSnapshotDir("storage");
    sptr<ScenePersistence> scenePersistence =
        new ScenePersistence(abilityInfo.bundleName_, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    auto result = scenePersistence->GetLocalSnapshotPixelMap(0.5, 0.5);
    EXPECT_EQ(result, nullptr);

    bool result2 = scenePersistence->IsSnapshotExisted();
    EXPECT_EQ(result2, false);

    // create pixelMap
    const uint32_t colors[1] = { 0x6f0000ff };
    constexpr uint32_t COMMON_SIZE = 1;
    uint32_t colorsLength = sizeof(colors) / sizeof(colors[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    opts.size.width = COMMON_SIZE;
    opts.size.height = COMMON_SIZE;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    int32_t stride = opts.size.width;
    std::shared_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(colors, colorsLength, offset, stride, opts);

    scenePersistence->SaveSnapshot(pixelMap1);
    int maxScenePersistencePollNum = 100;
    scenePersistence->snapshotPath_ = "/data/1.png";
    for (int i = 0; i < maxScenePersistencePollNum; i++) {
        result = scenePersistence->GetLocalSnapshotPixelMap(0.8, 0.2);
        result2 = scenePersistence->IsSnapshotExisted();
    }
    EXPECT_NE(result, nullptr);
    ASSERT_EQ(result2, true);

    result = scenePersistence->GetLocalSnapshotPixelMap(0.0, 0.2);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: IsSavingSnapshot
 * @tc.desc: test function : IsSavingSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, IsSavingSnapshot, Function | SmallTest | Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = new ScenePersistence(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    bool result = scenePersistence->IsSavingSnapshot();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetSnapshotFilePath
 * @tc.desc: test function : GetSnapshotFilePath
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, GetSnapshotFilePath, Function | SmallTest | Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = new ScenePersistence(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    scenePersistence->RenameSnapshotFromOldPersistentId(0);
    auto result = scenePersistence->GetSnapshotFilePath();
    ASSERT_EQ(result, scenePersistence->snapshotPath_);
}

/**
 * @tc.name: HasSnapshot
 * @tc.desc: test function: HasSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, HasSnapshot, Function | SmallTest | Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = new ScenePersistence(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    scenePersistence->SetHasSnapshot(true);
    ASSERT_EQ(scenePersistence->HasSnapshot(), true);
    scenePersistence->SetHasSnapshot(false);
    ASSERT_EQ(scenePersistence->HasSnapshot(), false);
}
}
}
}
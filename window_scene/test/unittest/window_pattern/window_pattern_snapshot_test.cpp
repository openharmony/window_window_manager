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
#include <regex>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
#include "screen_fold_data.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "application_info.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
constexpr uint32_t COMMON_SIZE = 1;

class WindowPatternSnapshotTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<SceneSessionManager> ssm_;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    sptr<Session> session_ = nullptr;
    std::shared_ptr<Media::PixelMap> mPixelMap = std::make_shared<Media::PixelMap>();
};


constexpr const char* UNDERLINE_SEPARATOR = "_";
constexpr const char* IMAGE_SUFFIX = ".jpg";

sptr<SceneSessionManager> WindowPatternSnapshotTest::ssm_ = nullptr;
static sptr<ScenePersistence> scenePersistence = sptr<ScenePersistence>::MakeSptr("WindowPatternSnapshotTest", 1423);

void WindowPatternSnapshotTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void WindowPatternSnapshotTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void WindowPatternSnapshotTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    session_ = sptr<Session>::MakeSptr(info);
    session_->surfaceNode_ = CreateRSSurfaceNode();
    EXPECT_NE(nullptr, session_);
    ssm_->sceneSessionMap_.clear();
}

void WindowPatternSnapshotTest::TearDown()
{
    session_ = nullptr;
    ssm_->sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
}

RSSurfaceNode::SharedPtr WindowPatternSnapshotTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTest2SurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (surfaceNode == nullptr) {
        GTEST_LOG_(INFO) << "WindowSessionTest2::CreateRSSurfaceNode surfaceNode is nullptr";
    }
    return surfaceNode;
}

namespace {
/**
 * @tc.name: CreateSnapshotDir
 * @tc.desc: test function : CreateSnapshotDir
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, CreateSnapshotDir, TestSize.Level1)
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
HWTEST_F(WindowPatternSnapshotTest, CreateUpdatedIconDir, TestSize.Level1)
{
    std::string directory = "0/Storage";
    ASSERT_NE(nullptr, scenePersistence);
    bool result = scenePersistence->CreateUpdatedIconDir(directory);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: scenePersistence::SaveSnapshot
 * @tc.desc: test function : scenePersistence::SaveSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, SaveSnapshot01, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    std::string directory = "0/Storage";
    std::string bundleName = "testBundleName";

    SessionInfo info;
    info.abilityName_ = bundleName;
    info.bundleName_ = bundleName;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    int32_t persistentId = 1423;
    ASSERT_NE(nullptr, scenePersistence);
    scenePersistence->SaveSnapshot(pixelMap);

    sptr<Session> session = sptr<Session>::MakeSptr(info);
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
HWTEST_F(WindowPatternSnapshotTest, RenameSnapshotFromOldPersistentId, TestSize.Level1)
{
    int ret = 0;
    int32_t persistentId = 1424;
    std::string bundleName = "testBundleName";
    sptr<ScenePersistence> scenePersistence2 = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    scenePersistence2->RenameSnapshotFromOldPersistentId(persistentId);
    ASSERT_EQ(ret, 0);

    sptr<ScenePersistence> scenePersistence3 = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence3);
    scenePersistence3->snapshotPath_ = "/data/1.png";
    scenePersistence3->RenameSnapshotFromOldPersistentId(persistentId);
}

/**
 * @tc.name: IsSnapshotExisted
 * @tc.desc: test function : IsSnapshotExisted
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, IsSnapshotExisted, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    bool result = scenePersistence->IsSnapshotExisted();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetLocalSnapshotPixelMap
 * @tc.desc: test function : get local snapshot pixelmap
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, GetLocalSnapshotPixelMap, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    sptr<Session> session = sptr<Session>::MakeSptr(info);
    ASSERT_NE(nullptr, session);
    auto abilityInfo = session->GetSessionInfo();
    auto persistentId = abilityInfo.persistentId_;
    ScenePersistence::CreateSnapshotDir("storage");
    sptr<ScenePersistence> scenePersistence =
        sptr<ScenePersistence>::MakeSptr(abilityInfo.bundleName_, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    auto result = scenePersistence->GetLocalSnapshotPixelMap(0.5, 0.5);
    EXPECT_EQ(result, nullptr);

    bool result2 = scenePersistence->IsSnapshotExisted();
    EXPECT_EQ(result2, false);

    // create pixelMap
    const uint32_t colors[1] = { 0x6f0000ff };
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
HWTEST_F(WindowPatternSnapshotTest, IsSavingSnapshot, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    bool result = scenePersistence->IsSavingSnapshot();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetSnapshotFilePath
 * @tc.desc: test function : GetSnapshotFilePath
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, GetSnapshotFilePath, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
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
HWTEST_F(WindowPatternSnapshotTest, HasSnapshot, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    ASSERT_NE(nullptr, scenePersistence);
    scenePersistence->SetHasSnapshot(true);
    ASSERT_EQ(scenePersistence->HasSnapshot(), true);
    scenePersistence->SetHasSnapshot(false);
    ASSERT_EQ(scenePersistence->HasSnapshot(), false);
}

/**
 * @tc.name: SetSaveSnapshotCallback
 * @tc.desc: test function: SetSaveSnapshotCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, SetSaveSnapshotCallback, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    SessionInfo info;
    info.abilityName_ = bundleName;
    info.bundleName_ = bundleName;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession->saveSnapshotCallback_);
    sceneSession->SetSaveSnapshotCallback(nullptr);
    ASSERT_NE(nullptr, sceneSession->saveSnapshotCallback_);
}

/**
 * @tc.name: SetRemoveSnapshotCallback
 * @tc.desc: test function: SetRemoveSnapshotCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, SetRemoveSnapshotCallback, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    SessionInfo info;
    info.abilityName_ = bundleName;
    info.bundleName_ = bundleName;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession->removeSnapshotCallback_);
    sceneSession->SetRemoveSnapshotCallback(nullptr);
    ASSERT_NE(nullptr, sceneSession->removeSnapshotCallback_);
}

/**
 * @tc.name: SetAddSnapshotCallback
 * @tc.desc: test function: SetAddSnapshotCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, SetAddSnapshotCallback, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    SessionInfo info;
    info.abilityName_ = bundleName;
    info.bundleName_ = bundleName;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession->addSnapshotCallback_);
    sceneSession->SetAddSnapshotCallback(nullptr);
    ASSERT_NE(nullptr, sceneSession->addSnapshotCallback_);
}

/**
 * @tc.name: NotifyAddSnapshot
 * @tc.desc: test function: NotifyAddSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, NotifyAddSnapshot, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    SessionInfo info;
    info.abilityName_ = bundleName;
    info.bundleName_ = bundleName;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession->addSnapshotCallback_);
    sceneSession->NotifyAddSnapshot(true);
    ASSERT_NE(nullptr, sceneSession->addSnapshotCallback_);
}

/**
 * @tc.name: ResetSnapshotCache
 * @tc.desc: test function: ResetSnapshotCache
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, ResetSnapshotCache, TestSize.Level1)
{
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    sptr<ScenePersistence> scenePersistence = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    scenePersistence->ResetSnapshotCache();
    ASSERT_EQ(scenePersistence->isSavingSnapshot_, false);
}

/**
 * @tc.name: GetSessionSnapshotPixelMap01
 * @tc.desc: SceneSesionManager get session snapshot pixelmap
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, GetSessionSnapshotPixelMap01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);

    int32_t persistentId = 65535;
    float scaleValue = 0.5f;
    auto pixelMap = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleValue);
    EXPECT_EQ(pixelMap, nullptr);
}

/**
 * @tc.name: GetSessionSnapshotPixelMap
 * @tc.desc: GetSessionSnapshotPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, GetSessionSnapshotPixelMap02, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    int32_t persistentId = 1;
    float scaleParam = 0.5f;
    auto result = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleParam);
    EXPECT_EQ(result, nullptr);

    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    std::string bundleName = "testBundleName";
    int32_t testpersistentId = 1;
    sceneSession->scenePersistence_ = sptr<ScenePersistence>::MakeSptr(bundleName, testpersistentId);
    ASSERT_NE(sceneSession->scenePersistence_, nullptr);
    struct RSSurfaceNodeConfig config;
    sceneSession->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession->surfaceNode_, nullptr);
    sceneSession->bufferAvailable_ = true;
    result = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleParam);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetSnapshot
 * @tc.desc: GetSnapshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, GetSnapshot, TestSize.Level1)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    std::shared_ptr<Media::PixelMap> snapshot = session_->Snapshot();
    ASSERT_EQ(snapshot, session_->GetSnapshot());
}

/**
 * @tc.name: NotifyAddSnapshot
 * @tc.desc: NotifyAddSnapshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, NotifyAddSnapshot02, TestSize.Level1)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->NotifyAddSnapshot();
    ASSERT_EQ(session_->GetSnapshot(), nullptr);
}

/**
 * @tc.name: NotifyRemoveSnapshot
 * @tc.desc: NotifyRemoveSnapshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, NotifyRemoveSnapshot, TestSize.Level1)
{
    ASSERT_NE(session_, nullptr);
    session_->scenePersistence_ = sptr<ScenePersistence>::MakeSptr("bundleName", 1);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->NotifyRemoveSnapshot();
    ASSERT_EQ(session_->GetScenePersistence()->HasSnapshot(), false);
}


/**
 * @tc.name: Snapshot01
 * @tc.desc: ret is false
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, Snapshot01, TestSize.Level1)
{
    ASSERT_NE(session_, nullptr);

    session_->surfaceNode_ = nullptr;

    ASSERT_EQ(nullptr, session_->Snapshot());
}

/**
 * @tc.name: Snapshot
 * @tc.desc: Snapshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, Snapshot02, TestSize.Level1)
{
    ASSERT_NE(session_, nullptr);
    int32_t persistentId = 1424;
    std::string bundleName = "testBundleName";
    session_->scenePersistence_ = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    ASSERT_NE(session_->scenePersistence_, nullptr);
    struct RSSurfaceNodeConfig config;
    session_->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(session_->surfaceNode_, nullptr);
    EXPECT_EQ(nullptr, session_->Snapshot(false, 0.0f));

    session_->bufferAvailable_ = true;
    EXPECT_EQ(nullptr, session_->Snapshot(false, 0.0f));

    session_->surfaceNode_->bufferAvailable_ = true;
    EXPECT_EQ(nullptr, session_->Snapshot(false, 0.0f));

    session_->surfaceNode_ = nullptr;
    EXPECT_EQ(nullptr, session_->Snapshot(false, 0.0f));
}

/**
 * @tc.name: ResetSnapshot
 * @tc.desc: ResetSnapshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, ResetSnapshot, TestSize.Level1)
{
    ASSERT_NE(session_, nullptr);
    std::string bundleName = "testBundleName";
    int32_t persistentId = 1423;
    session_->scenePersistence_ = sptr<ScenePersistence>::MakeSptr(bundleName, persistentId);
    session_->snapshot_ = std::make_shared<Media::PixelMap>();

    session_->ResetSnapshot();
    ASSERT_EQ(nullptr, session_->snapshot_);
}

/**
 * @tc.name: Session::SaveSnapshot
 * @tc.desc: Session::SaveSnapshot Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, SaveSnapshot02, TestSize.Level1)
{
    ASSERT_NE(session_, nullptr);

    session_->scenePersistence_ = nullptr;
    session_->snapshot_ = nullptr;
    session_->SaveSnapshot(true);
    EXPECT_EQ(session_->snapshot_, nullptr);

    session_->scenePersistence_ =
        sptr<ScenePersistence>::MakeSptr(session_->sessionInfo_.bundleName_, session_->persistentId_);

    session_->SaveSnapshot(false);
    ASSERT_EQ(session_->snapshot_, nullptr);

    session_->SaveSnapshot(true);
    ASSERT_EQ(session_->snapshot_, nullptr);
}

/**
 * @tc.name: GetSnapshotPixelMap
 * @tc.desc: GetSnapshotPixelMap Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternSnapshotTest, GetSnapshotPixelMap, TestSize.Level1)
{
    session_->scenePersistence_ = nullptr;
    EXPECT_EQ(nullptr, session_->GetSnapshotPixelMap(6.6f, 8.8f));
    session_->scenePersistence_ = sptr<ScenePersistence>::MakeSptr("GetSnapshotPixelMap", 2024);
    EXPECT_NE(nullptr, session_->scenePersistence_);
    session_->scenePersistence_->isSavingSnapshot_.store(true);
    session_->snapshot_ = nullptr;
    EXPECT_EQ(nullptr, session_->GetSnapshotPixelMap(6.6f, 8.8f));
}
}
}
}
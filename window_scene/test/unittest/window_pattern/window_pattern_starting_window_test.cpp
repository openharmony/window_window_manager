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
#include <regex>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>

#include "context.h"
#include "interfaces/include/ws_common.h"
#include "session_manager/include/rdb/starting_window_rdb_manager.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string TEST_RDB_PATH = "/data/test/";
const std::string TEST_INVALID_PATH = "";
const std::string TEST_RDB_NAME = "starting_window_config_test.db";
const uint32_t WAIT_SLEEP_TIME = 200000;
std::string g_logMsg;
void RdbLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
    const char* msg)
{
    g_logMsg = msg;
}
} // namespace

class WindowPatternStartingWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;

private:
    void InitTestStartingWindowRdb();
};

sptr<SceneSessionManager> WindowPatternStartingWindowTest::ssm_ = nullptr;

void WindowPatternStartingWindowTest::SetUpTestCase()
{
    ssm_ = new SceneSessionManager();
    ssm_->Init();
}

void WindowPatternStartingWindowTest::TearDownTestCase()
{
    ssm_ = nullptr;
    NativeRdb::RdbHelper::DeleteRdbStore(TEST_RDB_PATH + TEST_RDB_NAME);
}

void WindowPatternStartingWindowTest::SetUp()
{
}

void WindowPatternStartingWindowTest::TearDown()
{
}

void WindowPatternStartingWindowTest::InitTestStartingWindowRdb()
{
    NativeRdb::RdbHelper::DeleteRdbStore(TEST_RDB_PATH + TEST_RDB_NAME);
    WmsRdbConfig config;
    config.dbName = TEST_RDB_NAME;
    config.dbPath = TEST_RDB_PATH;
    ssm_->startingWindowRdbMgr_ = std::make_unique<StartingWindowRdbManager>(config);
}

namespace {
/**
 * @tc.name: GetStartupPage01
 * @tc.desc: GetStartupPage from want
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetStartupPage01, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    StartingWindowInfo startingWindowInfo;
    startingWindowInfo.iconPathEarlyVersion_ = "default";
    ssm_->GetStartupPage(sessionInfo, startingWindowInfo);
    ASSERT_EQ(startingWindowInfo.iconPathEarlyVersion_, "default");
    sptr<AppExecFwk::IBundleMgr> tempBundleMgr = ssm_->bundleMgr_;
    ssm_->bundleMgr_ = nullptr;
    ssm_->GetStartupPage(sessionInfo, startingWindowInfo);
    ssm_->bundleMgr_ = tempBundleMgr;
    ASSERT_EQ(startingWindowInfo.iconPathEarlyVersion_, "default");
    sessionInfo.want = std::make_shared<AAFwk::Want>();
    ASSERT_NE(sessionInfo.want, nullptr);
    const std::string pathFromDesk = "pathFromDesk";
    sessionInfo.want->SetParam("realAppIcon", pathFromDesk);
    ssm_->GetStartupPage(sessionInfo, startingWindowInfo);
    ASSERT_EQ(startingWindowInfo.iconPathEarlyVersion_, "pathFromDesk");
}

/**
 * @tc.name: GetStartupPage02
 * @tc.desc: GetStartupPage from cache
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetStartupPage02, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.moduleName_ = "moduleName";
    sessionInfo.abilityName_ = "abilityName";
    sessionInfo.bundleName_ = "bundleName";
    StartingWindowInfo cachedInfo;
    cachedInfo.iconPathEarlyVersion_ = "pathFromCache";
    ssm_->startingWindowMap_.clear();
    auto key = sessionInfo.moduleName_ + sessionInfo.abilityName_;
    std::map<std::string, StartingWindowInfo> startingWindowInfoMap{ { key, cachedInfo } };
    ssm_->startingWindowMap_.insert({ sessionInfo.bundleName_, startingWindowInfoMap });
    StartingWindowInfo outInfo;
    outInfo.iconPathEarlyVersion_ = "default";
    ssm_->GetStartupPage(sessionInfo, outInfo);
    ASSERT_EQ(outInfo.iconPathEarlyVersion_, "pathFromCache");
}

/**
 * @tc.name: GetStartupPage03
 * @tc.desc: GetStartupPage from rdb
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetStartupPage03, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    InitTestStartingWindowRdb();
    ASSERT_NE(ssm_->startingWindowRdbMgr_, nullptr);
    ssm_->startingWindowMap_.clear();
    SessionInfo sessionInfo;
    StartingWindowRdbItemKey itemKey;
    sessionInfo.moduleName_ = itemKey.moduleName = "moduleName";
    sessionInfo.abilityName_ = itemKey.abilityName = "abilityName";
    sessionInfo.bundleName_ = itemKey.bundleName = "bundleName";
    itemKey.darkMode = false;
    StartingWindowInfo rdbInfo;
    rdbInfo.iconPathEarlyVersion_ = "pathFromRdb";
    StartingWindowInfo outInfo;
    outInfo.iconPathEarlyVersion_ = "default";
    bool insertRes = ssm_->startingWindowRdbMgr_->InsertData(itemKey, rdbInfo);
    EXPECT_EQ(insertRes, true);
    ssm_->GetStartupPage(sessionInfo, outInfo);
    EXPECT_EQ(outInfo.iconPathEarlyVersion_, "pathFromRdb");
}

/**
 * @tc.name: GetStartingWindowInfoFromCache
 * @tc.desc: GetStartingWindowInfoFromCache
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetStartingWindowInfoFromCache, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->startingWindowMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.moduleName_ = "moduleName";
    sessionInfo.abilityName_ = "abilityName";
    sessionInfo.bundleName_ = "bundleName";
    StartingWindowInfo startingWindowInfo;
    auto res = ssm_->GetStartingWindowInfoFromCache(sessionInfo, startingWindowInfo);
    ASSERT_EQ(res, false);
    auto key = sessionInfo.moduleName_ + sessionInfo.abilityName_;
    std::map<std::string, StartingWindowInfo> startingWindowInfoMap{ { key, startingWindowInfo } };
    ssm_->startingWindowMap_.insert({ sessionInfo.bundleName_, startingWindowInfoMap });
    res = ssm_->GetStartingWindowInfoFromCache(sessionInfo, startingWindowInfo);
    ASSERT_EQ(res, true);
    sessionInfo.moduleName_ = "invalidModuleName";
    res = ssm_->GetStartingWindowInfoFromCache(sessionInfo, startingWindowInfo);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: GetStartingWindowInfoFromRdb
 * @tc.desc: GetStartingWindowInfoFromRdb
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetStartingWindowInfoFromRdb, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    InitTestStartingWindowRdb();
    ASSERT_NE(ssm_->startingWindowRdbMgr_, nullptr);
    NativeRdb::RdbHelper::DeleteRdbStore(TEST_RDB_PATH + TEST_RDB_NAME);
    SessionInfo sessionInfo;
    StartingWindowRdbItemKey itemKey;
    StartingWindowInfo outInfo;
    sessionInfo.moduleName_ = itemKey.moduleName = "moduleName";
    sessionInfo.abilityName_ = itemKey.abilityName = "abilityName";
    sessionInfo.bundleName_ = itemKey.bundleName = "bundleName";
    itemKey.darkMode = false;
    auto res = ssm_->GetStartingWindowInfoFromRdb(sessionInfo, outInfo);
    EXPECT_EQ(res, false);
    bool insertRes = ssm_->startingWindowRdbMgr_->InsertData(itemKey, outInfo);
    EXPECT_EQ(insertRes, true);
    res = ssm_->GetStartingWindowInfoFromRdb(sessionInfo, outInfo);
    EXPECT_EQ(res, true);
    ssm_->startingWindowRdbMgr_ = nullptr;
    res = ssm_->GetStartingWindowInfoFromRdb(sessionInfo, outInfo);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetIconFromDesk
 * @tc.desc: GetIconFromDesk
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetIconFromDesk, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    std::string outPath;
    bool res = ssm_->GetIconFromDesk(sessionInfo, outPath);
    ASSERT_EQ(res, false);
    sessionInfo.want = std::make_shared<AAFwk::Want>();
    ASSERT_NE(sessionInfo.want, nullptr);
    res = ssm_->GetIconFromDesk(sessionInfo, outPath);
    ASSERT_EQ(res, false);
    const std::string pathFromDesk = "pathFromDesk";
    sessionInfo.want->SetParam("realAppIcon", pathFromDesk);
    res = ssm_->GetIconFromDesk(sessionInfo, outPath);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: GetStartupPageFromResource
 * @tc.desc: GetStartupPageFromResource
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetStartupPageFromResource, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    AppExecFwk::AbilityInfo info;
    info.startWindowBackgroundId = 1;
    StartingWindowInfo outInfo;
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, outInfo));
    info.startWindowIconId = 1;
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, outInfo));
    info.hapPath = "hapPath";
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, outInfo));
}

/**
 * @tc.name: CacheStartingWindowInfo01
 * @tc.desc: Cache new starting window info
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, CacheStartingWindowInfo01, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->startingWindowMap_.clear();
    /**
     * @tc.steps: step1. Build input parameter.
     */
    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.name = "abilityName";
    abilityInfo.bundleName = "bundleName";
    abilityInfo.moduleName = "moduleName";
    StartingWindowInfo startingWindowInfo;
    startingWindowInfo.backgroundColorEarlyVersion_ = 0xff000000;
    startingWindowInfo.iconPathEarlyVersion_ = "cachedPath";

    /**
     * @tc.steps: step2. Cache info and check result.
     */
    ssm_->CacheStartingWindowInfo(abilityInfo.bundleName, abilityInfo.moduleName, abilityInfo.name, startingWindowInfo);
    auto iter = ssm_->startingWindowMap_.find(abilityInfo.bundleName);
    ASSERT_NE(iter, ssm_->startingWindowMap_.end());
    auto& infoMap = iter->second;
    auto infoIter = infoMap.find(abilityInfo.moduleName + abilityInfo.name);
    ASSERT_NE(infoIter, infoMap.end());
    ASSERT_EQ(infoIter->second.backgroundColorEarlyVersion_, 0xff000000);
    ASSERT_EQ(infoIter->second.iconPathEarlyVersion_, "cachedPath");
}

/**
 * @tc.name: CacheStartingWindowInfo02
 * @tc.desc: Execute when info is cached
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, CacheStartingWindowInfo02, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->startingWindowMap_.clear();
    /**
     * @tc.steps: step1. Build input parameter.
     */
    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.name = "abilityName";
    abilityInfo.bundleName = "bundleName";
    abilityInfo.moduleName = "moduleName";
    StartingWindowInfo startingWindowInfo;
    startingWindowInfo.backgroundColorEarlyVersion_ = 0xff000000;
    startingWindowInfo.iconPathEarlyVersion_ = "cachedPath";

    /**
     * @tc.steps: step2. Insert one item.
     */
    auto key = abilityInfo.moduleName + abilityInfo.name;
    StartingWindowInfo anotherStartingWindowInfo = {
        .backgroundColorEarlyVersion_ = 0x00000000,
        .iconPathEarlyVersion_ = "path",
    };
    std::map<std::string, StartingWindowInfo> startingWindowInfoMap{ { key, anotherStartingWindowInfo } };
    ssm_->startingWindowMap_.insert({ abilityInfo.bundleName, startingWindowInfoMap });

    /**
     * @tc.steps: step3. Execute and check result.
     */
    ssm_->CacheStartingWindowInfo(abilityInfo.bundleName, abilityInfo.moduleName, abilityInfo.name, startingWindowInfo);
    auto iter = ssm_->startingWindowMap_.find(abilityInfo.bundleName);
    ASSERT_NE(iter, ssm_->startingWindowMap_.end());
    auto& infoMap = iter->second;
    auto infoIter = infoMap.find(abilityInfo.moduleName + abilityInfo.name);
    ASSERT_NE(infoIter, infoMap.end());
    ASSERT_NE(infoIter->second.backgroundColorEarlyVersion_, 0xff000000);
    ASSERT_NE(infoIter->second.iconPathEarlyVersion_, "cachedPath");
}

/**
 * @tc.name: GetPathInfoFromResource
 * @tc.desc: GetPathInfoFromResource
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetPathInfoFromResource, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    uint32_t resourceId = 0;
    std::string path;
    bool res = ssm_->GetPathInfoFromResource(nullptr, true, resourceId, path);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: GetBundleStartingWindowInfos
 * @tc.desc: GetBundleStartingWindowInfos
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetBundleStartingWindowInfos, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    AppExecFwk::BundleInfo bundleInfo;
    std::vector<std::pair<StartingWindowRdbItemKey, StartingWindowInfo>> outValues;
    ssm_->GetBundleStartingWindowInfos(false, bundleInfo, outValues);
    EXPECT_EQ(outValues.size(), 0);
}

/**
 * @tc.name: GetPreLoadStartingWindow
 * @tc.desc: GetPreLoadStartingWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetPreLoadStartingWindow, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->preLoadStartingWindowMap_.clear();
    SessionInfo sessionInfo;
    ASSERT_EQ(nullptr, ssm_->GetPreLoadStartingWindow(sessionInfo));
    sessionInfo.bundleName_ = "bundleName_";
    sessionInfo.moduleName_ = "moduleName_";
    sessionInfo.abilityName_ = "abilityName_";
    std::string key = sessionInfo.bundleName_ + '_' + sessionInfo.moduleName_ + '_' +sessionInfo.abilityName_;
    ssm_->preLoadStartingWindowMap_[key] = std::make_shared<Media::PixelMap>();
    ASSERT_NE(nullptr, ssm_->GetPreLoadStartingWindow(sessionInfo));
    ssm_->preLoadStartingWindowMap_.clear();
    EXPECT_EQ(true, ssm_->preLoadStartingWindowMap_.empty());
}

/**
 * @tc.name: RemovePreLoadStartingWindowFromMap
 * @tc.desc: RemovePreLoadStartingWindowFromMap
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, RemovePreLoadStartingWindowFromMap, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->preLoadStartingWindowMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName_";
    sessionInfo.moduleName_ = "moduleName_";
    sessionInfo.abilityName_ = "abilityName_";
    std::string key = sessionInfo.bundleName_ + '_' + sessionInfo.moduleName_ + '_' +sessionInfo.abilityName_;
    ssm_->preLoadStartingWindowMap_[key] = std::make_shared<Media::PixelMap>();
    SessionInfo anotherSessionInfo;
    ssm_->RemovePreLoadStartingWindowFromMap(anotherSessionInfo);
    EXPECT_EQ(false, ssm_->preLoadStartingWindowMap_.empty());
    ssm_->RemovePreLoadStartingWindowFromMap(sessionInfo);
    EXPECT_EQ(true, ssm_->preLoadStartingWindowMap_.empty());
}

/**
 * @tc.name: EraseSceneSessionMapById
 * @tc.desc: release preload starting window when EraseSceneSessionMapById
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, EraseSceneSessionMapById, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->preLoadStartingWindowMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName_";
    sessionInfo.moduleName_ = "moduleName_";
    sessionInfo.abilityName_ = "abilityName_";
    std::string key = sessionInfo.bundleName_ + '_' + sessionInfo.moduleName_ + '_' +sessionInfo.abilityName_;
    ssm_->preLoadStartingWindowMap_[key] = std::make_shared<Media::PixelMap>();
    EXPECT_EQ(false, ssm_->preLoadStartingWindowMap_.empty());
    ssm_->EraseSceneSessionMapById(0);
    EXPECT_EQ(false, ssm_->preLoadStartingWindowMap_.empty());
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ASSERT_NE(nullptr, ssm_->GetSceneSession(sceneSession->GetPersistentId()));
    ssm_->EraseSceneSessionMapById(sceneSession->GetPersistentId());
    ASSERT_EQ(nullptr, ssm_->GetSceneSession(sceneSession->GetPersistentId()));
    EXPECT_EQ(true, ssm_->preLoadStartingWindowMap_.empty());
}

/**
 * @tc.name: CheckAndGetPreLoadResourceId
 * @tc.desc: CheckAndGetPreLoadResourceId
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, CheckAndGetPreLoadResourceId, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    StartingWindowInfo startingWindowInfo;
    uint32_t resId = 0;
    startingWindowInfo.configFileEnabled_ = true;
    EXPECT_EQ(false, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
    startingWindowInfo.configFileEnabled_ = false;
    EXPECT_EQ(false, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
    startingWindowInfo.iconPathEarlyVersion_ = "resource:///12345678.svg";
    EXPECT_EQ(false, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
    startingWindowInfo.iconPathEarlyVersion_ = "resource:///abc12345678.jpg";
    EXPECT_EQ(false, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
    startingWindowInfo.iconPathEarlyVersion_ = "resource:///12345678.png";
    EXPECT_EQ(true, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
    startingWindowInfo.iconPathEarlyVersion_ = "resource:///12345678.jpg";
    EXPECT_EQ(true, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
    startingWindowInfo.iconPathEarlyVersion_ = "resource:///12345678.webp";
    EXPECT_EQ(true, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
    startingWindowInfo.iconPathEarlyVersion_ = "resource:///12345678.astc";
    EXPECT_EQ(true, ssm_->CheckAndGetPreLoadResourceId(startingWindowInfo, resId));
}

/**
 * @tc.name: PreLoadStartingWindow
 * @tc.desc: PreLoadStartingWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, PreLoadStartingWindow, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ssm_->PreLoadStartingWindow(sceneSession);
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ssm_->PreLoadStartingWindow(sceneSession);
    SessionInfo info;
    info.bundleName_ = "bundleName_";
    info.moduleName_ = "moduleName_";
    info.abilityName_ = "abilityName_";
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->PreLoadStartingWindow(sceneSession);
    ASSERT_NE(nullptr, sceneSession);
    EXPECT_EQ(false, ssm_->needUpdateRdb_);
}

/**
 * @tc.name: NotifyPreLoadStartingWindowFinished
 * @tc.desc: NotifyPreLoadStartingWindowFinished
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, NotifyPreLoadStartingWindowFinished, TestSize.Level1)
{
    SessionInfo info;
    info.bundleName_ = "bundleName_";
    info.moduleName_ = "moduleName_";
    info.abilityName_ = "abilityName_";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->NotifyPreLoadStartingWindowFinished();
    ASSERT_NE(nullptr, sceneSession);
}

/**
 * @tc.name: GetIsDarkFromConfiguration
 * @tc.desc: GetIsDarkFromConfiguration
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, GetIsDarkFromConfiguration, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    bool isDark = ssm_->GetIsDarkFromConfiguration();
    EXPECT_EQ(false, isDark);
}

/**
 * @tc.name: UpdateAllStartingWindowRdb
 * @tc.desc: UpdateAllStartingWindowRdb
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowTest, UpdateAllStartingWindowRdb, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(RdbLogCallback);
    ASSERT_NE(ssm_, nullptr);
    ssm_->UpdateAllStartingWindowRdb();
    usleep(WAIT_SLEEP_TIME);
    EXPECT_TRUE(g_logMsg.find("GetBundleInfosV9 error") != std::string::npos);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
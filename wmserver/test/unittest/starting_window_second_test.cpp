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
#include <gmock/gmock.h>
#include <bundle_mgr_interface.h>
#include <filesystem>
#include <fstream>

#include "mock_resource_manager.h"
#include "starting_window.h"

using namespace testing;
using namespace testing::ext;
namespace {
constexpr size_t MAX_SOURCE_SIZE = 300 * 1024 * 1024;
const uint8_t RAW_PNG_DATA[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x15, 0xC4,
    0x89, 0x00, 0x00, 0x00, 0x0A, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0x63, 0x62, 0x00, 0x04, 0x00,
    0x00, 0xFF, 0xFF, 0x00, 0x06, 0x00, 0x03, 0x57, 0x0A, 0xC9, 0xD4, 0x00, 0x00, 0x00, 0x00, 0x49,
    0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82
};
}

namespace OHOS {
namespace AppExecFwk {
class MockIBundleMgr : public IBundleMgr {
public:
    MockIBundleMgr() = default;
    virtual ~MockIBundleMgr() = default;
    MOCK_METHOD4(QueryAbilityInfo, bool(const Want &want, int32_t flags, int32_t userId, AbilityInfo &abilityInfo));
    virtual OHOS::sptr<OHOS::IRemoteObject> AsObject()
    {
        return nullptr;
    }
};
}

namespace Rosen {
class StartingWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void StartingWindowTest::SetUpTestCase()
{
}

void StartingWindowTest::TearDownTestCase()
{
}

void StartingWindowTest::SetUp()
{
}

void StartingWindowTest::TearDown()
{
}

/**
 * @tc.name: GetBundleManager01
 * @tc.desc: GetBundleManager
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, GetBundleManager01, TestSize.Level1)
{
    auto bundleMgr = StartingWindow::GetBundleManager();
    EXPECT_NE(bundleMgr, nullptr);
}

/**
 * @tc.name: CreateResourceManager01
 * @tc.desc: CreateResourceManager
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, CreateResourceManager01, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = nullptr;
    auto ref = StartingWindow::CreateResourceManager(abilityInfo);
    EXPECT_EQ(ref, nullptr);

    abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    abilityInfo->bundleName = "com.ohos.test.bundleName";
    abilityInfo->moduleName = "com.ohos.test.moduleName";
    abilityInfo->hapPath = "data/resource/testHapPath";

    ref = StartingWindow::CreateResourceManager(abilityInfo);
    EXPECT_NE(ref, nullptr);
    EXPECT_EQ(ref->bundleInfo.first, abilityInfo->bundleName);
    EXPECT_EQ(ref->bundleInfo.second, abilityInfo->moduleName);
}

/**
 * @tc.name: DrawStartingWindow01
 * @tc.desc: DrawStartingWindow
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DrawStartingWindow01, TestSize.Level1)
{
    sptr<WindowNode> node = nullptr;
    Rosen::Rect rect {0, 0, 0, 0};
    StartingWindow::startingWindowPageDrawInfo_ = nullptr;
    auto ret = StartingWindow::DrawStartingWindow(node, rect);
    EXPECT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    sptr<WindowTransitionInfo> winInfo = sptr<WindowTransitionInfo>::MakeSptr();
    node = StartingWindow::CreateWindowNode(winInfo, 1);
    ASSERT_NE(node, nullptr);
    ret = StartingWindow::DrawStartingWindow(node, rect);
    EXPECT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    auto info = std::make_shared<Rosen::StartingWindowPageDrawInfo>();
    info->bgColor = 0x00660000;
    StartingWindow::startingWindowPageDrawInfo_ = info;
    ret = StartingWindow::DrawStartingWindow(node, rect);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);

    Rosen::RSSurfaceNodeConfig config;
    node->startingWinSurfaceNode_ = Rosen::RSSurfaceNode::Create(config);
    ASSERT_NE(node->startingWinSurfaceNode_, nullptr);
    ret = StartingWindow::DrawStartingWindow(node, rect);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);

    rect = {0, 0, 200, 600};
    ret = StartingWindow::DrawStartingWindow(node, rect);
    EXPECT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: GetAbilityInfoFromBMS01
 * @tc.desc: GetAbilityInfoFromBMS
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, GetAbilityInfoFromBMS01, TestSize.Level1)
{
    auto ref = StartingWindow::GetAbilityInfoFromBMS(nullptr, nullptr);
    EXPECT_EQ(ref, nullptr);

    sptr<WindowTransitionInfo> winInfo = sptr<WindowTransitionInfo>::MakeSptr();
    sptr<WindowNode> node = StartingWindow::CreateWindowNode(winInfo, 1);
    node->abilityInfo_.abilityName_ = "com.ohos.test.abilityName";
    node->abilityInfo_.bundleName_ = "com.ohos.test.bundleName";
    ASSERT_NE(node, nullptr);
    ref = StartingWindow::GetAbilityInfoFromBMS(node, nullptr);
    EXPECT_EQ(ref, nullptr);

    auto mockBundleMgr = sptr<AppExecFwk::MockIBundleMgr>::MakeSptr();
    EXPECT_CALL(*mockBundleMgr, QueryAbilityInfo(_, _, _, _))
        .Times(1).WillOnce(Return(false));
    ref = StartingWindow::GetAbilityInfoFromBMS(node, mockBundleMgr);
    EXPECT_EQ(ref, nullptr);

    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.name = node->abilityInfo_.abilityName_;
    EXPECT_CALL(*mockBundleMgr, QueryAbilityInfo(_, _, _, _))
        .Times(1).WillOnce(DoAll(
            SetArgReferee<3>(abilityInfo),
            Return(true)));
    ref = StartingWindow::GetAbilityInfoFromBMS(node, mockBundleMgr);
    EXPECT_NE(ref, nullptr);
    EXPECT_EQ(ref->name, node->abilityInfo_.abilityName_);
}

/**
 * @tc.name: GetPixelMap01
 * @tc.desc: GetPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, GetPixelMap01, TestSize.Level1)
{
    std::shared_ptr<Global::Resource::ResourceManager> resourceMgr = nullptr;
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = nullptr;
    auto ref = StartingWindow::GetPixelMap(1, resourceMgr, abilityInfo);
    EXPECT_EQ(ref, nullptr);

    abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    ref = StartingWindow::GetPixelMap(1, resourceMgr, abilityInfo);
    EXPECT_EQ(ref, nullptr);

    resourceMgr = StartingWindow::CreateResourceManager(abilityInfo);
    ref = StartingWindow::GetPixelMap(1, resourceMgr, abilityInfo);
    EXPECT_EQ(ref, nullptr);

    ref = StartingWindow::GetPixelMap(std::numeric_limits<uint32_t>::min(), resourceMgr, abilityInfo);
    EXPECT_EQ(ref, nullptr);

    ref = StartingWindow::GetPixelMap(-1, resourceMgr, abilityInfo);
    EXPECT_EQ(ref, nullptr);
}

/**
 * @tc.name: GetPixelMap02
 * @tc.desc: GetPixelMap With inValid Resource
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, GetPixelMap02, TestSize.Level1)
{
    auto mockResourceManager = std::make_shared<OHOS::Global::Resource::MockResourceManager>();
    auto abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    uint32_t mediaDataId = std::numeric_limits<uint32_t>::max();

    abilityInfo->hapPath = "";
    EXPECT_CALL(*mockResourceManager, GetMediaById(_, _, _))
        .Times(1).WillOnce(Return(Global::Resource::RState::NOT_FOUND));
    auto ref = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_EQ(ref, nullptr);

    EXPECT_CALL(*mockResourceManager, GetMediaById(_, _, _))
        .Times(1).WillOnce(Return(Global::Resource::RState::SUCCESS));
    ref = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_EQ(ref, nullptr);

    abilityInfo->hapPath = "/home/somepath/somefile";
    EXPECT_CALL(*mockResourceManager, GetMediaDataById(_, _, _, _))
        .Times(1).WillOnce(Return(Global::Resource::RState::NOT_FOUND));
    ref = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_EQ(ref, nullptr);

    EXPECT_CALL(*mockResourceManager, GetMediaDataById(_, _, _, _))
        .Times(1).WillOnce(Return(Global::Resource::RState::SUCCESS));
    ref = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_EQ(ref, nullptr);
}

/**
 * @tc.name: GetPixelMap03
 * @tc.desc: GetPixelMap With Resource
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, GetPixelMap03, TestSize.Level1)
{
    auto mockResourceManager = std::make_shared<OHOS::Global::Resource::MockResourceManager>();
    auto abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    uint32_t mediaDataId = 1;
    abilityInfo->hapPath = "/home/somepath/somefile";

    const size_t pngDataSize = sizeof(RAW_PNG_DATA);
    auto testData = std::make_unique<uint8_t[]>(pngDataSize);
    for (size_t i = 0; i < pngDataSize; ++i) {
        testData[i] = RAW_PNG_DATA[i];
    }

    EXPECT_CALL(*mockResourceManager, GetMediaDataById(_, _, _, _))
    .WillOnce(DoAll(
        SetArgReferee<1>(pngDataSize),
        testing::Invoke([&testData](uint32_t, size_t&, std::unique_ptr<uint8_t[]>& dataOut, uint32_t) {
            dataOut = std::move(testData);
        }),
        Return(Global::Resource::RState::SUCCESS)
    ));
    auto ref = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_NE(ref, nullptr);

    //SOURCE_SIZE is bigger than MAX_SOURCE_SIZE
    EXPECT_CALL(*mockResourceManager, GetMediaDataById(_, _, _, _))
    .WillOnce(DoAll(
        SetArgReferee<1>(MAX_SOURCE_SIZE + 1),
        testing::Invoke([&testData](uint32_t, size_t&, std::unique_ptr<uint8_t[]>& dataOut, uint32_t) {
            dataOut = std::move(testData);
        }),
        Return(Global::Resource::RState::SUCCESS)
    ));
    ref = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_EQ(ref, nullptr);
}

/**
 * @tc.name: GetPixelMap04
 * @tc.desc: GetPixelMap With Resource
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, GetPixelMap04, TestSize.Level1)
{
    auto mockResourceManager = std::make_shared<OHOS::Global::Resource::MockResourceManager>();
    auto abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    uint32_t mediaDataId = 1;
    abilityInfo->hapPath = "";

    //Resource path is inValid
    std::string tmpPath1 = "home/data:";
    EXPECT_CALL(*mockResourceManager, GetMediaById(_, _, _))
        .WillOnce(DoAll(
            SetArgReferee<1>(std::string(tmpPath1)),
            Return(Global::Resource::RState::SUCCESS)
    ));
    auto pixelMap = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_EQ(pixelMap, nullptr);

    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::filesystem::path tmpFile = tmpDir / "wms_test_XXXXXX.png";
    std::string tmpPath2 = tmpFile.string();
    std::ofstream outFile(tmpPath2, std::ios::binary);
    ASSERT_TRUE(outFile.is_open()) << "Failed to create temp file: " << tmpPath2;
    outFile.write(reinterpret_cast<const char*>(RAW_PNG_DATA), sizeof(RAW_PNG_DATA));
    outFile.close();

    EXPECT_CALL(*mockResourceManager, GetMediaById(_, _, _))
        .WillOnce(DoAll(
            SetArgReferee<1>(std::string(tmpPath2)),
            Return(Global::Resource::RState::SUCCESS)
        ));

    pixelMap = StartingWindow::GetPixelMap(mediaDataId, mockResourceManager, abilityInfo);
    EXPECT_NE(pixelMap, nullptr);
    std::filesystem::remove(tmpPath2);
}

/**
 * @tc.name: LoadCustomStartingWindowInfo01
 * @tc.desc: LoadCustomStartingWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, LoadCustomStartingWindowInfo01, TestSize.Level1)
{
    auto res = StartingWindow::LoadCustomStartingWindowInfo(nullptr, nullptr);
    EXPECT_EQ(res, false);

    sptr<WindowTransitionInfo> winInfo = sptr<WindowTransitionInfo>::MakeSptr();
    sptr<WindowNode> node = StartingWindow::CreateWindowNode(winInfo, 1);
    res = StartingWindow::LoadCustomStartingWindowInfo(node, nullptr);
    EXPECT_EQ(res, false);

    auto bundleMgr = StartingWindow::GetBundleManager();
    res = StartingWindow::LoadCustomStartingWindowInfo(node, bundleMgr);
    EXPECT_EQ(res, false);

    auto mockBundleMgr = sptr<AppExecFwk::MockIBundleMgr>::MakeSptr();
    EXPECT_CALL(*mockBundleMgr, QueryAbilityInfo(_, _, _, _))
        .Times(1).WillOnce(Return(true));
    res = StartingWindow::LoadCustomStartingWindowInfo(node, mockBundleMgr);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: DoLoadCustomStartingWindowInfo01
 * @tc.desc: DoLoadCustomStartingWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DoLoadCustomStartingWindowInfo01, TestSize.Level1)
{
    auto res = StartingWindow::DoLoadCustomStartingWindowInfo(nullptr, nullptr);
    EXPECT_EQ(res, false);

    auto abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    abilityInfo->startWindowResource.startWindowBackgroundColorId = 0;
    res = StartingWindow::DoLoadCustomStartingWindowInfo(abilityInfo, nullptr);
    EXPECT_EQ(res, false);

    auto mockResourceManager = std::make_shared<OHOS::Global::Resource::MockResourceManager>();
    EXPECT_CALL(*mockResourceManager, GetColorById(_, _))
        .Times(1).WillOnce(Return(Global::Resource::RState::NOT_FOUND));
    res = StartingWindow::DoLoadCustomStartingWindowInfo(abilityInfo, mockResourceManager);
    EXPECT_EQ(res, false);

    abilityInfo->startWindowResource.startWindowBackgroundColorId = 1;
    abilityInfo->startWindowResource.startWindowAppIconId = 1;
    abilityInfo->hapPath = "home/starting_window/hapPath";
    const size_t pngDataSize = sizeof(RAW_PNG_DATA);
    auto testData = std::make_unique<uint8_t[]>(pngDataSize);
    for (size_t i = 0; i < pngDataSize; ++i) {
        testData[i] = RAW_PNG_DATA[i];
    }

    EXPECT_CALL(*mockResourceManager, GetColorById(_, _))
        .Times(1).WillOnce(Return(Global::Resource::RState::SUCCESS));
    EXPECT_CALL(*mockResourceManager, GetMediaDataById(_, _, _, _))
    .WillOnce(DoAll(
        SetArgReferee<1>(pngDataSize),
        testing::Invoke([&testData](uint32_t, size_t&, std::unique_ptr<uint8_t[]>& dataOut, uint32_t) {
            dataOut = std::move(testData);
        }),
        Return(Global::Resource::RState::NOT_FOUND)
    ));
    res = StartingWindow::DoLoadCustomStartingWindowInfo(abilityInfo, mockResourceManager);
    EXPECT_EQ(res, true);

    EXPECT_CALL(*mockResourceManager, GetColorById(_, _))
        .Times(1).WillOnce(Return(Global::Resource::RState::SUCCESS));
    EXPECT_CALL(*mockResourceManager, GetMediaDataById(_, _, _, _))
    .WillOnce(DoAll(
        SetArgReferee<1>(pngDataSize),
        testing::Invoke([&testData](uint32_t, size_t&, std::unique_ptr<uint8_t[]>& dataOut, uint32_t) {
            dataOut = std::move(testData);
        }),
        Return(Global::Resource::RState::SUCCESS)
    ));
    res = StartingWindow::DoLoadCustomStartingWindowInfo(abilityInfo, mockResourceManager);
    EXPECT_EQ(res, true);
}
} // namespace Rosen
} // namespace OHOS
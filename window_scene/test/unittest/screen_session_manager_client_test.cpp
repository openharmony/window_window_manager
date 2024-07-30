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

#include "iremote_object_mocker.h"
#include "screen_session_manager_client.h"
#include "zidl/screen_session_manager_proxy.h"
#include "display_manager.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerClientTest"};
}
class DmPrivateWindowListener : public DisplayManager::IPrivateWindowListener {
public:
    void OnPrivateWindow(bool hasPrivate) {WLOGFI("IPrivateWindowListener hasPrivatewindow: %{public}u", hasPrivate);}
};
class DmPrivateWindowListChangeListener : public DisplayManager::IPrivateWindowListChangeListener {
public:
    void OnPrivateWindowListChange(DisplayId displayId, std::vector<std::string> privacyWindowList)
    {
        WLOGFI("IPrivateWindowListChangeListener displayId: %{public}" PRIu64".", displayId);
        std::string concatenatedString;
        std::vector<std::string> result = privacyWindowList;
        for (const auto& window : result) {
            concatenatedString.append(window);
            concatenatedString.append("  ");
        }
        WLOGFI("privacyWindowList: %{public}s", concatenatedString.c_str());
        callback_(privacyWindowList);
    }
    void setCallback(std::function<void(std::vector<std::string>)> callback)
    {
        callback_ = callback;
    }
private:
    std::function<void(std::vector<std::string>)> callback_;
};
class ScreenSessionManagerClientTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    ScreenSessionManagerClient* screenSessionManagerClient_;
};

void ScreenSessionManagerClientTest::SetUp()
{
    screenSessionManagerClient_ = &ScreenSessionManagerClient::GetInstance();
}

void ScreenSessionManagerClientTest::TearDown()
{
    screenSessionManagerClient_ = nullptr;
}

/**
 * @tc.name: RegisterScreenConnectionListener
 * @tc.desc: RegisterScreenConnectionListener test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, RegisterScreenConnectionListener, Function | SmallTest | Level2)
{
    IScreenConnectionListener* listener = nullptr;
    screenSessionManagerClient_->RegisterScreenConnectionListener(listener);
    EXPECT_EQ(screenSessionManagerClient_->screenConnectionListener_, nullptr);
}

/**
 * @tc.name: GetScreenSession
 * @tc.desc: GetScreenSession test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetScreenSession, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession, nullptr);

    screenSession = new ScreenSession(0, ScreenProperty(), 0);
    screenSessionManagerClient_->screenSessionMap_.emplace(screenId, screenSession);

    auto screenSession2 = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession2, screenSession);

    screenSessionManagerClient_->screenSessionMap_.clear();
    screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: OnScreenConnectionChanged01
 * @tc.desc: OnScreenConnectionChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenConnectionChanged01, Function | SmallTest | Level2)
{
    EXPECT_EQ(screenSessionManagerClient_->screenSessionManager_, nullptr);
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    screenSessionManagerClient_->screenSessionManager_ = new ScreenSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ScreenId rsId = 0;
    std::string name;
    screenSessionManagerClient_->OnScreenConnectionChanged(screenId, screenEvent, rsId, name);
    sptr<ScreenSession> screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_NE(screenSession, nullptr);
}

/**
 * @tc.name: OnScreenConnectionChanged02
 * @tc.desc: OnScreenConnectionChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenConnectionChanged02, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenEvent screenEvent = ScreenEvent::DISCONNECTED;
    ScreenId rsId = 0;
    std::string name;
    screenSessionManagerClient_->OnScreenConnectionChanged(screenId, screenEvent, rsId, name);
    sptr<ScreenSession> screenSession = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: CheckIfNeedConnectScreen01
 * @tc.desc: CheckIfNeedConnectScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, CheckIfNeedConnectScreen01, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenId rsId = 0;
    std::string name = "HiCar";
    screenSessionManagerClient_->screenSessionManager_->GetScreenProperty(screenId).SetScreenType(ScreenType::VIRTUAL);
    bool result = screenSessionManagerClient_->CheckIfNeedConnectScreen(screenId, rsId, name);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: CheckIfNeedConnectScreen02
 * @tc.desc: CheckIfNeedConnectScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, CheckIfNeedConnectScreen02, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenId rsId = SCREEN_ID_INVALID;
    std::string name;
    screenSessionManagerClient_->screenSessionManager_->GetScreenProperty(screenId).SetScreenType(ScreenType::VIRTUAL);
    bool result = screenSessionManagerClient_->CheckIfNeedConnectScreen(screenId, rsId, name);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: CheckIfNeedConnectScreen03
 * @tc.desc: CheckIfNeedConnectScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, CheckIfNeedConnectScreen03, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenId rsId = 0;
    std::string name;
    screenSessionManagerClient_->screenSessionManager_->GetScreenProperty(screenId).SetScreenType(ScreenType::REAL);
    bool result = screenSessionManagerClient_->CheckIfNeedConnectScreen(screenId, rsId, name);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: CheckIfNeedConnectScreen04
 * @tc.desc: CheckIfNeedConnectScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, CheckIfNeedConnectScreen04, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenId rsId = 0;
    std::string name = "HiCar";
    screenSessionManagerClient_->screenSessionManager_->GetScreenProperty(screenId).SetScreenType(ScreenType::VIRTUAL);
    bool result = screenSessionManagerClient_->CheckIfNeedConnectScreen(screenId, rsId, name);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: CheckIfNeedConnectScreen05
 * @tc.desc: CheckIfNeedConnectScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, CheckIfNeedConnectScreen05, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenId rsId = 0;
    std::string name = "SuperLauncher";
    screenSessionManagerClient_->screenSessionManager_->GetScreenProperty(screenId).SetScreenType(ScreenType::VIRTUAL);
    bool result = screenSessionManagerClient_->CheckIfNeedConnectScreen(screenId, rsId, name);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: CheckIfNeedConnectScreen06
 * @tc.desc: CheckIfNeedConnectScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, CheckIfNeedConnectScreen06, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    ScreenId screenId = 0;
    ScreenId rsId = 0;
    std::string name = "CastEngine";
    screenSessionManagerClient_->screenSessionManager_->GetScreenProperty(screenId).SetScreenType(ScreenType::VIRTUAL);
    bool result = screenSessionManagerClient_->CheckIfNeedConnectScreen(screenId, rsId, name);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: OnPowerStatusChanged
 * @tc.desc: OnPowerStatusChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnPowerStatusChanged, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);
    ScreenId screenId = 0;
    sptr<ScreenSession> screenSession = new ScreenSession(0, ScreenProperty(), 0);
    screenSessionManagerClient_->screenSessionMap_.emplace(screenId, screenSession);
    DisplayPowerEvent event = DisplayPowerEvent::WAKE_UP;
    EventStatus status = EventStatus::BEGIN;
    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_ACCESS;
    screenSessionManagerClient_->OnPowerStatusChanged(event, status, reason);
    sptr<ScreenSession> screenSession1 = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_NE(screenSession1, nullptr);
}

/**
 * @tc.name: GetAllScreensProperties
 * @tc.desc: GetAllScreensProperties test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetAllScreensProperties, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);
    screenSessionManagerClient_->screenSessionMap_.clear();
    ScreenId screenId = 0;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    screenSessionManagerClient_->screenSessionMap_.emplace(screenId, screenSession);
    EXPECT_EQ(1, screenSessionManagerClient_->GetAllScreensProperties().size());
}

/**
 * @tc.name: SetPrivacyStateByDisplayId01
 * @tc.desc: SetPrivacyStateByDisplayId01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetPrivacyStateByDisplayId01, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);
    screenSessionManagerClient_->screenSessionManager_ = nullptr;
    EXPECT_EQ(screenSessionManagerClient_->screenSessionManager_, nullptr);
    screenSessionManagerClient_->ConnectToServer();
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    sptr<DisplayManager::IPrivateWindowListener> displayListener_ = new DmPrivateWindowListener();
    DisplayManager::GetInstance().RegisterPrivateWindowListener(displayListener_);
    
    DisplayId id = 0;
    bool hasPrivate = true;
    screenSessionManagerClient_->SetPrivacyStateByDisplayId(id, hasPrivate);

    bool result = false;
    screenSessionManagerClient_->screenSessionManager_->HasPrivateWindow(id, result);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: SetPrivacyStateByDisplayId02
 * @tc.desc: SetPrivacyStateByDisplayId02 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetPrivacyStateByDisplayId02, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);
    DisplayId id = 0;
    bool hasPrivate = false;
    screenSessionManagerClient_->SetPrivacyStateByDisplayId(id, hasPrivate);
    bool result = true;
    screenSessionManagerClient_->screenSessionManager_->HasPrivateWindow(id, result);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SetScreenPrivacyWindowList
 * @tc.desc: SetScreenPrivacyWindowList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetScreenPrivacyWindowList, Function | SmallTest | Level2)
{
    screenSessionManagerClient_->screenSessionManager_ = nullptr;
    EXPECT_EQ(screenSessionManagerClient_->screenSessionManager_, nullptr);
    screenSessionManagerClient_->ConnectToServer();
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);

    DisplayId id = 0;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    std::vector<std::string> privacyWindowList2{"win0"};
    sptr<DisplayManager::IPrivateWindowListChangeListener> listener_ = new DmPrivateWindowListChangeListener();
    listener_->setCallback([privacyWindowList, privacyWindowList2](std::vector<std::string> windowList)
    {
        EXPECT_EQ(windowList, privacyWindowList);
        EXPECT_NE(windowList, privacyWindowList2);
    });
    DisplayManager::GetInstance().RegisterPrivateWindowListChangeListener(listener_);

    screenSessionManagerClient_->SetScreenPrivacyWindowList(id, privacyWindowList);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @tc.name: GetFoldDisplayMode01
 * @tc.desc: GetFoldDisplayMode test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetFoldDisplayMode01, Function | SmallTest | Level2)
{
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);
    EXPECT_NE(FoldDisplayMode::FULL, screenSessionManagerClient_->GetFoldDisplayMode());
    EXPECT_NE(FoldStatus::FOLDED, screenSessionManagerClient_->GetFoldStatus());
    EXPECT_EQ(0, screenSessionManagerClient_->GetCurvedCompressionArea());
}

/**
 * @tc.name: GetFoldDisplayMode02
 * @tc.desc: GetFoldDisplayMode test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetFoldDisplayMode02, Function | SmallTest | Level2)
{
    screenSessionManagerClient_->screenSessionManager_ = nullptr;
    ScreenId screenId = 0;
    bool foldToExpand = true;
    DMRect area;
    screenSessionManagerClient_->GetPhyScreenProperty(screenId);
    screenSessionManagerClient_->UpdateAvailableArea(screenId, area);
    screenSessionManagerClient_->NotifyFoldToExpandCompletion(foldToExpand);
    EXPECT_EQ(FoldDisplayMode::UNKNOWN, screenSessionManagerClient_->GetFoldDisplayMode());
    EXPECT_EQ(FoldStatus::UNKNOWN, screenSessionManagerClient_->GetFoldStatus());
    EXPECT_EQ(0, screenSessionManagerClient_->GetCurvedCompressionArea());
}

/**
 * @tc.name: UpdateScreenRotationProperty
 * @tc.desc: UpdateScreenRotationProperty test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, UpdateScreenRotationProperty, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    ScreenId displayNodeScreenId = 0;
    sptr<ScreenSession> screenSession = new ScreenSession(0, ScreenProperty(), 0);
    screenSessionManagerClient_->screenSessionMap_.emplace(screenId, screenSession);
    DMRect area;
    bool foldToExpand = true;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    float rotation = 90;
    float scaleX = 1.0;
    float scaleY = 1.0;
    ScreenPropertyChangeType screenPropertyChangeType = ScreenPropertyChangeType::ROTATION_BEGIN;
    screenSessionManagerClient_->UpdateScreenRotationProperty(screenId, bounds, rotation,
        screenPropertyChangeType);
    screenSessionManagerClient_->SetDisplayNodeScreenId(screenId, displayNodeScreenId);
    screenSessionManagerClient_->GetPhyScreenProperty(screenId);
    screenSessionManagerClient_->UpdateAvailableArea(screenId, area);
    screenSessionManagerClient_->NotifyFoldToExpandCompletion(foldToExpand);
    screenSessionManagerClient_->GetScreenSnapshot(screenId, scaleX, scaleY);
    sptr<ScreenSession> screenSession1 = screenSessionManagerClient_->GetScreenSession(screenId);
    EXPECT_NE(screenSession1, nullptr);
}

/**
 * @tc.name: GetScreenSnapshot
 * @tc.desc: GetScreenSnapshot test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetScreenSnapshot, Function | SmallTest | Level2)
{
    screenSessionManagerClient_->screenSessionMap_.clear();
    ScreenId screenId = 0;
    ScreenId displayNodeScreenId = 0;
    float scaleX = 1.0;
    float scaleY = 1.0;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    float rotation = 90;
    ScreenPropertyChangeType screenPropertyChangeType = ScreenPropertyChangeType::ROTATION_BEGIN;
    screenSessionManagerClient_->UpdateScreenRotationProperty(screenId, bounds, rotation,
        screenPropertyChangeType);
    screenSessionManagerClient_->SetDisplayNodeScreenId(screenId, displayNodeScreenId);
    std::shared_ptr<Media::PixelMap> res = screenSessionManagerClient_->GetScreenSnapshot(screenId, scaleX, scaleY);
    EXPECT_EQ(nullptr, res);
}

/**
 * @tc.name: SetScreenOffDelayTime
 * @tc.desc: SetScreenOffDelayTime test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetScreenOffDelayTime, Function | SmallTest | Level2)
{
    screenSessionManagerClient_->screenSessionManager_ = nullptr;
    int32_t ret = screenSessionManagerClient_->SetScreenOffDelayTime(1);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: GetDeviceScreenConfig
 * @tc.desc: GetDeviceScreenConfig test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetDeviceScreenConfig, Function | SmallTest | Level2)
{
    DeviceScreenConfig deviceScreenConfig = screenSessionManagerClient_->GetDeviceScreenConfig();
    EXPECT_FALSE(deviceScreenConfig.rotationPolicy_.empty());
}

/**
 * @tc.name: OnUpdateFoldDisplayMode
 * @tc.desc: OnUpdateFoldDisplayMode test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetFoldDisplayMode, Function | SmallTest | Level2)
{
    screenSessionManagerClient_->OnUpdateFoldDisplayMode(FoldDisplayMode::UNKNOWN);
    auto ret1 = screenSessionManagerClient_->GetFoldDisplayMode();
    EXPECT_EQ(FoldDisplayMode::UNKNOWN, ret1);
    screenSessionManagerClient_->OnUpdateFoldDisplayMode(FoldDisplayMode::FULL);
    auto ret2 = screenSessionManagerClient_->GetFoldDisplayMode();
    EXPECT_EQ(FoldDisplayMode::FULL, ret2);
    screenSessionManagerClient_->OnUpdateFoldDisplayMode(FoldDisplayMode::MAIN);
    auto ret3 = screenSessionManagerClient_->GetFoldDisplayMode();
    EXPECT_EQ(FoldDisplayMode::MAIN, ret3);
}

/**
 * @tc.name: GetScreenSessionById
 * @tc.desc: GetScreenSessionById test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetScreenSessionById, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    sptr<ScreenSession> ret = screenSessionManagerClient_->GetScreenSessionById(screenId);
    EXPECT_EQ(nullptr, ret);
}

/**
 * @tc.name: ConnectToServer01
 * @tc.desc: ConnectToServer test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, ConnectToServer01, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    
    screenSessionManagerClient_->screenSessionManager_ = nullptr;
    screenSessionManagerClient_->ConnectToServer();
}

/**
 * @tc.name: OnPropertyChanged
 * @tc.desc: OnPropertyChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnPropertyChanged, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    ScreenProperty property;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnPropertyChanged(screenId, property, reason);
}

/**
 * @tc.name: OnSensorRotationChanged01
 * @tc.desc: OnSensorRotationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnSensorRotationChanged01, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    float sensorRotation = 0;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnSensorRotationChanged(screenId, sensorRotation);
}

/**
 * @tc.name: OnSensorRotationChanged02
 * @tc.desc: OnSensorRotationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnSensorRotationChanged02, Function | SmallTest | Level2)
{
    ScreenId screenId = 1;
    float sensorRotation = 90.0f;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnSensorRotationChanged(screenId, sensorRotation);
}

/**
 * @tc.name: OnScreenOrientationChanged01
 * @tc.desc: OnScreenOrientationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenOrientationChanged01, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    float screenOrientation = 0;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnScreenOrientationChanged(screenId, screenOrientation);
}

/**
 * @tc.name: OnScreenOrientationChanged02
 * @tc.desc: OnScreenOrientationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenOrientationChanged02, Function | SmallTest | Level2)
{
    ScreenId screenId = 1;
    float screenOrientation = 90.0f;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnScreenOrientationChanged(screenId, screenOrientation);
}

/**
 * @tc.name: OnScreenRotationLockedChanged01
 * @tc.desc: OnScreenRotationLockedChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenRotationLockedChanged01, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    bool isLocked = false;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnScreenRotationLockedChanged(screenId, isLocked);
}

/**
 * @tc.name: OnScreenRotationLockedChanged02
 * @tc.desc: OnScreenRotationLockedChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenRotationLockedChanged02, Function | SmallTest | Level2)
{
    ScreenId screenId = 1;
    bool isLocked = true;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnScreenRotationLockedChanged(screenId, isLocked);
}

/**
 * @tc.name: RegisterDisplayChangeListener
 * @tc.desc: RegisterDisplayChangeListener test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, RegisterDisplayChangeListener, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->RegisterDisplayChangeListener(nullptr);
}

/**
 * @tc.name: OnDisplayStateChanged01
 * @tc.desc: OnDisplayStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnDisplayStateChanged01, Function | SmallTest | Level2)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap {
        { displayInfo->GetDisplayId(), displayInfo },
    };
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnDisplayStateChanged(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: OnDisplayStateChanged02
 * @tc.desc: OnDisplayStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnDisplayStateChanged02, Function | SmallTest | Level2)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap {
        { displayInfo->GetDisplayId(), displayInfo },
    };
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->displayChangeListener_ = nullptr;
    screenSessionManagerClient_->OnDisplayStateChanged(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: OnGetSurfaceNodeIdsFromMissionIdsChanged01
 * @tc.desc: OnGetSurfaceNodeIdsFromMissionIdsChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnGetSurfaceNodeIdsFromMissionIdsChanged01, Function | SmallTest | Level2)
{
    std::vector<uint64_t> missionIds = {0, 1};
    std::vector<uint64_t> surfaceNodeIds;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnGetSurfaceNodeIdsFromMissionIdsChanged(missionIds, surfaceNodeIds);
}

/**
 * @tc.name: OnGetSurfaceNodeIdsFromMissionIdsChanged02
 * @tc.desc: OnGetSurfaceNodeIdsFromMissionIdsChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnGetSurfaceNodeIdsFromMissionIdsChanged02, Function | SmallTest | Level2)
{
    std::vector<uint64_t> missionIds = {0, 1};
    std::vector<uint64_t> surfaceNodeIds;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->displayChangeListener_ = nullptr;
    screenSessionManagerClient_->OnGetSurfaceNodeIdsFromMissionIdsChanged(missionIds, surfaceNodeIds);
}

/**
 * @tc.name: OnScreenshot01
 * @tc.desc: OnScreenshot test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenshot01, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnScreenshot(displayId);
}

/**
 * @tc.name: OnScreenshot02
 * @tc.desc: OnScreenshot test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnScreenshot02, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->displayChangeListener_ = nullptr;
    screenSessionManagerClient_->OnScreenshot(displayId);
}

/**
 * @tc.name: OnImmersiveStateChanged01
 * @tc.desc: OnImmersiveStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnImmersiveStateChanged01, Function | SmallTest | Level2)
{
    bool immersive = false;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnImmersiveStateChanged(immersive);
}

/**
 * @tc.name: OnImmersiveStateChanged02
 * @tc.desc: OnImmersiveStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnImmersiveStateChanged02, Function | SmallTest | Level2)
{
    bool immersive = false;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->displayChangeListener_ = nullptr;
    screenSessionManagerClient_->OnImmersiveStateChanged(immersive);
}

/**
 * @tc.name: SetDisplayNodeScreenId
 * @tc.desc: SetDisplayNodeScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetDisplayNodeScreenId, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    ScreenId displayNodeScreenId = 0;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->SetDisplayNodeScreenId(screenId, displayNodeScreenId);
}

/**
 * @tc.name: GetCurvedCompressionArea
 * @tc.desc: GetCurvedCompressionArea test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetCurvedCompressionArea, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->GetCurvedCompressionArea();
}

/**
 * @tc.name: GetPhyScreenProperty
 * @tc.desc: GetPhyScreenProperty test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetPhyScreenProperty, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->GetPhyScreenProperty(screenId);
}

/**
 * @tc.name: NotifyDisplayChangeInfoChanged
 * @tc.desc: NotifyDisplayChangeInfoChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, NotifyDisplayChangeInfoChanged, Function | SmallTest | Level2)
{
    sptr<DisplayChangeInfo> info = new DisplayChangeInfo();
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->NotifyDisplayChangeInfoChanged(info);
}

/**
 * @tc.name: SetScreenPrivacyState01
 * @tc.desc: SetScreenPrivacyState test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetScreenPrivacyState01, Function | SmallTest | Level2)
{
    bool hasPrivate = false;
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->SetScreenPrivacyState(hasPrivate);
}

/**
 * @tc.name: SetScreenPrivacyState02
 * @tc.desc: SetScreenPrivacyState test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetScreenPrivacyState02, Function | SmallTest | Level2)
{
    bool hasPrivate = false;
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    screenSessionManagerClient_->screenSessionManager_ = new ScreenSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(screenSessionManagerClient_->screenSessionManager_, nullptr);
    screenSessionManagerClient_->SetScreenPrivacyState(hasPrivate);
}

/**
 * @tc.name: SetPrivacyStateByDisplayId
 * @tc.desc: SetPrivacyStateByDisplayId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetPrivacyStateByDisplayId, Function | SmallTest | Level2)
{
    DisplayId id = 0;
    bool hasPrivate = false;
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->SetPrivacyStateByDisplayId(id, hasPrivate);
}

/**
 * @tc.name: UpdateAvailableArea
 * @tc.desc: UpdateAvailableArea test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, UpdateAvailableArea, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    DMRect area;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->UpdateAvailableArea(screenId, area);
}

/**
 * @tc.name: NotifyFoldToExpandCompletion
 * @tc.desc: NotifyFoldToExpandCompletion test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, NotifyFoldToExpandCompletion, Function | SmallTest | Level2)
{
    bool foldToExpand = true;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->NotifyFoldToExpandCompletion(foldToExpand);
}

/**
 * @tc.name: SwitchUserCallback01
 * @tc.desc: SwitchUserCallback test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SwitchUserCallback01, Function | SmallTest | Level2)
{
    std::vector<int32_t> oldScbPids = {0, 1};
    int32_t currentScbPid = 1;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->SwitchUserCallback(oldScbPids, currentScbPid);
}

/**
 * @tc.name: SwitchUserCallback02
 * @tc.desc: SwitchUserCallback test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SwitchUserCallback02, Function | SmallTest | Level2)
{
    std::vector<int32_t> oldScbPids = {};
    int32_t currentScbPid = 1;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->SwitchUserCallback(oldScbPids, currentScbPid);
}

/**
 * @tc.name: SwitchingCurrentUser
 * @tc.desc: SwitchingCurrentUser test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SwitchingCurrentUser, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->SwitchingCurrentUser();
}

/**
 * @tc.name: GetFoldStatus
 * @tc.desc: GetFoldStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetFoldStatus, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->GetFoldStatus();
}

/**
 * @tc.name: GetDefaultScreenId
 * @tc.desc: GetDefaultScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, GetDefaultScreenId, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->GetDefaultScreenId();
}

/**
 * @tc.name: IsFoldable
 * @tc.desc: IsFoldable test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, IsFoldable, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->IsFoldable();
}

/**
 * @tc.name: SetVirtualPixelRatioSystem
 * @tc.desc: SetVirtualPixelRatioSystem test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, SetVirtualPixelRatioSystem, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    float virtualPixelRatio = 1.0f;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->SetVirtualPixelRatioSystem(screenId, virtualPixelRatio);
}

/**
 * @tc.name: UpdateDisplayHookInfo
 * @tc.desc: UpdateDisplayHookInfo test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, UpdateDisplayHookInfo, Function | SmallTest | Level2)
{
    int32_t uid = 0;
    bool enable = false;
    DMHookInfo hookInfo;
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->UpdateDisplayHookInfo(uid, enable, hookInfo);
}

/**
 * @tc.name: OnFoldStatusChangedReportUE
 * @tc.desc: OnFoldStatusChangedReportUE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientTest, OnFoldStatusChangedReportUE, Function | SmallTest | Level2)
{
    std::vector<std::string> screenFoldInfo;

    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    screenSessionManagerClient_->OnFoldStatusChangedReportUE(screenFoldInfo);
}

/**
 * @tc.name: UpdateDisplayScale01
 * @tc.desc: UpdateDisplayScale test
 * @tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerClientTest, UpdateDisplayScale01, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    const float scaleX = 1.0f;
    const float scaleY = 1.0f;
    const float pivotX = 0.5f;
    const float pivotY = 0.5f;
    ScreenId defaultScreenId = screenSessionManagerClient_->GetDefaultScreenId();
    screenSessionManagerClient_->UpdateDisplayScale(defaultScreenId, scaleX, scaleY, pivotX, pivotY);
}

/**
 * @tc.name: UpdateDisplayScale02
 * @tc.desc: UpdateDisplayScale test
 * @tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerClientTest, UpdateDisplayScale02, Function | SmallTest | Level2)
{
    ASSERT_TRUE(screenSessionManagerClient_ != nullptr);
    const float scaleX = 1.0f;
    const float scaleY = 1.0f;
    const float pivotX = 0.5f;
    const float pivotY = 0.5f;
    uint64_t fakeScreenId = 100;
    screenSessionManagerClient_->UpdateDisplayScale(fakeScreenId, scaleX, scaleY, pivotX, pivotY);
}
} // namespace Rosen
} // namespace OHOS

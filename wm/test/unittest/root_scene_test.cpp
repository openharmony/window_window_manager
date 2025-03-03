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

#include <event_handler.h>
#include <gtest/gtest.h>
#include <input_manager.h>
#include <ui_content.h>
#include <viewport_config.h>
#include "root_scene.h"

#include "app_mgr_client.h"
#include "mock_uicontent.h"
#include "singleton.h"
#include "singleton_container.h"

#include "vsync_station.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const uint32_t MOCK_LEM_SUB_WIDTH = 340;
const uint32_t MOCK_LEM_SUB_HEIGHT = 340;

class RootSceneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void RootSceneTest::SetUpTestCase() {}

void RootSceneTest::TearDownTestCase() {}

void RootSceneTest::SetUp() {}

void RootSceneTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: LoadContent01
 * @tc.desc: context is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, LoadContent01, Function | SmallTest | Level3)
{
    RootScene rootScene;
    rootScene.LoadContent("a", nullptr, nullptr, nullptr);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: UpdateViewportConfig01
 * @tc.desc: UpdateViewportConfig Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UpdateViewportConfig01, Function | SmallTest | Level3)
{
    RootScene rootScene;
    Rect rect;

    rootScene.uiContent_ = nullptr;
    rootScene.UpdateViewportConfig(rect, WindowSizeChangeReason::UNDEFINED);

    rect.width_ = MOCK_LEM_SUB_WIDTH;
    rect.height_ = MOCK_LEM_SUB_HEIGHT;
    rootScene.UpdateViewportConfig(rect, WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: UpdateConfiguration
 * @tc.desc: UpdateConfiguration Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UpdateConfiguration, Function | SmallTest | Level3)
{
    RootScene rootScene;
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();

    rootScene.uiContent_ = nullptr;
    rootScene.UpdateConfiguration(configuration);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: UpdateConfigurationForSpecified
 * @tc.desc: UpdateConfigurationForSpecified Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UpdateConfigurationForSpecified, Function | SmallTest | Level3)
{
    RootScene rootScene;
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager;

    rootScene.uiContent_ = std::make_unique<Ace::UIContentMocker>();
    rootScene.UpdateConfigurationForSpecified(configuration, resourceManager);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: UpdateConfigurationForAll
 * @tc.desc: UpdateConfigurationForAll Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UpdateConfigurationForAll, Function | SmallTest | Level3)
{
    RootScene rootScene;
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();

    auto prevStaticRootScene = RootScene::staticRootScene_;
    rootScene.UpdateConfigurationForAll(configuration);

    sptr<RootScene> staticRootScene;
    RootScene::staticRootScene_ = staticRootScene;
    rootScene.UpdateConfigurationForAll(configuration);

    RootScene::staticRootScene_ = prevStaticRootScene;
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: RegisterInputEventListener01
 * @tc.desc: RegisterInputEventListener Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, RegisterInputEventListener01, Function | SmallTest | Level3)
{
    RootScene rootScene;
    rootScene.RegisterInputEventListener();
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: RequestVsyncErr
 * @tc.desc: RequestVsync Test Err
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, RequestVsyncErr, Function | SmallTest | Level3)
{
    RootScene rootScene;
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    rootScene.RequestVsync(vsyncCallback);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: GetVSyncPeriod
 * @tc.desc: GetVSyncPeriod Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, GetVSyncPeriod, Function | SmallTest | Level3)
{
    RootScene rootScene;
    rootScene.GetVSyncPeriod();
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: FlushFrameRate
 * @tc.desc: FlushFrameRate Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, FlushFrameRate, Function | SmallTest | Level3)
{
    RootScene rootScene;
    uint32_t rate = 120;
    int32_t animatorExpectedFrameRate = -1;
    rootScene.FlushFrameRate(rate, animatorExpectedFrameRate);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: SetFrameLayoutFinishCallback
 * @tc.desc: SetFrameLayoutFinishCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, SetFrameLayoutFinishCallback, Function | SmallTest | Level3)
{
    RootScene rootScene;

    rootScene.SetFrameLayoutFinishCallback(nullptr);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: SetUiDvsyncSwitch
 * @tc.desc: SetUiDvsyncSwitch Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, SetUiDvsyncSwitchSucc, Function | SmallTest | Level3)
{
    RootScene rootScene;
    rootScene.SetUiDvsyncSwitch(true);
    rootScene.SetUiDvsyncSwitch(false);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: SetUiDvsyncSwitch
 * @tc.desc: SetUiDvsyncSwitch Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, SetUiDvsyncSwitchErr, Function | SmallTest | Level3)
{
    RootScene rootScene;
    rootScene.SetUiDvsyncSwitch(true);
    rootScene.SetUiDvsyncSwitch(false);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: IsLastFrameLayoutFinished
 * @tc.desc: IsLastFrameLayoutFinished Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, IsLastFrameLayoutFinished, Function | SmallTest | Level3)
{
    RootScene rootScene;
    auto ret = rootScene.IsLastFrameLayoutFinished();
    ASSERT_EQ(ret, true);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: OnFlushUIParams
 * @tc.desc: OnFlushUIParams Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, OnFlushUIParams, Function | SmallTest | Level3)
{
    RootScene rootScene;
    rootScene.OnFlushUIParams();
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: OnBundleUpdated
 * @tc.desc: OnBundleUpdated Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, OnBundleUpdated, Function | SmallTest | Level3)
{
    RootScene rootScene;
    std::string bundleName = "test";
    rootScene.OnBundleUpdated(bundleName);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: SetDisplayOrientation
 * @tc.desc: SetDisplayOrientation Test01
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, SetDisplayOrientationTest01, Function | SmallTest | Level3)
{
    RootScene rootScene;
    int32_t orientation = 0;
    rootScene.SetDisplayOrientation(orientation);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: SetDisplayOrientation
 * @tc.desc: SetDisplayOrientation Test02
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, SetDisplayOrientationTest02, Function | SmallTest | Level3)
{
    RootScene rootScene;
    int32_t orientation = 100;
    rootScene.SetDisplayOrientation(orientation);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: RegisterAvoidAreaChangeListener
 * @tc.desc: RegisterAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, RegisterAvoidAreaChangeListener, Function | SmallTest | Level3)
{
    RootScene rootScene;
    rootScene.updateRootSceneAvoidAreaCallback_ = [] {};
    sptr<IAvoidAreaChangedListener> listener = sptr<IAvoidAreaChangedListener>::MakeSptr();
    auto ret = rootScene.RegisterAvoidAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_DO_NOTHING, ret);
    listener = nullptr;
    ret = rootScene.RegisterAvoidAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UnregisterAvoidAreaChangeListener
 * @tc.desc: UnregisterAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UnregisterAvoidAreaChangeListener, Function | SmallTest | Level3)
{
    RootScene rootScene;
    sptr<IAvoidAreaChangedListener> listener = sptr<IAvoidAreaChangedListener>::MakeSptr();
    auto ret = rootScene.UnregisterAvoidAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_DO_NOTHING, ret);
    listener = nullptr;
    ret = rootScene.UnregisterAvoidAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: NotifyAvoidAreaChangeForRoot
 * @tc.desc: NotifyAvoidAreaChangeForRoot Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, NotifyAvoidAreaChangeForRoot, Function | SmallTest | Level3)
{
    RootScene rootScene;
    sptr<IAvoidAreaChangedListener> listener = sptr<IAvoidAreaChangedListener>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    rootScene.avoidAreaChangeListeners_.insert(listener);
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM_GESTURE;
    AvoidArea avoidArea;
    rootScene.NotifyAvoidAreaChangeForRoot(new AvoidArea(avoidArea), type);
}

/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: GetAvoidAreaByType Test err
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, GetAvoidAreaByType, Function | SmallTest | Level3)
{
    RootScene rootScene;
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM_GESTURE;
    AvoidArea avoidArea;

    auto ret = rootScene.GetAvoidAreaByType(type, avoidArea);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UpdateConfigurationSync
 * @tc.desc: UpdateConfigurationSync Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UpdateConfigurationSync, Function | SmallTest | Level3)
{
    RootScene rootScene;
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();

    rootScene.uiContent_ = nullptr;
    rootScene.UpdateConfigurationSync(configuration);
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: UpdateConfigurationSyncForAll
 * @tc.desc: UpdateConfigurationSyncForAll Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UpdateConfigurationSyncForAll, Function | SmallTest | Level3)
{
    RootScene rootScene;
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();

    auto prevStaticRootScene = RootScene::staticRootScene_;
    rootScene.UpdateConfigurationSyncForAll(configuration);

    sptr<RootScene> staticRootScene;
    RootScene::staticRootScene_ = staticRootScene;
    rootScene.UpdateConfigurationSyncForAll(configuration);

    RootScene::staticRootScene_ = prevStaticRootScene;
    ASSERT_EQ(1, rootScene.GetWindowId());
}

/**
 * @tc.name: IsSystemWindow
 * @tc.desc: IsSystemWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, IsSystemWindow, Function | SmallTest | Level3)
{
    RootScene rootScene;
    bool res = rootScene.IsSystemWindow();
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: IsAppWindow
 * @tc.desc: IsAppWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, IsAppWindow, Function | SmallTest | Level3)
{
    RootScene rootScene;
    bool res = rootScene.IsAppWindow();
    ASSERT_EQ(false, res);
}

/**
 * @tc.name: RegisterOccupiedAreaChangeListener
 * @tc.desc: RegisterOccupiedAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, RegisterOccupiedAreaChangeListener, Function | SmallTest | Level3)
{
    RootScene rootScene;
    sptr<IOccupiedAreaChangeListener> listener = sptr<IOccupiedAreaChangeListener>::MakeSptr();
    auto ret = rootScene.RegisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    listener = nullptr;
    ret = rootScene.RegisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UnregisterOccupiedAreaChangeListener
 * @tc.desc: UnregisterOccupiedAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UnregisterOccupiedAreaChangeListener, Function | SmallTest | Level3)
{
    RootScene rootScene;
    sptr<IOccupiedAreaChangeListener> listener = sptr<IOccupiedAreaChangeListener>::MakeSptr();
    auto ret = rootScene.UnregisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    listener = nullptr;
    ret = rootScene.UnregisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: NotifyOccupiedAreaChangeForRoot
 * @tc.desc: NotifyOccupiedAreaChangeForRoot Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, NotifyOccupiedAreaChangeForRoot, Function | SmallTest | Level3)
{
    auto rootScene = sptr<RootScene>::MakeSptr();
    sptr<IOccupiedAreaChangeListener> listener = sptr<IOccupiedAreaChangeListener>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    auto ret = rootScene->RegisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    sptr<OccupiedAreaChangeInfo> info = nullptr;
    rootScene->NotifyOccupiedAreaChangeForRoot(info);
    info = sptr<OccupiedAreaChangeInfo>::MakeSptr();
    ASSERT_NE(nullptr, info);
    rootScene->NotifyOccupiedAreaChangeForRoot(info);
}

/**
 * @tc.name: RegisterWaterfallModeChangeListener
 * @tc.desc: RegisterWaterfallModeChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, RegisterWaterfallModeChangeListener, Function | SmallTest | Level3)
{
    RootScene rootScene;
    sptr<IWaterfallModeChangeListener> listener = sptr<IWaterfallModeChangeListener>::MakeSptr();
    auto ret = rootScene.RegisterWaterfallModeChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    listener = nullptr;
    ret = rootScene.RegisterWaterfallModeChangeListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    auto listeners = rootScene.GetWaterfallModeChangeListeners();
    ASSERT_EQ(listeners.size(), 1);
}

/**
 * @tc.name: UnregisterWaterfallModeChangeListener
 * @tc.desc: UnregisterWaterfallModeChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, UnregisterWaterfallModeChangeListener, Function | SmallTest | Level3)
{
    RootScene rootScene;
    sptr<IWaterfallModeChangeListener> listener = sptr<IWaterfallModeChangeListener>::MakeSptr();
    auto ret = rootScene.RegisterWaterfallModeChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    auto listeners = rootScene.GetWaterfallModeChangeListeners();
    ASSERT_EQ(listeners.size(), 1);
    ret = rootScene.UnregisterWaterfallModeChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    listeners = rootScene.GetWaterfallModeChangeListeners();
    ASSERT_EQ(listeners.size(), 0);
    listener = nullptr;
    ret = rootScene.UnregisterWaterfallModeChangeListener(listener);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: NotifyWaterfallModeChange
 * @tc.desc: NotifyWaterfallModeChange Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, NotifyWaterfallModeChange, Function | SmallTest | Level3)
{
    RootScene rootScene;
    sptr<IWaterfallModeChangeListener> listener = sptr<IWaterfallModeChangeListener>::MakeSptr();
    auto ret = rootScene.RegisterWaterfallModeChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    rootScene.NotifyWaterfallModeChange(true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
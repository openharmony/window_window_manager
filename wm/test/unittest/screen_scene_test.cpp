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
#include <ui_content.h>
#include <viewport_config.h>

#include "app_mgr_client.h"
#include "singleton.h"
#include "singleton_container.h"

#include "screen_scene.h"
#include "vsync_station.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSceneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSceneTest::SetUpTestCase()
{
}

void ScreenSceneTest::TearDownTestCase()
{
}

void ScreenSceneTest::SetUp()
{
}

void ScreenSceneTest::TearDown()
{
}

namespace {
/**
 * @tc.name: LoadContent01
 * @tc.desc: context is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, LoadContent01, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    screenScene.LoadContent("a", nullptr, nullptr, nullptr);
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: Destroy01
 * @tc.desc: context is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, Destroy01, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    screenScene.LoadContent("a", nullptr, nullptr, nullptr);
    screenScene.Destroy();
    ASSERT_EQ(1, screenScene.GetWindowId());
}


/**
 * @tc.name: UpdateViewportConfig01
 * @tc.desc: UpdateViewportConfig Test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, UpdateViewportConfig01, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    Rect rect;
    screenScene.UpdateViewportConfig(rect, WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: UpdateConfiguration
 * @tc.desc: UpdateConfiguration Test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, UpdateConfiguration, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();
    screenScene.UpdateConfiguration(configuration);
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: RequestVsync
 * @tc.desc: RequestVsync Test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, RequestVsync, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    screenScene.RequestVsync(vsyncCallback);
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: RequestVsyncErr
 * @tc.desc: RequestVsync Test Err
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, RequestVsyncErr, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    screenScene.vsyncStation_ = nullptr;
    screenScene.RequestVsync(vsyncCallback);
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: GetVSyncPeriod
 * @tc.desc: GetVSyncPeriod Test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, GetVSyncPeriod, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    screenScene.GetVSyncPeriod();
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: FlushFrameRate
 * @tc.desc: FlushFrameRate Test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, FlushFrameRate, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    uint32_t rate = 120;
    bool isAnimatorStopped = true;
    screenScene.FlushFrameRate(rate, isAnimatorStopped);
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: SetFrameLayoutFinishCallback
 * @tc.desc: SetFrameLayoutFinishCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, SetFrameLayoutFinishCallback, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");;
    screenScene.SetFrameLayoutFinishCallback([](){});
    ASSERT_EQ(1, screenScene.GetWindowId());
}

/**
 * @tc.name: SetDisplayDensity
 * @tc.desc: SetDisplayDensity Test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneTest, SetDisplayDensity, Function | SmallTest | Level3)
{
    ScreenScene screenScene("UNKNOW");
    float density = 2.0f;
    screenScene.SetDisplayDensity(density);
    ASSERT_EQ(1, screenScene.GetWindowId());
}
} // namespace
} // namespace Rosen
} // namespace OHOS
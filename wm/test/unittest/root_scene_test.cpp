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
#include "root_scene.h"
#include <event_handler.h>
#include <input_manager.h>
#include <ui_content.h>
#include <viewport_config.h>

#include "app_mgr_client.h"
#include "singleton.h"
#include "singleton_container.h"

#include "vsync_station.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RootSceneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RootSceneTest::SetUpTestCase()
{
}

void RootSceneTest::TearDownTestCase()
{
}

void RootSceneTest::SetUp()
{
}

void RootSceneTest::TearDown()
{
}

namespace{
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
    std::shared_ptr<AppExecFwk::Configuration> configuration = 
        std::make_shared<AppExecFwk::Configuration>();
    rootScene.UpdateConfiguration(configuration);
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
    std::shared_ptr<AppExecFwk::Configuration> configuration = 
        std::make_shared<AppExecFwk::Configuration>();
    rootScene.UpdateConfigurationForAll(configuration);
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
 * @tc.name: RequestVsync
 * @tc.desc: RequestVsync Test
 * @tc.type: FUNC
 */
HWTEST_F(RootSceneTest, RequestVsync, Function | SmallTest | Level3)
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

}
}// namespace Rosen
}// namespace OHOS
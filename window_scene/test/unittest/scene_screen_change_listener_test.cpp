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

#include "scene_screen_change_listener.h"

#include <gtest/gtest.h>

#include "mock_scene_session.h"
#include "session_info.h"
#include "session_manager/include/scene_session_manager.h"
#include "session/host/include/scene_session.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneScreenChangeListenerTest : public Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: OnPropertyChangeWithRelativePositionChanged
 * @tc.desc: Ensure SceneScreenChangeListener triggers UpdateGlobalDisplayRect on relative position change
 * @tc.type: FUNC
 */
HWTEST_F(SceneScreenChangeListenerTest, OnPropertyChangeWithRelativePositionChanged, TestSize.Level1)
{
    constexpr ScreenId screenId = 1001;
    ScreenProperty property;
    property.SetX(1000);
    property.SetY(1000);

    SessionInfo info;
    auto mockSession = sptr<SceneSessionMocker>::MakeSptr(info, nullptr);
    mockSession->SetScreenId(screenId);

    {
        std::unique_lock<std::shared_mutex> lock(SceneSessionManager::GetInstance().sceneSessionMapMutex_);
        auto& sessionMap = SceneSessionManager::GetInstance().sceneSessionMap_;
        sessionMap.clear();
        sessionMap[1] = mockSession;
    }

    EXPECT_CALL(*mockSession, UpdateGlobalDisplayRect(_, _))
        .Times(1)
        .WillOnce(Return(WSError::WS_OK));

    SceneScreenChangeListener::GetInstance().OnPropertyChange(
        property, ScreenPropertyChangeReason::RELATIVE_POSITION_CHANGE, screenId);
}
} // namespace Rosen
} // namespace OHOS
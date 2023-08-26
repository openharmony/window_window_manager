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


#include "pixel_map.h"
#include "session_listener_controller.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Rosen {
class SessionListenerControllerTest : public testing::Test {
  public:
    SessionListenerControllerTest() {}
    ~SessionListenerControllerTest() {}
};

namespace {
/**
 * @tc.name: AddSessionListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, AddSessionListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: AddSessionListener start";
    SessionListenerController slController;
    sptr<ISessionListener> listener;
    ASSERT_EQ(listener, nullptr);
    WSError res = slController.AddSessionListener(listener);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: AddSessionListener end";
}

/**
 * @tc.name: DelSessionListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, DelSessionListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: DelSessionListener start";
    SessionListenerController slController;
    sptr<ISessionListener> listener;
    ASSERT_EQ(listener, nullptr);
    slController.DelSessionListener(listener);
    int32_t persistentId = 1;
    slController.NotifySessionLabelUpdated(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: DelSessionListener end";
}

/**
 * @tc.name: NotifySessionCreated
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionCreated, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionCreated start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionCreated(persistentId);

    persistentId = 1;
    slController.NotifySessionCreated(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionCreated end";
}

/**
 * @tc.name: NotifySessionDestroyed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionDestroyed, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionDestroyed start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionDestroyed(persistentId);

    persistentId = 1;
    slController.NotifySessionDestroyed(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionDestroyed end";
}

/**
 * @tc.name: HandleUnInstallApp1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, HandleUnInstallApp1, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: HandleUnInstallApp1 start";
    SessionListenerController slController;
    std::list<int32_t> sessions;
    slController.HandleUnInstallApp(sessions);
    int32_t persistentId = 1;
    slController.NotifySessionLabelUpdated(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: HandleUnInstallApp1 end";
}

/**
 * @tc.name: HandleUnInstallApp2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, HandleUnInstallApp2, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: HandleUnInstallApp2 start";
    SessionListenerController slController;
    std::list<int32_t> sessions;
    sessions.push_front(1);
    slController.HandleUnInstallApp(sessions);
    int32_t persistentId = 1;
    slController.NotifySessionLabelUpdated(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: HandleUnInstallApp2 end";
}

/**
 * @tc.name: NotifySessionSnapshotChanged
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionSnapshotChanged, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionSnapshotChanged start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionSnapshotChanged(persistentId);

    persistentId = 1;
    slController.NotifySessionSnapshotChanged(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionSnapshotChanged end";
}

/**
 * @tc.name: NotifySessionMovedToFront
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionMovedToFront, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionMovedToFront start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionMovedToFront(persistentId);

    persistentId = 1;
    slController.NotifySessionMovedToFront(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionMovedToFront end";
}

/**
 * @tc.name: NotifySessionFocused
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionFocused, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionFocused start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionFocused(persistentId);

    persistentId = 1;
    slController.NotifySessionFocused(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionFocused end";
}

/**
 * @tc.name: NotifySessionUnfocused
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionUnfocused, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionUnfocused start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionUnfocused(persistentId);

    persistentId = 1;
    slController.NotifySessionUnfocused(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionUnfocused end";
}

/**
 * @tc.name: NotifySessionClosed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionClosed, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionClosed start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionClosed(persistentId);

    persistentId = 1;
    slController.NotifySessionClosed(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionClosed end";
}

/**
 * @tc.name: NotifySessionLabelUpdated
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionLabelUpdated, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionLabelUpdated start";
    SessionListenerController slController;
    int32_t persistentId = -1;
    slController.NotifySessionLabelUpdated(persistentId);

    persistentId = 1;
    slController.NotifySessionLabelUpdated(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionLabelUpdated end";
}

/**
 * @tc.name: OnListenerDied
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, OnListenerDied, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: OnListenerDied start";
    SessionListenerController slController;
    wptr<IRemoteObject> remote;
    slController.OnListenerDied(remote);

    int32_t persistentId = 1;
    slController.NotifySessionLabelUpdated(persistentId);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: OnListenerDied end";
}

/**
 * @tc.name: NotifySessionIconChanged
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionListenerControllerTest, NotifySessionIconChanged, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionIconChanged start";
    SessionListenerController slController;
    int32_t persistentId = -1;

    int32_t pixelMapWidth = 4;
    int32_t pixelMapHeight = 3;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = std::make_unique<OHOS::Media::PixelMap>();
    OHOS::Media::ImageInfo info;
    info.size.width = pixelMapWidth;
    info.size.height = pixelMapHeight;
    info.pixelFormat = OHOS::Media::PixelFormat::RGB_888;
    pixelMap->SetImageInfo(info);
    std::shared_ptr<OHOS::Media::PixelMap> icon = std::move(pixelMap);
    slController.NotifySessionIconChanged(persistentId, icon);

    persistentId = 1;
    slController.NotifySessionIconChanged(persistentId, icon);
    ASSERT_EQ(persistentId, 1);
    GTEST_LOG_(INFO) << "SessionListenerControllerTest: NotifySessionIconChanged end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
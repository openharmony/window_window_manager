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

#include "mock/mock_session_stub.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include <gtest/gtest.h>
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStubPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStubMocker> session_ = nullptr;
};

void SessionStubPropertyTest::SetUpTestCase() {}

void SessionStubPropertyTest::TearDownTestCase() {}

void SessionStubPropertyTest::SetUp()
{
    session_ = sptr<SessionStubMocker>::MakeSptr();
    EXPECT_NE(nullptr, session_);

    EXPECT_CALL(*session_, OnRemoteRequest(_, _, _, _))
        .WillOnce(Invoke([&](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            return session_->SessionStub::OnRemoteRequest(code, data, reply, option);
        }));
}

void SessionStubPropertyTest::TearDown()
{
    session_ = nullptr;
}

namespace {

/**
 * @tc.name: HandleUpdatePropertyByAction01
 * @tc.desc: No error
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubPropertyTest, HandleUpdatePropertyByAction01, TestSize.Level1)
{
    EXPECT_CALL(*session_, UpdateSessionPropertyByAction(_, _)).WillOnce(Return(WMError::WM_OK));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option{ MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteUint64(static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_MAIN_WINDOW_TOPMOST));
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, ret);
}

/**
 * @tc.name: HandleUpdatePropertyByAction02
 * @tc.desc: Invalid data
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubPropertyTest, HandleUpdatePropertyByAction02, TestSize.Level1)
{
    EXPECT_CALL(*session_, UpdateSessionPropertyByAction(_, _)).Times(0);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option{ MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    const uint32_t invalidData = 0;
    data.WriteUint32(invalidData);
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, ret);
}

/**
 * @tc.name: HandleUpdatePropertyByAction03
 * @tc.desc: No action
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubPropertyTest, HandleUpdatePropertyByAction03, TestSize.Level1)
{
    EXPECT_CALL(*session_, UpdateSessionPropertyByAction(_, _)).Times(0);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option{ MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, ret);
}

class SessionStubWindowShadowsRecorder : public SessionStub {
public:
    SessionStubWindowShadowsRecorder() = default;
    ~SessionStubWindowShadowsRecorder() = default;

    WSError SetWindowShadows(const ShadowsInfo& shadowsInfo) override
    {
        setWindowShadowsCallCount_++;
        lastShadowsInfo_ = shadowsInfo;
        return setWindowShadowsRet_;
    }

    int32_t setWindowShadowsCallCount_ = 0;
    ShadowsInfo lastShadowsInfo_ {};
    WSError setWindowShadowsRet_ = WSError::WS_OK;
};

class SessionStubWindowShadowsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    sptr<SessionStubWindowShadowsRecorder> session_ = nullptr;
};

void SessionStubWindowShadowsTest::SetUpTestCase() {}

void SessionStubWindowShadowsTest::TearDownTestCase() {}

void SessionStubWindowShadowsTest::SetUp()
{
    session_ = sptr<SessionStubWindowShadowsRecorder>::MakeSptr();
    ASSERT_NE(nullptr, session_);
}

void SessionStubWindowShadowsTest::TearDown()
{
    session_ = nullptr;
}

/**
 * @tc.name: HandleSetWindowShadows_NullShadowsInfo
 * @tc.desc: Verify null shadows info returns ERR_INVALID_DATA
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubWindowShadowsTest, HandleSetWindowShadows_NullShadowsInfo, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteParcelable(nullptr);
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_WINDOW_SHADOWS), data, reply, option);
    EXPECT_EQ(ERR_INVALID_DATA, ret);
    EXPECT_EQ(0, session_->setWindowShadowsCallCount_);
}

/**
 * @tc.name: HandleSetWindowShadows_Success
 * @tc.desc: Verify shadows info is parsed and returned code is written into reply
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubWindowShadowsTest, HandleSetWindowShadows_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    ShadowsInfo shadowsInfo { 24.5f, "#11AAEE", 5.0f, -2.5f, true, true, true, true };
    session_->setWindowShadowsRet_ = WSError::WS_OK;
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteParcelable(&shadowsInfo);
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_WINDOW_SHADOWS), data, reply, option);
    EXPECT_EQ(ERR_NONE, ret);
    EXPECT_EQ(1, session_->setWindowShadowsCallCount_);
    EXPECT_EQ(shadowsInfo, session_->lastShadowsInfo_);
    EXPECT_EQ(static_cast<int32_t>(WSError::WS_OK), reply.ReadInt32());
}

class SessionStubAttributeRecorder : public SessionStub {
public:
    SessionStubAttributeRecorder() = default;
    ~SessionStubAttributeRecorder() = default;

    WMError OnUpdateColorMode(const std::string& colorMode, bool hasDarkRes) override
    {
        updateColorModeCallCount_++;
        lastColorMode_ = colorMode;
        lastHasDarkRes_ = hasDarkRes;
        return updateColorModeRet_;
    }

    WSError SetWindowCornerRadius(float cornerRadius) override
    {
        setWindowCornerRadiusCallCount_++;
        lastCornerRadius_ = cornerRadius;
        return setWindowCornerRadiusRet_;
    }

    WMError UpdateScreenshotAppEventRegistered(int32_t persistentId, bool isRegister) override
    {
        updateScreenshotRegisteredCallCount_++;
        lastScreenshotPersistentId_ = persistentId;
        lastScreenshotIsRegister_ = isRegister;
        return updateScreenshotRegisteredRet_;
    }

    WMError UpdateAcrossDisplaysChangeRegistered(bool isRegister) override
    {
        updateAcrossDisplaysRegisteredCallCount_++;
        lastAcrossDisplaysIsRegister_ = isRegister;
        return updateAcrossDisplaysRegisteredRet_;
    }

    WSError GetWaterfallMode(bool& isWaterfallMode) override
    {
        getWaterfallModeCallCount_++;
        isWaterfallMode = waterfallModeValue_;
        return getWaterfallModeRet_;
    }

    WMError IsMainWindowFullScreenAcrossDisplays(bool& isAcrossDisplays) override
    {
        isMainAcrossDisplaysCallCount_++;
        isAcrossDisplays = acrossDisplaysValue_;
        return isMainAcrossDisplaysRet_;
    }

    int32_t updateColorModeCallCount_ = 0;
    std::string lastColorMode_ = "";
    bool lastHasDarkRes_ = false;
    WMError updateColorModeRet_ = WMError::WM_OK;

    int32_t setWindowCornerRadiusCallCount_ = 0;
    float lastCornerRadius_ = 0.0f;
    WSError setWindowCornerRadiusRet_ = WSError::WS_OK;

    int32_t updateScreenshotRegisteredCallCount_ = 0;
    int32_t lastScreenshotPersistentId_ = -1;
    bool lastScreenshotIsRegister_ = false;
    WMError updateScreenshotRegisteredRet_ = WMError::WM_OK;

    int32_t updateAcrossDisplaysRegisteredCallCount_ = 0;
    bool lastAcrossDisplaysIsRegister_ = false;
    WMError updateAcrossDisplaysRegisteredRet_ = WMError::WM_OK;

    int32_t getWaterfallModeCallCount_ = 0;
    bool waterfallModeValue_ = false;
    WSError getWaterfallModeRet_ = WSError::WS_OK;

    int32_t isMainAcrossDisplaysCallCount_ = 0;
    bool acrossDisplaysValue_ = false;
    WMError isMainAcrossDisplaysRet_ = WMError::WM_OK;
};

class SessionStubAttributeInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    sptr<SessionStubAttributeRecorder> session_ = nullptr;
};

void SessionStubAttributeInterfaceTest::SetUpTestCase() {}

void SessionStubAttributeInterfaceTest::TearDownTestCase() {}

void SessionStubAttributeInterfaceTest::SetUp()
{
    session_ = sptr<SessionStubAttributeRecorder>::MakeSptr();
    ASSERT_NE(nullptr, session_);
}

void SessionStubAttributeInterfaceTest::TearDown()
{
    session_ = nullptr;
}

/**
 * @tc.name: HandleUpdateColorMode_MissingColorMode
 * @tc.desc: Verify missing colorMode returns ERR_INVALID_DATA
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleUpdateColorMode_MissingColorMode, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_COLOR_MODE), data, reply, option);
    EXPECT_EQ(ERR_INVALID_DATA, ret);
    EXPECT_EQ(0, session_->updateColorModeCallCount_);
}

/**
 * @tc.name: HandleUpdateColorMode_MissingHasDarkRes
 * @tc.desc: Verify missing hasDarkRes returns ERR_INVALID_DATA
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleUpdateColorMode_MissingHasDarkRes, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    data.WriteString("DARK");
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_COLOR_MODE), data, reply, option);
    EXPECT_EQ(ERR_INVALID_DATA, ret);
    EXPECT_EQ(0, session_->updateColorModeCallCount_);
}

/**
 * @tc.name: HandleUpdateColorMode_Success
 * @tc.desc: Verify OnUpdateColorMode receives expected params
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleUpdateColorMode_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    data.WriteString("LIGHT");
    data.WriteBool(true);
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_COLOR_MODE), data, reply, option);
    EXPECT_EQ(ERR_NONE, ret);
    EXPECT_EQ(1, session_->updateColorModeCallCount_);
    EXPECT_EQ("LIGHT", session_->lastColorMode_);
    EXPECT_EQ(true, session_->lastHasDarkRes_);
}

/**
 * @tc.name: HandleSetWindowCornerRadius_MissingData
 * @tc.desc: Verify missing cornerRadius returns ERR_INVALID_DATA
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleSetWindowCornerRadius_MissingData, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_WINDOW_CORNER_RADIUS), data, reply, option);
    EXPECT_EQ(ERR_INVALID_DATA, ret);
    EXPECT_EQ(0, session_->setWindowCornerRadiusCallCount_);
}

/**
 * @tc.name: HandleSetWindowCornerRadius_Success
 * @tc.desc: Verify SetWindowCornerRadius receives parsed value
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleSetWindowCornerRadius_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    data.WriteFloat(12.5f);
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_WINDOW_CORNER_RADIUS), data, reply, option);
    EXPECT_EQ(ERR_NONE, ret);
    EXPECT_EQ(1, session_->setWindowCornerRadiusCallCount_);
    EXPECT_FLOAT_EQ(12.5f, session_->lastCornerRadius_);
}

/**
 * @tc.name: HandleUpdateScreenshotAppEventRegistered_InvalidData
 * @tc.desc: Verify invalid input for screenshot event registration
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleUpdateScreenshotAppEventRegistered_InvalidData, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SCREEN_SHOT_APP_EVENT_REGISTERED),
        data, reply, option);
    EXPECT_EQ(ERR_INVALID_DATA, ret);
    EXPECT_EQ(0, session_->updateScreenshotRegisteredCallCount_);
}

/**
 * @tc.name: HandleUpdateScreenshotAppEventRegistered_Success
 * @tc.desc: Verify screenshot registration params and reply error code
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleUpdateScreenshotAppEventRegistered_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    session_->updateScreenshotRegisteredRet_ = WMError::WM_OK;
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    data.WriteInt32(101);
    data.WriteBool(true);
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SCREEN_SHOT_APP_EVENT_REGISTERED),
        data, reply, option);
    EXPECT_EQ(ERR_NONE, ret);
    EXPECT_EQ(1, session_->updateScreenshotRegisteredCallCount_);
    EXPECT_EQ(101, session_->lastScreenshotPersistentId_);
    EXPECT_EQ(true, session_->lastScreenshotIsRegister_);
    EXPECT_EQ(static_cast<int32_t>(WMError::WM_OK), reply.ReadInt32());
}

/**
 * @tc.name: HandleUpdateAcrossDisplaysChangeRegistered_InvalidData
 * @tc.desc: Verify missing register flag returns ERR_INVALID_DATA
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleUpdateAcrossDisplaysChangeRegistered_InvalidData, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_ACROSS_DISPLAYS_REGISTERED),
        data, reply, option);
    EXPECT_EQ(ERR_INVALID_DATA, ret);
    EXPECT_EQ(0, session_->updateAcrossDisplaysRegisteredCallCount_);
}

/**
 * @tc.name: HandleUpdateAcrossDisplaysChangeRegistered_Success
 * @tc.desc: Verify across-displays registration params and reply error code
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleUpdateAcrossDisplaysChangeRegistered_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    session_->updateAcrossDisplaysRegisteredRet_ = WMError::WM_OK;
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    data.WriteBool(false);
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_ACROSS_DISPLAYS_REGISTERED),
        data, reply, option);
    EXPECT_EQ(ERR_NONE, ret);
    EXPECT_EQ(1, session_->updateAcrossDisplaysRegisteredCallCount_);
    EXPECT_EQ(false, session_->lastAcrossDisplaysIsRegister_);
    EXPECT_EQ(static_cast<int32_t>(WMError::WM_OK), reply.ReadInt32());
}

/**
 * @tc.name: HandleGetWaterfallMode_Success
 * @tc.desc: Verify waterfall mode is written to reply
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleGetWaterfallMode_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    session_->waterfallModeValue_ = true;
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_WATERFALL_MODE),
        data, reply, option);
    EXPECT_EQ(ERR_NONE, ret);
    EXPECT_EQ(1, session_->getWaterfallModeCallCount_);
    EXPECT_EQ(true, reply.ReadBool());
}

/**
 * @tc.name: HandleIsMainWindowFullScreenAcrossDisplays_Success
 * @tc.desc: Verify return code and status are written to reply
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubAttributeInterfaceTest, HandleIsMainWindowFullScreenAcrossDisplays_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option { MessageOption::TF_SYNC };
    session_->acrossDisplaysValue_ = true;
    session_->isMainAcrossDisplaysRet_ = WMError::WM_OK;
    data.WriteInterfaceToken(SessionStub::GetDescriptor());
    int ret = session_->OnRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MAIN_WINDOW_FULL_SCREEN_ACROSS_DISPLAYS),
        data, reply, option);
    EXPECT_EQ(ERR_NONE, ret);
    EXPECT_EQ(1, session_->isMainAcrossDisplaysCallCount_);
    EXPECT_EQ(static_cast<int32_t>(WMError::WM_OK), reply.ReadInt32());
    EXPECT_EQ(true, reply.ReadBool());
}
} // namespace
} // namespace Rosen
} // namespace OHOS

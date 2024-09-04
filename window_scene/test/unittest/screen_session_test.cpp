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
#include "screen_session.h"
#include <gtest/gtest.h>
#include "session_manager/include/screen_session_manager.h"

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class ScreenSessionTest : public testing::Test {
  public:
    ScreenSessionTest() {}
    ~ScreenSessionTest() {}
};

namespace {

/**
 * @tc.name: create ScreenSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession02, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession03, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_MIRROR);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession04, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession05
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession05, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::INVALID);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: CreateDisplayNode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CreateDisplayNode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CreateDisplayNode start";
    Rosen::RSDisplayNodeConfig rsConfig;
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    rsConfig.isMirrored = true;
    rsConfig.screenId = 101;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    screenSession->CreateDisplayNode(rsConfig);
    GTEST_LOG_(INFO) << "ScreenSessionTest: CreateDisplayNode end";
}

/**
 * @tc.name: SetMirrorScreenType
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetMirrorScreenType, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetMirrorScreenType start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    Rosen::RSDisplayNodeConfig rsConfig;
    rsConfig.isMirrored = true;
    rsConfig.screenId = 101;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    MirrorScreenType mirrorType = MirrorScreenType::VIRTUAL_MIRROR;
    screenSession->SetMirrorScreenType(mirrorType);
    GTEST_LOG_(INFO) << "SetMirrorScreenType end";
}

/**
 * @tc.name: GetMirrorScreenType
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetMirrorScreenType, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetMirrorScreenType start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    MirrorScreenType mirrorType = MirrorScreenType::VIRTUAL_MIRROR;
    screenSession->SetMirrorScreenType(mirrorType);
    MirrorScreenType res = screenSession->GetMirrorScreenType();
    ASSERT_EQ(mirrorType, res);
    GTEST_LOG_(INFO) << "GetMirrorScreenType end";
}

/**
 * @tc.name: SetDefaultDeviceRotationOffset
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetDefaultDeviceRotationOffset, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetDefaultDeviceRotationOffset start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t defaultRotationOffset = 0;
    screenSession->SetDefaultDeviceRotationOffset(defaultRotationOffset);
    GTEST_LOG_(INFO) << "SetDefaultDeviceRotationOffset end";
}

/**
 * @tc.name: UpdateDisplayState
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateDisplayState, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateDisplayState start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DisplayState displayState = DisplayState::OFF;
    screenSession->UpdateDisplayState(displayState);
    GTEST_LOG_(INFO) << "UpdateDisplayState end";
}

/**
 * @tc.name: UpdateRefreshRate
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateRefreshRate, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateRefreshRate start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t refreshRate = 2;
    screenSession->UpdateRefreshRate(refreshRate);
    GTEST_LOG_(INFO) << "UpdateRefreshRate end";
}

/**
 * @tc.name: GetRefreshRate
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetRefreshRate, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetRefreshRate start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t refreshRate = 2;
    screenSession->UpdateRefreshRate(refreshRate);
    auto res = screenSession->GetRefreshRate();
    ASSERT_EQ(refreshRate, res);
    GTEST_LOG_(INFO) << "GetRefreshRate end";
}

/**
 * @tc.name: UpdatePropertyByResolution
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdatePropertyByResolution, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdatePropertyByResolution start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t width = 1000;
    uint32_t height = 1500;
    screenSession->UpdatePropertyByResolution(width, height);
    GTEST_LOG_(INFO) << "UpdatePropertyByResolution end";
}

/**
 * @tc.name: HandleSensorRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, HandleSensorRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "HandleSensorRotation start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    float sensorRotation = 0.0f;
    screenSession->HandleSensorRotation(sensorRotation);
    GTEST_LOG_(INFO) << "HandleSensorRotation end";
}

/**
 * @tc.name: ConvertIntToRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ConvertIntToRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertIntToRotation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int rotation = 0;
    auto res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_0);
    rotation = 90;
    res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_90);
    rotation = 180;
    res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_180);
    rotation = 270;
    res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_270);
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertIntToRotation end";
}

/**
 * @tc.name: SetVirtualScreenFlag
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetVirtualScreenFlag, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetVirtualScreenFlag start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    VirtualScreenFlag screenFlag = VirtualScreenFlag::CAST;
    screenSession->SetVirtualScreenFlag(screenFlag);
    GTEST_LOG_(INFO) << "SetVirtualScreenFlag end";
}

/**
 * @tc.name: GetVirtualScreenFlag
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetVirtualScreenFlag, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetVirtualScreenFlag start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    VirtualScreenFlag screenFlag = VirtualScreenFlag::CAST;
    screenSession->SetVirtualScreenFlag(screenFlag);
    auto res = screenSession->GetVirtualScreenFlag();
    ASSERT_EQ(screenFlag, res);
    GTEST_LOG_(INFO) << "GetVirtualScreenFlag end";
}

/**
 * @tc.name: UpdateToInputManager
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateToInputManager, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateToInputManager start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    RRect bounds;
    int rotation = 90;
    FoldDisplayMode foldDisplayMode = FoldDisplayMode::FULL;
    screenSession->UpdateToInputManager(bounds, rotation, foldDisplayMode);
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    rotation = 0;
    foldDisplayMode = FoldDisplayMode::MAIN;
    screenSession->UpdateToInputManager(bounds, rotation, foldDisplayMode);
    GTEST_LOG_(INFO) << "UpdateToInputManager end";
}

/**
 * @tc.name: UpdatePropertyAfterRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdatePropertyAfterRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdatePropertyAfterRotation start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    int rotation = 90;
    FoldDisplayMode foldDisplayMode = FoldDisplayMode::MAIN;
    screenSession->UpdatePropertyAfterRotation(bounds, rotation, foldDisplayMode);
    GTEST_LOG_(INFO) << "UpdatePropertyAfterRotation end";
}

/**
 * @tc.name: ReportNotifyModeChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ReportNotifyModeChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ReportNotifyModeChange start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DisplayOrientation displayOrientation = DisplayOrientation::LANDSCAPE;
    screenSession->ReportNotifyModeChange(displayOrientation);
    displayOrientation = DisplayOrientation::PORTRAIT;
    screenSession->ReportNotifyModeChange(displayOrientation);
    GTEST_LOG_(INFO) << "ReportNotifyModeChange end";
}

/**
 * @tc.name: UpdateRotationAfterBoot
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateRotationAfterBoot, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateRotationAfterBoot start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    bool foldToExpand = true;
    screenSession->UpdateRotationAfterBoot(foldToExpand);
    GTEST_LOG_(INFO) << "UpdateRotationAfterBoot end";
}

/**
 * @tc.name: SetScreenSceneDpiChangeListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenSceneDpiChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenSceneDpiChangeListener start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    SetScreenSceneDpiFunc func = nullptr;
    screenSession->SetScreenSceneDpiChangeListener(func);
    GTEST_LOG_(INFO) << "SetScreenSceneDpiChangeListener end";
}

/**
 * @tc.name: SetScreenSceneDestroyListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenSceneDestroyListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenSceneDestroyListener start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DestroyScreenSceneFunc func = nullptr;
    screenSession->SetScreenSceneDestroyListener(func);
    GTEST_LOG_(INFO) << "SetScreenSceneDestroyListener end";
}

/**
 * @tc.name: SetScreenSceneDpi
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenSceneDpi, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenSceneDpi start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    float density = 1.5f;
    SetScreenSceneDpiFunc func = nullptr;
    screenSession->SetScreenSceneDpiChangeListener(func);
    screenSession->SetScreenSceneDpi(density);
    SetScreenSceneDpiFunc func2 = [](float density) {
            EXPECT_TRUE(true);
        };
    screenSession->SetScreenSceneDpiChangeListener(func2);
    screenSession->SetScreenSceneDpi(density);
    GTEST_LOG_(INFO) << "SetScreenSceneDpi end";
}

/**
 * @tc.name: DestroyScreenScene
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, DestroyScreenScene, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DestroyScreenScene start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DestroyScreenSceneFunc func = nullptr;
    screenSession->SetScreenSceneDestroyListener(func);
    screenSession->DestroyScreenScene();
    DestroyScreenSceneFunc func2 = []() {
            EXPECT_TRUE(true);
        };
    screenSession->SetScreenSceneDestroyListener(func2);
    screenSession->DestroyScreenScene();
    GTEST_LOG_(INFO) << "DestroyScreenScene end";
}

/**
 * @tc.name: SetDensityInCurResolution
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetDensityInCurResolution, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetDensityInCurResolution start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    float densityInCurResolution = 1.5f;
    screenSession->SetDensityInCurResolution(densityInCurResolution);
    GTEST_LOG_(INFO) << "SetDensityInCurResolution end";
}

/**
 * @tc.name: GetSourceMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSourceMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->screenId_ = session->defaultScreenId_;
    ScreenSourceMode mode = session->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_MAIN);
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_ALONE);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_EXPAND);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_EXTEND);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_MIRROR);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_UNIQUE);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_UNIQUE);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode end";
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetPixelFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetPixelFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicPixelFormat pixelFormat;
    auto res = screenSession->GetPixelFormat(pixelFormat);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "GetPixelFormat end";
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetPixelFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetPixelFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    auto res = screenSession->SetPixelFormat(pixelFormat);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "SetPixelFormat end";
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSupportedHDRFormats, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetSupportedHDRFormats start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<ScreenHDRFormat> hdrFormats;
    auto res = screenSession->GetSupportedHDRFormats(hdrFormats);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "GetSupportedHDRFormats end";
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenHDRFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetScreenHDRFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    ScreenHDRFormat hdrFormat;
    auto res = screenSession->GetScreenHDRFormat(hdrFormat);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "GetScreenHDRFormat end";
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenHDRFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenHDRFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    int32_t modeIdx = 0;
    auto res = screenSession->SetScreenHDRFormat(modeIdx);
    ASSERT_EQ(res, DMError::DM_OK);
    modeIdx = -1;
    res = screenSession->SetScreenHDRFormat(modeIdx);
    ASSERT_EQ(res, DMError::DM_ERROR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "SetScreenHDRFormat end";
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSupportedColorSpaces, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetSupportedColorSpaces start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    auto res = screenSession->GetSupportedColorSpaces(colorSpaces);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "GetSupportedColorSpaces end";
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenColorSpace, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetScreenColorSpace start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicCM_ColorSpaceType colorSpace;
    auto res = screenSession->GetScreenColorSpace(colorSpace);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "GetScreenColorSpace end";
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenColorSpace, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenColorSpace start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    screenSession->SetScreenColorSpace(colorSpace);
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "SetScreenColorSpace end";
}

/**
 * @tc.name: SetPrivateSessionForeground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetPrivateSessionForeground, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetPrivateSessionForeground start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    bool hasPrivate = true;
    screenSession->SetPrivateSessionForeground(hasPrivate);
    auto res = screenSession->HasPrivateSessionForeground();
    ASSERT_EQ(res, hasPrivate);
    GTEST_LOG_(INFO) << "SetPrivateSessionForeground end";
}

/**
 * @tc.name: GetScreenCombination
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenCombination, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetScreenCombination start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    ScreenCombination combination { ScreenCombination::SCREEN_ALONE };
    screenSession->SetScreenCombination(combination);
    auto res = screenSession->GetScreenCombination();
    ASSERT_EQ(res, combination);
    GTEST_LOG_(INFO) << "GetScreenCombination end";
}

/**
 * @tc.name: Resize
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, Resize, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "Resize start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t width = 1000;
    uint32_t height = 1000;
    screenSession->Resize(width, height);
    GTEST_LOG_(INFO) << "Resize end";
}

/**
 * @tc.name: UpdateAvailableArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateAvailableArea, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateAvailableArea start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DMRect area = screenSession->GetAvailableArea();
    auto res = screenSession->UpdateAvailableArea(area);
    ASSERT_EQ(res, false);
    area = {2, 2, 2, 2};
    res = screenSession->UpdateAvailableArea(area);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "UpdateAvailableArea end";
}

/**
 * @tc.name: SetAvailableArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetAvailableArea, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetAvailableArea start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DMRect area = {2, 2, 2, 2};
    screenSession->SetAvailableArea(area);
    GTEST_LOG_(INFO) << "SetAvailableArea end";
}

/**
 * @tc.name: GetAvailableArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetAvailableArea, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetAvailableArea start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DMRect area = {2, 2, 2, 2};
    screenSession->SetAvailableArea(area);
    auto res = screenSession->GetAvailableArea();
    ASSERT_EQ(res, area);
    GTEST_LOG_(INFO) << "GetAvailableArea end";
}

/**
 * @tc.name: SetFoldScreen
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetFoldScreen, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetFoldScreen start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    bool isFold = false;
    screenSession->SetFoldScreen(isFold);
    GTEST_LOG_(INFO) << "SetFoldScreen end";
}

/**
 * @tc.name: SetHdrFormats
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetHdrFormats, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetHdrFormats start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<uint32_t> hdrFormats = { 0, 0, 0, 0 };
    screenSession->SetHdrFormats(std::move(hdrFormats));
    GTEST_LOG_(INFO) << "SetHdrFormats end";
}

/**
 * @tc.name: SetColorSpaces
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetColorSpaces, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetColorSpaces start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<uint32_t> colorSpaces = { 0, 0, 0, 0 };
    screenSession->SetColorSpaces(std::move(colorSpaces));
    GTEST_LOG_(INFO) << "SetColorSpaces end";
}

/**
 * @tc.name: SetDisplayNodeScreenId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetDisplayNodeScreenId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetDisplayNodeScreenId start";
    ScreenId screenId = 0;
    ScreenId rsId = 1;
    std::string name = "OpenHarmony";
    ScreenProperty property;
    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSDisplayNode> displayNode = RSDisplayNode::Create(displayNodeConfig);
    EXPECT_NE(nullptr, displayNode);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, rsId, name, property, displayNode);
    session->SetDisplayNodeScreenId(screenId);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetDisplayNodeScreenId end";
}

/**
 * @tc.name: UnregisterScreenChangeListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UnregisterScreenChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener start";
    IScreenChangeListener* screenChangeListener = nullptr;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int64_t ret = 0;
    session->UnregisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener end";
}

/**
 * @tc.name: UnregisterScreenChangeListener02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UnregisterScreenChangeListener02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener02 start";
    IScreenChangeListener* screenChangeListener = new ScreenSessionManager();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int64_t ret = 0;
    session->UnregisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener02 end";
}

/**
 * @tc.name: ConvertToDisplayInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ConvertToDisplayInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToDisplayInfo start";
    sptr<DisplayInfo> displayInfo = new(std::nothrow) DisplayInfo();
    EXPECT_NE(displayInfo, nullptr);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    EXPECT_NE(nullptr, session->ConvertToDisplayInfo());

    sptr<ScreenInfo> info = new(std::nothrow) ScreenInfo();
    EXPECT_NE(info, nullptr);
    EXPECT_NE(nullptr, session->ConvertToScreenInfo());
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToDisplayInfo end";
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenSupportedColorGamuts, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSupportedColorGamuts start";
    std::vector<ScreenColorGamut> colorGamuts;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    DMError ret = session->GetScreenSupportedColorGamuts(colorGamuts);
    ASSERT_EQ(ret, DMError::DM_OK);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSupportedColorGamuts end";
}

/**
 * @tc.name: GetActiveScreenMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetActiveScreenMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetActiveScreenMode start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    sptr<SupportedScreenModes> mode1 = session->GetActiveScreenMode();
    session->GetActiveScreenMode();
    ASSERT_EQ(mode1, nullptr);

    session->activeIdx_ = 100;
    sptr<SupportedScreenModes> mode2 = session->GetActiveScreenMode();
    session->GetActiveScreenMode();
    ASSERT_EQ(mode2, nullptr);

    GTEST_LOG_(INFO) << "ScreenSessionTest: GetActiveScreenMode end";
}

/**
 * @tc.name: SetScreenCombination
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenCombination, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenCombination start";
    ScreenId screenId = 2024;
    ScreenProperty property;
    NodeId nodeId = 0;
    ScreenId defaultScreenId = 0;
    ScreenCombination combination { ScreenCombination::SCREEN_ALONE };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, property, nodeId, defaultScreenId);
    session->SetScreenCombination(combination);
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(combination, session->GetScreenCombination());
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenCombination end";
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenColorGamut, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenColorGamut start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    ScreenColorGamut colorGamut;
    DMError res = session->GetScreenColorGamut(colorGamut);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenColorGamut end";
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenColorGamut, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    int32_t colorGamut = 1;
    DMError res = session->SetScreenColorGamut(colorGamut);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut end";
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenGamutMap, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenGamutMap start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    ScreenGamutMap gamutMap;
    DMError res = session->GetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenGamutMap end";
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenGamutMap, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenGamutMap start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);
    ScreenGamutMap gamutMap = GAMUT_MAP_CONSTANT;
    DMError res = session->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);

    gamutMap = GAMUT_MAP_HDR_EXTENSION;
    res = session->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenGamutMap end";
}

/**
 * @tc.name: InitRSDisplayNode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, InitRSDisplayNode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: InitRSDisplayNode start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);
    RSDisplayNodeConfig config;
    Point startPoint;
    int res = 0;
    sessionGroup.InitRSDisplayNode(config, startPoint);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: InitRSDisplayNode end";
}

/**
 * @tc.name: GetRSDisplayNodeConfig
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetRSDisplayNodeConfig, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    RSDisplayNodeConfig config = {1};
    sptr<ScreenSession> defaultScreenSession;
    bool res = sessionGroup.GetRSDisplayNodeConfig(session0, config, defaultScreenSession);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    sessionGroup.combination_ = ScreenCombination::SCREEN_ALONE;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);

    sessionGroup.combination_ = ScreenCombination::SCREEN_EXPAND;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);

    sessionGroup.combination_ = ScreenCombination::SCREEN_MIRROR;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig end";
}

/**
 * @tc.name: AddChild
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, AddChild, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChild start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    Point startPoint(0, 0);
    sptr<ScreenSession> defaultScreenSession;
    bool res = sessionGroup.AddChild(session0, startPoint, defaultScreenSession);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    res = sessionGroup.AddChild(session, startPoint, defaultScreenSession);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChild end";
}

/**
 * @tc.name: AddChildren
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, AddChildren, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    std::vector<sptr<ScreenSession>> smsScreens;
    std::vector<Point> startPoints;
    bool res = sessionGroup.AddChildren(smsScreens, startPoints);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren end";
}

/**
 * @tc.name: RemoveChild
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, RemoveChild, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: RemoveChild start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    bool res = sessionGroup.RemoveChild(session0);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    res = sessionGroup.RemoveChild(session);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig end";
}

/**
 * @tc.name: GetChildPosition
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetChildPosition, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetChildPosition start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    ScreenId screenId = 1;
    Point res = sessionGroup.GetChildPosition(screenId);
    ASSERT_EQ(res.posX_, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetChildPosition end";
}

/**
 * @tc.name: ConvertToScreenGroupInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ConvertToScreenGroupInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToScreenGroupInfo start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenGroupInfo> res = sessionGroup.ConvertToScreenGroupInfo();
    ASSERT_NE(res, nullptr);
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToScreenGroupInfo end";
}

/**
 * @tc.name: RegisterScreenChangeListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, RegisterScreenChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: RegisterScreenChangeListener start";
    int res = 0;
    IScreenChangeListener* screenChangeListener = nullptr;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->RegisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: RegisterScreenChangeListener end";
}

/**
 * @tc.name: UpdatePropertyByActiveMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdatePropertyByActiveMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: UpdatePropertyByActiveMode start";
    int res = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->UpdatePropertyByActiveMode();
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: UpdatePropertyByActiveMode end";
}

/**
 * @tc.name: Disconnect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, Disconnect, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: Disconnect start";
    int res = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->Disconnect();
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: Disconnect end";
}

/**
 * @tc.name: SensorRotationChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SensorRotationChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SensorRotationChange start";
    int res = 0;
    Rotation sensorRotation = Rotation::ROTATION_0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SensorRotationChange(sensorRotation);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SensorRotationChange end";
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetOrientation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation orientation = Orientation::UNSPECIFIED;
    session->SetOrientation(orientation);
    Orientation res = session->GetOrientation();
    ASSERT_EQ(res, orientation);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetOrientation end";
}

/**
 * @tc.name: SetScreenRequestedOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRequestedOrientation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation orientation = Orientation::UNSPECIFIED;
    session->SetScreenRequestedOrientation(orientation);
    Orientation res = session->GetScreenRequestedOrientation();
    ASSERT_EQ(res, orientation);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRequestedOrientation end";
}

/**
 * @tc.name: SetUpdateToInputManagerCallback
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetUpdateToInputManagerCallback, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetUpdateToInputManagerCallback start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    auto func = [session](float x) {
        session->SetVirtualPixelRatio(x);
    };
    session->SetUpdateToInputManagerCallback(func);
    EXPECT_NE(nullptr, &func);

    ScreenProperty screenPropert = session->GetScreenProperty();
    EXPECT_NE(nullptr, &screenPropert);
    session->SetVirtualPixelRatio(3.14);
     
    ScreenType screenType { ScreenType::REAL };
    session->SetScreenType(screenType);

    GTEST_LOG_(INFO) << "ScreenSessionTest: SetUpdateToInputManagerCallbackend";
}
/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenRotationLocked, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLocked start";
    bool isLocked = true;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SetScreenRotationLocked(isLocked);
    bool res = session->IsScreenRotationLocked();
    ASSERT_EQ(res, isLocked);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLocked end";
}

/**
 * @tc.name: SetScreenRotationLockedFromJs
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenRotationLockedFromJs, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLockedFromJs start";
    bool isLocked = true;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SetScreenRotationLockedFromJs(isLocked);
    bool res = session->IsScreenRotationLocked();
    ASSERT_EQ(res, isLocked);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLockedFromJs end";
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, IsScreenRotationLocked, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: IsScreenRotationLocked start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    bool res = session->IsScreenRotationLocked();
    ASSERT_EQ(res, session->isScreenLocked_);
    GTEST_LOG_(INFO) << "ScreenSessionTest: IsScreenRotationLocked end";
}

/**
 * @tc.name: GetScreenRequestedOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenRequestedOrientation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation res = session->GetScreenRequestedOrientation();
    ASSERT_EQ(res, session->property_.GetScreenRequestedOrientation());
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenRequestedOrientation end";
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetVirtualPixelRatio start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    float virtualPixelRatio = 1;
    session->SetVirtualPixelRatio(virtualPixelRatio);
    float res = session->property_.GetVirtualPixelRatio();
    ASSERT_EQ(res, virtualPixelRatio);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetVirtualPixelRatio end";
}

/**
 * @tc.name: screen_session_test001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test001, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test001 start";
    IScreenChangeListener* screenChangeListener = nullptr;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->screenState_ = ScreenState::CONNECTION;
    int res = 0;
    session->RegisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test001 end";
}

/**
 * @tc.name: screen_session_test002
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test002, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test002 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ScreenId res = session->GetScreenId();
    ASSERT_EQ(res, session->screenId_);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test002 end";
}

/**
 * @tc.name: screen_session_test003
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test003, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test003 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    DMError res = session->SetScreenColorTransform();
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test003 end";
}

/**
 * @tc.name: screen_session_test004
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test004, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test004 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ScreenProperty newProperty;
    int res = 0;
    session->UpdatePropertyByFoldControl(newProperty);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test004 end";
}

/**
 * @tc.name: screen_session_test005
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test005, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test005 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ScreenProperty newProperty;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::CHANGE_MODE;
    int res = 0;
    session->PropertyChange(newProperty, reason);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test005 end";
}

/**
 * @tc.name: screen_session_test006
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test006, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test006 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Rotation sensorRotation = Rotation::ROTATION_90;
    float res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 90.f);
    sensorRotation = Rotation::ROTATION_180;
    res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 180.f);
    sensorRotation = Rotation::ROTATION_270;
    res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 270.f);
    sensorRotation = Rotation::ROTATION_0;
    res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 0.f);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test006 end";
}

/**
 * @tc.name: screen_session_test007
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test007, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test007 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation orientation = Orientation::UNSPECIFIED;
    int res = 0;
    session->ScreenOrientationChange(orientation, FoldDisplayMode::UNKNOWN);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test007 end";
}

/**
 * @tc.name: screen_session_test008
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test008, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test008 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    bool res = session->HasPrivateSessionForeground();
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test008 end";
}

/**
 * @tc.name: screen_session_test009
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test009, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test009 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Rotation rotation = Rotation::ROTATION_90;
    session->SetRotation(rotation);
    Rotation res = session->GetRotation();
    ASSERT_EQ(res, rotation);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test009 end";
}

/**
 * @tc.name: screen_session_test010
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test010, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test010 start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    ScreenId childScreen = 1;
    bool res = sessionGroup.HasChild(childScreen);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test010 end";
}

/**
 * @tc.name: screen_session_test011
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test011, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test011 start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    std::vector<sptr<ScreenSession>> res = sessionGroup.GetChildren();
    ASSERT_EQ(res.empty(), true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test011 end";
}

/**
 * @tc.name: screen_session_test012
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test012, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test012 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int res = 0;
    RectF rect = RectF(0, 0, 0, 0);
    uint32_t offsetY = 0;
    session->SetDisplayBoundary(rect, offsetY);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test012 end";
}

/**
 * @tc.name: GetName
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetName, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetName start";
    std::string name { "UNKNOW" };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_EQ(name, session->GetName());
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetName end";
}

/**
 * @tc.name: SetName
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetName, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetName start";
    std::string name { "UNKNOW" };
    int ret = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SetName(name);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetName end";
}

/**
 * @tc.name: GetScreenSnapshot
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenSnapshot, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSnapshot start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->displayNode_ = nullptr;
    auto pixelmap = session->GetScreenSnapshot(1.0, 1.0);
    EXPECT_EQ(pixelmap, nullptr);

    ScreenProperty newScreenProperty;
    session = new(std::nothrow) ScreenSession(0, newScreenProperty, 0);
    pixelmap = session->GetScreenSnapshot(1.0, 1.0);
    int ret = 0;
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSnapshot end";
}

/**
 * @tc.name: GetRSScreenId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetRSScreenId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSScreenId start";
    ScreenProperty property;
    sptr<ScreenSession> session = new(std::nothrow)
        ScreenSession("OpenHarmony", 1, 100, 0);
    EXPECT_EQ(100, session->GetRSScreenId());

    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSDisplayNode> displayNode = RSDisplayNode::Create(displayNodeConfig);
    session = new ScreenSession(1, 100, "OpenHarmony",
        property, displayNode);
    EXPECT_NE(nullptr, session->GetDisplayNode());
    session->ReleaseDisplayNode();
    EXPECT_EQ(nullptr, session->GetDisplayNode());
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSScreenId end";
}

/**
 * @tc.name: CalcRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcRotation start";
    Orientation orientation { Orientation::BEGIN };
    FoldDisplayMode foldDisplayMode { FoldDisplayMode::COORDINATION };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    auto res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    session->modes_ = { supportedScreenModes };
    session->activeIdx_ = 0;

    orientation = Orientation::UNSPECIFIED;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    orientation = Orientation::VERTICAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_90, res);
    
    orientation = Orientation::HORIZONTAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    orientation = Orientation::REVERSE_VERTICAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_270, res);

    orientation = Orientation::REVERSE_HORIZONTAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_180, res);

    orientation = Orientation::LOCKED;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcRotation end";
}

/**
 * @tc.name: CalcDisplayOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDisplayOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcDisplayOrientation start";
    Rotation rotation { Rotation::ROTATION_0 };
    FoldDisplayMode foldDisplayMode { FoldDisplayMode::COORDINATION };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    auto res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE, res);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    session->modes_ = { supportedScreenModes };
    session->activeIdx_ = 0;

    rotation = Rotation::ROTATION_0;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE, res);

    rotation = Rotation::ROTATION_90;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::PORTRAIT, res);

    rotation = Rotation::ROTATION_180;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE_INVERTED, res);

    rotation = Rotation::ROTATION_270;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::PORTRAIT_INVERTED, res);

    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcDisplayOrientation end";
}

/**
 * @tc.name: FillScreenInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, FillScreenInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: FillScreenInfo start";
    ScreenProperty property;
    property.SetVirtualPixelRatio(3.1415927);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(2024, property, 0);
    sptr<ScreenInfo> info = nullptr;
    session->FillScreenInfo(info);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    session->modes_ = { supportedScreenModes };
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    info = new ScreenInfo;
    session->FillScreenInfo(info);
    EXPECT_NE(nullptr, info);
    GTEST_LOG_(INFO) << "ScreenSessionTest: FillScreenInfo end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS

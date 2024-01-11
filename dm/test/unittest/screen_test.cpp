/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "display_manager.h"
#include "display_manager_proxy.h"
#include "screen_manager.h"
#include "screen_manager_utils.h"
#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<ScreenManagerAdapter, MockScreenManagerAdapter>;
class ScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<Display> defaultDisplay_;
    static ScreenId defaultScreenId_;
    static sptr<Screen> screen_;
};
sptr<Display> ScreenTest::defaultDisplay_ = nullptr;
ScreenId ScreenTest::defaultScreenId_ = SCREEN_ID_INVALID;
sptr<Screen> ScreenTest::screen_ = nullptr;

void ScreenTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultScreenId_ = static_cast<ScreenId>(defaultDisplay_->GetId());
    screen_ = ScreenManager::GetInstance().GetScreenById(defaultScreenId_);
}

void ScreenTest::TearDownTestCase()
{
    defaultDisplay_ = nullptr;
    screen_ = nullptr;
}

void ScreenTest::SetUp()
{
}

void ScreenTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetBasicProperty01
 * @tc.desc: Basic property getter test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetBasicProperty01, Function | SmallTest | Level1)
{
    ASSERT_GT(screen_->GetName().size(), 0);
    ASSERT_GT(screen_->GetWidth(), 0);
    ASSERT_GT(screen_->GetHeight(), 0);
    ASSERT_GT(screen_->GetVirtualWidth(), 0);
    ASSERT_GT(screen_->GetVirtualHeight(), 0);
    ASSERT_GT(screen_->GetVirtualPixelRatio(), 0);
    ASSERT_EQ(screen_->IsReal(), true);
    ASSERT_NE(screen_->GetScreenInfo(), nullptr);
}

/**
 * @tc.name: SetScreenActiveMode01
 * @tc.desc: SetScreenActiveMode with valid modeId and return success
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenActiveMode01, Function | SmallTest | Level1)
{
    auto supportedModes = screen_->GetSupportedModes();
    ASSERT_GT(supportedModes.size(), 0);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenActiveMode(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    DMError res = screen_->SetScreenActiveMode(supportedModes.size() - 1);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetScreenActiveMode02
 * @tc.desc: SetScreenActiveMode with valid modeId and return failed
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenActiveMode02, Function | SmallTest | Level1)
{
    auto supportedModes = screen_->GetSupportedModes();
    ASSERT_GT(supportedModes.size(), 0);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenActiveMode(_, _)).Times(1).WillOnce(Return(DMError::DM_ERROR_NULLPTR));
    DMError res = screen_->SetScreenActiveMode(supportedModes.size() - 1);
    ASSERT_TRUE(DMError::DM_OK != res);
}

/**
 * @tc.name: GetScreenSupportedColorGamuts01
 * @tc.desc: GetScreenSupportedColorGamuts
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetScreenSupportedColorGamuts01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetScreenSupportedColorGamuts(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    std::vector<ScreenColorGamut> colorGamuts;
    auto res = screen_->GetScreenSupportedColorGamuts(colorGamuts);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: GetScreenColorGamut01
 * @tc.desc: GetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetScreenColorGamut01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetScreenColorGamut(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    auto res = screen_->GetScreenColorGamut(colorGamut);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetScreenColorGamut01
 * @tc.desc: SetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenColorGamut01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenColorGamut(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    auto res = screen_->SetScreenColorGamut(colorGamut);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: GetScreenGamutMap01
 * @tc.desc: GetScreenGamutMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetScreenGamutMap01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetScreenGamutMap(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto res = screen_->GetScreenGamutMap(gamutMap);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetScreenGamutMap01
 * @tc.desc: SetScreenGamutMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenGamutMap01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenGamutMap(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto res = screen_->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetScreenColorTransform01
 * @tc.desc: SetScreenColorTransform
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenColorTransform01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenColorTransform(_)).Times(1).WillOnce(Return(DMError::DM_OK));
    auto res = screen_->SetScreenColorTransform();
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: IsGroup
 * @tc.desc: for interface coverage and check IsGroup
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, IsGroup, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<ScreenInfo> screenInfo = screen_->GetScreenInfo();
    screenInfo->SetIsScreenGroup(true);
    EXPECT_CALL(m->Mock(), GetScreenInfo(_)).Times(1).WillOnce(Return(screenInfo));
    ASSERT_EQ(true, screen_->IsGroup());
    screenInfo->SetIsScreenGroup(false);
    EXPECT_CALL(m->Mock(), GetScreenInfo(_)).Times(1).WillOnce(Return(screenInfo));
    ASSERT_EQ(false, screen_->IsGroup());
}

/**
 * @tc.name: GetParentId
 * @tc.desc: for interface coverage and check GetParentId
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetParentId, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<ScreenInfo> screenInfo = screen_->GetScreenInfo();
    screenInfo->SetParentId(0);
    EXPECT_CALL(m->Mock(), GetScreenInfo(_)).Times(1).WillOnce(Return(screenInfo));
    ASSERT_EQ(0, screen_->GetParentId());
    screenInfo->SetParentId(SCREEN_ID_INVALID);
    EXPECT_CALL(m->Mock(), GetScreenInfo(_)).Times(1).WillOnce(Return(screenInfo));
    ASSERT_EQ(SCREEN_ID_INVALID, screen_->GetParentId());
}

/**
 * @tc.name: GetRotation
 * @tc.desc: for interface coverage and check GetRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetRotation, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<ScreenInfo> screenInfo = screen_->GetScreenInfo();
    screenInfo->SetParentId(0);
    EXPECT_CALL(m->Mock(), GetScreenInfo(_)).Times(1).WillOnce(Return(screenInfo));
    ASSERT_EQ(Rotation::ROTATION_0, screen_->GetRotation());
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetOrientation, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetOrientation(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    Orientation orientation = Orientation{0};
    auto res = screen_->SetOrientation(orientation);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: GetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetPixelFormat, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetPixelFormat(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    auto res = screen_->GetPixelFormat(pixelFormat);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: SetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetPixelFormat, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetPixelFormat(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    auto res = screen_->SetPixelFormat(pixelFormat);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: GetSupportedHDRFormats
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetSupportedHDRFormats, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSupportedHDRFormats(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    std::vector<ScreenHDRFormat> hdrFormats;
    auto res = screen_->GetSupportedHDRFormats(hdrFormats);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: GetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetScreenHDRFormat, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetScreenHDRFormat(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    ScreenHDRFormat hdrFormat = ScreenHDRFormat{0};
    auto res = screen_->GetScreenHDRFormat(hdrFormat);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: SetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenHDRFormat, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenHDRFormat(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    auto res = screen_->SetScreenHDRFormat(0);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: GetSupportedColorSpaces
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetSupportedColorSpaces, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSupportedColorSpaces(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    auto res = screen_->GetSupportedColorSpaces(colorSpaces);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: GetScreenColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, GetScGetScreenColorSpacereenHDRFormat, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetScreenColorSpace(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    auto res = screen_->GetScreenColorSpace(colorSpace);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: SetScreenColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetScreenColorSpace, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), SetScreenColorSpace(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    auto res = screen_->SetScreenColorSpace(colorSpace);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetDensityDpi01
 * @tc.desc: SetDensityDpi
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetDensityDpi, Function | SmallTest | Level2)
{
    auto res = screen_->SetDensityDpi(1000);
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, res);

    res = screen_->SetDensityDpi(100);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetResolution
 * @tc.desc: SetResolution
 * @tc.type: FUNC
 */
HWTEST_F(ScreenTest, SetResolution, Function | SmallTest | Level2)
{
    auto res = screen_->SetResolution(0, 0, 1000);
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, res);

    res = screen_->SetResolution(1, 1, 100);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, res);
}
}
} // namespace Rosen
} // namespace OHOS
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
#include "display.h"
#include "display_info.h"
#include "oh_display_capture.h"
#include "oh_display_manager.h"
#include "oh_display_manager_inner.h"
#include "pixelmap_native_impl.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHDisplayManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static void FoldDisplayModeChangeCallback(NativeDisplayManager_FoldDisplayMode displayMode);
    static void DisplayChangeCallback(uint64_t displayId);
    static void AvailableAreaChangeCallback(uint64_t displayId);
    static void DisplayAddCallback(uint64_t displayId);
    static void DisplayRemoveCallback(uint64_t displayId);
    void SetUp() override;
    void TearDown() override;
};


void OHDisplayManagerTest::SetUpTestCase()
{
}

void OHDisplayManagerTest::TearDownTestCase()
{
}

void OHDisplayManagerTest::SetUp()
{
}

void OHDisplayManagerTest::TearDown()
{
}

void OHDisplayManagerTest::FoldDisplayModeChangeCallback(NativeDisplayManager_FoldDisplayMode displayMode)
{
}

void OHDisplayManagerTest::DisplayChangeCallback(uint64_t displayId)
{
}

void OHDisplayManagerTest::AvailableAreaChangeCallback(uint64_t displayId)
{
}

void OHDisplayManagerTest::DisplayAddCallback(uint64_t displayId)
{
}

void OHDisplayManagerTest::DisplayRemoveCallback(uint64_t displayId)
{
}
namespace {

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayId
 * @tc.desc: displayId == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayId01, TestSize.Level1)
{
    uint64_t *displayId = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayId(displayId);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayId
 * @tc.desc: displayId != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayId02, TestSize.Level1)
{
    uint64_t testParm = 4000;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayId(&testParm);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayWidth
 * @tc.desc: displayWidth == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayWidth01, TestSize.Level1)
{
    int32_t *displayWidth = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayWidth(displayWidth);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayWidth
 * @tc.desc: displayWidth != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayWidth02, TestSize.Level1)
{
    int32_t testWidth = 200;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayWidth(&testWidth);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayHeight
 * @tc.desc: displayHeight == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayHeight01, TestSize.Level1)
{
    int32_t *displayHeight = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayHeight(displayHeight);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayHeight
 * @tc.desc: displayHeight != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayHeight02, TestSize.Level1)
{
    int32_t testHeight = 200;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayHeight(&testHeight);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRotation
 * @tc.desc: displayRotation == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRotation01, TestSize.Level1)
{
    NativeDisplayManager_Rotation *displayRotation = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRotation(displayRotation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRotation
 * @tc.desc: displayRotation != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRotation02, TestSize.Level1)
{
    NativeDisplayManager_Rotation testRotation = NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_180;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRotation(&testRotation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayOrientation
 * @tc.desc: displayOrientation == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayOrientation01, TestSize.Level1)
{
    NativeDisplayManager_Orientation *displayOrientation = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayOrientation(displayOrientation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayOrientation
 * @tc.desc: displayOrientation != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayOrientation02, TestSize.Level1)
{
    NativeDisplayManager_Orientation testOrientation = NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayOrientation(&testOrientation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio
 * @tc.desc: virtualPixel == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio01,
    TestSize.Level1)
{
    float *virtualPixel = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio(virtualPixel);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio
 * @tc.desc: virtualPixel != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio02,
    TestSize.Level1)
{
    float testPixel = 3.14;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio(&testPixel);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRefreshRate
 * @tc.desc: refreshRate == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRefreshRate01, TestSize.Level1)
{
    uint32_t *refreshRate = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRefreshRate(refreshRate);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRefreshRate
 * @tc.desc: refreshRate != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRefreshRate02, TestSize.Level1)
{
    uint32_t testRate = 60;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRefreshRate(&testRate);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityDpi
 * @tc.desc: densityDpi == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityDpi01, TestSize.Level1)
{
    int32_t *densityDpi = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(densityDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityDpi
 * @tc.desc: densityDpi != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityDpi02, TestSize.Level1)
{
    int32_t testDpi = 160;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(&testDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityPixels
 * @tc.desc: densityPixels == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityPixels01, TestSize.Level1)
{
    float *densityPixels = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityPixels(densityPixels);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityPixels
 * @tc.desc: densityPixels != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityPixels02, TestSize.Level1)
{
    float testPixels = 1.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityPixels(&testPixels);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayScaledDensity
 * @tc.desc: scaledDensity == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayScaledDensity01, TestSize.Level1)
{
    float *scaledDensity = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayScaledDensity(scaledDensity);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayScaledDensity
 * @tc.desc: scaledDensity != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayScaledDensity02, TestSize.Level1)
{
    float testDensity = 1.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayScaledDensity(&testDensity);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi
 * @tc.desc: xDpi == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi01, TestSize.Level1)
{
    float *xDpi = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(xDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi
 * @tc.desc: xDpi != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi02, TestSize.Level1)
{
    float testXDpi = 2.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(&testXDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi
 * @tc.desc: yDpi == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi01, TestSize.Level1)
{
    float *yDpi = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(yDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi
 * @tc.desc: yDpi != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi02, TestSize.Level1)
{
    float testYDpi = 2.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(&testYDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetFoldDisplayMode
 * @tc.desc: foldDisplayMode == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetFoldDisplayMode01, TestSize.Level1)
{
    NativeDisplayManager_FoldDisplayMode *foldDisplayMode = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetFoldDisplayMode(foldDisplayMode);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetFoldDisplayMode
 * @tc.desc: foldDisplayMode != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetFoldDisplayMode02, TestSize.Level1)
{
    NativeDisplayManager_FoldDisplayMode testDisplayMode =
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetFoldDisplayMode(&testDisplayMode);
    if (OH_NativeDisplayManager_IsFoldable()) {
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    } else {
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED);
    }
}

/**
 * @tc.name: FoldDisplayModeChangeListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, FoldDisplayModeChangeListener, TestSize.Level1)
{
    uint32_t testIndex;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterFoldDisplayModeChangeListener(
        FoldDisplayModeChangeCallback, &testIndex);
    if (OH_NativeDisplayManager_IsFoldable()) {
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
        NativeDisplayManager_ErrorCode ret1 = OH_NativeDisplayManager_UnregisterFoldDisplayModeChangeListener(
            testIndex);
        EXPECT_EQ(ret1, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    } else {
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED);
    }
}

/**
 * @tc.name: DisplayChangeListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, DisplayChangeListener, TestSize.Level1)
{
    uint32_t testIndex;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterDisplayChangeListener(
        DisplayChangeCallback, &testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    ret = OH_NativeDisplayManager_UnregisterDisplayChangeListener(testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: AllDisplays
 * @tc.desc: create and destroy
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, CreateAndDestroyAllDisplays, TestSize.Level1)
{
    NativeDisplayManager_DisplaysInfo *displayInfo = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CreateAllDisplays(&displayInfo);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    ASSERT_NE(displayInfo, nullptr);
    
    OH_NativeDisplayManager_DestroyAllDisplays(displayInfo);
}

/**
 * @tc.name: CreateAndDestroyDisplayById
 * @tc.desc: CreateAndDestroyDisplayById
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, CreateAndDestroyDisplayById, TestSize.Level1)
{
    uint32_t testDisplayId = 0;
    NativeDisplayManager_DisplayInfo *testPtr = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CreateDisplayById(testDisplayId, &testPtr);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    
    OH_NativeDisplayManager_DestroyDisplay(testPtr);
}

/**
 * @tc.name: CreatePrimaryDisplay
 * @tc.desc: CreatePrimaryDisplay
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, CreatePrimaryDisplay, TestSize.Level1)
{
    NativeDisplayManager_DisplayInfo *testPtr = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CreatePrimaryDisplay(&testPtr);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: CaptureScreenPixelmap
 * @tc.desc: CaptureScreenPixelmap
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, CaptureScreenPixelmap, TestSize.Level1)
{
    uint32_t testDisplayId = 1001;
    OH_PixelmapNative *pixelMap = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CaptureScreenPixelmap(testDisplayId, &pixelMap);
    EXPECT_NE(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_NO_PERMISSION);
}

/**
 * @tc.name: AvailableAreaChangeListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, AvailableAreaChangeListener, TestSize.Level1)
{
    uint32_t testIndex;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterAvailableAreaChangeListener(
        AvailableAreaChangeCallback, &testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    ret = OH_NativeDisplayManager_UnregisterAvailableAreaChangeListener(testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: AvailableAreaChangeListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, AvailableAreaChangeListener02, TestSize.Level1)
{
    uint32_t* testIndex = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterAvailableAreaChangeListener(
        AvailableAreaChangeCallback, testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: DisplayAddListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, DisplayAddListener, TestSize.Level1)
{
    uint32_t testIndex;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterDisplayAddListener(
        DisplayAddCallback, &testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    ret = OH_NativeDisplayManager_UnregisterDisplayAddListener(testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: DisplayAddListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, DisplayAddListener02, TestSize.Level1)
{
    uint32_t* testIndex = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterDisplayAddListener(
        DisplayAddCallback, testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: DisplayRemoveListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, DisplayRemoveListener, TestSize.Level1)
{
    uint32_t testIndex;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterDisplayRemoveListener(
        DisplayRemoveCallback, &testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    ret = OH_NativeDisplayManager_UnregisterDisplayRemoveListener(testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: DisplayRemoveListener
 * @tc.desc: register and unregister
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, DisplayRemoveListener02, TestSize.Level1)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        return;
    }
    uint32_t* testIndex = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_RegisterDisplayRemoveListener(
        DisplayRemoveCallback, testIndex);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_CreateAvailableArea

 * @tc.desc: availableArea
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_CreateAvailableArea01, TestSize.Level1)
{
    uint64_t testId = 0;
    NativeDisplayManager_Rect *availableArea;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CreateAvailableArea(testId, &availableArea);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    if (ret == NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
        ret = OH_NativeDisplayManager_DestroyAvailableArea(availableArea);
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    }
}

/**
 * @tc.name: OH_NativeDisplayManager_CreateAvailableArea02

 * @tc.desc: availableArea
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_CreateAvailableArea02, TestSize.Level1)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        return;
    }
    uint64_t testId = 0;
    NativeDisplayManager_Rect *availableArea = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CreateAvailableArea(testId, &availableArea);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    if (ret == NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
        ret = OH_NativeDisplayManager_DestroyAvailableArea(availableArea);
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
    }
}

/**
 * @tc.name: OH_NativeDisplayManager_CreateAvailableArea03

 * @tc.desc: availableArea
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_CreateAvailableArea03, TestSize.Level1)
{
    uint64_t testId = 0;
    NativeDisplayManager_Rect **availableArea = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CreateAvailableArea(testId, availableArea);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_CreateAvailableArea04

 * @tc.desc: availableArea
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_CreateAvailableArea04, TestSize.Level1)
{
    uint64_t testId = -1;
    NativeDisplayManager_Rect *availableArea = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_CreateAvailableArea(testId, &availableArea);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_DestroyAvailableArea01
 * @tc.desc: availableArea = nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_DestroyAvailableArea01, TestSize.Level1)
{
    NativeDisplayManager_Rect *availableArea = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_DestroyAvailableArea(availableArea);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplaySourceMode
 * @tc.desc: sourcemode
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplaySourceMode01, TestSize.Level1)
{
    uint64_t testIndex = 0;
    NativeDisplayManager_SourceMode sourceMode = DISPLAY_SOURCE_MODE_NONE;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplaySourceMode(testIndex, &sourceMode);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplaySourceMode
 * @tc.desc: sourcemode == null
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplaySourceMode02, TestSize.Level1)
{
    uint64_t testId = 0;
    NativeDisplayManager_SourceMode *sourceMode = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplaySourceMode(testId, sourceMode);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplaySourceMode_03
 * @tc.desc: sourcemode == null
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplaySourceMode03, TestSize.Level1)
{
    uint64_t testId = -1;
    NativeDisplayManager_SourceMode sourceMode = DISPLAY_SOURCE_MODE_NONE;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplaySourceMode(testId, &sourceMode);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplayPosition
 * @tc.desc: x,y != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplayPosition01, TestSize.Level1)
{
    uint64_t testId = 0;
    int32_t x = -1;
    int32_t y = -1;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplayPosition(testId, &x, &y);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    EXPECT_NE(x, -1);
    EXPECT_NE(y, -1);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplayPosition
 * @tc.desc: x = nullptr y!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplayPosition02, TestSize.Level1)
{
    uint64_t testId = 0;
    int32_t *x = nullptr;
    int32_t y;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplayPosition(testId, x, &y);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplayPosition
 * @tc.desc: x != nullptr y=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplayPosition03, TestSize.Level1)
{
    uint64_t testId = 0;
    int32_t x;
    int32_t *y = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplayPosition(testId, &x, y);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplayPosition
 * @tc.desc: x = nullptr y=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplayPosition04, TestSize.Level1)
{
    uint64_t testId = 0;
    int32_t *x = nullptr;
    int32_t *y = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplayPosition(testId, x, y);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplayPosition_EXTEND
 * @tc.desc: x,y
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplayPosition05, TestSize.Level1)
{
    uint64_t testId = 11; // 扩展屏
    int32_t x = -1;
    int32_t y = -1;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplayPosition(testId, &x, &y);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_SYSTEM_ABNORMAL);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplayPosition_ERROR
 * @tc.desc: x,y
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplayPosition06, TestSize.Level1)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        return;
    }
    uint64_t testId = 2; // 异常屏幕
    int32_t x = -1;
    int32_t y = -1;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplayPosition(testId, &x, &y);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDisplayPosition_07
 * @tc.desc: x,y
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDisplayPosition07, TestSize.Level1)
{
    uint64_t testId = -1; // 异常屏幕
    int32_t x = -1;
    int32_t y = -1;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDisplayPosition(testId, &x, &y);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_ILLEGAL_PARAM);
}

}
} // namespace Rosen
} // namespace OHOS
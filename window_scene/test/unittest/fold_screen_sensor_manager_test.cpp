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

#include "fold_screen_sensor_manager.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FoldScreenSensorManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<FoldScreenSensorManager> foldScreenSensorManager_ = nullptr;
};

void FoldScreenSensorManagerTest::SetUpTestCase()
{
}

void FoldScreenSensorManagerTest::TearDownTestCase()
{
}

void FoldScreenSensorManagerTest::SetUp()
{
    foldScreenSensorManager_ = new (std::nothrow) FoldScreenSensorManager();
    EXPECT_NE(nullptr, foldScreenSensorManager_);
}

void FoldScreenSensorManagerTest::TearDown()
{
    foldScreenSensorManager_ = nullptr;
}

namespace {
/**
 * @tc.name: RegisterSensorCallback
 * @tc.desc: RegisterSensorCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, RegisterSensorCallback, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    foldScreenSensorManager_->user = {"test", nullptr, nullptr};

    foldScreenSensorManager_->RegisterSensorCallback();

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: UnRegisterSensorCallback
 * @tc.desc: UnRegisterSensorCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, UnRegisterSensorCallback, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    
    foldScreenSensorManager_->user = {"test", nullptr, nullptr};

    foldScreenSensorManager_->UnRegisterSensorCallback();

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: HandlePostureData01
 * @tc.desc: event is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, HandlePostureData01, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    foldScreenSensorManager_->HandlePostureData(nullptr);

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: HandlePostureData02
 * @tc.desc: event->data is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, HandlePostureData02, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    SensorEvent * const event = new SensorEvent();
    event->data = nullptr;

    foldScreenSensorManager_->HandlePostureData(event);

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: HandlePostureData03
 * @tc.desc: event->dataLen less than sizeof(PostureData)
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, HandlePostureData03, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    SensorEvent * const event = new SensorEvent();
    event->data = new uint8_t();
    event->dataLen = 1;

    foldScreenSensorManager_->HandlePostureData(event);

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: HandleSensorData01
 * @tc.desc: branch 1
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, HandleSensorData01, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    foldScreenSensorManager_->mState_ = FoldStatus::UNKNOWN;
    foldScreenSensorManager_->HandleSensorData(0.1f, 1);

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: HandleSensorData02
 * @tc.desc: branch 2
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, HandleSensorData02, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    foldScreenSensorManager_->mState_ = FoldStatus::UNKNOWN;
    foldScreenSensorManager_->foldScreenPolicy_ = new FoldScreenPolicy();
    foldScreenSensorManager_->HandleSensorData(140.0f, 1);

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: TransferAngleToScreenState01
 * @tc.desc: branch 1
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, TransferAngleToScreenState01, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    foldScreenSensorManager_->mState_ = FoldStatus::UNKNOWN;
    ASSERT_EQ(FoldStatus::UNKNOWN, foldScreenSensorManager_->TransferAngleToScreenState(-0.1f, 1));
}

/**
 * @tc.name: TransferAngleToScreenState02
 * @tc.desc: branch 2
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, TransferAngleToScreenState02, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    ASSERT_EQ(FoldStatus::EXPAND, foldScreenSensorManager_->TransferAngleToScreenState(140.0f, 1));
}

/**
 * @tc.name: TransferAngleToScreenState03
 * @tc.desc: branch 3
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, TransferAngleToScreenState03, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    ASSERT_EQ(FoldStatus::FOLDED, foldScreenSensorManager_->TransferAngleToScreenState(25.0f, 1));
    ASSERT_EQ(FoldStatus::HALF_FOLD, foldScreenSensorManager_->TransferAngleToScreenState(130.0f, 1));

    foldScreenSensorManager_->mState_ = FoldStatus::UNKNOWN;
    ASSERT_EQ(FoldStatus::HALF_FOLD, foldScreenSensorManager_->TransferAngleToScreenState(120.0f, 1));
}

/**
 * @tc.name: TransferAngleToScreenState04
 * @tc.desc: branch 4
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenSensorManagerTest, TransferAngleToScreenState04, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, foldScreenSensorManager_);

    ASSERT_EQ(FoldStatus::FOLDED, foldScreenSensorManager_->TransferAngleToScreenState(90.0f, 2));
    ASSERT_EQ(FoldStatus::HALF_FOLD, foldScreenSensorManager_->TransferAngleToScreenState(130.0f, 2));

    foldScreenSensorManager_->mState_ = FoldStatus::UNKNOWN;
    ASSERT_EQ(FoldStatus::HALF_FOLD, foldScreenSensorManager_->TransferAngleToScreenState(139.0f, 2));
}
}
}
}
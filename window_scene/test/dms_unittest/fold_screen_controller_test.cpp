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

#include "session_manager/include/fold_screen_controller/fold_screen_controller.h"
#include "session_manager/include/screen_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class FoldScreenControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static ScreenSessionManager& ssm_;
};

ScreenSessionManager& FoldScreenControllerTest::ssm_ = ScreenSessionManager::GetInstance();

void FoldScreenControllerTest::SetUpTestCase()
{
}

void FoldScreenControllerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void FoldScreenControllerTest::SetUp()
{
}

void FoldScreenControllerTest::TearDown()
{
}

namespace {

    /**
     * @tc.name: GetFoldScreenPolicy
     * @tc.desc: test function :GetFoldScreenPolicy
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetFoldScreenPolicy, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        DisplayDeviceType productType = DisplayDeviceType::SINGLE_DISPLAY_DEVICE;
        auto ret = fsc_.GetFoldScreenPolicy(productType);
        ASSERT_NE(ret, nullptr);

        productType = DisplayDeviceType::DOUBLE_DISPLAY_DEVICE;
        ret = fsc_.GetFoldScreenPolicy(productType);
        ASSERT_NE(ret, nullptr);

        productType = DisplayDeviceType::SINGLE_DISPLAY_POCKET_DEVICE;
        ret = fsc_.GetFoldScreenPolicy(productType);
        ASSERT_NE(ret, nullptr);

        productType = DisplayDeviceType::DISPLAY_DEVICE_UNKNOWN;
        ret = fsc_.GetFoldScreenPolicy(productType);
        ASSERT_EQ(ret, nullptr);
    }

    /**
     * @tc.name: SetDisplayMode01
     * @tc.desc: test function :SetDisplayMode
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, SetDisplayMode01, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = nullptr;
        FoldDisplayMode displayMode = FoldDisplayMode::FULL;
        ASSERT_EQ(fsc_.foldScreenPolicy_, nullptr);
        fsc_.SetDisplayMode(displayMode);
    }

    /**
     * @tc.name: SetDisplayMode02
     * @tc.desc: test function :SetDisplayMode
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, SetDisplayMode02, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = new FoldScreenPolicy();
        FoldDisplayMode displayMode = FoldDisplayMode::FULL;
        ASSERT_NE(fsc_.foldScreenPolicy_, nullptr);
        fsc_.SetDisplayMode(displayMode);
    }

    /**
     * @tc.name: LockDisplayStatus01
     * @tc.desc: test function :LockDisplayStatus
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, LockDisplayStatus01, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = nullptr;
        bool locked = false;
        fsc_.LockDisplayStatus(locked);
        ASSERT_EQ(fsc_.foldScreenPolicy_, nullptr);
    }

    /**
     * @tc.name: LockDisplayStatus02
     * @tc.desc: test function :LockDisplayStatus
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, LockDisplayStatus02, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = new FoldScreenPolicy();
        bool locked = false;
        fsc_.LockDisplayStatus(locked);
        ASSERT_NE(fsc_.foldScreenPolicy_, nullptr);
    }

    /**
     * @tc.name: GetDisplayMode
     * @tc.desc: test function :GetDisplayMode
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetDisplayMode, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = nullptr;
        auto ret = fsc_.GetDisplayMode();
        ASSERT_EQ(ret, FoldDisplayMode::UNKNOWN);
    }

    /**
     * @tc.name: IsFoldable
     * @tc.desc: test function :IsFoldable
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, IsFoldable, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        auto ret = fsc_.IsFoldable();
        ASSERT_EQ(ret, true);
    }

    /**
     * @tc.name: GetFoldStatus
     * @tc.desc: test function :GetFoldStatus
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetFoldStatus, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = nullptr;
        auto ret = fsc_.GetFoldStatus();
        ASSERT_EQ(ret, FoldStatus::UNKNOWN);
    }

    /**
     * @tc.name: SetFoldStatus01
     * @tc.desc: test function :SetFoldStatus
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, SetFoldStatus01, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        FoldStatus foldStatus = FoldStatus::HALF_FOLD;
        fsc_.foldScreenPolicy_ = nullptr;
        fsc_.SetFoldStatus(foldStatus);
        ASSERT_EQ(fsc_.foldScreenPolicy_, nullptr);
    }

    /**
     * @tc.name: SetFoldStatus02
     * @tc.desc: test function :SetFoldStatus
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, SetFoldStatus02, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        FoldStatus foldStatus = FoldStatus::HALF_FOLD;
        fsc_.foldScreenPolicy_ = new FoldScreenPolicy();
        fsc_.SetFoldStatus(foldStatus);
        ASSERT_NE(fsc_.foldScreenPolicy_, nullptr);
    }

    /**
     * @tc.name: GetCurrentFoldCreaseRegion
     * @tc.desc: test function :GetCurrentFoldCreaseRegion
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetCurrentFoldCreaseRegion, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = nullptr;
        auto ret = fsc_.GetCurrentFoldCreaseRegion();
        ASSERT_EQ(ret, nullptr);
    }

    /**
     * @tc.name: GetCurrentScreenId01
     * @tc.desc: test function :GetCurrentScreenId
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetCurrentScreenId01, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = nullptr;
        auto ret = fsc_.GetCurrentScreenId();
        ASSERT_EQ(ret, 0);
    }

    /**
     * @tc.name: GetCurrentScreenId02
     * @tc.desc: test function :GetCurrentScreenId
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetCurrentScreenId02, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = new FoldScreenPolicy();
        auto ret = fsc_.GetCurrentScreenId();
        ASSERT_NE(ret, 0);
    }

    /**
     * @tc.name: SetOnBootAnimation01
     * @tc.desc: test function :SetOnBootAnimation
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, SetOnBootAnimation01, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        bool onBootAnimation = false;
        fsc_.foldScreenPolicy_ = nullptr;
        fsc_.SetOnBootAnimation(onBootAnimation);
        ASSERT_EQ(onBootAnimation, false);
    }

    /**
     * @tc.name: SetOnBootAnimation02
     * @tc.desc: test function :SetOnBootAnimation
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, SetOnBootAnimation02, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        bool onBootAnimation = false;
        fsc_.foldScreenPolicy_ = new FoldScreenPolicy();
        fsc_.SetOnBootAnimation(onBootAnimation);
        ASSERT_NE(fsc_.foldScreenPolicy_, nullptr);
    }

    /**
     * @tc.name: UpdateForPhyScreenPropertyChange01
     * @tc.desc: test function :UpdateForPhyScreenPropertyChange
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, UpdateForPhyScreenPropertyChange01, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = nullptr;
        fsc_.UpdateForPhyScreenPropertyChange();
        ASSERT_EQ(fsc_.foldScreenPolicy_, nullptr);
    }

    /**
     * @tc.name: UpdateForPhyScreenPropertyChange02
     * @tc.desc: test function :UpdateForPhyScreenPropertyChange
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, UpdateForPhyScreenPropertyChange02, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        FoldScreenController fsc_(mutex, std::shared_ptr<TaskScheduler>());

        fsc_.foldScreenPolicy_ = new FoldScreenPolicy();
        fsc_.UpdateForPhyScreenPropertyChange();
        ASSERT_NE(fsc_.foldScreenPolicy_, nullptr);
    }
}
} // namespace Rosen
} // namespace OHOS


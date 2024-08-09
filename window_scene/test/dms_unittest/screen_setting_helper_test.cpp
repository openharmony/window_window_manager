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

#include "session_manager/include/screen_setting_helper.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_US = 100000;
}

class ScreenSettingHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSettingHelperTest::SetUpTestCase()
{
}

void ScreenSettingHelperTest::TearDownTestCase()
{
}

void ScreenSettingHelperTest::SetUp()
{
}

void ScreenSettingHelperTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

    /**
     * @tc.name: RegisterSettingDpiObserver
     * @tc.desc: RegisterSettingDpiObserver
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, RegisterSettingDpiObserver, Function | SmallTest | Level3)
    {
        auto func = [] (const std::string&) {
            TLOGI(WmsLogTag::DMS, "UT test");
        };
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        screenSettingHelper.RegisterSettingDpiObserver(func);
        screenSettingHelper.dpiObserver_ = nullptr;
        ASSERT_EQ(screenSettingHelper.dpiObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingDpiObserver01
     * @tc.desc: UnregisterSettingDpiObserver
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingDpiObserver01, Function | SmallTest | Level3)
    {
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        screenSettingHelper.UnregisterSettingDpiObserver();
        ASSERT_EQ(screenSettingHelper.dpiObserver_, nullptr);
    }

     /**
     * @tc.name: UnregisterSettingDpiObserver02
     * @tc.desc: UnregisterSettingDpiObserver
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingDpiObserver02, Function | SmallTest | Level3)
    {
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        screenSettingHelper.dpiObserver_ = nullptr;
        screenSettingHelper.UnregisterSettingDpiObserver();
        ASSERT_EQ(screenSettingHelper.dpiObserver_, nullptr);
    }

    /**
     * @tc.name: GetSettingDpi
     * @tc.desc: GetSettingDpi
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingDpi, Function | SmallTest | Level3)
    {
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        uint32_t dpi = 0;
        std::string key = "test";
        bool ret = screenSettingHelper.GetSettingDpi(dpi, key);
        ASSERT_FALSE(ret);
    }

    /**
     * @tc.name: RegisterSettingCastObserver01
     * @tc.desc: RegisterSettingCastObserver
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, RegisterSettingCastObserver01, Function | SmallTest | Level3)
    {
        auto func = [] (const std::string&) {
            TLOGI(WmsLogTag::DMS, "UT test");
        };
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        screenSettingHelper.RegisterSettingCastObserver(func);
        screenSettingHelper.castObserver_ = nullptr;
        ASSERT_EQ(screenSettingHelper.castObserver_, nullptr);
    }

    /**
     * @tc.name: RegisterSettingCastObserver02
     * @tc.desc: RegisterSettingCastObserver
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, RegisterSettingCastObserver02, Function | SmallTest | Level3)
    {
        auto func = [] (const std::string&) {
            TLOGI(WmsLogTag::DMS, "UT test");
        };
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        screenSettingHelper.castObserver_ = nullptr;
        screenSettingHelper.RegisterSettingCastObserver(func);
        ASSERT_EQ(screenSettingHelper.castObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingCastObserver
     * @tc.desc: UnregisterSettingCastObserver
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingCastObserver, Function | SmallTest | Level3)
    {
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        screenSettingHelper.UnregisterSettingCastObserver();
        ASSERT_EQ(screenSettingHelper.castObserver_, nullptr);
    }


    /**
     * @tc.name: GetSettingCast
     * @tc.desc: GetSettingCast
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingCast, Function | SmallTest | Level3)
    {
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        bool enable = true;
        std::string key = "test";
        bool ret = screenSettingHelper.GetSettingCast(enable, key);
        ASSERT_FALSE(ret);
    }

    /**
     * @tc.name: UnregisterSettingRotationObserver
     * @tc.desc: UnregisterSettingRotationObserver
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingRotationObserver, Function | SmallTest | Level3)
    {
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        screenSettingHelper.UnregisterSettingRotationObserver();
        ASSERT_EQ(screenSettingHelper.rotationObserver_, nullptr);
    }

    /**
     * @tc.name: SetSettingRotation
     * @tc.desc: SetSettingRotation
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, SetSettingRotation, Function | SmallTest | Level3)
    {
        int32_t rotation = -1;
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        ASSERT_EQ(rotation, -1);
        screenSettingHelper.SetSettingRotation(rotation);
    }

    /**
     * @tc.name: GetSettingRotation
     * @tc.desc: GetSettingRotation
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingRotation, Function | SmallTest | Level3)
    {
        int32_t screenId = 1;
        std::string key = "test";
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        auto result = screenSettingHelper.GetSettingRotation(screenId, key);
        ASSERT_EQ(result, false);
    }

    /**
     * @tc.name: SetSettingRotationScreenId
     * @tc.desc: SetSettingRotationScreenId
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, SetSettingRotationScreenId, Function | SmallTest | Level3)
    {
        int32_t screenId = 1;
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        ASSERT_EQ(screenId, 1);
        screenSettingHelper.SetSettingRotationScreenId(screenId);
    }

    /**
     * @tc.name: GetSettingRotationScreenID
     * @tc.desc: GetSettingRotationScreenID
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingRotationScreenID, Function | SmallTest | Level3)
    {
        int32_t screenId = 1;
        std::string key = "test";
        ScreenSettingHelper screenSettingHelper = ScreenSettingHelper();
        auto result = screenSettingHelper.GetSettingRotationScreenID(screenId, key);
        ASSERT_EQ(result, false);
    }
}
} // namespace Rosen
} // namespace OHOS
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

#include "screen_session_manager/include/screen_setting_helper.h"
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
     * @tc.name: SetSettingDefaultDpi
     * @tc.desc: SetSettingDefaultDpi
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, SetSettingDefaultDpi, Function | SmallTest | Level3)
    {
        uint32_t dpi = 520;
        std::string key = "default_display_dpi";
        auto ret = ScreenSettingHelper::SetSettingDefaultDpi(dpi, key);
        ASSERT_TRUE(ret);
    }

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
        ScreenSettingHelper::RegisterSettingDpiObserver(func);
        ScreenSettingHelper::dpiObserver_ = nullptr;
        ASSERT_EQ(ScreenSettingHelper::dpiObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingDpiObserver01
     * @tc.desc: UnregisterSettingDpiObserver01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingDpiObserver01, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::dpiObserver_ = new SettingObserver;
        ScreenSettingHelper::UnregisterSettingDpiObserver();
        ASSERT_EQ(ScreenSettingHelper::dpiObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingDpiObserver02
     * @tc.desc: UnregisterSettingDpiObserver02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingDpiObserver02, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::dpiObserver_ = nullptr;
        ScreenSettingHelper::UnregisterSettingDpiObserver();
        ASSERT_EQ(ScreenSettingHelper::dpiObserver_, nullptr);
    }

    /**
     * @tc.name: GetSettingDpi01
     * @tc.desc: GetSettingDpi01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingDpi01, Function | SmallTest | Level3)
    {
        uint32_t dpi = 0;
        std::string key = "test";
        bool ret = ScreenSettingHelper::GetSettingDpi(dpi, key);
        ASSERT_FALSE(ret);
    }

    /**
     * @tc.name: GetSettingDpi02
     * @tc.desc: GetSettingDpi02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingDpi02, Function | SmallTest | Level3)
    {
        uint32_t dpi = 0;
        std::string key = "user_set_dpi_value";
        bool ret = ScreenSettingHelper::GetSettingDpi(dpi, key);
        ASSERT_TRUE(ret);
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
        ScreenSettingHelper::castObserver_ = new SettingObserver;
        ScreenSettingHelper::RegisterSettingCastObserver(func);
        ASSERT_NE(ScreenSettingHelper::castObserver_, nullptr);
    }

    /**
     * @tc.name: RegisterSettingCastObserver02
     * @tc.desc: RegisterSettingCastObserver02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, RegisterSettingCastObserver02, Function | SmallTest | Level3)
    {
        auto func = [] (const std::string&) {
            TLOGI(WmsLogTag::DMS, "UT test");
        };
        ScreenSettingHelper::castObserver_ = nullptr;
        ScreenSettingHelper::RegisterSettingCastObserver(func);
        ASSERT_NE(ScreenSettingHelper::castObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingCastObserver01
     * @tc.desc: UnregisterSettingCastObserver01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingCastObserver01, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::castObserver_ = nullptr;
        ScreenSettingHelper::UnregisterSettingCastObserver();
        ASSERT_EQ(ScreenSettingHelper::castObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingCastObserver02
     * @tc.desc: UnregisterSettingCastObserver02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingCastObserver02, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::castObserver_ = new SettingObserver;
        ScreenSettingHelper::UnregisterSettingCastObserver();
        ASSERT_EQ(ScreenSettingHelper::castObserver_, nullptr);
    }

    /**
     * @tc.name: GetSettingCast01
     * @tc.desc: GetSettingCast01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingCast01, Function | SmallTest | Level3)
    {
        bool enable = true;
        std::string key = "test";
        bool ret = ScreenSettingHelper::GetSettingCast(enable, key);
        ASSERT_FALSE(ret);
    }

    /**
     * @tc.name: GetSettingCast02
     * @tc.desc: GetSettingCast02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingCast02, Function | SmallTest | Level3)
    {
        bool enable = true;
        std::string key = "default_display_dpi";
        bool ret = ScreenSettingHelper::GetSettingCast(enable, key);
        ASSERT_TRUE(ret);
    }

    /**
     * @tc.name: UnregisterExtendSettingDpiObserver01
     * @tc.desc: UnregisterExtendSettingDpiObserver01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterExtendSettingDpiObserver01, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::extendDpiObserver_ = nullptr;
        ScreenSettingHelper::UnregisterExtendSettingDpiObserver();
        ASSERT_EQ(ScreenSettingHelper::extendDpiObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterExtendSettingDpiObserver02
     * @tc.desc: UnregisterExtendSettingDpiObserver02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterExtendSettingDpiObserver02, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::extendDpiObserver_ = new SettingObserver;
        ScreenSettingHelper::UnregisterExtendSettingDpiObserver();
        ASSERT_EQ(ScreenSettingHelper::extendDpiObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingRotationObserver01
     * @tc.desc: UnregisterSettingRotationObserver01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingRotationObserver01, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::rotationObserver_ = nullptr;
        ScreenSettingHelper::UnregisterExtendSettingDpiObserver();
        ASSERT_EQ(ScreenSettingHelper::rotationObserver_, nullptr);
    }

    /**
     * @tc.name: UnregisterSettingRotationObserver02
     * @tc.desc: UnregisterSettingRotationObserver02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, UnregisterSettingRotationObserver02, Function | SmallTest | Level3)
    {
        ScreenSettingHelper::rotationObserver_ = new SettingObserver;
        ScreenSettingHelper::UnregisterExtendSettingDpiObserver();
        ASSERT_NE(ScreenSettingHelper::rotationObserver_, nullptr);
    }

    /**
     * @tc.name: SetSettingRotation
     * @tc.desc: SetSettingRotation
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, SetSettingRotation, Function | SmallTest | Level3)
    {
        int32_t rotation = 180;
        ScreenSettingHelper::SetSettingRotation(rotation);
        ASSERT_NE(ScreenSettingHelper::rotationObserver_, nullptr);
    }

    /**
     * @tc.name: GetSettingRotation01
     * @tc.desc: GetSettingRotation01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingRotation01, Function | SmallTest | Level3)
    {
        int32_t rotation = 0;
        std::string key = "test";
        auto result = ScreenSettingHelper::GetSettingRotation(rotation, key);
        ASSERT_EQ(result, false);
    }

    /**
     * @tc.name: GetSettingRotation02
     * @tc.desc: GetSettingRotation02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingRotation02, Function | SmallTest | Level3)
    {
        int32_t rotation = 0;
        std::string key = "default_display_dpi";
        auto result = ScreenSettingHelper::GetSettingRotation(rotation, key);
        ASSERT_EQ(result, true);
    }

    /**
     * @tc.name: SetSettingRotationScreenId
     * @tc.desc: SetSettingRotationScreenId
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, SetSettingRotationScreenId, Function | SmallTest | Level3)
    {
        int32_t screenId = 0;
        ScreenSettingHelper::SetSettingRotationScreenId(screenId);
        ASSERT_NE(ScreenSettingHelper::rotationObserver_, nullptr);
    }

    /**
     * @tc.name: GetSettingRotationScreenID01
     * @tc.desc: GetSettingRotationScreenID01
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingRotationScreenID01, Function | SmallTest | Level3)
    {
        int32_t screenId = 0;
        std::string key = "test";
        auto result = ScreenSettingHelper::GetSettingRotation(screenId, key);
        ASSERT_EQ(result, false);
    }

    /**
     * @tc.name: GetSettingRotationScreenID02
     * @tc.desc: GetSettingRotationScreenID02
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenSettingHelperTest, GetSettingRotationScreenID02, Function | SmallTest | Level3)
    {
        int32_t screenId = 0;
        std::string key = "screen_rotation_screen_id_value";
        auto result = ScreenSettingHelper::GetSettingRotation(screenId, key);
        ASSERT_EQ(result, false);
    }
}
} // namespace Rosen
} // namespace OHOS
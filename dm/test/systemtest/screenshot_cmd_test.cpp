/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

// gtest
#include <cstdlib>
#include <gtest/gtest.h>
#include "display_test_utils.h"
#include "pixel_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "ScreenshotCmdTest"};
}
using utils = DisplayTestUtils;
class ScreenshotCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static DisplayId defaultId_;
    DisplayId invalidId_ = DISPLAY_ID_INVALD;
    const std::string defaultCmd_ = "/system/bin/snapshot_display";
    const std::string defaultImg_ = "/data/snapshot_display_1.png";
};

DisplayId ScreenshotCmdTest::defaultId_ = DISPLAY_ID_INVALD;

void ScreenshotCmdTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        WLOGFI("GetDefaultDisplay: failed!\n");
    } else {
        WLOGFI("GetDefaultDisplay: id %llu, w %d, h %d, fps %u\n", display->GetId(), display->GetWidth(),
            display->GetHeight(), display->GetFreshRate());
    }

    defaultId_ = display->GetId();
}

void ScreenshotCmdTest::TearDownTestCase()
{
}

void ScreenshotCmdTest::SetUp()
{
}

void ScreenshotCmdTest::TearDown()
{
}

bool CheckFileExist(const std::string fPath)
{
    if (!fPath.empty()) {
        FILE* fp = fopen(fPath.c_str(), "r");
        if (fp != nullptr) {
            fclose(fp);
            return true;
        }
    }
    return false;
}

namespace {
/**
 * @tc.name: ScreenShotCmdValid
 * @tc.desc: Call screenshot default cmd and check if it saves image in default path
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotCmdTest, ScreenShotCmdValid01, Function | MediumTest | Level2)
{
    if (CheckFileExist(defaultImg_)) {
        remove(defaultImg_.c_str());
    }
    (void)system(defaultCmd_.c_str());
    bool isExist = CheckFileExist(defaultImg_);
    if (isExist) {
        remove(defaultImg_.c_str());
    }
    ASSERT_EQ(true, isExist);
}

/**
 * @tc.name: ScreenShotCmdValid
 * @tc.desc: Call screenshot with default displayID and default path
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotCmdTest, ScreenShotCmdValid02, Function | MediumTest | Level2)
{
    if (CheckFileExist(defaultImg_)) {
        remove(defaultImg_.c_str());
    }
    const std::string cmd = defaultCmd_ + " -i " + std::to_string(defaultId_);
    (void)system(cmd.c_str());
    bool isExist = CheckFileExist(defaultImg_);
    if (isExist) {
        remove(defaultImg_.c_str());
    }
    ASSERT_EQ(true, isExist);
}

/**
 * @tc.name: ScreenShotCmdValid
 * @tc.desc: Call screenshot with default displayID and custom path
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotCmdTest, ScreenShotCmdValid03, Function | MediumTest | Level2)
{
    const std::string imgPath = "/data/snapshot_display_test.png";
    if (CheckFileExist(imgPath)) {
        remove(imgPath.c_str());
    }
    const std::string cmd = defaultCmd_ + " -i " + std::to_string(defaultId_) + " -f " + imgPath;
    (void)system(cmd.c_str());
    bool isExist = CheckFileExist(imgPath);
    if (isExist) {
        remove(imgPath.c_str());
    }
    ASSERT_EQ(true, isExist);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
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

#ifndef FRAMEWORKS_WM_TEST_UT_AVOID_AREA_CONTROLLER_TEST_H
#define FRAMEWORKS_WM_TEST_UT_AVOID_AREA_CONTROLLER_TEST_H

#include <gtest/gtest.h>
#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class AvoidAreaControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowNode> statusbarWindowNode;
    static sptr<WindowNode> navigationBarWindowNode;
    static sptr<WindowNode> keyboardWindowNode;
    static Rect screenRect;
    static Rect cut_out_rect;
};
} // namespace ROSEN
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_AVOID_AREA_CONTROLLER_TEST_H
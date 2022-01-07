/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_impl_test.h"
#include "mock/mock_window_adapter.h"
#include "mock/singleton_mocker.h"
#include "test_header.h"
#include "window_property.h"


namespace OHOS {
namespace Rosen {
using namespace testing;

void WindowImplTest::SetUpTestCase()
{
    property_ = sptr<WindowProperty>(new WindowProperty());
    property_->SetWindowId(0);
    window_ = sptr<Window>(new WindowImpl(property_));
}

void WindowImplTest::TearDownTestCase()
{
}

void WindowImplTest::SetUp()
{
}

void WindowImplTest::TearDown()
{
}

namespace {
/*
 * Function: Show
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock WindowAdapter
 *                  2. mock AddWindow return WM_OK
 *                  3. call Show to AddWindow and check return is WM_OK
 *                  4. call Show with isAdded_=true and check return is WM_OK
 *                  5. mock RemoveWindow return WM_OK
 *                  6. call Destroy check return is WM_OK
 */
HWTEST_F(WindowImplTest, ShowWindow01, testing::ext::TestSize.Level0)
{
    PART("CaseShow01") {
#ifdef _NEW_RENDERSERVER_
        using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
        std::unique_ptr<Mocker> m = nullptr;
    STEP("1. mock WindowAdapter") {
        m = std::make_unique<Mocker>();
    }

    STEP("2. mock AddWindow return WM_OK") {
        EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    }

    STEP("3. call Show and check return is WM_OK") {
        STEP_ASSERT_EQ(WMError::WM_OK, window_->Show());
    }

    STEP("4. mock RemoveWindow return WM_OK") {
        EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    }

    STEP("5. call Destroy check return is WM_OK") {
        STEP_ASSERT_EQ(WMError::WM_OK, window_->Destroy());
    }
#endif
    }
}

/*
 * Function: Show
 * Type: Reliability
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. mock WindowAdapter
 *                  2. mock AddWindow return WM_ERROR_DEATH_RECIPIENT
 *                  3. call Show to AddWindow and check return is WM_ERROR_DEATH_RECIPIENT
 *                  4. call Show with isAdded_=true and check return is WM_ERROR_DEATH_RECIPIENT
 *                  5. mock RemoveWindow return WM_OK
 *                  6. call Destroy check return is WM_OK
 */
HWTEST_F(WindowImplTest, ShowWindow02, testing::ext::TestSize.Level0)
{
    PART("CaseShow02") {
#ifdef _NEW_RENDERSERVER_
        using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
        std::unique_ptr<Mocker> m = nullptr;
    STEP("1. mock WindowAdapter") {
        m = std::make_unique<Mocker>();
    }

    STEP("2. mock AddWindow return WM_ERROR_DEATH_RECIPIENT") {
        EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_DEATH_RECIPIENT));
    }

    STEP("3. call Show and check return is WM_ERROR_DEATH_RECIPIENT") {
        STEP_ASSERT_EQ(WMError::WM_ERROR_DEATH_RECIPIENT, window_->Show());
    }

    STEP("4. mock RemoveWindow return WM_OK") {
        EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    }

    STEP("5. call Destroy check return is WM_OK") {
        STEP_ASSERT_EQ(WMError::WM_OK, window_->Destroy());
    }
#endif
    }
}
}
} // namespace Rosen
} // namespace OHOS
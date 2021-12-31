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

} // namespace Rosen
} // namespace OHOS

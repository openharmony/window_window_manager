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

#include "setting_observer.h"

using namespace testing;
using namespace testing::ext;

using UpdateFunc = std::function<void(const std::string&)>;

namespace OHOS {
namespace Rosen {
class SettingObserverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SettingObserver> settingObserver_ = nullptr;
};

void SettingObserverTest::SetUpTestCase()
{
}

void SettingObserverTest::TearDownTestCase()
{
}

void SettingObserverTest::SetUp()
{
    settingObserver_ = new SettingObserver();
}

void SettingObserverTest::TearDown()
{
    settingObserver_ = nullptr;
}

namespace {
/**
 * @tc.name: SetUpdateFunc
 * @tc.desc: test function : SetUpdateFunc
 * @tc.type: FUNC
 */
HWTEST_F(SettingObserverTest, SetUpdateFunc, Function | SmallTest | Level1)
{
    UpdateFunc func = [this](const std::string& key) {
        settingObserver_->SetKey(key);
    };
    EXPECT_EQ(nullptr, settingObserver_->update_);
    settingObserver_->SetUpdateFunc(func);
    EXPECT_NE(nullptr, settingObserver_->update_);
}

/**
 * @tc.name: SetKey
 * @tc.desc: test function : SetKey
 * @tc.type: FUNC
 */
HWTEST_F(SettingObserverTest, SetKey, Function | SmallTest | Level1)
{
    const char* s = "";
    EXPECT_EQ(settingObserver_->key_, s);
    settingObserver_->SetKey("SetKey");
    std::string res = settingObserver_->GetKey();
    EXPECT_EQ(res, settingObserver_->key_);
}

/**
 * @tc.name: OnChange
 * @tc.desc: test function : OnChange
 * @tc.type: FUNC
 */
HWTEST_F(SettingObserverTest, OnChange, Function | SmallTest | Level1)
{
    UpdateFunc func = [this](const std::string& key) {
        settingObserver_->SetKey(key);
    };
    settingObserver_->SetUpdateFunc(func);
    EXPECT_NE(nullptr, settingObserver_->update_);
    settingObserver_->SetKey("OnChange");
    settingObserver_->OnChange();
    EXPECT_EQ("OnChange", settingObserver_->key_);
}
}
}
}
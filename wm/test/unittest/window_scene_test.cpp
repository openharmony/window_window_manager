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
#include "window_scene_test.h"
#include "ability_context_impl.h"
#include "mock_static_call.h"
#include "singleton_mocker.h"
#include "window_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<StaticCall, MockStaticCall>;
void WindowSceneTest::SetUpTestCase()
{
    int displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    scene_ = new WindowScene();
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(option)));
    ASSERT_EQ(WMError::WM_OK, scene_->Init(displayId, abilityContext_, listener));
}

void WindowSceneTest::TearDownTestCase()
{
}

void WindowSceneTest::SetUp()
{
}

void WindowSceneTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Init01
 * @tc.desc: Init Scene with null abilityContext, null listener
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Init01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    int displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext, listener));
}

/**
 * @tc.name: Init02
 * @tc.desc: Mock window Create Static Method return nullptr, init Scene with null abilityContext, null listener
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Init02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(nullptr));
    int displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->Init(displayId, abilityContext, listener));
}

/**
 * @tc.name: Init03
 * @tc.desc: Init Scene with abilityContext, null listener
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Init03, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    int displayId = 0;
    sptr<IWindowLifeCycle> listener = nullptr;
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_OK, scene->Init(displayId, abilityContext_, listener));
}

/**
 * @tc.name: Create01
 * @tc.desc: CreateWindow without windowName
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Create01, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = new WindowOption();
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(nullptr, scene->CreateWindow("", optionTest));
}

/**
 * @tc.name: Create02
 * @tc.desc: CreateWindow with windowName and without mainWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Create02, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = new WindowOption();
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(nullptr, scene->CreateWindow("WindowSceneTest02", optionTest));
}

/**
 * @tc.name: Create03
 * @tc.desc: CreateWindow with windowName and mainWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Create03, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> optionTest = new WindowOption();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(new WindowImpl(optionTest)));
    ASSERT_NE(nullptr, scene_->CreateWindow("WindowSceneTest03", optionTest));
}

/**
 * @tc.name: Create04
 * @tc.desc: Mock window Create Static Method return nullptr, createWindow with windowName and mainWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Create04, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = new WindowOption();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _)).Times(1).WillOnce(Return(nullptr));
    ASSERT_EQ(nullptr, scene_->CreateWindow("WindowSceneTest04", optionTest));
}

/**
 * @tc.name: Create05
 * @tc.desc: createWindow with windowName and null option
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, Create05, Function | SmallTest | Level2)
{
    sptr<WindowOption> optionTest = nullptr;
    ASSERT_EQ(nullptr, scene_->CreateWindow("WindowSceneTest05", optionTest));
}

/**
 * @tc.name: GetMainWindow01
 * @tc.desc: GetMainWindow without scene init
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, GetMainWindow01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(nullptr, scene->GetMainWindow());
}

/**
 * @tc.name: GetMainWindow02
 * @tc.desc: GetMainWindow01 with scene init success
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, GetMainWindow02, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, scene_->GetMainWindow());
}

/**
 * @tc.name: GoForeground01
 * @tc.desc: GoForeground01 without mainWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, GoForeground01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->GoForeground());
}

/**
 * @tc.name: GoBackground01
 * @tc.desc: GoBackground01 without mainWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, GoBackground01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->GoBackground());
}

/**
 * @tc.name: RequestFocus01
 * @tc.desc: RequestFocus01 without mainWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSceneTest, RequestFocus01, Function | SmallTest | Level2)
{
    sptr<WindowScene> scene = new WindowScene();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, scene->RequestFocus());
}
}
} // namespace Rosen
} // namespace OHOS
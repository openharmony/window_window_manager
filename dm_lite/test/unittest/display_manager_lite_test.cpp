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
#include "display_manager_lite.h"
#include "mock_display_manager_adapter_lite.h"
#include "singleton_mocker.h"
#include "display_manager_lite.cpp"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapterLite, MockDisplayManagerAdapterLite>;
class DmMockDisplayListener : public DisplayManagerLite::IDisplayListener {
public:
    void OnCreate(DisplayId) override {}
    void OnDestroy(DisplayId) override {}
    void OnChange(DisplayId) override {}
};
class DmMockFoldStatusListener : public DisplayManagerLite::IFoldStatusListener {
public:
    virtual void OnFoldStatusChanged([[maybe_unused]]FoldStatus foldStatus) {}
};
class DisplayManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerTest::SetUpTestCase()
{
}

void DisplayManagerTest::TearDownTestCase()
{
}

void DisplayManagerTest::SetUp()
{
}

void DisplayManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnDisplayCreate01
 * @tc.desc: OnDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, OnDisplayCreate01, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManagerLite::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_NE(displayManagerListener, nullptr);
    displayManagerListener->OnDisplayCreate(nullptr);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(DISPLAY_ID_INVALID);
    displayManagerListener->OnDisplayCreate(displayInfo);
    displayInfo->SetDisplayId(0);
    displayManagerListener->OnDisplayCreate(displayInfo);
    ASSERT_NE(displayManagerListener->pImpl_, nullptr);
    displayManagerListener->pImpl_ = nullptr;
    displayManagerListener->OnDisplayCreate(displayInfo);
    DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_ = nullptr;
}

/**
 * @tc.name: OnDisplayDestroy
 * @tc.desc: OnDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, OnDisplayDestroy, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManagerLite::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_NE(displayManagerListener, nullptr);
    displayManagerListener->OnDisplayDestroy(DISPLAY_ID_INVALID);
    displayManagerListener->OnDisplayDestroy(0);
    displayManagerListener->pImpl_ = nullptr;
    displayManagerListener->OnDisplayDestroy(1);
    DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_ = nullptr;
}

/**
 * @tc.name: OnDisplayChange
 * @tc.desc: OnDisplayChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, OnDisplayChange, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManagerLite::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_NE(displayManagerListener, nullptr);
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_SIZE_CHANGED;
    displayManagerListener->OnDisplayChange(nullptr, event);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(DISPLAY_ID_INVALID);
    displayManagerListener->OnDisplayChange(displayInfo, event);
    displayInfo->SetDisplayId(0);
    displayManagerListener->OnDisplayChange(displayInfo, event);
    ASSERT_NE(displayManagerListener->pImpl_, nullptr);
    displayManagerListener->pImpl_ = nullptr;
    displayManagerListener->OnDisplayChange(displayInfo, event);
    DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_ = nullptr;
}

/**
 * @tc.name: ImplUpdateDisplayInfoLocked
 * @tc.desc: ImplUpdateDisplayInfoLocked fun
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, ImplUpdateDisplayInfoLocked, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(DISPLAY_ID_INVALID);
    auto ret = DisplayManagerLite::GetInstance().pImpl_->UpdateDisplayInfoLocked(displayInfo);
    ASSERT_EQ(ret, false);
    displayInfo.clear();
}

/**
 * @tc.name: ImplUpdateDisplayInfoLocked
 * @tc.desc: ImplUpdateDisplayInfoLocked fun
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, ImplUpdateDisplayInfoLocked01, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    auto ret = DisplayManagerLite::GetInstance().pImpl_->UpdateDisplayInfoLocked(displayInfo);
    ASSERT_EQ(ret, false);
    displayInfo.clear();
}

/**
 * @tc.name: RegisterDisplayListener
 * @tc.desc: displayManagerListener_ == nullptr, ret == DMError::DM_OK
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterDisplayListener01, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_ = nullptr;
    DisplayManagerLite::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_NE(displayManagerListener, nullptr);
}

/**
 * @tc.name: RegisterDisplayListener
 * @tc.desc: ret ! = DMError::DM_OK
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterDisplayListener02, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_ERROR_IPC_FAILED));
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_ = nullptr;
    DisplayManagerLite::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManagerLite::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_EQ(displayManagerListener, nullptr);
}

/**
 * @tc.name: RegisterDisplayListener
 * @tc.desc: listener == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterDisplayListener03, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IDisplayListener> listener = nullptr;
    auto ret = DisplayManagerLite::GetInstance().RegisterDisplayListener(listener);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: RegisterDisplayListener
 * @tc.desc: displayManagerListener_ != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterDisplayListener04, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    sptr<DisplayManagerLite::IDisplayListener> listener1 = new DmMockDisplayListener();
    DisplayManagerLite::GetInstance().RegisterDisplayListener(listener);
    auto ret = DisplayManagerLite::GetInstance().RegisterDisplayListener(listener1);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: UnregisterDisplayListener
 * @tc.desc: listener == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, UnregisterDisplayListener01, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IDisplayListener> listener = nullptr;
    auto ret = DisplayManagerLite::GetInstance().UnregisterDisplayListener(listener);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: UnregisterDisplayListener
 * @tc.desc: iter == displayListeners_.end()
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, UnregisterDisplayListener02, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    auto ret = DisplayManagerLite::GetInstance().UnregisterDisplayListener(listener);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: UnregisterDisplayListener
 * @tc.desc: iter == displayListeners_.end()
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, UnregisterDisplayListener03, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManagerLite::GetInstance().pImpl_->displayListeners_.insert(listener);
    auto ret = DisplayManagerLite::GetInstance().UnregisterDisplayListener(listener);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: RegisterFoldStatusListener
 * @tc.desc: listener == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterFoldStatusListener01, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IFoldStatusListener> listener = nullptr;
    auto ret = DisplayManagerLite::GetInstance().RegisterFoldStatusListener(listener);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: RegisterFoldStatusListener
 * @tc.desc: listener == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterFoldStatusListener02, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    sptr<DisplayManagerLite::IFoldStatusListener> listener = new DmMockFoldStatusListener();
    auto ret = DisplayManagerLite::GetInstance().RegisterFoldStatusListener(listener);
    ASSERT_EQ(ret, DMError::DM_OK);
    DisplayManagerLite::GetInstance().pImpl_->foldStatusListenerAgent_ = nullptr;
}

/**
 * @tc.name: RegisterFoldStatusListener
 * @tc.desc: foldStatusListenerAgent_ == nullptr, ret != DMError::DM_OK
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterFoldStatusListener03, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_ERROR_IPC_FAILED));
    sptr<DisplayManagerLite::IFoldStatusListener> listener = new DmMockFoldStatusListener();
    DisplayManagerLite::GetInstance().RegisterFoldStatusListener(listener);
    ASSERT_EQ(DisplayManagerLite::GetInstance().pImpl_->foldStatusListenerAgent_, nullptr);
}

/**
 * @tc.name: RegisterFoldStatusListener
 * @tc.desc: foldStatusListenerAgent_ != nullptr, ret == DMError::DM_OK
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterFoldStatusListener04, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(DMError::DM_OK));
    sptr<DisplayManagerLite::IFoldStatusListener> listener = new DmMockFoldStatusListener();
    sptr<DisplayManagerLite::IFoldStatusListener> listener1 = new DmMockFoldStatusListener();
    DisplayManagerLite::GetInstance().RegisterFoldStatusListener(listener);
    auto ret = DisplayManagerLite::GetInstance().RegisterFoldStatusListener(listener1);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: UnregisterFoldStatusListener
 * @tc.desc: foldStatusListenerAgent_ != nullptr, ret == DMError::DM_OK
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, UnregisterFoldStatusListener01, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IFoldStatusListener> listener = nullptr;
    auto ret = DisplayManagerLite::GetInstance().UnregisterFoldStatusListener(listener);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: UnregisterFoldStatusListener
 * @tc.desc: iter == foldStatusListeners_.end()
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, UnregisterFoldStatusListener02, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IFoldStatusListener> listener = new DmMockFoldStatusListener();
    auto ret = DisplayManagerLite::GetInstance().UnregisterFoldStatusListener(listener);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: UnregisterFoldStatusListener
 * @tc.desc: iter != foldStatusListeners_.end()
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, UnregisterFoldStatusListener03, Function | SmallTest | Level1)
{
    sptr<DisplayManagerLite::IFoldStatusListener> listener = new DmMockFoldStatusListener();
    DisplayManagerLite::GetInstance().pImpl_->foldStatusListeners_.insert(listener);
    auto ret = DisplayManagerLite::GetInstance().UnregisterFoldStatusListener(listener);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: GetDefaultDisplay
 * @tc.desc: GetDefaultDisplay
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, GetDefaultDisplay, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetDefaultDisplayInfo()).Times(1).WillOnce(Return(nullptr));
    auto ret = DisplayManagerLite::GetInstance().GetDefaultDisplay();
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDisplayById
 * @tc.desc: destroyed_ == false
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, GetDisplayById01, Function | SmallTest | Level1)
{
    DisplayId displayId = 1000;
    auto ret = DisplayManagerLite::GetInstance().GetDisplayById(displayId);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDisplayById
 * @tc.desc: UpdateDisplayInfoLocked
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, GetDisplayById02, Function | SmallTest | Level1)
{
    DisplayManagerLite::GetInstance().destroyed_ = false;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetDisplayInfo(_)).Times(1).WillOnce(Return(nullptr));
    DisplayId displayId = 1000;
    auto ret = DisplayManagerLite::GetInstance().GetDisplayById(displayId);
    ASSERT_EQ(ret, nullptr);
}
}
} // namespace Rosen
} // namespace OHOS
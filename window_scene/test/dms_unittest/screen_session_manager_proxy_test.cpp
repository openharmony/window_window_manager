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
#include <message_option.h>
#include <message_parcel.h>
#include <common/rs_rect.h>
#include <transaction/rs_marshalling_helper.h>
#include <iremote_broker.h>
#include <iremote_object_mocker.h>
#include "mock_message_parcel.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager_agent.h"
#include "zidl/screen_session_manager_proxy.h"
#include "zidl/window_manager_agent_interface.h"
#include "display_manager_adapter.h"
#include "display_manager_agent_default.h"
#include "scene_board_judgement.h"
#include "screen_session_manager/include/screen_session_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class ScreenSessionManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSessionManagerProxyTest::SetUpTestCase()
{
}

void ScreenSessionManagerProxyTest::TearDownTestCase()
{
}

void ScreenSessionManagerProxyTest::SetUp()
{
}

void ScreenSessionManagerProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetPrivacyStateByDisplayId
 * @tc.desc: SetPrivacyStateByDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetPrivacyStateByDisplayId, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    DisplayId id = 0;
    bool hasPrivate = false;
    
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        screenSessionManagerProxy->SetPrivacyStateByDisplayId(id, hasPrivate);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: SetScreenPrivacyWindowList
 * @tc.desc: SetScreenPrivacyWindowList
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenPrivacyWindowList, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    DisplayId id = 0;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        screenSessionManagerProxy->SetScreenPrivacyWindowList(id, privacyWindowList);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: SetVirtualScreenBlackList
 * @tc.desc: SetVirtualScreenBlackList
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualScreenBlackList, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    std::vector<uint64_t> windowIdList{10, 20, 30};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        screenSessionManagerProxy->SetVirtualScreenBlackList(id, windowIdList);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: ProxyForFreeze
 * @tc.desc: ProxyForFreeze
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, ProxyForFreeze, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    const std::set<int32_t>& pidList = {1, 2, 3};
    bool isProxy = true;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->ProxyForFreeze(pidList, isProxy));
    } else {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->ProxyForFreeze(pidList, isProxy));
    }
}

/**
 * @tc.name: SetVirtualScreenStatus
 * @tc.desc: SetVirtualScreenStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualScreenStatus, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    VirtualScreenStatus screenStatus = VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(expectation, screenSessionManagerProxy->SetVirtualScreenStatus(id, screenStatus));
    } else {
        EXPECT_EQ(expectation, screenSessionManagerProxy->SetVirtualScreenStatus(id, screenStatus));
    }
}

/**
 * @tc.name: SetVirtualScreenSecurityExemption
 * @tc.desc: SetVirtualScreenSecurityExemption
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualScreenSecurityExemption, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    uint32_t pid = 1;
    std::vector<uint64_t> windowIdList{10, 20, 30};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetVirtualScreenSecurityExemption(id, pid, windowIdList));
    } else {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetVirtualScreenSecurityExemption(id, pid, windowIdList));
    }
}

/**
 * @tc.name: GetAllDisplayPhysicalResolution
 * @tc.desc: GetAllDisplayPhysicalResolution
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetAllDisplayPhysicalResolution, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    std::vector<DisplayPhysicalResolution> allSize {};
    std::function<void()> func = [&]()
    {
        allSize = screenSessionManagerProxy->GetAllDisplayPhysicalResolution();
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(!allSize.empty());
    } else {
        ASSERT_FALSE(!allSize.empty());
    }
}

/**
 * @tc.name: GetDefaultDisplayInfo
 * @tc.desc: GetDefaultDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetDefaultDisplayInfo, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    sptr<DisplayInfo> expectation = nullptr;
    sptr<DisplayInfo> res = nullptr;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetDefaultDisplayInfo();
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, expectation);
    } else {
        ASSERT_EQ(res, expectation);
    }
}

/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: SetScreenActiveMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenActiveMode, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    uint32_t modeId = 1;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetScreenActiveMode(id, modeId);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: SetVirtualPixelRatio
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualPixelRatio, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    float virtualPixelRatio = 1.0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetVirtualPixelRatio(id, virtualPixelRatio);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetVirtualPixelRatioSystem
 * @tc.desc: SetVirtualPixelRatioSystem
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualPixelRatioSystem, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    float virtualPixelRatio = 1.0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetVirtualPixelRatioSystem(id, virtualPixelRatio);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetDefaultDensityDpi
 * @tc.desc: SetDefaultDensityDpi
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetDefaultDensityDpi, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    float virtualPixelRatio = 1.0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetDefaultDensityDpi(id, virtualPixelRatio);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetResolution
 * @tc.desc: SetResolution
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetResolution, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    uint32_t width = 1024;
    uint32_t height = 1024;
    float virtualPixelRatio = 1.0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetResolution(id, width, height, virtualPixelRatio);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: GetDensityInCurResolution
 * @tc.desc: GetDensityInCurResolution
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetDensityInCurResolution, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    float virtualPixelRatio = 1.0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetDensityInCurResolution(id, virtualPixelRatio);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: GetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenColorGamut, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    ScreenColorGamut colorGamut {0};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetScreenColorGamut(id, colorGamut);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: SetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenColorGamut, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    uint32_t colorGamut = 0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetScreenColorGamut(id, colorGamut);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: GetScreenGamutMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenGamutMap, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    ScreenGamutMap colorGamut;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetScreenGamutMap(id, colorGamut);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: SetScreenGamutMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenGamutMap, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    ScreenGamutMap colorGamut {0};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetScreenGamutMap(id, colorGamut);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetScreenColorTransform
 * @tc.desc: SetScreenColorTransform
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenColorTransform, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetScreenColorTransform(id);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: GetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetPixelFormat, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    GraphicPixelFormat pixelFormat = GraphicPixelFormat {GRAPHIC_PIXEL_FMT_CLUT8};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetPixelFormat(id, pixelFormat);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: SetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetPixelFormat, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    GraphicPixelFormat pixelFormat = GraphicPixelFormat {GRAPHIC_PIXEL_FMT_CLUT8};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetPixelFormat(id, pixelFormat);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: GetSupportedHDRFormats
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetSupportedHDRFormats, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    vector<ScreenHDRFormat> hdrFormats;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetSupportedHDRFormats(id, hdrFormats);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: GetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenHDRFormat, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    ScreenHDRFormat hdrFormatS {0};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetScreenHDRFormat(id, hdrFormatS);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: SetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenHDRFormat, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenId id = 1001;
    ScreenHDRFormat hdrFormatS {0};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);


    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetScreenHDRFormat(id, hdrFormatS);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: RegisterDisplayManagerAgent
 * @tc.desc: RegisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, RegisterDisplayManagerAgent, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::SCREEN_EVENT_LISTENER;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(DMError::DM_OK, screenSessionManagerProxy->RegisterDisplayManagerAgent(displayManagerAgent, type));
    } else {
        EXPECT_NE(DMError::DM_OK, screenSessionManagerProxy->RegisterDisplayManagerAgent(displayManagerAgent, type));
    }
    displayManagerAgent = nullptr;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM,
              screenSessionManagerProxy->RegisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: UnregisterDisplayManagerAgent
 * @tc.desc: UnregisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, UnregisterDisplayManagerAgent, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::SCREEN_EVENT_LISTENER;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(DMError::DM_OK, screenSessionManagerProxy->UnregisterDisplayManagerAgent(displayManagerAgent, type));
    } else {
        EXPECT_NE(DMError::DM_OK, screenSessionManagerProxy->UnregisterDisplayManagerAgent(displayManagerAgent, type));
    }
    displayManagerAgent = nullptr;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM,
              screenSessionManagerProxy->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeUpBegin
 * @tc.desc: WakeUpBegin
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, WakeUpBegin, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    PowerStateChangeReason reason {0};
    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(expectation, screenSessionManagerProxy->WakeUpBegin(reason));
    } else {
        EXPECT_NE(expectation, screenSessionManagerProxy->WakeUpBegin(reason));
    }
}

/**
 * @tc.name: WakeUpEnd
 * @tc.desc: WakeUpEnd
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, WakeUpEnd, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(expectation, screenSessionManagerProxy->WakeUpEnd());
    } else {
        EXPECT_NE(expectation, screenSessionManagerProxy->WakeUpEnd());
    }
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: SuspendBegin
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SuspendBegin, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    PowerStateChangeReason reason {0};
    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(expectation, screenSessionManagerProxy->SuspendBegin(reason));
    } else {
        EXPECT_NE(expectation, screenSessionManagerProxy->SuspendBegin(reason));
    }
}

/**
 * @tc.name: SuspendEnd
 * @tc.desc: SuspendEnd
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SuspendEnd, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(expectation, screenSessionManagerProxy->SuspendEnd());
    } else {
        EXPECT_NE(expectation, screenSessionManagerProxy->SuspendEnd());
    }
}

/**
 * @tc.name: SetScreenPowerById
 * @tc.desc: SetScreenPowerById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenPowerById, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    ScreenPowerState state {0};
    ScreenId id = 1001;
    PowerStateChangeReason reason {1};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(expectation, screenSessionManagerProxy->SetScreenPowerById(id, state, reason));
    } else {
        EXPECT_NE(expectation, screenSessionManagerProxy->SetScreenPowerById(id, state, reason));
    }
}

/**
 * @tc.name: SetDisplayState
 * @tc.desc: SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetDisplayState, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    DisplayState state {1};
    screenSessionManagerProxy->SetDisplayState(state);
    int resultValue = 0;
    ASSERT_EQ(resultValue, 0);
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: SetSpecifiedScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetSpecifiedScreenPower, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenPowerState state {0};
    ScreenId id = 1001;
    PowerStateChangeReason reason {1};
    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(expectation, screenSessionManagerProxy->SetSpecifiedScreenPower(id, state, reason));
    } else {
        EXPECT_NE(expectation, screenSessionManagerProxy->SetSpecifiedScreenPower(id, state, reason));
    }
}

/**
 * @tc.name: SetScreenPowerForAll
 * @tc.desc: SetScreenPowerForAll
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenPowerForAll, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenPowerState state {0};
    PowerStateChangeReason reason {1};
    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(expectation, screenSessionManagerProxy->SetScreenPowerForAll(state, reason));
    } else {
        EXPECT_NE(expectation, screenSessionManagerProxy->SetScreenPowerForAll(state, reason));
    }
}

/**
 * @tc.name: GetDisplayState
 * @tc.desc: GetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetDisplayState, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    DisplayId displayId {0};
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DisplayState::UNKNOWN, screenSessionManagerProxy->GetDisplayState(displayId));
    } else {
        EXPECT_EQ(DisplayState::UNKNOWN, screenSessionManagerProxy->GetDisplayState(displayId));
    }
}

/**
 * @tc.name: TryToCancelScreenOff
 * @tc.desc: TryToCancelScreenOff
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, TryToCancelScreenOff, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool expectation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(expectation, screenSessionManagerProxy->TryToCancelScreenOff());
    } else {
        EXPECT_EQ(expectation, screenSessionManagerProxy->TryToCancelScreenOff());
    }
}

/**
 * @tc.name: NotifyDisplayEvent
 * @tc.desc: NotifyDisplayEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, NotifyDisplayEvent, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    DisplayEvent event {0};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        screenSessionManagerProxy->NotifyDisplayEvent(event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: GetScreenPower
 * @tc.desc: GetScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenPower, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId dmsScreenId = 1001;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(ScreenPowerState::INVALID_STATE, screenSessionManagerProxy->GetScreenPower(dmsScreenId));
    } else {
        EXPECT_EQ(ScreenPowerState::INVALID_STATE, screenSessionManagerProxy->GetScreenPower(dmsScreenId));
    }
}

/**
 * @tc.name: AddVirtualScreenBlockList
 * @tc.desc: AddVirtualScreenBlockList
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, AddVirtualScreenBlockList, TestSize.Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    std::vector<int32_t> persistentIds {0, 1, 2};
    EXPECT_NE(DMError::DM_ERROR_IPC_FAILED, screenSessionManagerProxy->AddVirtualScreenBlockList(persistentIds));
}

/**
 * @tc.name: RemoveVirtualScreenBlockList
 * @tc.desc: RemoveVirtualScreenBlockList
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, RemoveVirtualScreenBlockList, TestSize.Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    std::vector<int32_t> persistentIds {0, 1, 2};
    EXPECT_NE(DMError::DM_ERROR_IPC_FAILED, screenSessionManagerProxy->RemoveVirtualScreenBlockList(persistentIds));
}

/**
 * @tc.name: SetVirtualMirrorScreenCanvasRotation
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualMirrorScreenCanvasRotation, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId ScreenId = 1001;
    bool canvasRotation = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
            screenSessionManagerProxy->SetVirtualMirrorScreenCanvasRotation(ScreenId, canvasRotation));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
            screenSessionManagerProxy->SetVirtualMirrorScreenCanvasRotation(ScreenId, canvasRotation));
    }
}

/**
 * @tc.name: SetVirtualMirrorScreenScaleMode
 * @tc.desc: SetVirtualMirrorScreenScaleMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualMirrorScreenScaleMode, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId ScreenId = 1001;
    ScreenScaleMode scaleMode {0};
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetVirtualMirrorScreenScaleMode(ScreenId, scaleMode));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetVirtualMirrorScreenScaleMode(ScreenId, scaleMode));
    }
}

/**
 * @tc.name: ResizeVirtualScreen
 * @tc.desc: ResizeVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, ResizeVirtualScreen, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId ScreenId = 1001;
    uint32_t width = 1024;
    uint32_t height = 1024;
    EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
              screenSessionManagerProxy->ResizeVirtualScreen(ScreenId, width, height));
}

/**
 * @tc.name: DestroyVirtualScreen
 * @tc.desc: DestroyVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, DestroyVirtualScreen, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId ScreenId = 1001;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->DestroyVirtualScreen(ScreenId));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->DestroyVirtualScreen(ScreenId));
    }
}

/**
 * @tc.name: MakeMirror
 * @tc.desc: MakeMirror
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, MakeMirror, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId Id = 1001;
    std::vector<ScreenId> mirrorScreenIds = {1003, 1004, 1005};
    ScreenId screenGroupId = 1002;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->MakeMirror(Id, mirrorScreenIds, screenGroupId));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->MakeMirror(Id, mirrorScreenIds, screenGroupId));
    }
}

/**
 * @tc.name: StopMirror
 * @tc.desc: StopMirror
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, StopMirror, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    const std::vector<ScreenId> mirrorScreenIds = {1003, 1004, 1005};
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->StopMirror(mirrorScreenIds));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->StopMirror(mirrorScreenIds));
    }
}

/**
 * @tc.name: DisableMirror
 * @tc.desc: DisableMirror
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, DisableMirror, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool disableOrNot = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->DisableMirror(disableOrNot));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->DisableMirror(disableOrNot));
    }
}

/**
 * @tc.name: MakeExpand
 * @tc.desc: MakeExpand
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, MakeExpand, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    std::vector<ScreenId> screenId = {1003, 1004, 1005};
    std::vector<Point> startPoint {};
    ScreenId screenGroupId = 1001;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->MakeExpand(screenId, startPoint, screenGroupId));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->MakeExpand(screenId, startPoint, screenGroupId));
    }
}

/**
 * @tc.name: StopExpand
 * @tc.desc: StopExpand
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, StopExpand, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    const std::vector<ScreenId> expandScreenIds = {1003, 1004, 1005};
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->StopExpand(expandScreenIds));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->StopExpand(expandScreenIds));
    }
}

/**
 * @tc.name: GetScreenGroupInfoById
 * @tc.desc: GetScreenGroupInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenGroupInfoById, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    sptr<ScreenGroupInfo> expectation = nullptr;
    sptr<ScreenGroupInfo> res = nullptr;
    ScreenId Id {0};
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetScreenGroupInfoById(Id);
    };
    func();
    ASSERT_EQ(res, expectation);
}

/**
 * @tc.name: RemoveVirtualScreenFromGroup
 * @tc.desc: RemoveVirtualScreenFromGroup
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, RemoveVirtualScreenFromGroup, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    std::vector<ScreenId> screens = {1002, 1003, 1004};
    screenSessionManagerProxy->RemoveVirtualScreenFromGroup(screens);
    auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(2000);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: GetDisplaySnapshot
 * @tc.desc: GetDisplaySnapshot
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetDisplaySnapshot, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    std::shared_ptr<Media::PixelMap> expectation = nullptr;
    std::shared_ptr<Media::PixelMap> res = nullptr;
    DisplayId displayId {0};
    DmErrorCode* errorCode = nullptr;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetDisplaySnapshot(displayId, errorCode, false);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, expectation);
    } else {
        ASSERT_EQ(res, expectation);
    }
}

/**
 * @tc.name: GetDisplayInfoById
 * @tc.desc: GetDisplayInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetDisplayInfoById, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    sptr<DisplayInfo> expectation = nullptr;
    sptr<DisplayInfo> res = nullptr;
    DisplayId displayId {0};
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetDisplayInfoById(displayId);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, expectation);
    } else {
        ASSERT_EQ(res, expectation);
    }
}

/**
 * @tc.name: GetDisplayInfoByScreen
 * @tc.desc: GetDisplayInfoByScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetDisplayInfoByScreen, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    sptr<DisplayInfo> expectation = nullptr;
    sptr<DisplayInfo> res = nullptr;
    ScreenId screenId {0};
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetDisplayInfoByScreen(screenId);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, expectation);
    } else {
        ASSERT_EQ(res, expectation);
    }
}

/**
 * @tc.name: GetAllDisplayIds
 * @tc.desc: GetAllDisplayIds
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetAllDisplayIds, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        screenSessionManagerProxy->GetAllDisplayIds();
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: GetScreenInfoById
 * @tc.desc: GetScreenInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenInfoById, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    sptr<ScreenInfo> expectation = nullptr;
    sptr<ScreenInfo> res = nullptr;
    ScreenId Id {0};
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetScreenInfoById(Id);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, expectation);
    } else {
        ASSERT_EQ(res, expectation);
    }
}

/**
 * @tc.name: GetAllScreenInfos
 * @tc.desc: GetAllScreenInfos
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetAllScreenInfos, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    std::vector<sptr<ScreenInfo>> screenInfos {};
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->GetAllScreenInfos(screenInfos));
    } else {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->GetAllScreenInfos(screenInfos));
    }
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: GetScreenSupportedColorGamuts
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenSupportedColorGamuts, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId screenId = 1001;
    std::vector<ScreenColorGamut> colorGamuts {};
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->GetScreenSupportedColorGamuts(screenId, colorGamuts));
    } else {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->GetScreenSupportedColorGamuts(screenId, colorGamuts));
    }
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetOrientation, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId screenId = 1001;
    Orientation orientation = Orientation::BEGIN;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetOrientation(screenId, orientation));
    } else {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetOrientation(screenId, orientation));
    }
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: SetScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenRotationLocked, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool isLocked = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetScreenRotationLocked(isLocked));
    } else {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetScreenRotationLocked(isLocked));
    }
}

/**
 * @tc.name: SetScreenRotationLockedFromJs
 * @tc.desc: SetScreenRotationLockedFromJs
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenRotationLockedFromJs, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool isLocked = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetScreenRotationLockedFromJs(isLocked));
    } else {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetScreenRotationLockedFromJs(isLocked));
    }
}

/**
 * @tc.name: SetMultiScreenMode
 * @tc.desc: SetMultiScreenMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetMultiScreenMode, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId mainScreenId = 0;
    ScreenId secondaryScreenId = 1;
    MultiScreenMode screenMode = MultiScreenMode::SCREEN_EXTEND;

    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetMultiScreenMode(mainScreenId, secondaryScreenId, screenMode);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: SetMultiScreenRelativePosition
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetMultiScreenRelativePosition, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId testId = 2060;
    ScreenId testId1 = 3060;
    MultiScreenPositionOptions mainScreenOptions = {testId, 100, 100};
    MultiScreenPositionOptions secondScreenOption = {testId1, 100, 100};

    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: SetVirtualScreenMaxRefreshRate
 * @tc.desc: SetVirtualScreenMaxRefreshRate
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetVirtualScreenMaxRefreshRate, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId id = 0;
    uint32_t refreshRate = 12;
    uint32_t actualRefreshRate = 0;

    DMError res = DMError::DM_ERROR_NOT_SYSTEM_APP;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->SetVirtualScreenMaxRefreshRate(id, refreshRate, actualRefreshRate);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(res, DMError::DM_ERROR_IPC_FAILED);
    }
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: IsScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, IsScreenRotationLocked, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool isLocked = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->IsScreenRotationLocked(isLocked));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->IsScreenRotationLocked(isLocked));
    }
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: GetCutoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetCutoutInfo, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    sptr<CutoutInfo> expectation = nullptr;
    sptr<CutoutInfo> res = nullptr;
    DisplayId displayId = 0;
    std::function<void()> func = [&]()
    {
        res = screenSessionManagerProxy->GetCutoutInfo(displayId);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(res, expectation);
    } else {
        ASSERT_EQ(res, expectation);
    }
}

/**
 * @tc.name: HasImmersiveWindow
 * @tc.desc: HasImmersiveWindow
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, HasImmersiveWindow, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool immersive = true;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->HasImmersiveWindow(0u, immersive));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->HasImmersiveWindow(0u, immersive));
    }
}

/**
 * @tc.name: ConvertScreenIdToRsScreenId
 * @tc.desc: ConvertScreenIdToRsScreenId
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, ConvertScreenIdToRsScreenId, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId screenId = 1001;
    ScreenId rsScreenId = 1002;
    auto ret = screenSessionManagerProxy->ConvertScreenIdToRsScreenId(screenId, rsScreenId);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: HasPrivateWindow
 * @tc.desc: HasPrivateWindow
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, HasPrivateWindow, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool hasPrivateWindow = true;
    DisplayId displayId = 0;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->HasPrivateWindow(displayId, hasPrivateWindow));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->HasPrivateWindow(displayId, hasPrivateWindow));
    }
}

/**
 * @tc.name: DumpAllScreensInfo
 * @tc.desc: DumpAllScreensInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, DumpAllScreensInfo, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    std::string dumpInfo;
    screenSessionManagerProxy->DumpAllScreensInfo(dumpInfo);
    EXPECT_NE(dumpInfo, "");
}

/**
 * @tc.name: DumpSpecialScreenInfo
 * @tc.desc: DumpSpecialScreenInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, DumpSpecialScreenInfo, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    ScreenId id = 1001;
    std::string dumpInfo;
    screenSessionManagerProxy->DumpSpecialScreenInfo(id, dumpInfo);
    EXPECT_NE(dumpInfo, "");
}

/**
 * @tc.name: SetFoldDisplayMode
 * @tc.desc: SetFoldDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetFoldDisplayMode, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    const FoldDisplayMode displayMode {0};
    screenSessionManagerProxy->SetFoldDisplayMode(displayMode);
    if (screenSessionManagerProxy->IsFoldable()) {
        EXPECT_NE(ScreenSessionManager::GetInstance().foldScreenController_, nullptr);
    } else {
        EXPECT_EQ(ScreenSessionManager::GetInstance().foldScreenController_, nullptr);
    }
}

/**
 * @tc.name: SetFoldDisplayModeFromJs
 * @tc.desc: SetFoldDisplayModeFromJs
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetFoldDisplayModeFromJs, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    const FoldDisplayMode displayMode {0};

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetFoldDisplayModeFromJs(displayMode));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->SetFoldDisplayModeFromJs(displayMode));
    }
}

/**
 * @tc.name: SetFoldStatusLocked
 * @tc.desc: SetFoldStatusLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetFoldStatusLocked, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    bool locked = true;
    screenSessionManagerProxy->SetFoldStatusLocked(locked);
    if (screenSessionManagerProxy->IsFoldable()) {
        EXPECT_NE(ScreenSessionManager::GetInstance().foldScreenController_, nullptr);
    } else {
        EXPECT_EQ(ScreenSessionManager::GetInstance().foldScreenController_, nullptr);
    }
}

/**
 * @tc.name: GetFoldDisplayMode
 * @tc.desc: GetFoldDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetFoldDisplayMode, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    EXPECT_EQ(FoldDisplayMode::UNKNOWN,
              screenSessionManagerProxy->GetFoldDisplayMode());
}

/**
 * @tc.name: IsFoldable
 * @tc.desc: IsFoldable
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, IsFoldable, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    screenSessionManagerProxy->IsFoldable();
    auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(2000);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: IsCaptured
 * @tc.desc: IsCaptured
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, IsCaptured, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    auto ret = screenSessionManagerProxy->IsCaptured();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetFoldStatus
 * @tc.desc: GetFoldStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetFoldStatus, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_NE(screenSessionManagerProxy, nullptr);
    screenSessionManagerProxy->GetFoldStatus();
}

/**
 * @tc.name: GetSuperFoldStatus
 * @tc.desc: GetSuperFoldStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetSuperFoldStatus, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_NE(screenSessionManagerProxy, nullptr);
    screenSessionManagerProxy->GetSuperFoldStatus();
}

/**
 * @tc.name: SetLandscapeLockStatus
 * @tc.desc: SetLandscapeLockStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetLandscapeLockStatus, Function | SmallTest | Level1)
{
    bool isLocked = false;
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_NE(screenSessionManagerProxy, nullptr);
    screenSessionManagerProxy->SetLandscapeLockStatus(isLocked);
}

/**
 * @tc.name: GetCurrentFoldCreaseRegion
 * @tc.desc: GetCurrentFoldCreaseRegion
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetCurrentFoldCreaseRegion, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_NE(screenSessionManagerProxy, nullptr);
    
    screenSessionManagerProxy->GetCurrentFoldCreaseRegion();
}

/**
 * @tc.name: MakeUniqueScreen
 * @tc.desc: MakeUniqueScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, MakeUniqueScreen, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    
    const std::vector<ScreenId> screenIds {1001, 1002, 1003};
    std::vector<DisplayId> displayIds;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, screenSessionManagerProxy->MakeUniqueScreen(screenIds, displayIds));
}

/**
 * @tc.name: SetClient
 * @tc.desc: SetClient
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetClient, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);

    const sptr<IScreenSessionManagerClient> client = nullptr;
    screenSessionManagerProxy->SetClient(client);
    EXPECT_EQ(client, nullptr);
}

/**
 * @tc.name: SwitchUser
 * @tc.desc: SwitchUser
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SwitchUser, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    screenSessionManagerProxy->SwitchUser();
    auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(2000);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: GetScreenProperty
 * @tc.desc: GetScreenProperty
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenProperty, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ScreenId screenId = 1001;
    screenSessionManagerProxy->GetScreenProperty(screenId);
    auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(2000);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: GetScreenCapture
 * @tc.desc: GetScreenCapture test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetScreenCapture, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_TRUE(screenSessionManagerProxy != nullptr);

    std::shared_ptr<Media::PixelMap> res = nullptr;
    CaptureOption option;
    option.displayId_ = 0;
    DmErrorCode errorCode = DmErrorCode::DM_OK;
    std::function<void()> func = [&]() {
        res = screenSessionManagerProxy->GetScreenCapture(option, &errorCode);
    };
    func();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        if (errorCode == DmErrorCode::DM_OK) {
            ASSERT_NE(res, nullptr);
        } else {
            ASSERT_EQ(res, nullptr);
        }
    } else {
        ASSERT_EQ(res, nullptr);
    }
}

/**
 * @tc.name: GetPrimaryDisplayInfo
 * @tc.desc: GetPrimaryDisplayInfo test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetPrimaryDisplayInfo, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_TRUE(screenSessionManagerProxy != nullptr);

    sptr<DisplayInfo> res = nullptr;
    std::function<void()> func = [&]() {
        res = screenSessionManagerProxy->GetPrimaryDisplayInfo();
    };
    func();
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: SetScreenSkipProtectedWindow
 * @tc.desc: SetScreenSkipProtectedWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, SetScreenSkipProtectedWindow, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_TRUE(screenSessionManagerProxy != nullptr);

    const std::vector<ScreenId> screenIds = {1001, 1002};
    bool isEnable = true;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        screenSessionManagerProxy->SetScreenSkipProtectedWindow(screenIds, isEnable);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: GetDisplayCapability
 * @tc.desc: GetDisplayCapability test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, GetDisplayCapability, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<ScreenSessionManagerProxy> screenSessionManagerProxy = new ScreenSessionManagerProxy(impl);
    ASSERT_TRUE(screenSessionManagerProxy != nullptr);
    std::string capabilitInfo;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_NE(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->GetDisplayCapability(capabilitInfo));
    } else {
        EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED,
                screenSessionManagerProxy->GetDisplayCapability(capabilitInfo));
    }
}

/**
 * @tc.name: NotifyIsFullScreenInForceSplitMode
 * @tc.desc: NotifyIsFullScreenInForceSplitMode test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerProxyTest, NotifyIsFullScreenInForceSplitMode, TestSize.Level3)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockMessageParcel::ClearAllErrorFlag();
    auto proxy = sptr<ScreenSessionManagerProxy>::MakeSptr(nullptr);
    proxy->NotifyIsFullScreenInForceSplitMode(0, true);
    sptr<MockIRemoteObject> remoteMocker = sptr<MockIRemoteObject>::MakeSptr();
    proxy = sptr<ScreenSessionManagerProxy>::MakeSptr(remoteMocker);
    proxy->NotifyIsFullScreenInForceSplitMode(0, true);
    remoteMocker->SetRequestResult(ERR_INVALID_DATA);
    proxy->NotifyIsFullScreenInForceSplitMode(0, true);
    remoteMocker->SetRequestResult(ERR_NONE);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    proxy->NotifyIsFullScreenInForceSplitMode(0, true);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    proxy->NotifyIsFullScreenInForceSplitMode(0, true);
    EXPECT_TRUE(logMsg.find("Write uid failed") != std::string::npos);
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteBoolErrorFlag(true);
    proxy->NotifyIsFullScreenInForceSplitMode(0, true);
    EXPECT_TRUE(logMsg.find("Write isFullScreen failed") != std::string::npos);
    MockMessageParcel::ClearAllErrorFlag();
    LOG_SetCallback(nullptr);
}
}
}
}
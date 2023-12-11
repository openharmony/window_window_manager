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
#include <bundle_mgr_interface.h>
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string EMPTY_DEVICE_ID = "";
    constexpr int WAIT_SLEEP_TIME = 1;
    using ConfigItem = WindowSceneConfig::ConfigItem;
    ConfigItem ReadConfig(const std::string& xmlStr)
    {
        ConfigItem config;
        xmlDocPtr docPtr = xmlParseMemory(xmlStr.c_str(), xmlStr.length() + 1);
        if (docPtr == nullptr) {
            return config;
        }

        xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
        if (rootPtr == nullptr || rootPtr->name == nullptr ||
            xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
            xmlFreeDoc(docPtr);
            return config;
        }

        std::map<std::string, ConfigItem> configMap;
        config.SetValue(configMap);
        WindowSceneConfig::ReadConfig(rootPtr, *config.mapValue_);
        xmlFreeDoc(docPtr);
        return config;
    }
}
class SceneSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static bool statusBarEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static ProcessStatusBarEnabledChangeFunc statusBarEnabledCallbackFunc_;
    static sptr<SceneSessionManager> ssm_;
};

sptr<SceneSessionManager> SceneSessionManagerTest::ssm_ = nullptr;

bool SceneSessionManagerTest::gestureNavigationEnabled_ = true;
bool SceneSessionManagerTest::statusBarEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest::callbackFunc_ = [](bool enable) {
    gestureNavigationEnabled_ = enable;
};
ProcessStatusBarEnabledChangeFunc SceneSessionManagerTest::statusBarEnabledCallbackFunc_ = [](bool enable) {
    statusBarEnabled_ = enable;
};

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest::SetUpTestCase()
{
}

void SceneSessionManagerTest::TearDownTestCase()
{
}

void SceneSessionManagerTest::SetUp()
{
    ssm_ = new SceneSessionManager();
}

void SceneSessionManagerTest::TearDown()
{
    ssm_ = nullptr;
}

namespace {
/**
 * @tc.name: SetBrightness
 * @tc.desc: ScreenSesionManager set session brightness
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetBrightness, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result = SceneSessionManager::GetInstance().SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
    delete sceneSession;
}

/**
 * @tc.name: SetGestureNavigaionEnabled
 * @tc.desc: SceneSessionManager set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetGestureNavigaionEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(callbackFunc_, nullptr);

    WMError result00 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result00, WMError::WM_DO_NOTHING);

    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(callbackFunc_);
    WMError result01 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result01, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, true);

    WMError result02 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(false);
    ASSERT_EQ(result02, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, false);

    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(nullptr);
    WMError result03 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result03, WMError::WM_DO_NOTHING);
}

/**
 * @tc.name: SetStatusBarEnabled
 * @tc.desc: SceneSessionManager set status bar enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetStatusBarEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(statusBarEnabledCallbackFunc_, nullptr);
    SceneSessionManager::GetInstance().SetStatusBarEnabledChangeListener(nullptr);

    WMError result00 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result00, WMError::WM_DO_NOTHING);

    SceneSessionManager::GetInstance().SetStatusBarEnabledChangeListener(statusBarEnabledCallbackFunc_);
    WMError result01 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result01, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(statusBarEnabled_, true);

    WMError result02 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(false);
    ASSERT_EQ(result02, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(statusBarEnabled_, false);

    SceneSessionManager::GetInstance().SetStatusBarEnabledChangeListener(nullptr);
    WMError result03 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result03, WMError::WM_DO_NOTHING);
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: SceneSesionManager rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;

    ASSERT_EQ(WMError::WM_OK, SceneSessionManager::GetInstance().RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, SceneSessionManager::GetInstance().UnregisterWindowManagerAgent(
        type, windowManagerAgent));
}

/**
 * @tc.name: ConfigWindowSizeLimits01
 * @tc.desc: call ConfigWindowSizeLimits and check the systemConfig_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSizeLimits01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<mainWindowSizeLimits>"
        "<miniWidth>10</miniWidth>"
        "<miniHeight>20</miniHeight>"
        "</mainWindowSizeLimits>"
        "<subWindowSizeLimits>"
        "<miniWidth>30</miniWidth>"
        "<miniHeight>40</miniHeight>"
        "</subWindowSizeLimits>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSizeLimits();
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniWidthOfMainWindow_, static_cast<uint32_t>(10));
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniHeightOfMainWindow_, static_cast<uint32_t>(20));
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniWidthOfSubWindow_, static_cast<uint32_t>(30));
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniHeightOfSubWindow_, static_cast<uint32_t>(40));
}

/**
 * @tc.name: ConfigWindowEffect01
 * @tc.desc: call ConfigWindowEffect all success focused
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<cornerRadius>"
                        "<fullScreen>off</fullScreen>"
                        "<split>off</split>"
                        "<float>off</float>"
                    "</cornerRadius>"
                    "<shadow>"
                        "<focused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</focused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.radius_, 0.5);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowEffect02
 * @tc.desc: call ConfigWindowEffect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<cornerRadius>"
                        "<fullScreen>off</fullScreen>"
                        "<split>off</split>"
                    "</cornerRadius>"
                    "<shadow>"
                        "<focused>"
                            "<elevation>0</elevation>"
                            "<alpha>0</alpha>"
                        "</focused>"
                        "<unfocused>"
                            "<elevation>0</elevation>"
                        "</unfocused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.alpha_, 0);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowEffect03
 * @tc.desc: call ConfigWindowEffect ConfigAppWindowShadow unfocused
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect03, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<shadow>"
                        "<unfocused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</unfocused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowEffect04
 * @tc.desc: call ConfigWindowEffect all
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect04, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<cornerRadius>"
                        "<fullScreen>off</fullScreen>"
                        "<split>off</split>"
                        "<float>off</float>"
                    "</cornerRadius>"
                    "<shadow>"
                        "<focused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</focused>"
                        "<unfocused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</unfocused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.focusedShadow_.radius_, 0.5);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowEffect05
 * @tc.desc: call ConfigWindowEffect all offsetX.size is not 1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect05, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<shadow>"
                        "<focused>"
                            "<elevation>0</elevation>"
                            "<offsetX>1</offsetX>"
                            "<offsetX>2</offsetX>"
                        "</focused>"
                        "<unfocused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</unfocused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowEffect06
 * @tc.desc: call ConfigWindowEffect offsetY.size is not 1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect06, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<shadow>"
                        "<focused>"
                            "<elevation>0</elevation>"
                            "<offsetY>1</offsetY>"
                            "<offsetY>2</offsetY>"
                        "</focused>"
                        "<unfocused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</unfocused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowEffect07
 * @tc.desc: call ConfigWindowEffect alpha.size is not 1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect07, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<shadow>"
                        "<focused>"
                            "<elevation>0</elevation>"
                            "<alpha>1</alpha>"
                            "<alpha>2</alpha>"
                        "</focused>"
                        "<unfocused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</unfocused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowEffect08
 * @tc.desc: call ConfigWindowEffect radius.size is not 1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect08, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowEffect>"
                "<appWindows>"
                    "<shadow>"
                        "<focused>"
                            "<elevation>0</elevation>"
                            "<radius>1</radius>"
                            "<radius>2</radius>"
                        "</focused>"
                        "<unfocused>"
                            "<elevation>0</elevation>"
                            "<color>#000000</color>"
                            "<offsetX>1</offsetX>"
                            "<offsetY>1</offsetY>"
                            "<alpha>0</alpha>"
                            "<radius>0.5</radius>"
                        "</unfocused>"
                    "</shadow>"
                "</appWindows>"
            "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor fullscreen
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor01, Function | SmallTest | Level3)
{
    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"111\">"
        "<supportedMode>fullscreen</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    SceneSessionManager* sceneSessionManager1 = new SceneSessionManager();
    sceneSessionManager1->ConfigWindowSceneXml();
    delete sceneSessionManager1;

    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>fullscreen</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.decorModeSupportInfo_,
        static_cast<uint32_t>(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN));
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor floating
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor03, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>floating</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor pip
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor04, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>pip</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_PIP);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor split
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor05, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>split</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
        WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor06, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>111</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.decorModeSupportInfo_,
        WINDOW_MODE_SUPPORT_ALL);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowSceneXml01
 * @tc.desc: call defaultWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSceneXml01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>10</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    delete sceneSessionManager;

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>102</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    SceneSessionManager* sceneSessionManager1 = new SceneSessionManager();
    sceneSessionManager1->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager1->systemConfig_.defaultWindowMode_,
        static_cast<WindowMode>(static_cast<uint32_t>(102)));
    delete sceneSessionManager1;
}

/**
 * @tc.name: ConfigWindowSceneXml02
 * @tc.desc: call defaultWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSceneXml02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>1 1</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    delete sceneSessionManager;

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>1</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    SceneSessionManager* sceneSessionManager1 = new SceneSessionManager();
    sceneSessionManager1->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager1->systemConfig_.defaultWindowMode_,
        static_cast<WindowMode>(static_cast<uint32_t>(1)));
    delete sceneSessionManager1;
}

/**
 * @tc.name: ConfigWindowSceneXml03
 * @tc.desc: call defaultMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSceneXml03, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>1 1</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    delete sceneSessionManager;

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>1</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    SceneSessionManager* sceneSessionManager1 = new SceneSessionManager();
    sceneSessionManager1->ConfigWindowSceneXml();
    ASSERT_EQ(SceneSession::maximizeMode_,
        static_cast<MaximizeMode>(static_cast<uint32_t>(1)));
    delete sceneSessionManager1;
}

/**
 * @tc.name: ConfigWindowSceneXml04
 * @tc.desc: call defaultMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSceneXml04, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>111</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    delete sceneSessionManager;

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>0</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    SceneSessionManager* sceneSessionManager1 = new SceneSessionManager();
    sceneSessionManager1->ConfigWindowSceneXml();
    ASSERT_EQ(SceneSession::maximizeMode_,
        static_cast<MaximizeMode>(static_cast<uint32_t>(0)));
    delete sceneSessionManager1;
}

/**
 * @tc.name: ConfigWindowSceneXml05
 * @tc.desc: call maxFloatingWindowSize
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSceneXml05, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxFloatingWindowSize>1</maxFloatingWindowSize>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    delete sceneSessionManager;

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxFloatingWindowSize>1</maxFloatingWindowSize>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    SceneSessionManager* sceneSessionManager1 = new SceneSessionManager();
    sceneSessionManager1->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager1->systemConfig_.maxFloatingWindowSize_,
        static_cast<uint32_t>(1));
    delete sceneSessionManager1;
}

/**
 * @tc.name: ConfigKeyboardAnimation01
 * @tc.desc: call ConfigKeyboardAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigKeyboardAnimation01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<keyboardAnimation>"
                "<animationIn>"
                    "<timing>"
                        "<duration>abv</duration>"
                        "<curve name=\"cubic\">0.2 0.0 0.2 1.0</curve>"
                    "</timing>"
                "</animationIn>"
                "<animationOut>"
                    "<timing>"
                        "<duration>abv</duration>"
                        "<curve name=\"cubic\">0.2 0.0 0.2 1.0</curve>"
                    "</timing>"
                "</animationOut>"
            "</keyboardAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager1 = new SceneSessionManager();
    sceneSessionManager1->ConfigWindowSceneXml();
    delete sceneSessionManager1;

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<keyboardAnimation>"
                "<animationIn>"
                    "<timing>"
                        "<duration>500</duration>"
                        "<curve name=\"cubic\">0.2 0.0 0.2 1.0</curve>"
                    "</timing>"
                "</animationIn>"
                "<animationOut>"
                    "<timing>"
                        "<duration>300</duration>"
                        "<curve name=\"cubic\">0.2 0.0 0.2 1.0</curve>"
                    "</timing>"
                "</animationOut>"
            "</keyboardAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.keyboardAnimationConfig_.durationIn_, static_cast<uint32_t>(500));
    ASSERT_EQ(sceneSessionManager->systemConfig_.keyboardAnimationConfig_.durationOut_, static_cast<uint32_t>(300));
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigKeyboardAnimation02
 * @tc.desc: call ConfigKeyboardAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigKeyboardAnimation02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<keyboardAnimation>"
                "<animationIn>"
                    "<timing>"
                        "<duration>500</duration>"
                        "<duration>600</duration>"
                    "</timing>"
                "</animationIn>"
                "<animationOut>"
                    "<timing>"
                        "<duration>300</duration>"
                    "</timing>"
                "</animationOut>"
            "</keyboardAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.keyboardAnimationConfig_.durationOut_, static_cast<uint32_t>(300));
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigKeyboardAnimation03
 * @tc.desc: call ConfigKeyboardAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigKeyboardAnimation03, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<keyboardAnimation>"
                "<animationIn>"
                    "<timing>"
                        "<duration>500</duration>"
                    "</timing>"
                "</animationIn>"
                "<animationOut>"
                    "<timing>"
                        "<duration>300</duration>"
                        "<duration>400</duration>"
                    "</timing>"
                "</animationOut>"
            "</keyboardAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.keyboardAnimationConfig_.durationIn_, static_cast<uint32_t>(500));
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigKeyboardAnimation04
 * @tc.desc: call ConfigKeyboardAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigKeyboardAnimation04, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<keyboardAnimation>"
                "<animationIn>"
                    "<timing>"
                        "<duration>500</duration>"
                    "</timing>"
                "</animationIn>"
            "</keyboardAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->systemConfig_.keyboardAnimationConfig_.durationIn_, static_cast<uint32_t>(500));
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowAnimation01
 * @tc.desc: call ConfigWindowAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowAnimation>"
                "<timing>"
                    "<duration>350</duration>"
                    "<curve name=\"easeOut\"></curve>"
                "</timing>"
                "<scale>0.7 0.7</scale>"
                "<rotation>0 0 1 0</rotation>"
                "<translate>0 0</translate>"
                "<opacity>0</opacity>"
            "</windowAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.duration_, 350);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowAnimation02
 * @tc.desc: call ConfigWindowAnimation no change
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowAnimation>"
                "<timing>"
                    "<duration>350</duration>"
                    "<curve name=\"easeOut\"></curve>"
                "</timing>"
            "</windowAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.duration_, 350);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowAnimation03
 * @tc.desc: call ConfigWindowAnimation no timing
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation03, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowAnimation>"
                "<timing>"
                "</timing>"
                "<scale>0.7 0.7</scale>"
                "<rotation>0 0 1 0</rotation>"
                "<translate>0 0</translate>"
                "<opacity>0</opacity>"
            "</windowAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowAnimation04
 * @tc.desc: call ConfigWindowAnimation default timing is not int
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation04, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowAnimation>"
                "<timing>"
                    "<duration>aaa</duration>"
                    "<curve></curve>"
                "</timing>"
                "<scale>0.7 0.7</scale>"
                "<rotation>0 0 1 0</rotation>"
                "<translate>0 0</translate>"
                "<opacity>0</opacity>"
            "</windowAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowAnimation05
 * @tc.desc: call ConfigWindowAnimation default timing is error size
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation05, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowAnimation>"
                "<timing>"
                    "<duration>350 350</duration>"
                    "<curve></curve>"
                "</timing>"
                "<scale>0.7 0.7</scale>"
                "<rotation>0 0 1 0</rotation>"
                "<translate>0 0</translate>"
                "<opacity>0</opacity>"
            "</windowAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowAnimation06
 * @tc.desc: call ConfigWindowAnimation default change is not int
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation06, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowAnimation>"
                "<timing>"
                    "<duration>350</duration>"
                    "<curve name=\"easeOut\"></curve>"
                "</timing>"
                "<scale>a a</scale>"
                "<rotation>a a a a</rotation>"
                "<translate>a a</translate>"
                "<opacity>a</opacity>"
            "</windowAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.duration_, 350);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigWindowAnimation07
 * @tc.desc: call ConfigWindowAnimation default change error size
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation07, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<windowAnimation>"
                "<timing>"
                    "<duration>350</duration>"
                    "<curve name=\"easeOut\"></curve>"
                "</timing>"
                "<scale>0.7 0.7 0.7</scale>"
                "<rotation>0 0 1 0 1</rotation>"
                "<translate>0 0 1</translate>"
                "<opacity>0 1</opacity>"
            "</windowAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.windowAnimation_.duration_, 350);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigStartingWindowAnimation01
 * @tc.desc: call ConfigStartingWindowAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigStartingWindowAnimation01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<startWindowTransitionAnimation enable=\"false\">"
                "<timing>"
                    "<duration>200</duration>"
                    "<curve name=\"linear\"></curve>"
                "</timing>"
                "<opacityStart>1</opacityStart>"
                "<opacityEnd>0</opacityEnd>"
            "</startWindowTransitionAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.duration_, 200);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityStart_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityEnd_, 0);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigStartingWindowAnimation02
 * @tc.desc: call ConfigStartingWindowAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigStartingWindowAnimation02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<startWindowTransitionAnimation enable=\"aaa\">"
                "<timing>"
                    "<duration>200</duration>"
                    "<curve name=\"linear\"></curve>"
                "</timing>"
                "<opacityStart>1</opacityStart>"
                "<opacityEnd>0</opacityEnd>"
            "</startWindowTransitionAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.duration_, 200);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityStart_, 1);
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityEnd_, 0);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigStartingWindowAnimation03
 * @tc.desc: call ConfigStartingWindowAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigStartingWindowAnimation03, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<startWindowTransitionAnimation enable=\"false\">"
                "<timing>"
                    "<duration>aaa</duration>"
                    "<curve name=\"linear\"></curve>"
                "</timing>"
                "<opacityStart>aaa</opacityStart>"
                "<opacityEnd>aaa</opacityEnd>"
            "</startWindowTransitionAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigStartingWindowAnimation04
 * @tc.desc: call ConfigStartingWindowAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigStartingWindowAnimation04, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<startWindowTransitionAnimation enable=\"false\">"
                "<timing>"
                    "<duration>200 200</duration>"
                    "<curve name=\"linear\"></curve>"
                "</timing>"
                "<opacityStart>1 1</opacityStart>"
                "<opacityEnd>0 1</opacityEnd>"
            "</startWindowTransitionAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigStartingWindowAnimation05
 * @tc.desc: call ConfigStartingWindowAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigStartingWindowAnimation05, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<startWindowTransitionAnimation enable=\"false\">"
                "<timing>"
                    "<duration>aaa aaa</duration>"
                    "<curve name=\"linear\"></curve>"
                "</timing>"
                "<opacityStart>a a</opacityStart>"
                "<opacityEnd>a a</opacityEnd>"
            "</startWindowTransitionAnimation>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
    delete sceneSessionManager;
}

/**
 * @tc.name: ConfigSnapshotScale01
 * @tc.desc: call ConfigSnapshotScale and check the snapshotScale_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigSnapshotScale01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<snapshotScale>0.7</snapshotScale>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigSnapshotScale();
    ASSERT_EQ(sceneSessionManager->snapshotScale_, static_cast<float>(0.7));
}

/**
 * @tc.name: ConfigSnapshotScale02
 * @tc.desc: call ConfigSnapshotScale and check the snapshotScale_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigSnapshotScale02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<snapshotScale>0.7 0.7</snapshotScale>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigSnapshotScale();
    ASSERT_EQ(sceneSessionManager->snapshotScale_, 0.5);
}

/**
 * @tc.name: ConfigSnapshotScale03
 * @tc.desc: call ConfigSnapshotScale and check the snapshotScale_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigSnapshotScale03, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<snapshotScale>aaa</snapshotScale>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigSnapshotScale();
    ASSERT_EQ(sceneSessionManager->snapshotScale_, 0.5);
}

/**
 * @tc.name: ConfigSnapshotScale04
 * @tc.desc: call ConfigSnapshotScale and check the snapshotScale_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigSnapshotScale04, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<snapshotScale>-0.1</snapshotScale>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigSnapshotScale();
    ASSERT_EQ(sceneSessionManager->snapshotScale_, 0.5);
}

/**
 * @tc.name: ConfigSnapshotScale05
 * @tc.desc: call ConfigSnapshotScale and check the snapshotScale_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigSnapshotScale05, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<snapshotScale>1.5</snapshotScale>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigSnapshotScale();
    ASSERT_EQ(sceneSessionManager->snapshotScale_, 0.5);
}

/**
 * @tc.name: DumpSessionAll
 * @tc.desc: ScreenSesionManager dump all session info
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, DumpSessionAll, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest";
    sessionInfo.abilityName_ = "DumpSessionAll";
    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    std::vector<std::string> infos;
    WSError result = ssm_->DumpSessionAll(infos);
    ASSERT_EQ(WSError::WS_OK, result);
    ASSERT_FALSE(infos.empty());
}

/**
 * @tc.name: DumpSessionWithId
 * @tc.desc: ScreenSesionManager dump session with id
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, DumpSessionWithId, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    std::vector<std::string> infos;
    WSError result = ssm_->DumpSessionWithId(sceneSession->GetPersistentId(), infos);
    ASSERT_EQ(WSError::WS_OK, result);
    ASSERT_FALSE(infos.empty());
}

/**
 * @tc.name: Init
 * @tc.desc: SceneSesionManager init
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, Init, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->Init();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: LoadWindowSceneXml
 * @tc.desc: SceneSesionManager load window scene xml
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, LoadWindowSceneXml, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->LoadWindowSceneXml();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigWindowSceneXml
 * @tc.desc: SceneSesionManager config window scene xml run
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSceneXml, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetSessionContinueState
 * @tc.desc: SceneSesionManager set session continue state
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetSessionContinueState, Function | SmallTest | Level3)
{
    MessageParcel *data = new MessageParcel();
    sptr <IRemoteObject> token = data->ReadRemoteObject();
    auto continueState = static_cast<ContinueState>(data->ReadInt32());
    WSError result02 = ssm_->SetSessionContinueState(nullptr, continueState);
    WSError result01 = ssm_->SetSessionContinueState(token, continueState);
    ASSERT_EQ(result02, WSError::WS_ERROR_INVALID_PARAM);
    ASSERT_EQ(result01, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: SceneSesionManager config decor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem* item = new WindowSceneConfig::ConfigItem;
    int ret = 0;
    ssm_->ConfigDecor(*item);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigWindowEffect
 * @tc.desc: SceneSesionManager config window effect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem* item = new WindowSceneConfig::ConfigItem;
    int ret = 0;
    ssm_->ConfigWindowEffect(*item);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigAppWindowCornerRadius
 * @tc.desc: SceneSesionManager config app window coener radius
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigAppWindowCornerRadius, Function | SmallTest | Level3)
{
    float out = 0.0f;
    WindowSceneConfig::ConfigItem item01;
    std::string str = "defaultCornerRadiusM";
    item01.SetValue(str);
    bool result01 = ssm_->ConfigAppWindowCornerRadius(item01, out);
    ASSERT_EQ(result01, true);

    WindowSceneConfig::ConfigItem item02;
    item02.SetValue(new string("defaultCornerRadiusS"));
    bool result02 = ssm_->ConfigAppWindowCornerRadius(item02, out);
    ASSERT_EQ(result02, false);
    ASSERT_EQ(out, 12.0f);
}

/**
 * @tc.name: ConfigAppWindowShadow
 * @tc.desc: SceneSesionManager config app window shadow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigAppWindowShadow, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem item;
    WindowSceneConfig::ConfigItem shadowConfig;
    WindowShadowConfig outShadow;
    std::vector<float> floatTest = {0.0f, 0.1f, 0.2f, 0.3f};
    bool result01 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result01, true);

    item.SetValue(floatTest);
    shadowConfig.SetValue({{"radius", item}});
    bool result02 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result02, false);
    shadowConfig.SetValue({{"alpha", item}});
    bool result03 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result03, false);
    shadowConfig.SetValue({{"sffsetY", item}});
    bool result04 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result04, true);
    shadowConfig.SetValue({{"sffsetX", item}});
    bool result05 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result05, true);

    item.SetValue(new std::string("color"));
    shadowConfig.SetValue({{"color", item}});
    bool result06 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result06, true);
}

/**
 * @tc.name: ConfigKeyboardAnimation
 * @tc.desc: SceneSesionManager config keyboard animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigKeyboardAnimation, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem animationConfig;
    WindowSceneConfig::ConfigItem itemCurve;
    WindowSceneConfig::ConfigItem itemDurationIn;
    WindowSceneConfig::ConfigItem itemDurationOut;
    std::vector<int> curve = {39};
    std::vector<int> durationIn = {39};
    std::vector<int> durationOut = {39};

    itemCurve.SetValue(curve);
    itemCurve.SetValue({{"curve", itemCurve}});
    itemDurationIn.SetValue(durationIn);
    itemDurationIn.SetValue({{"durationIn", itemDurationIn}});
    itemDurationOut.SetValue(durationOut);
    itemDurationOut.SetValue({{"durationOut", itemDurationOut}});
    animationConfig.SetValue({{"timing", itemCurve}, {"timing", itemDurationIn}, {"timing", itemDurationOut}});
    int ret = 0;
    ssm_->ConfigKeyboardAnimation(animationConfig);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigWindowAnimation
 * @tc.desc: SceneSesionManager config window animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem windowAnimationConfig;
    WindowSceneConfig::ConfigItem item;
    std::vector<float> opacity = {0.1f};
    std::vector<float> translate = {0.1f, 0.2f};
    std::vector<float> rotation = {0.1f, 0.2f, 0.3f, 0.4f};
    std::vector<float> scale = {0.1f, 0.2f};
    std::vector<int> duration = {39};

    item.SetValue(opacity);
    windowAnimationConfig.SetValue({{"opacity", item}});
    int ret = 0;
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(rotation);
    windowAnimationConfig.SetValue({{"rotation", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(translate);
    windowAnimationConfig.SetValue({{"translate", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(scale);
    windowAnimationConfig.SetValue({{"scale", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(duration);
    item.SetValue({{"duration", item}});
    windowAnimationConfig.SetValue({{"timing", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(duration);
    item.SetValue({{"curve", item}});
    windowAnimationConfig.SetValue({{"timing", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigStartingWindowAnimation
 * @tc.desc: SceneSesionManager config start window animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigStartingWindowAnimation, Function | SmallTest | Level3)
{
    std::vector<float> midFloat = {0.1f};
    std::vector<int> midInt = {1};
    WindowSceneConfig::ConfigItem middleFloat;
    middleFloat.SetValue(midFloat);
    WindowSceneConfig::ConfigItem middleInt;
    middleInt.SetValue(midInt);
    WindowSceneConfig::ConfigItem curve;
    curve.SetValue(midFloat);
    curve.SetValue({{"curve", curve}});
    WindowSceneConfig::ConfigItem enableConfigItem;
    enableConfigItem.SetValue(false);
    std::map<std::string, WindowSceneConfig::ConfigItem> midMap = {{"duration", middleInt}, {"curve", curve}};
    WindowSceneConfig::ConfigItem timing;
    timing.SetValue(midMap);
    std::map<std::string, WindowSceneConfig::ConfigItem> middleMap = {{"enable", enableConfigItem},
        {"timing", timing}, {"opacityStart", middleFloat}, {"opacityEnd", middleFloat}};
    WindowSceneConfig::ConfigItem configItem;
    configItem.SetValue(middleMap);
    int ret = 0;
    ssm_->ConfigStartingWindowAnimation(configItem);
    ASSERT_EQ(ret, 0);
    midMap.clear();
    middleMap.clear();
}

/**
 * @tc.name: CreateCurve
 * @tc.desc: SceneSesionManager create curve
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CreateCurve, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem curveConfig;
    std::string result01 = std::get<std::string>(ssm_->CreateCurve(curveConfig));
    ASSERT_EQ(result01, "easeOut");

    std::string value02 = "userName";
    curveConfig.SetValue(value02);
    curveConfig.SetValue({{"name", curveConfig}});
    std::string result02 = std::get<std::string>(ssm_->CreateCurve(curveConfig));
    ASSERT_EQ(result02, "easeOut");

    std::string value03 = "interactiveSpring";
    curveConfig.SetValue(value03);
    curveConfig.SetValue({{"name", curveConfig}});
    std::string result03 = std::get<std::string>(ssm_->CreateCurve(curveConfig));
    ASSERT_EQ(result03, "easeOut");

    std::string value04 = "cubic";
    curveConfig.SetValue(value04);
    curveConfig.SetValue({{"name", curveConfig}});
    std::string result04 = std::get<std::string>(ssm_->CreateCurve(curveConfig));
    ASSERT_EQ(result04, "easeOut");
}

/**
 * @tc.name: SetRootSceneContext
 * @tc.desc: SceneSesionManager set root scene context
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetRootSceneContext, Function | SmallTest | Level3)
{
    int ret = 0;
    std::weak_ptr<AbilityRuntime::Context> contextWeakPtr;
    ssm_->SetRootSceneContext(contextWeakPtr);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetRootSceneSession
 * @tc.desc: SceneSesionManager get root scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetRootSceneSession, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->GetRootSceneSession();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetSceneSession
 * @tc.desc: SceneSesionManager get scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSession, Function | SmallTest | Level3)
{
    int32_t persistentId = 65535;
    ASSERT_EQ(ssm_->GetSceneSession(persistentId), nullptr);
}

/**
 * @tc.name: GetSceneSessionByName
 * @tc.desc: SceneSesionManager get scene session by name
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSessionByName, Function | SmallTest | Level3)
{
    std::string bundleName = "movie";
    std::string moduleName = "button";
    std::string abilityName = "userAccess";
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName, moduleName, abilityName, 0), nullptr);
}

/**
 * @tc.name: GetSceneSessionVectorByType
 * @tc.desc: SceneSesionManager get scene session vector by type
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSessionVectorByType, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RegisterInputMethodShownFunc
 * @tc.desc: SceneSesionManager register input method show func
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterInputMethodShownFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    int ret = 0;
    ssm_->RegisterInputMethodShownFunc(nullptr);
    ASSERT_EQ(ret, 0);
    ssm_->RegisterInputMethodShownFunc(sceneSession);
    ASSERT_EQ(ret, 0);
    delete sceneSession;
}

/**
 * @tc.name: OnInputMethodShown
 * @tc.desc: SceneSesionManager on input method shown
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, OnInputMethodShown, Function | SmallTest | Level3)
{
    int32_t persistentId = 65535;
    int ret = 0;
    ssm_->OnInputMethodShown(persistentId);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RegisterInputMethodHideFunc
 * @tc.desc: SceneSesionManager register input method hide func
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterInputMethodHideFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    int ret = 0;
    ssm_->RegisterInputMethodHideFunc(nullptr);
    ASSERT_EQ(ret, 0);
    ssm_->RegisterInputMethodHideFunc(sceneSession);
    ASSERT_EQ(ret, 0);
    delete sceneSession;
}

/**
 * @tc.name: SetAbilitySessionInfo
 * @tc.desc: SceneSesionManager set ability session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetAbilitySessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<OHOS::AAFwk::SessionInfo> ret = ssm_->SetAbilitySessionInfo(scensession);
    OHOS::AppExecFwk::ElementName retElementName = ret->want.GetElement();
    ASSERT_EQ(retElementName.GetAbilityName(), info.abilityName_);
    ASSERT_EQ(retElementName.GetBundleName(), info.bundleName_);
    delete scensession;
}

/**
 * @tc.name: PrepareTerminate
 * @tc.desc: SceneSesionManager prepare terminate
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, PrepareTerminate, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    bool isPrepareTerminate = false;
    ASSERT_EQ(WSError::WS_OK, ssm_->PrepareTerminate(persistentId, isPrepareTerminate));
}

/**
 * @tc.name: RequestSceneSessionBackground
 * @tc.desc: SceneSesionManager request scene session background
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionBackground, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(scensession, isDelegator));
    delete scensession;
}

/**
 * @tc.name: DestroyDialogWithMainWindow
 * @tc.desc: SceneSesionManager destroy dialog with main window
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroyDialogWithMainWindow, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->DestroyDialogWithMainWindow(scensession));
    delete scensession;
}

/**
 * @tc.name: RequestSceneSessionDestruction
 * @tc.desc: SceneSesionManager request scene session destruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionDestruction, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionDestruction(scensession, needRemoveSession));
    delete scensession;
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: SceneSesionManager notify scene session interactive status
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyForegroundInteractiveStatus, Function | SmallTest | Level3)
{
    bool interactive = false;
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    int ret = 0;
    ssm_->NotifyForegroundInteractiveStatus(scensession, interactive);
    ASSERT_EQ(ret, 0);
    delete scensession;
}

/**
 * @tc.name: AddClientDeathRecipient
 * @tc.desc: SceneSesionManager add client death recipient
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AddClientDeathRecipient, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    int ret = 0;
    ssm_->AddClientDeathRecipient(nullptr, scensession);
    ASSERT_EQ(ret, 0);
    delete scensession;
}

/**
 * @tc.name: DestroySpecificSession
 * @tc.desc: SceneSesionManager destroy specific session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroySpecificSession, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> remoteObject = nullptr;
    int ret = 0;
    ssm_->DestroySpecificSession(remoteObject);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetCreateSystemSessionListener
 * @tc.desc: SceneSesionManager set create specific session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetCreateSystemSessionListener, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->SetCreateSystemSessionListener(nullptr);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetGestureNavigationEnabledChangeListener
 * @tc.desc: SceneSesionManager set gesture navigation enabled change listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetGestureNavigationEnabledChangeListener, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->SetGestureNavigationEnabledChangeListener(nullptr);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: OnOutsideDownEvent
 * @tc.desc: SceneSesionManager on out side down event
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, OnOutsideDownEvent, Function | SmallTest | Level3)
{
    int32_t x = 32;
    int32_t y = 32;
    int ret = 0;
    ssm_->OnOutsideDownEvent(x, y);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifySessionTouchOutside
 * @tc.desc: SceneSesionManager notify session touch outside
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifySessionTouchOutside, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->NotifySessionTouchOutside(INVALID_SESSION_ID);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetOutsideDownEventListener
 * @tc.desc: SceneSesionManager set outside down event listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetOutsideDownEventListener, Function | SmallTest | Level3)
{
    ProcessOutsideDownEventFunc func = [](int32_t x, int32_t y) {
        ssm_->OnOutsideDownEvent(x, y);
    };
    int ret = 0;
    ssm_->SetOutsideDownEventListener(func);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSession
 * @tc.desc: SceneSesionManager destroy and disconnect specific session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroyAndDisconnectSpecificSession, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    WSError result = ssm_->DestroyAndDisconnectSpecificSession(persistentId);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: GetWindowSceneConfig
 * @tc.desc: SceneSesionManager get window scene config
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetWindowSceneConfig, Function | SmallTest | Level3)
{
    int ret = 0;
    AppWindowSceneConfig appWindowSceneConfig_ = ssm_->GetWindowSceneConfig();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ProcessBackEvent
 * @tc.desc: SceneSesionManager process back event
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, ProcessBackEvent, Function | SmallTest | Level3)
{
    WSError result = ssm_->ProcessBackEvent();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: CleanUserMap
 * @tc.desc: SceneSesionManager clear user map
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CleanUserMap, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->CleanUserMap();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SwitchUser
 * @tc.desc: SceneSesionManager switch user
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SwitchUser, Function | SmallTest | Level3)
{
    int32_t oldUserId = 10086;
    int32_t newUserId = 10086;
    std::string fileDir;
    WSError result01 = ssm_->SwitchUser(oldUserId, newUserId, fileDir);
    ASSERT_EQ(result01, WSError::WS_DO_NOTHING);
    fileDir = "newFileDir";
    oldUserId = ssm_->GetCurrentUserId();
    WSError result02 = ssm_->SwitchUser(oldUserId, newUserId, fileDir);
    ASSERT_EQ(result02, WSError::WS_OK);
}

/**
 * @tc.name: PreHandleCollaborator
 * @tc.desc: SceneSesionManager prehandle collaborator
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, PreHandleCollaborator, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession = nullptr;
    ssm_->PreHandleCollaborator(scensession);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    scensession = nullptr;
    AppExecFwk::ApplicationInfo applicationInfo_;
    applicationInfo_.codePath = std::to_string(CollaboratorType::RESERVE_TYPE);
    AppExecFwk::AbilityInfo abilityInfo_;
    abilityInfo_.applicationInfo = applicationInfo_;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    scensession = nullptr;
    applicationInfo_.codePath = std::to_string(CollaboratorType::OTHERS_TYPE);
    abilityInfo_.applicationInfo = applicationInfo_;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    EXPECT_EQ(scensession->GetSessionInfo().want, nullptr);
    scensession = nullptr;
    info.want = std::make_shared<AAFwk::Want>();
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    ASSERT_NE(scensession->GetSessionInfo().want, nullptr);
    delete scensession;
}

/**
 * @tc.name: CheckCollaboratorType
 * @tc.desc: SceneSesionManager check collborator type
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CheckCollaboratorType, Function | SmallTest | Level3)
{
    int32_t type_ = CollaboratorType::RESERVE_TYPE;
    EXPECT_TRUE(ssm_->CheckCollaboratorType(type_));
    type_ = CollaboratorType::OTHERS_TYPE;
    EXPECT_TRUE(ssm_->CheckCollaboratorType(type_));
    type_ = CollaboratorType::DEFAULT_TYPE;
    ASSERT_FALSE(ssm_->CheckCollaboratorType(type_));
}

/**
 * @tc.name: NotifyUpdateSessionInfo
 * @tc.desc: SceneSesionManager notify update session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyUpdateSessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifyUpdateSessionInfo(nullptr);
    ASSERT_EQ(scensession->GetSessionInfo().want, nullptr);
    ssm_->NotifyUpdateSessionInfo(scensession);
    int32_t collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    int32_t persistentId_ = 10086;
    ssm_->NotifyMoveSessionToForeground(collaboratorType_, persistentId_);
    ssm_->NotifyClearSession(collaboratorType_, persistentId_);
    delete scensession;
}

/**
 * @tc.name: NotifySessionCreate
 * @tc.desc: SceneSesionManager notify session create
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifySessionCreate, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionCreate(nullptr, info);
    EXPECT_EQ(info.want, nullptr);
    ssm_->NotifySessionCreate(scensession, info);
    info.want = std::make_shared<AAFwk::Want>();
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionCreate(scensession, info);
    EXPECT_NE(info.want, nullptr);
    AppExecFwk::AbilityInfo aInfo_;
    sptr<AAFwk::SessionInfo> abilitySessionInfo_ = new AAFwk::SessionInfo();
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo_ = std::make_shared<AppExecFwk::AbilityInfo>(aInfo_);
    int32_t collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    ssm_->NotifyLoadAbility(collaboratorType_, abilitySessionInfo_, abilityInfo_);
    delete scensession;
}

/**
 * @tc.name: QueryAbilityInfoFromBMS
 * @tc.desc: SceneSesionManager QueryAbilityInfoFromBMS NotifyStartAbility
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, QueryAbilityInfoFromBMS, Function | SmallTest | Level3)
{
    const int32_t uId_ = 32;
    SessionInfo sessionInfo_;
    sessionInfo_.bundleName_ = "BundleName";
    sessionInfo_.abilityName_ = "AbilityName";
    sessionInfo_.moduleName_ = "ModuleName";
    AppExecFwk::AbilityInfo abilityInfo_;
    int32_t collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    ssm_->QueryAbilityInfoFromBMS(uId_, sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
    EXPECT_EQ(sessionInfo_.want, nullptr);
    ssm_->Init();
    ssm_->QueryAbilityInfoFromBMS(uId_, sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
    ssm_->NotifyStartAbility(collaboratorType_, sessionInfo_);
    sessionInfo_.want = std::make_shared<AAFwk::Want>();
    collaboratorType_ = CollaboratorType::OTHERS_TYPE;
    ssm_->NotifyStartAbility(collaboratorType_, sessionInfo_);
    ASSERT_NE(sessionInfo_.want, nullptr);
}

/**
 * @tc.name: IsSessionClearable
 * @tc.desc: SceneSesionManager is session clearable
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, IsSessionClearable, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(nullptr));
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    AppExecFwk::AbilityInfo abilityInfo_;
    abilityInfo_.excludeFromMissions = true;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    abilityInfo_.excludeFromMissions = false;
    abilityInfo_.unclearableMission = true;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    abilityInfo_.unclearableMission = false;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    info.lockedState = true;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_TRUE(ssm_->IsSessionClearable(scensession));
    info.lockedState = false;
    info.isSystem_ = true;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    info.isSystem_ = false;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_TRUE(ssm_->IsSessionClearable(scensession));
    delete scensession;
}

/**
 * @tc.name: UpdateSessionProperty
 * @tc.desc: SceneSesionManager update property
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateSessionProperty, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE;
    WMError result = ssm_->UpdateSessionProperty(property, action);
    ASSERT_EQ(result, WMError::WM_OK);
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->UpdatePropertyRaiseEnabled(property, scensession);
    delete scensession;
    delete property;
}

/**
 * @tc.name: HandleUpdateProperty01
 * @tc.desc: SceneSesionManager handle update property
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleUpdateProperty01, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSPropertyChangeAction action;
    action = WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    WMError result = ssm_->UpdateSessionProperty(property, action);
    EXPECT_EQ(result, WMError::WM_OK);
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    delete scensession;
    delete property;
}

/**
 * @tc.name: HandleUpdateProperty02
 * @tc.desc: SceneSesionManager handle update property
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleUpdateProperty02, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSPropertyChangeAction action;
    action = WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_FLAGS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_MODE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED;
    ssm_->HandleUpdateProperty(property, action, scensession);
    WMError result = ssm_->UpdateSessionProperty(property, action);
    EXPECT_EQ(result, WMError::WM_OK);
    action = WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    delete scensession;
    delete property;
}

/**
 * @tc.name: HandleTurnScreenOn
 * @tc.desc: SceneSesionManager handle turn screen on and keep screen on
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleTurnScreenOn, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE;
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ssm_->HandleTurnScreenOn(scensession);
    bool requireLock = true;
    ssm_->HandleKeepScreenOn(scensession, requireLock);
    requireLock = false;
    ssm_->HandleKeepScreenOn(scensession, requireLock);
    WMError result = ssm_->UpdateSessionProperty(property, action);
    ASSERT_EQ(result, WMError::WM_OK);
    delete scensession;
    delete property;
}

/**
 * @tc.name: UpdateHideNonSystemFloatingWindows
 * @tc.desc: SceneSesionManager update hide non system floating windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateHideNonSystemFloatingWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    ssm_->UpdateForceHideState(scensession, property, true);
    property = new WindowSessionProperty();
    ssm_->UpdateHideNonSystemFloatingWindows(property, scensession);
    property->SetHideNonSystemFloatingWindows(true);
    ssm_->UpdateForceHideState(scensession, property, true);
    ssm_->UpdateForceHideState(scensession, property, false);
    property->SetHideNonSystemFloatingWindows(false);
    property->SetFloatingWindowAppType(true);
    ssm_->UpdateForceHideState(scensession, property, true);
    ssm_->UpdateForceHideState(scensession, property, false);
    uint32_t result = property->GetModeSupportInfo();
    ASSERT_EQ(result, WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);
    delete scensession;
    delete property;
}

/**
 * @tc.name: UpdateBrightness
 * @tc.desc: SceneSesionManager update brightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateBrightness, Function | SmallTest | Level3)
{
    int32_t persistentId_ = 10086;
    WSError result01 = ssm_->UpdateBrightness(persistentId_);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: SetDisplayBrightness
 * @tc.desc: SceneSesionManager set display brightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetDisplayBrightness, Function | SmallTest | Level3)
{
    float brightness = 2.0f;
    float result01 = ssm_->GetDisplayBrightness();
    EXPECT_EQ(result01, UNDEFINED_BRIGHTNESS);
    ssm_->SetDisplayBrightness(brightness);
    float result02 = ssm_->GetDisplayBrightness();
    ASSERT_EQ(result02, 2.0f);
}

/**
 * @tc.name: SetGestureNavigaionEnabled02
 * @tc.desc: SceneSesionManager set gesture navigaion enable
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetGestureNavigaionEnabled02, Function | SmallTest | Level3)
{
    bool enable = true;
    WMError result01 = ssm_->SetGestureNavigaionEnabled(enable);
    EXPECT_EQ(result01, WMError::WM_DO_NOTHING);
    ProcessGestureNavigationEnabledChangeFunc funcGesture_ = SceneSessionManagerTest::callbackFunc_;
    ssm_->SetGestureNavigationEnabledChangeListener(funcGesture_);
    WMError result02 = ssm_->SetGestureNavigaionEnabled(enable);
    EXPECT_EQ(result02, WMError::WM_OK);
    ProcessStatusBarEnabledChangeFunc funcStatus_ = ProcessStatusBarEnabledChangeFuncTest;
    ssm_->SetStatusBarEnabledChangeListener(funcStatus_);
    WMError result03 = ssm_->SetGestureNavigaionEnabled(enable);
    ASSERT_EQ(result03, WMError::WM_OK);
}

/**
 * @tc.name: SetFocusedSession
 * @tc.desc: SceneSesionManager set focused session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetFocusedSession, Function | SmallTest | Level3)
{
    int32_t focusedSession_ = ssm_->GetFocusedSession();
    EXPECT_EQ(focusedSession_, INVALID_SESSION_ID);
    int32_t persistendId_ = INVALID_SESSION_ID;
    WSError result01 = ssm_->SetFocusedSession(persistendId_);
    EXPECT_EQ(result01, WSError::WS_DO_NOTHING);
    persistendId_ = 10086;
    WSError result02 = ssm_->SetFocusedSession(persistendId_);
    EXPECT_EQ(result02, WSError::WS_OK);
    ASSERT_EQ(ssm_->GetFocusedSession(), 10086);
}

/**
 * @tc.name: RequestFocusStatus
 * @tc.desc: SceneSesionManager request focus status
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestFocusStatus, Function | SmallTest | Level3)
{
    int32_t focusedSession_ = ssm_->GetFocusedSession();
    EXPECT_EQ(focusedSession_, INVALID_SESSION_ID);
    int32_t persistentId_ = INVALID_SESSION_ID;
    WMError result01 = ssm_->RequestFocusStatus(persistentId_, true);
    EXPECT_EQ(result01, WMError::WM_OK);
    persistentId_ = 10000;
    WMError result02 = ssm_->RequestFocusStatus(persistentId_, true);
    EXPECT_EQ(result02, WMError::WM_OK);
    WMError result03 = ssm_->RequestFocusStatus(persistentId_, false);
    EXPECT_EQ(result03, WMError::WM_OK);
}

/**
 * @tc.name: RaiseWindowToTop
 * @tc.desc: SceneSesionManager raise window to top
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RaiseWindowToTop, Function | SmallTest | Level3)
{
    int32_t focusedSession_ = ssm_->GetFocusedSession();
    EXPECT_EQ(focusedSession_, INVALID_SESSION_ID);
    int32_t persistentId_ = INVALID_SESSION_ID;
    WSError result01 = ssm_->RaiseWindowToTop(persistentId_);
    EXPECT_EQ(result01, WSError::WS_OK);
    persistentId_ = 10000;
    WSError result02 = ssm_->RaiseWindowToTop(persistentId_);
    EXPECT_EQ(result02, WSError::WS_OK);
    WSError result03 = ssm_->RaiseWindowToTop(persistentId_);
    EXPECT_EQ(result03, WSError::WS_OK);
}

/**
 * @tc.name: RegisterSessionExceptionFunc
 * @tc.desc: SceneSesionManager register session expection func
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterSessionExceptionFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession = nullptr;
    ssm_->RegisterSessionExceptionFunc(scensession);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->RegisterSessionExceptionFunc(scensession);
    bool result01 = ssm_->IsSessionVisible(scensession);
    EXPECT_FALSE(result01);
    scensession->UpdateNativeVisibility(true);
    bool result02 = ssm_->IsSessionVisible(scensession);
    ASSERT_TRUE(result02);
    delete scensession;
}

/**
 * @tc.name: DumpSessionInfo
 * @tc.desc: SceneSesionManager dump session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DumpSessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    std::ostringstream oss;
    std::string dumpInfo;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    info.isSystem_ = false;
    sptr<SceneSession> scensession = nullptr;
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpAllAppSessionInfo(oss);
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: SceneSesionManager dump session element info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DumpSessionElementInfo, Function | SmallTest | Level3)
{
    DumpRootSceneElementInfoFunc func_ = DumpRootSceneElementInfoFuncTest;
    ssm_->SetDumpRootSceneElementInfoListener(func_);
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    info.isSystem_ = false;
    std::string strId = "10086";
    sptr<SceneSession> scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    std::vector<std::string> params_;
    std::string dumpInfo_;
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    WSError result01 = ssm_->GetSpecifiedSessionDumpInfo(dumpInfo_, params_, strId);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: NotifyDumpInfoResult
 * @tc.desc: SceneSesionManager notify dump info result
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyDumpInfoResult, Function | SmallTest | Level3)
{
    std::vector<std::string> info = {"std::", "vector", "<std::string>"};
    ssm_->NotifyDumpInfoResult(info);
    std::vector<std::string> params = {"-a"};
    std::string dumpInfo = "";
    WSError result01 = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result01, WSError::WS_OK);
    params.clear();
    params.push_back("-w");
    params.push_back("23456");
    WSError result02 = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result02, WSError::WS_ERROR_INVALID_PARAM);
    params.clear();
    WSError result03 = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result03, WSError::WS_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: UpdateFocus
 * @tc.desc: SceneSesionManager update focus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateFocus, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    SessionInfo info;
    info.bundleName_ = "bundleName_";
    bool isFocused = true;
    WSError result = ssm_->UpdateFocus(persistentId, isFocused);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateWindowMode
 * @tc.desc: SceneSesionManager update window mode
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateWindowMode, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    int32_t windowMode = 3;
    WSError result = ssm_->UpdateWindowMode(persistentId, windowMode);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_WINDOW);
    std::function<void(int32_t persistentId, WindowUpdateType type)> func = WindowChangedFuncTest;
    ssm_->RegisterWindowChanged(func);
}

/**
 * @tc.name: SetScreenLocked && IsScreenLocked
 * @tc.desc: SceneSesionManager update screen locked state
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, IsScreenLocked, Function | SmallTest | Level3)
{
    ssm_->SetScreenLocked(true);
    EXPECT_TRUE(ssm_->IsScreenLocked());
    ssm_->SetScreenLocked(false);
    EXPECT_FALSE(ssm_->IsScreenLocked());
}

/**
 * @tc.name: UpdatePrivateStateAndNotify
 * @tc.desc: SceneSesionManager update private state and notify
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdatePrivateStateAndNotify, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sptr<SceneSession> scensession = nullptr;
    ssm_->RegisterSessionStateChangeNotifyManagerFunc(scensession);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->RegisterSessionStateChangeNotifyManagerFunc(scensession);
    ssm_->UpdatePrivateStateAndNotify(persistentId);
    int result = ssm_->GetSceneSessionPrivacyModeCount();
    EXPECT_EQ(result, 0);
    delete scensession;
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SceneSesionManager set window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "bundleName";
    uint32_t flags = 1;
    sptr<SceneSession> scensession = nullptr;
    WSError result01 = ssm_->SetWindowFlags(scensession, flags);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result02 = ssm_->SetWindowFlags(scensession, flags);
    EXPECT_EQ(result02, WSError::WS_ERROR_NULLPTR);
    WSError result03 = ssm_->SetWindowFlags(scensession, flags);
    ASSERT_EQ(result03, WSError::WS_OK);
    delete scensession;
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: SceneSesionManager notify water mark flag changed result
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyWaterMarkFlagChangedResult, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    ssm_->NotifyCompleteFirstFrameDrawing(persistentId);
    bool hasWaterMark = true;
    AppExecFwk::AbilityInfo abilityInfo;
    WSError result01 = ssm_->NotifyWaterMarkFlagChangedResult(hasWaterMark);
    EXPECT_EQ(result01, WSError::WS_OK);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    ssm_->ProcessPreload(abilityInfo);
}

/**
 * @tc.name: IsValidSessionIds
 * @tc.desc: SceneSesionManager is valid session id
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, IsValidSessionIds, Function | SmallTest | Level3)
{
    std::vector<int32_t> sessionIds = {0, 1, 2, 3, 4, 5, 24, 10086};
    std::vector<bool> results = {};
    WSError result = ssm_->IsValidSessionIds(sessionIds, results);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UnRegisterSessionListener
 * @tc.desc: SceneSesionManager un register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UnRegisterSessionListener, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError result = ssm_->UnRegisterSessionListener(listener);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    delete listener;
}

/**
 * @tc.name: GetSessionInfos
 * @tc.desc: SceneSesionManager get session infos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSessionInfos, Function | SmallTest | Level3)
{
    std::string deviceId = "1245";
    int32_t numMax = 1024;
    AAFwk::MissionInfo infoFrist;
    infoFrist.label = "fristBundleName";
    AAFwk::MissionInfo infoSecond;
    infoSecond.label = "secondBundleName";
    std::vector<SessionInfoBean> sessionInfos = {infoFrist, infoSecond};
    WSError result = ssm_->GetSessionInfos(deviceId, numMax, sessionInfos);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    int32_t persistentId = 24;
    SessionInfoBean sessionInfo;
    int result01 = ssm_->GetRemoteSessionInfo(deviceId, persistentId, sessionInfo);
    ASSERT_NE(result01, ERR_OK);
}

/**
 * @tc.name: CheckIsRemote
 * @tc.desc: SceneSesionManager check is remote
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CheckIsRemote, Function | SmallTest | Level3)
{
    std::string deviceId;
    bool result = ssm_->CheckIsRemote(deviceId);
    EXPECT_FALSE(result);
    deviceId.assign("deviceId");
    result = ssm_->CheckIsRemote(deviceId);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AnonymizeDeviceId
 * @tc.desc: SceneSesionManager anonymize deviceId
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AnonymizeDeviceId, Function | SmallTest | Level3)
{
    std::string deviceId;
    std::string result(ssm_->AnonymizeDeviceId(deviceId));
    EXPECT_EQ(result, EMPTY_DEVICE_ID);
    deviceId.assign("100964857");
    std::string result01 = "100964******";
    ASSERT_EQ(ssm_->AnonymizeDeviceId(deviceId), result01);
}

/**
 * @tc.name: TerminateSessionNew
 * @tc.desc: SceneSesionManager terminate session new
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, TerminateSessionNew, Function | SmallTest | Level3)
{
    sptr<AAFwk::SessionInfo> info = nullptr;
    bool needStartCaller = true;
    WSError result01 = ssm_->TerminateSessionNew(info, needStartCaller);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, result01);
    info = new (std::nothrow) AAFwk::SessionInfo();
    WSError result02 = ssm_->TerminateSessionNew(info, needStartCaller);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, result02);
    delete info;
}

/**
 * @tc.name: RegisterSessionListener01
 * @tc.desc: SceneSesionManager register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterSessionListener01, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError result = ssm_->RegisterSessionListener(listener);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    delete listener;
}

/**
 * @tc.name: RegisterSessionListener02
 * @tc.desc: SceneSesionManager register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterSessionListener02, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionChangeListener> sessionListener = nullptr;
    WSError result01 = ssm_->RegisterSessionListener(sessionListener);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_SESSION_LISTENER);
    ssm_->UnregisterSessionListener();
    delete sessionListener;
}

/**
 * @tc.name: RequestSceneSessionByCall
 * @tc.desc: SceneSesionManager request scene session by call
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionByCall, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    WSError result01 = ssm_->RequestSceneSessionByCall(nullptr);
    EXPECT_EQ(result01, WSError::WS_OK);
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result02 = ssm_->RequestSceneSessionByCall(scensession);
    ASSERT_EQ(result02, WSError::WS_OK);
    delete scensession;
}

/**
 * @tc.name: FindMainWindowWithToken
 * @tc.desc: SceneSesionManager find main window with token
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, FindMainWindowWithToken, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> targetToken = nullptr;
    sptr<SceneSession> result = ssm_->FindMainWindowWithToken(targetToken);
    EXPECT_EQ(result, nullptr);
    uint64_t persistentId = 1423;
    WSError result01 = ssm_->BindDialogSessionTarget(persistentId, targetToken);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: MoveSessionsToBackground
 * @tc.desc: SceneSesionManager move sessions to background
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, MoveSessionsToBackground, Function | SmallTest | Level3)
{
    int32_t type = CollaboratorType::RESERVE_TYPE;
    WSError result01 = ssm_->UnregisterIAbilityManagerCollaborator(type);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PERMISSION);
    std::vector<std::int32_t> sessionIds = {1, 2, 3, 15, 1423};
    std::vector<int32_t> res = {1, 2, 3, 15, 1423};
    WSError result03 = ssm_->MoveSessionsToBackground(sessionIds, res);
    ASSERT_EQ(result03, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: MoveSessionsToForeground
 * @tc.desc: SceneSesionManager move sessions to foreground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, MoveSessionsToForeground, Function | SmallTest | Level3)
{
    std::vector<std::int32_t> sessionIds = {1, 2, 3, 15, 1423};
    int32_t topSessionId = 1;
    WSError result = ssm_->MoveSessionsToForeground(sessionIds, topSessionId);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: UnlockSession
 * @tc.desc: SceneSesionManager unlock session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UnlockSession, Function | SmallTest | Level3)
{
    int32_t sessionId = 1;
    WSError result = ssm_->UnlockSession(sessionId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    result = ssm_->LockSession(sessionId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: UpdateImmersiveState
 * @tc.desc: test UpdateImmersiveState
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateImmersiveState, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->UpdateImmersiveState();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifyAINavigationBarShowStatus
 * @tc.desc: test NotifyAINavigationBarShowStatus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyAINavigationBarShowStatus, Function | SmallTest | Level3)
{
    bool isVisible = false;
    WSRect barArea = { 0, 0, 320, 240}; // width: 320, height: 240
    WSError result = ssm_->NotifyAINavigationBarShowStatus(isVisible, barArea);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyWindowExtensionVisibilityChange
 * @tc.desc: test NotifyWindowExtensionVisibilityChange
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyWindowExtensionVisibilityChange, Function | SmallTest | Level3)
{
    int32_t pid = 1;
    int32_t uid = 32;
    bool isVisible = false;
    WSError result = ssm_->NotifyWindowExtensionVisibilityChange(pid, uid, isVisible);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: SceneSesionManager NotifySessionForeground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifySessionForeground, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, scensession);
    uint32_t reason = 1;
    bool withAnimation = true;
    scensession->NotifySessionForeground(reason, withAnimation);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: SceneSesionManager NotifySessionForeground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifySessionBackground, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, scensession);
    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    scensession->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

/**
 * @tc.name: UpdateSessionWindowVisibilityListener
 * @tc.desc: SceneSesionManager update window visibility listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateSessionWindowVisibilityListener, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    bool haveListener = true;
    WSError result = ssm_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: GetSessionSnapshotPixelMap
 * @tc.desc: SceneSesionManager get session snapshot pixelmap
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSessionSnapshotPixelMap, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);

    int32_t persistentId = 65535;
    float scaleValue = 0.5f;
    auto pixelMap = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleValue);
    EXPECT_EQ(pixelMap, nullptr);

    persistentId = 1;
    pixelMap = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleValue);
    EXPECT_EQ(pixelMap, nullptr);
}
}
} // namespace Rosen
} // namespace OHOS

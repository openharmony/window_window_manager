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
#include <regex>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
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

    static void SetVisibleForAccessibility(sptr<SceneSession>& sceneSession);
    int32_t GetTaskCount(sptr<SceneSession>& session);
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
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest::TearDown()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
}

int32_t SceneSessionManagerTest::GetTaskCount(sptr<SceneSession>& session)
{
    std::string dumpInfo = session->handler_->GetEventRunner()->GetEventQueue()->DumpCurrentQueueSize();
    std::regex pattern("\\d+");
    std::smatch matches;
    int32_t taskNum = 0;
    while (std::regex_search(dumpInfo, matches, pattern)) {
        taskNum += std::stoi(matches.str());
        dumpInfo = matches.suffix();
    }
    return taskNum;
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
    WSError result = ssm_->SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: SetGestureNavigaionEnabled
 * @tc.desc: SceneSessionManager set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetGestureNavigaionEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(callbackFunc_, nullptr);

    WMError result00 = ssm_->SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result00, WMError::WM_DO_NOTHING);

    ssm_->SetGestureNavigationEnabledChangeListener(callbackFunc_);
    WMError result01 = ssm_->SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result01, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, true);

    WMError result02 = ssm_->SetGestureNavigaionEnabled(false);
    ASSERT_EQ(result02, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, false);

    ssm_->SetGestureNavigationEnabledChangeListener(nullptr);
    WMError result03 = ssm_->SetGestureNavigaionEnabled(true);
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
    ssm_->SetStatusBarEnabledChangeListener(nullptr);

    WMError result00 = ssm_->SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result00, WMError::WM_DO_NOTHING);

    ssm_->SetStatusBarEnabledChangeListener(statusBarEnabledCallbackFunc_);
    WMError result01 = ssm_->SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result01, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(statusBarEnabled_, true);

    WMError result02 = ssm_->SetGestureNavigaionEnabled(false);
    ASSERT_EQ(result02, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(statusBarEnabled_, false);

    ssm_->SetStatusBarEnabledChangeListener(nullptr);
    WMError result03 = ssm_->SetGestureNavigaionEnabled(true);
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

    ASSERT_EQ(WMError::WM_OK, ssm_->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, ssm_->UnregisterWindowManagerAgent(
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
    ssm_->ConfigWindowSizeLimits();
    ASSERT_EQ(ssm_->systemConfig_.miniWidthOfMainWindow_, static_cast<uint32_t>(10));
    ASSERT_EQ(ssm_->systemConfig_.miniHeightOfMainWindow_, static_cast<uint32_t>(20));
    ASSERT_EQ(ssm_->systemConfig_.miniWidthOfSubWindow_, static_cast<uint32_t>(30));
    ASSERT_EQ(ssm_->systemConfig_.miniHeightOfSubWindow_, static_cast<uint32_t>(40));
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.radius_, 0.5);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.alpha_, 0);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.radius_, 0.5);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
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
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>fullscreen</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorModeSupportInfo_,
        static_cast<uint32_t>(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN));
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_PIP);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorModeSupportInfo_,
        WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
        WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorModeSupportInfo_,
        WINDOW_MODE_SUPPORT_ALL);
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
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>102</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.defaultWindowMode_,
        static_cast<WindowMode>(static_cast<uint32_t>(102)));
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
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>1</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.defaultWindowMode_,
        static_cast<WindowMode>(static_cast<uint32_t>(1)));
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
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>1</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(SceneSession::maximizeMode_,
        static_cast<MaximizeMode>(static_cast<uint32_t>(1)));
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
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>0</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(SceneSession::maximizeMode_,
        static_cast<MaximizeMode>(static_cast<uint32_t>(0)));
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
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxFloatingWindowSize>1</maxFloatingWindowSize>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.maxFloatingWindowSize_,
        static_cast<uint32_t>(1));
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
    ssm_->ConfigWindowSceneXml();

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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.keyboardAnimationConfig_.durationIn_, static_cast<uint32_t>(500));
    ASSERT_EQ(ssm_->systemConfig_.keyboardAnimationConfig_.durationOut_, static_cast<uint32_t>(300));
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.keyboardAnimationConfig_.durationOut_, static_cast<uint32_t>(300));
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.keyboardAnimationConfig_.durationIn_, static_cast<uint32_t>(500));
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.keyboardAnimationConfig_.durationIn_, static_cast<uint32_t>(500));
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.duration_, 350);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.duration_, 350);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleX_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.scaleY_, static_cast<float>(0.7));
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.rotationZ_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.angle_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateX_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.translateY_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.opacity_, 0);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.duration_, 350);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowAnimation_.duration_, 350);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.duration_, 200);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityStart_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityEnd_, 0);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.duration_, 200);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityStart_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.opacityEnd_, 0);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
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
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.startingWindowAnimationConfig_.enabled_, false);
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
    ssm_->ConfigSnapshotScale();
    ASSERT_EQ(ssm_->snapshotScale_, static_cast<float>(0.7));
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
    ssm_->ConfigSnapshotScale();
    ASSERT_EQ(ssm_->snapshotScale_, 0.7f);
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
    ssm_->ConfigSnapshotScale();
    ASSERT_EQ(ssm_->snapshotScale_, 0.7f);
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
    ssm_->ConfigSnapshotScale();
    ASSERT_EQ(ssm_->snapshotScale_, 0.7f);
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
    ssm_->ConfigSnapshotScale();
    ASSERT_EQ(ssm_->snapshotScale_, 0.7f);
}

/**
 * @tc.name: ConfigSystemUIStatusBar01
 * @tc.desc: call ConfigSystemUIStatusBar default.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigSystemUIStatusBar01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<systemUIStatusBar>"
                "<showInLandscapeMode>1</showInLandscapeMode>"
                "<immersiveStatusBarBgColor>#4c000000</immersiveStatusBarBgColor>"
                "<immersiveStatusBarContentColor>#ffffee</immersiveStatusBarContentColor>"
            "</systemUIStatusBar>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.systemUIStatusBarConfig_.showInLandscapeMode_, 1);
    ASSERT_STREQ(sceneSessionManager->appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarBgColor_.c_str(),
        "#4c000000");
    ASSERT_STREQ(sceneSessionManager->appWindowSceneConfig_.systemUIStatusBarConfig_.
        immersiveStatusBarContentColor_.c_str(), "#ffffee");
    delete sceneSessionManager;
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
    sptr<WindowSessionProperty> windowSessionProperty = new WindowSessionProperty();
    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(sessionInfo, windowSessionProperty);
    ASSERT_EQ(nullptr, sceneSession);
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
    sptr<WindowSessionProperty> windowSessionProperty = new WindowSessionProperty();
    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(sessionInfo, windowSessionProperty);
    ASSERT_EQ(nullptr, sceneSession);
    std::vector<std::string> infos;
    WSError result = ssm_->DumpSessionWithId(windowSessionProperty->GetPersistentId(), infos);
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
    ssm_->RegisterAppListener();
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
    ssm_->ConfigWindowSceneXml();
    ssm_->SetEnableInputEvent(true);
    ssm_->SetEnableInputEvent(false);
    ASSERT_EQ(ssm_->IsInputEventEnabled(), false);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateRecoveredSessionInfo
 * @tc.desc: SceneSessionManager load window scene xml
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, UpdateRecoveredSessionInfo, Function | SmallTest | Level3)
{
    int ret = 0;
    std::vector<int32_t> recoveredPersistentIds;
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    recoveredPersistentIds.push_back(0);
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    ssm_->sceneSessionMap_.erase(0);
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
    delete data;
}

/**
 * @tc.name: SetSessionContinueState002
 * @tc.desc: SceneSesionManager set session continue state
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetSessionContinueState002, Function | SmallTest | Level3)
{
    MessageParcel *data = new MessageParcel();
    sptr<IRemoteObject> token = data->ReadRemoteObject();
    auto continueState = static_cast<ContinueState>(0);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        delete data;
        return;
    }
    sceneSession->SetSelfToken(token);
    ssm_->sceneSessionMap_.insert({1000, sceneSession});
    ssm_->SetSessionContinueState(token, continueState);
    ASSERT_NE(sceneSession, nullptr);
    delete data;
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
    delete item;
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
    delete item;
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
    ssm_->ConfigDefaultKeyboardAnimation();
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
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: check func RecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    auto result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, nullptr, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = {0, 1, 2};
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    property->SetPersistentId(1);
    result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, property, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
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
 * @tc.name: GetSceneSession002
 * @tc.desc: SceneSesionManager get scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSession002, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    ssm_->sceneSessionMap_.insert({65535, sceneSession});
    int32_t persistentId = 65535;
    ASSERT_NE(ssm_->GetSceneSession(persistentId), nullptr);
}

/**
 * @tc.name: GetSceneSessionByName
 * @tc.desc: SceneSesionManager get scene session by name
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSessionByName, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.moduleName_ = "test3";
    info.appIndex_ = 10;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    std::string bundleName1 = "test1";
    std::string moduleName1 = "test2";
    std::string abilityName1 = "test3";
    int32_t appIndex1 = 10;
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName1, moduleName1, abilityName1, appIndex1), nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    std::string bundleName2 = "test11";
    std::string moduleName2 = "test22";
    std::string abilityName2 = "test33";
    int32_t appIndex2 = 100;
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName1, moduleName1, abilityName1, appIndex1), nullptr);
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName1, moduleName2, abilityName2, appIndex2), nullptr);
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName2, moduleName1, abilityName2, appIndex2), nullptr);
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName2, moduleName2, abilityName1, appIndex2), nullptr);
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName2, moduleName2, abilityName2, appIndex1), nullptr);
    ssm_->sceneSessionMap_.erase(1);
}

/**
 * @tc.name: GetSceneSessionVectorByType
 * @tc.desc: SceneSesionManager get scene session vector by type
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSessionVectorByType, Function | SmallTest | Level3)
{
    int ret = 0;
    uint64_t displayId = -1ULL;
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE, displayId);
    displayId = 1;
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE, displayId);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.moduleName_ = "test3";
    info.appIndex_ = 10;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE, displayId);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    sceneSession->SetSessionProperty(property);
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE, displayId);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE, displayId);
    property->SetDisplayId(1);
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE, displayId);
    ssm_->sceneSessionMap_.erase(1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateParentSessionForDialog
 * @tc.desc: SceneSesionManager UpdateParentSessionForDialog
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateParentSessionForDialog, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->UpdateParentSessionForDialog(nullptr, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ssm_->UpdateParentSessionForDialog(nullptr, property);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.moduleName_ = "test3";
    info.appIndex_ = 10;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->UpdateParentSessionForDialog(sceneSession, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->UpdateParentSessionForDialog(sceneSession, property);
    property->SetParentPersistentId(2);
    ssm_->UpdateParentSessionForDialog(sceneSession, property);
    SessionInfo info1;
    info1.abilityName_ = "test2";
    info1.bundleName_ = "test3";
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info1, nullptr);
    ssm_->sceneSessionMap_.insert({2, sceneSession2});
    ssm_->UpdateParentSessionForDialog(sceneSession, property);
    ssm_->sceneSessionMap_.erase(2);
    ssm_->CreateSpecificSessionCallback();
    ssm_->CreateKeyboardSessionCallback();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CheckWindowId
 * @tc.desc: CheckWindowId
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CheckWindowId, Function | SmallTest | Level3)
{
    int ret = 0;
    int32_t windowId = 1;
    int32_t pid = 2;
    ssm_->CheckWindowId(windowId, pid);
    ssm_->sceneSessionMap_.insert({windowId, nullptr});
    ssm_->CheckWindowId(windowId, pid);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->sceneSessionMap_.insert({windowId, sceneSession});
    ssm_->CheckWindowId(windowId, pid);
    ssm_->PerformRegisterInRequestSceneSession(sceneSession);
    ssm_->sceneSessionMap_.erase(windowId);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CreateSceneSession
 * @tc.desc: CreateSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CreateSceneSession, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    ssm_->CreateSceneSession(info, nullptr);
    info.isSystem_ = true;
    info.windowType_ = 3000;
    ssm_->CreateSceneSession(info, nullptr);
    info.windowType_ = 3;
    ssm_->CreateSceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ssm_->CreateSceneSession(info, property);
    ssm_->NotifySessionUpdate(info, ActionType::SINGLE_START, 0);
    info.persistentId_ = 0;
    ssm_->UpdateSceneSessionWant(info);
    info.persistentId_ = 1;
    ssm_->UpdateSceneSessionWant(info);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateSceneSessionWant(info);
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    info.want = want;
    ssm_->UpdateSceneSessionWant(info);
    sceneSession->SetCollaboratorType(0);
    ssm_->UpdateSceneSessionWant(info);
    sceneSession->SetCollaboratorType(1);
    ssm_->UpdateSceneSessionWant(info);
    ssm_->sceneSessionMap_.erase(1);
    sptr<SceneSession> sceneSession1;
    ssm_->UpdateCollaboratorSessionWant(sceneSession1, 1);
    ssm_->UpdateCollaboratorSessionWant(sceneSession, 1);
    SessionInfo info1;
    info1.ancoSceneState = 0;
    sceneSession1 = new (std::nothrow) SceneSession(info1, nullptr);
    ssm_->UpdateCollaboratorSessionWant(sceneSession1, 1);
    SessionInfo info2;
    info1.ancoSceneState = 5;
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info2, nullptr);
    ssm_->UpdateCollaboratorSessionWant(sceneSession2, 1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CheckAppIsInDisplay
 * @tc.desc: CheckAppIsInDisplay
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CheckAppIsInDisplay, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<SceneSession> sceneSession;
    ssm_->CheckAppIsInDisplay(sceneSession, 1);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    SessionInfo info;
    ret++;
    sptr<AAFwk::SessionInfo> abilitySessionInfo;
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->CheckAppIsInDisplay(sceneSession, 1);
    ssm_->DestroySubSession(sceneSession);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    sceneSession->SetSessionProperty(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->CheckAppIsInDisplay(sceneSession, 1);
    property->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    ssm_->CheckAppIsInDisplay(sceneSession, 1);
    AppExecFwk::Configuration config;
    ssm_->UpdateConfig(info, config, true);
    ssm_->UpdateConfig(info, config, false);
    ssm_->SetAbilitySessionInfo(sceneSession);
    bool res = true;
    ssm_->PrepareTerminate(1, res);
    ssm_->isPrepareTerminateEnable_ = true;
    ssm_->PrepareTerminate(1, res);
    ssm_->isPrepareTerminateEnable_ = false;
    ssm_->PrepareTerminate(1, res);
    ssm_->StartUIAbilityBySCB(sceneSession);
    ssm_->sceneSessionMap_.insert({1, nullptr});
    ssm_->IsKeyboardForeground();
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, false);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ssm_->IsKeyboardForeground();
    ssm_->IsKeyboardForeground();
    ssm_->StartUIAbilityBySCB(sceneSession);
    ssm_->PrepareTerminate(1, res);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->sceneSessionMap_.erase(1);
    abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    ssm_->StartUIAbilityBySCB(abilitySessionInfo);
    ssm_->DestroySubSession(sceneSession);
    ssm_->EraseSceneSessionMapById(2);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: RequestSceneSessionBackground
 * @tc.desc: RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionBackground, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<SceneSession> sceneSession;
    ssm_->CheckAppIsInDisplay(sceneSession, 1);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    SessionInfo info;
    ret++;
    sptr<AAFwk::SessionInfo> abilitySessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, false);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, false);
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->DestroySubSession(sceneSession);
    ssm_->SetAbilitySessionInfo(sceneSession);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    ssm_->RequestInputMethodCloseKeyboard(1);
    ssm_->sceneSessionMap_.insert({1, nullptr});
    ssm_->RequestInputMethodCloseKeyboard(1);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    ssm_->RequestSceneSessionActivation(sceneSession, false);
    ssm_->StartUIAbilityBySCB(sceneSession);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ssm_->DestroySubSession(sceneSession);
    ssm_->EraseSceneSessionMapById(2);
    ASSERT_EQ(ret, 1);
}


/**
 * @tc.name: RequestSceneSessionDestruction
 * @tc.desc: RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionDestruction, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    info.abilityName_ = "111";
    info.bundleName_ = "222";
    sptr<SceneSession> sceneSession;
    sptr<ISessionStage> sessionStage;
    ssm_->AddClientDeathRecipient(sessionStage, sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->AddClientDeathRecipient(sessionStage, sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    ssm_->HandleCastScreenDisConnection(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    ssm_->sceneSessionMap_.erase(sceneSession->GetPersistentId());
    sptr<AAFwk::SessionInfo> scnSessionInfo = new AAFwk::SessionInfo();
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, true);
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, false);
    std::shared_ptr<SessionListenerController> listenerController = std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listenerController;
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, true);
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, false);
    ssm_->AddClientDeathRecipient(sessionStage, sceneSession);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CreateAndConnectSpecificSession
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CreateAndConnectSpecificSession, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property;
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property = new WindowSessionProperty();
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: StartUIAbilityBySCB
 * @tc.desc: StartUIAbilityBySCB
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, StartUIAbilityBySCB, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "StartUIAbilityBySCB";
    info.bundleName_ = "StartUIAbilityBySCB";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    int32_t ret = ssm_->StartUIAbilityBySCB(sceneSession);
    EXPECT_EQ(ret, 2097202);
}

/**
 * @tc.name: ChangeUIAbilityVisibilityBySCB
 * @tc.desc: ChangeUIAbilityVisibilityBySCB
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, ChangeUIAbilityVisibilityBySCB, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "ChangeUIAbilityVisibilityBySCB";
    info.bundleName_ = "ChangeUIAbilityVisibilityBySCB";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    int32_t ret = ssm_->ChangeUIAbilityVisibilityBySCB(sceneSession, true);
    EXPECT_EQ(ret, 2097202);
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
 * @tc.name: RequestSceneSessionBackground002
 * @tc.desc: SceneSesionManager request scene session background
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionBackground002, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(scensession, isDelegator));
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
}

/**
 * @tc.name: RequestSceneSessionDestruction002
 * @tc.desc: SceneSesionManager request scene session destruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionDestruction002, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionDestruction(scensession, needRemoveSession));
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
    ssm_->NotifySessionTouchOutside(0);
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
 * @tc.name: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.desc: SceneSesionManager destroy and disconnect specific session with detach callback
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroyAndDisconnectSpecificSessionWithDetachCallback, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    WSError result = ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, nullptr);
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
 * @tc.name: InitUserInfo
 * @tc.desc: SceneSesionManager init user info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, InitUserInfo, Function | SmallTest | Level3)
{
    int32_t newUserId = 10086;
    std::string fileDir;
    WSError result01 = ssm_->InitUserInfo(newUserId, fileDir);
    ASSERT_EQ(result01, WSError::WS_DO_NOTHING);
    fileDir = "newFileDir";
    WSError result02 = ssm_->InitUserInfo(newUserId, fileDir);
    ASSERT_EQ(result02, WSError::WS_OK);
}

/**
 * @tc.name: HandleSwitchingToAnotherUser
 * @tc.desc: SceneSesionManager handle switching to another user
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleSwitchingToAnotherUser, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->HandleSwitchingToAnotherUser();
    ASSERT_EQ(ret, 0);
}
 
/**
 * @tc.name: NotifySwitchingToCurrentUser
 * @tc.desc: SceneSesionManager notify switching to current user
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifySwitchingToCurrentUser, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->NotifySwitchingToCurrentUser();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetSessionInfoByContinueSessionId
 * @tc.desc: SceneSesionManager GetSessionInfoByContinueSessionId
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSessionInfoByContinueSessionId, Function | SmallTest | Level3)
{
    std::string continueSessionId = "";
    SessionInfoBean missionInfo;
    EXPECT_EQ(ssm_->GetSessionInfoByContinueSessionId(continueSessionId, missionInfo),
        WSError::WS_ERROR_INVALID_PERMISSION);
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
    ssm_->QueryAbilityInfoFromBMS(uId_,
        sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
    EXPECT_EQ(sessionInfo_.want, nullptr);
    ssm_->Init();
    ssm_->QueryAbilityInfoFromBMS(uId_,
        sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
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
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    info.lockedState = false;
    info.isSystem_ = true;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    info.isSystem_ = false;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_TRUE(ssm_->IsSessionClearable(scensession));
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
    ASSERT_EQ(result, WMError::WM_DO_NOTHING);
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->UpdatePropertyRaiseEnabled(property, scensession);
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
    EXPECT_EQ(result, WMError::WM_DO_NOTHING);
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE;
    ssm_->HandleUpdateProperty(property, action, scensession);
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
    EXPECT_EQ(result, WMError::WM_DO_NOTHING);
    action = WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE;
    ssm_->HandleUpdateProperty(property, action, scensession);
}

/**
 * @tc.name: HandleUpdateProperty03
 * @tc.desc: SceneSesionManager handle update property
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleUpdateProperty03, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSPropertyChangeAction action;
    action = WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS;
    ssm_->HandleUpdateProperty(property, action, scensession);
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
    ASSERT_EQ(result, WMError::WM_DO_NOTHING);
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
 * @tc.name: SetFocusedSessionId
 * @tc.desc: SceneSesionManager set focused session id
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetFocusedSessionId, Function | SmallTest | Level3)
{
    int32_t focusedSession_ = ssm_->GetFocusedSessionId();
    EXPECT_NE(focusedSession_, 10086);
    int32_t persistentId_ = INVALID_SESSION_ID;
    WSError result01 = ssm_->SetFocusedSessionId(persistentId_);
    EXPECT_EQ(result01, WSError::WS_DO_NOTHING);
    persistentId_ = 10086;
    WSError result02 = ssm_->SetFocusedSessionId(persistentId_);
    EXPECT_EQ(result02, WSError::WS_OK);
    ASSERT_EQ(ssm_->GetFocusedSessionId(), 10086);
}

/**
 * @tc.name: RequestFocusStatus
 * @tc.desc: SceneSesionManager request focus status
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestFocusStatus, Function | SmallTest | Level3)
{
    FocusChangeReason reasonInput = FocusChangeReason::DEFAULT;
    FocusChangeReason reasonResult = FocusChangeReason::DEFAULT;
    int32_t focusedSession_ = ssm_->GetFocusedSessionId();
    EXPECT_EQ(focusedSession_, 10086);

    int32_t persistentId_ = INVALID_SESSION_ID;
    WMError result01 = ssm_->RequestFocusStatus(persistentId_, true);
    EXPECT_EQ(result01, WMError::WM_OK);
    reasonResult = ssm_->GetFocusChangeReason();
    EXPECT_EQ(reasonResult, FocusChangeReason::DEFAULT);

    persistentId_ = 10000;
    reasonInput = FocusChangeReason::SCB_SESSION_REQUEST;
    WMError result02 = ssm_->RequestFocusStatus(persistentId_, true, true, reasonInput);
    EXPECT_EQ(result02, WMError::WM_OK);
    reasonResult = ssm_->GetFocusChangeReason();

    reasonInput = FocusChangeReason::SPLIT_SCREEN;
    WMError result03 = ssm_->RequestFocusStatus(persistentId_, false, true, reasonInput);
    EXPECT_EQ(result03, WMError::WM_OK);
    reasonResult = ssm_->GetFocusChangeReason();
    EXPECT_EQ(reasonResult, FocusChangeReason::DEFAULT);
}

/**
 * @tc.name: RaiseWindowToTop
 * @tc.desc: SceneSesionManager raise window to top
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RaiseWindowToTop, Function | SmallTest | Level3)
{
    int32_t focusedSession_ = ssm_->GetFocusedSessionId();
    EXPECT_EQ(focusedSession_, 10086);
    int32_t persistentId_ = INVALID_SESSION_ID;
    WSError result01 = ssm_->RaiseWindowToTop(persistentId_);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PERMISSION);
    persistentId_ = 10000;
    WSError result02 = ssm_->RaiseWindowToTop(persistentId_);
    EXPECT_EQ(result02, WSError::WS_ERROR_INVALID_PERMISSION);
    WSError result03 = ssm_->RaiseWindowToTop(persistentId_);
    EXPECT_EQ(result03, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: ShiftAppWindowFocus
 * @tc.desc: SceneSesionManager shift app window focus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, ShiftAppWindowFocus, Function | SmallTest | Level3)
{
    int32_t focusedSession_ = ssm_->GetFocusedSessionId();
    EXPECT_EQ(focusedSession_, 10086);
    int32_t sourcePersistentId_ = INVALID_SESSION_ID;
    int32_t targetPersistentId_ = INVALID_SESSION_ID;
    WSError result01 = ssm_->ShiftAppWindowFocus(sourcePersistentId_, targetPersistentId_);
    EXPECT_NE(result01, WSError::WS_DO_NOTHING);
    targetPersistentId_ = 1;
    WSError result02 = ssm_->ShiftAppWindowFocus(sourcePersistentId_, targetPersistentId_);
    EXPECT_NE(result02, WSError::WS_ERROR_INVALID_SESSION);
    sourcePersistentId_ = 1;
    WSError result03 = ssm_->ShiftAppWindowFocus(sourcePersistentId_, targetPersistentId_);
    EXPECT_EQ(result03, WSError::WS_ERROR_INVALID_OPERATION);
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
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());

    sptr<SceneSession::SpecificSessionCallback> specific = new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, specific);
    scensession = new (std::nothrow) SceneSession(info, specific);
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, specific);
    ssm_->DumpAllAppSessionInfo(oss, ssm_->sceneSessionMap_);
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
    std::vector<std::string> params_(5, "");
    std::string dumpInfo_;
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    WSError result01 = ssm_->GetSpecifiedSessionDumpInfo(dumpInfo_, params_, strId);
    EXPECT_NE(result01, WSError::WS_OK);
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
    EXPECT_NE(result02, WSError::WS_OK);
    params.clear();
    WSError result03 = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_NE(result03, WSError::WS_OK);
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
    WindowChangedFunc func = [](int32_t persistentId, WindowUpdateType type) {
        OHOS::Rosen::WindowChangedFuncTest(persistentId, type);
    };
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
    ASSERT_NE(scensession, nullptr);
    ssm_->RegisterSessionStateChangeNotifyManagerFunc(scensession);
    ssm_->UpdatePrivateStateAndNotify(persistentId);
    auto displayId = scensession->GetSessionProperty()->GetDisplayId();
    std::vector<string> privacyBundleList;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);
}

/**
 * @tc.name: GerPrivacyBundleListOneWindow
 * @tc.desc: get privacy bundle list when one window exist only.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GerPrivacyBundleListOneWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sceneSession->GetSessionProperty()->displayId_ = 0;
    sceneSession->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<std::string> privacyBundleList;
    sceneSession->GetSessionProperty()->isPrivacyMode_ = false;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);

    sceneSession->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSession->state_ = SessionState::STATE_BACKGROUND;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);

    sceneSession->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);
    EXPECT_EQ(privacyBundleList.at(0), sessionInfo.bundleName_);

    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(1, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);
}

/**
 * @tc.name: GerPrivacyBundleListTwoWindow
 * @tc.desc: get privacy bundle list when two windows exist.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GerPrivacyBundleListTwoWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    SessionInfo sessionInfoSecond;
    sessionInfoSecond.bundleName_ = "privacy.test.second";
    sessionInfoSecond.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfoSecond, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    sceneSessionFirst->GetSessionProperty()->displayId_ = 0;
    sceneSessionFirst->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSessionFirst->state_ = SessionState::STATE_FOREGROUND;

    sceneSessionSecond->GetSessionProperty()->displayId_ = 0;
    sceneSessionSecond->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSessionSecond->state_ = SessionState::STATE_FOREGROUND;

    std::vector<std::string> privacyBundleList;
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 2);

    sceneSessionSecond->GetSessionProperty()->displayId_ = 1;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);

    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(1, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);
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
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    property->SetWindowFlags(flags);
    sptr<SceneSession> scensession = nullptr;
    WSError result01 = ssm_->SetWindowFlags(scensession, property);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result02 = ssm_->SetWindowFlags(scensession, property);
    EXPECT_EQ(result02, WSError::WS_ERROR_NOT_SYSTEM_APP);
    property->SetSystemCalling(true);
    WSError result03 = ssm_->SetWindowFlags(scensession, property);
    ASSERT_EQ(result03, WSError::WS_OK);
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
}

/**
 * @tc.name: StartAbilityBySpecified
 * @tc.desc: SceneSesionManager start ability by specified
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, StartAbilityBySpecified, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    ssm_->StartAbilityBySpecified(info);

    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    AAFwk::WantParams wantParams;
    want->SetParams(wantParams);
    info.want = want;
    ssm_->StartAbilityBySpecified(info);
    ASSERT_EQ(ret, 0);
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
 * @tc.name: UpdateParentSessionForDialog001
 * @tc.desc: SceneSesionManager update parent session for dialog
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateParentSessionForDialog001, Function | SmallTest | Level3)
{
    SessionInfo dialogInfo;
    dialogInfo.abilityName_ = "DialogWindows";
    dialogInfo.bundleName_ = "DialogWindows";
    SessionInfo parentInfo;
    parentInfo.abilityName_ = "ParentWindows";
    parentInfo.bundleName_ = "ParentWindows";

    int32_t persistentId = 1005;
    sptr<SceneSession> parentSession = new (std::nothrow) MainSession(parentInfo, nullptr);
    EXPECT_NE(parentSession, nullptr);
    ssm_->sceneSessionMap_.insert({ persistentId, parentSession });

    sptr<SceneSession> dialogSession = new (std::nothrow) SystemSession(dialogInfo, nullptr);
    EXPECT_NE(dialogSession, nullptr);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetParentPersistentId(persistentId);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);

    WSError result = ssm_->UpdateParentSessionForDialog(dialogSession, property);
    EXPECT_EQ(dialogSession->GetParentPersistentId(), persistentId);
    EXPECT_NE(dialogSession->GetParentSession(), nullptr);
    EXPECT_EQ(result, WSError::WS_OK);
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
    uint64_t displayId = 0;
    WSError result = ssm_->NotifyAINavigationBarShowStatus(isVisible, barArea, displayId);
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
 * @tc.name: UpdateTopmostProperty
 * @tc.desc: test UpdateTopmostProperty
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateTopmostProperty, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "UpdateTopmostProperty";
    info.bundleName_ = "UpdateTopmostProperty";
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetTopmost(true);
    property->SetSystemCalling(true);
    sptr<SceneSession> scenesession = new (std::nothrow) MainSession(info, nullptr);
    scenesession->SetSessionProperty(property);
    WMError result = ssm_->UpdateTopmostProperty(property, scenesession);
    ASSERT_EQ(WMError::WM_OK, result);
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

/**
 * @tc.name: AddSecureSession
 * @tc.desc: SceneSesionManager add secure session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AddSecureSession, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    size_t sizeBefore = 0;
    size_t sizeAfter = 0;
    ssm_->AddSecureSession(persistentId, true, sizeBefore, sizeAfter);
    EXPECT_EQ(sizeBefore, 0);
    EXPECT_EQ(sizeAfter, 1);
    EXPECT_EQ(*ssm_->secureSessionSet_.begin(), persistentId);
    ssm_->AddSecureSession(persistentId, false, sizeBefore, sizeAfter);
    EXPECT_EQ(sizeBefore, 1);
    EXPECT_EQ(sizeAfter, 0);
    ssm_->secureSessionSet_.clear();
}

/**
 * @tc.name: HideNonSecureFloatingWindows
 * @tc.desc: SceneSesionManager hide non-secure floating windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HideNonSecureFloatingWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HideNonSecureFloatingWindows";
    info.bundleName_ = "HideNonSecureFloatingWindows";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);

    ssm_->nonSystemFloatSceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));
    EXPECT_FALSE(sceneSession->GetSessionProperty()->GetForceHide());
    ssm_->HideNonSecureFloatingWindows(0, 0, true);
    EXPECT_FALSE(sceneSession->GetSessionProperty()->GetForceHide());
    ssm_->HideNonSecureFloatingWindows(0, 1, true);
    EXPECT_TRUE(sceneSession->GetSessionProperty()->GetForceHide());
    ssm_->HideNonSecureFloatingWindows(1, 0, false);
    EXPECT_FALSE(sceneSession->GetSessionProperty()->GetForceHide());
    ssm_->nonSystemFloatSceneSessionMap_.clear();
}

/**
 * @tc.name: HideNonSecureSubWindows
 * @tc.desc: SceneSesionManager hide non-secure sub windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HideNonSecureSubWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HideNonSecureSubWindows";
    info.bundleName_ = "HideNonSecureSubWindows";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;

    sptr<SceneSession> subSession;
    subSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(subSession, nullptr);

    sceneSession->AddSubSession(subSession);
    EXPECT_FALSE(subSession->GetSessionProperty()->GetForceHide());
    ssm_->HideNonSecureSubWindows(sceneSession, 0, 0, true);
    EXPECT_FALSE(subSession->GetSessionProperty()->GetForceHide());
    ssm_->HideNonSecureSubWindows(sceneSession, 0, 1, true);
    EXPECT_TRUE(subSession->GetSessionProperty()->GetForceHide());
    ssm_->HideNonSecureSubWindows(sceneSession, 1, 0, false);
    EXPECT_FALSE(subSession->GetSessionProperty()->GetForceHide());
}

/**
 * @tc.name: HandleSecureSessionShouldHide
 * @tc.desc: SceneSesionManager handle secure session should hide
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleSecureSessionShouldHide, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HandleSecureSessionShouldHide";
    info.bundleName_ = "HandleSecureSessionShouldHide";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    EXPECT_TRUE(ssm_->secureSessionSet_.empty());
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->SetShouldHideNonSecureWindows(true);
    auto ret = ssm_->HandleSecureSessionShouldHide(sceneSession);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_EQ(ssm_->secureSessionSet_.size(), 1);
    EXPECT_EQ(*ssm_->secureSessionSet_.begin(), sceneSession->persistentId_);
    sceneSession->SetShouldHideNonSecureWindows(false);
    ret = ssm_->HandleSecureSessionShouldHide(sceneSession);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_TRUE(ssm_->secureSessionSet_.empty());
    ssm_->secureSessionSet_.clear();
}

/**
 * @tc.name: HandleSecureExtSessionShouldHide
 * @tc.desc: SceneSesionManager handle secure extension session should hide
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleSecureExtSessionShouldHide, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    EXPECT_TRUE(ssm_->secureSessionSet_.empty());
    auto ret = ssm_->HandleSecureExtSessionShouldHide(persistentId, true);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_EQ(ssm_->secureSessionSet_.size(), 1);
    EXPECT_EQ(*ssm_->secureSessionSet_.begin(), persistentId);
    ret = ssm_->HandleSecureExtSessionShouldHide(persistentId, false);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_TRUE(ssm_->secureSessionSet_.empty());
    ssm_->secureSessionSet_.clear();
}

/**
 * @tc.name: HandleSCBExtWaterMarkChange
 * @tc.desc: SceneSesionManager handle scb uiextension water mark change
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleSCBExtWaterMarkChange, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    EXPECT_TRUE(ssm_->waterMarkSessionSet_.empty());
    auto ret = ssm_->HandleSCBExtWaterMarkChange(persistentId, true);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_EQ(ssm_->waterMarkSessionSet_.size(), 1);
    EXPECT_EQ(*ssm_->waterMarkSessionSet_.begin(), persistentId);
    ret = ssm_->HandleSCBExtWaterMarkChange(persistentId, false);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_TRUE(ssm_->waterMarkSessionSet_.empty());
    ssm_->waterMarkSessionSet_.clear();
}

/**
 * @tc.name: HandleSpecialExtWindowFlagChange
 * @tc.desc: SceneSesionManager handle special uiextension window flag change
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleSpecialExtWindowFlagChange, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    EXPECT_TRUE(ssm_->secureSessionSet_.empty());
    EXPECT_TRUE(ssm_->waterMarkSessionSet_.empty());
    ssm_->HandleSpecialExtWindowFlagChange(persistentId, 3, 3);
    EXPECT_EQ(ssm_->secureSessionSet_.size(), 1);
    EXPECT_EQ(*ssm_->secureSessionSet_.begin(), persistentId);
    EXPECT_EQ(ssm_->waterMarkSessionSet_.size(), 1);
    EXPECT_EQ(*ssm_->waterMarkSessionSet_.begin(), persistentId);
    ssm_->HandleSpecialExtWindowFlagChange(persistentId, 0, 3);
    EXPECT_TRUE(ssm_->secureSessionSet_.empty());
    EXPECT_TRUE(ssm_->waterMarkSessionSet_.empty());
    ssm_->secureSessionSet_.clear();
    ssm_->waterMarkSessionSet_.clear();
}

/**
 * @tc.name: ClearUnrecoveredSessions
 * @tc.desc: test func ClearUnrecoveredSessions
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ClearUnrecoveredSessions, Function | SmallTest | Level1)
{
    ssm_->alivePersistentIds_.push_back(23);
    ssm_->alivePersistentIds_.push_back(24);
    ssm_->alivePersistentIds_.push_back(25);
    std::vector<int32_t> recoveredPersistentIds;
    recoveredPersistentIds.push_back(23);
    recoveredPersistentIds.push_back(24);
    ssm_->ClearUnrecoveredSessions(recoveredPersistentIds);
}

/**
 * @tc.name: RecoverSessionInfo
 * @tc.desc: test func RecoverSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RecoverSessionInfo, Function | SmallTest | Level1)
{
    SessionInfo info = ssm_->RecoverSessionInfo(nullptr);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    info = ssm_->RecoverSessionInfo(property);
}

/**
 * @tc.name: AddOrRemoveSecureSession
 * @tc.desc: SceneSesionManager hide non-secure windows by scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AddOrRemoveSecureSession, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "AddOrRemoveSecureSession";
    info.bundleName_ = "AddOrRemoveSecureSession1";

    int32_t persistentId = 12345;
    auto ret = ssm_->AddOrRemoveSecureSession(persistentId, true);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: SceneSesionManager update uiextension window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateExtWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "UpdateExtWindowFlags";
    info.bundleName_ = "UpdateExtWindowFlags";

    int32_t parentId = 1234;
    int32_t persistentId = 12345;
    auto ret = ssm_->UpdateExtWindowFlags(parentId, persistentId, 7, 7);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: SetScreenLocked001
 * @tc.desc: SetScreenLocked001
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetScreenLocked001, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetEventHandler(ssm_->taskScheduler_->GetEventHandler(), ssm_->eventHandler_);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));
    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::ATTACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_UNDEFINED;
    sceneSession->SetDetectTaskInfo(detectTaskInfo);
    std::string taskName = "wms:WindowStateDetect" + std::to_string(sceneSession->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    sceneSession->handler_->PostTask(task, taskName, delayTime);
    int32_t beforeTaskNum = GetTaskCount(sceneSession);
    ssm_->SetScreenLocked(true);
    ASSERT_EQ(beforeTaskNum - 1, GetTaskCount(sceneSession));
    ASSERT_EQ(DetectTaskState::NO_TASK, sceneSession->detectTaskInfo_.taskState);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, sceneSession->detectTaskInfo_.taskWindowMode);
}

/**
 * @tc.name: AccessibilityFillEmptySceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill empty scene session list to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptySceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    EXPECT_EQ(accessibilityInfo.size(), 0);
}

/**
 * @tc.name: AccessibilityFillOneSceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill one sceneSession to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillOneSceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}

/**
 * @tc.name: AccessibilityFillTwoSceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill two sceneSessions to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillTwoSceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    SetVisibleForAccessibility(sceneSessionFirst);

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    SetVisibleForAccessibility(sceneSessionSecond);

    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 2);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 2);
}

/**
 * @tc.name: AccessibilityFillEmptyBundleName
 * @tc.desc: SceneSesionManager fill empty bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptyBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, "");
    ASSERT_EQ(sceneSessionList.at(0)->GetSessionInfo().bundleName_, "");
    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, sceneSessionList.at(0)->GetSessionInfo().bundleName_);
}

/**
 * @tc.name: AccessibilityFillBundleName
 * @tc.desc: SceneSesionManager fill bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, "accessibilityNotifyTesterBundleName");
    ASSERT_EQ(sceneSessionList.at(0)->GetSessionInfo().bundleName_, "accessibilityNotifyTesterBundleName");
    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, sceneSessionList.at(0)->GetSessionInfo().bundleName_);
}

/**
 * @tc.name: AccessibilityFillFilterBundleName
 * @tc.desc: SceneSesionManager fill filter bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillFilterBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SCBGestureTopBar";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 0);
}

/**
 * @tc.name: AccessibilityFillEmptyHotAreas
 * @tc.desc: SceneSesionManager fill empty hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptyHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 0);
}

/**
 * @tc.name: AccessibilityFillOneHotAreas
 * @tc.desc: SceneSesionManager fill one hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillOneHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    Rect rect = {100, 200, 100, 200};
    std::vector<Rect> hotAreas;
    hotAreas.push_back(rect);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetTouchHotAreas(hotAreas);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 1);

    ASSERT_EQ(rect.posX_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).posX_);
    ASSERT_EQ(rect.posY_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).posY_);
    ASSERT_EQ(rect.width_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).width_);
    ASSERT_EQ(rect.height_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).height_);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posX_, rect.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posY_, rect.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).width_, rect.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).height_, rect.height_);
}

/**
 * @tc.name: AccessibilityFillTwoHotAreas
 * @tc.desc: SceneSesionManager fill two hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillTwoHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    std::vector<Rect> hotAreas;
    Rect rectFitst = {100, 200, 100, 200};
    Rect rectSecond = {50, 50, 20, 30};
    hotAreas.push_back(rectFitst);
    hotAreas.push_back(rectSecond);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetTouchHotAreas(hotAreas);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 2);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posX_, rectFitst.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posY_, rectFitst.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).width_, rectFitst.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).height_, rectFitst.height_);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).posX_, rectSecond.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).posY_, rectSecond.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).width_, rectSecond.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).height_, rectSecond.height_);
}

/**
 * @tc.name: AccessibilityFilterEmptySceneSessionList
 * @tc.desc: SceneSesionManager filter empty scene session list;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterEmptySceneSessionList, Function | SmallTest | Level3)
{
    std::vector<sptr<SceneSession>> sceneSessionList;

    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 0);
}

/**
 * @tc.name: AccessibilityFilterOneWindow
 * @tc.desc: SceneSesionManager filter one window;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterOneWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetSessionRect({100, 100, 200, 200});
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}

/**
 * @tc.name: AccessibilityFilterTwoWindowNotCovered
 * @tc.desc: SceneSesionManager filter two windows that not covered each other;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterTwoWindowNotCovered, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->SetSessionRect({0, 0, 200, 200});
    SetVisibleForAccessibility(sceneSessionFirst);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    sceneSessionSecond->SetSessionRect({300, 300, 200, 200});
    SetVisibleForAccessibility(sceneSessionSecond);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 2);
}

/**
 * @tc.name: AccessibilityFilterTwoWindowCovered
 * @tc.desc: SceneSesionManager filter two windows that covered each other;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterTwoWindowCovered, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->SetSessionRect({0, 0, 200, 200});
    SetVisibleForAccessibility(sceneSessionFirst);
    sceneSessionFirst->SetZOrder(20);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    sceneSessionSecond->SetSessionRect({50, 50, 50, 50});
    SetVisibleForAccessibility(sceneSessionSecond);
    sceneSessionSecond->SetZOrder(10);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}

/**
 * @tc.name: GetMainWindowInfos
 * @tc.desc: SceneSesionManager get topN main window infos;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetMainWindowInfos, Function | SmallTest | Level3)
{
    int32_t topNum = 1024;
    std::vector<MainWindowInfo> topNInfos;
    auto result = ssm_->GetMainWindowInfos(topNum, topNInfos);

    topNum = 0;
    result = ssm_->GetMainWindowInfos(topNum, topNInfos);

    topNum = 1000;
    MainWindowInfo info;
    topNInfos.push_back(info);
    result = ssm_->GetMainWindowInfos(topNum, topNInfos);
    EXPECT_NE(result, WMError::WM_ERROR_INVALID_PERMISSION);
}

}
} // namespace Rosen
} // namespace OHOS

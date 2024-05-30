/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
class SceneSessionManagerTest2 : public testing::Test {
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

sptr<SceneSessionManager> SceneSessionManagerTest2::ssm_ = nullptr;

bool SceneSessionManagerTest2::gestureNavigationEnabled_ = true;
bool SceneSessionManagerTest2::statusBarEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest2::callbackFunc_ = [](bool enable) {
    gestureNavigationEnabled_ = enable;
};
ProcessStatusBarEnabledChangeFunc SceneSessionManagerTest2::statusBarEnabledCallbackFunc_ = [](bool enable) {
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

void SceneSessionManagerTest2::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest2::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest2::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest2::TearDown()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest2::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
}

int32_t SceneSessionManagerTest2::GetTaskCount(sptr<SceneSession>& session)
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
 * @tc.name: SetGestureNavigaionEnabled
 * @tc.desc: SceneSessionManager set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, SetGestureNavigaionEnabled, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, SetStatusBarEnabled, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, RegisterWindowManagerAgent, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSizeLimits01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect02, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect03, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect04, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect05, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect06, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect07, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect08, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigDecor01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigDecor02, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigDecor03, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigDecor04, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigDecor05, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigDecor06, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml02, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>1 1</defaultWindowMode>"
        "<uiType>phone</uiType>"
        "<backgroundScreenLock enable=\"true\"></backgroundScreenLock>"
        "<rotationMode>windowRotation</rotationMode>"
        "<supportTypeFloatWindow enable=\"true\"></supportTypeFloatWindow>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml03, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml04, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml05, Function | SmallTest | Level3)
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
 * @tc.name: ConfigWindowSceneXml06
 * @tc.desc: call uiType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml06, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<uiType>ut</uiType>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.uiType_, "ut");
}

/**
 * @tc.name: ConfigWindowSceneXml07
 * @tc.desc: call backgroundScreenLock
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml07, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<backgroundScreenLock enable=\"true\"></backgroundScreenLock>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.backgroundScreenLock_, true);
}

/**
 * @tc.name: ConfigWindowSceneXml08
 * @tc.desc: call rotationMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml08, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<rotationMode>rotation</rotationMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.rotationMode_, "rotation");
}

/**
 * @tc.name: ConfigKeyboardAnimation01
 * @tc.desc: call ConfigKeyboardAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigKeyboardAnimation01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigKeyboardAnimation02, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigKeyboardAnimation03, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigKeyboardAnimation04, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation02, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation03, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation04, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation05, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation06, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation07, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation02, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation03, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation04, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation05, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale02, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale03, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale04, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale05, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigSystemUIStatusBar01, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, DumpSessionAll, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
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
HWTEST_F(SceneSessionManagerTest2, DumpSessionWithId, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
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
HWTEST_F(SceneSessionManagerTest2, Init, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, LoadWindowSceneXml, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, UpdateRecoveredSessionInfo, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, SetSessionContinueState, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest2, SetSessionContinueState002, Function | SmallTest | Level3)
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
}
} // namespace Rosen
} // namespace OHOS
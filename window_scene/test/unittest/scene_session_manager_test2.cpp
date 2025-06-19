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
#include "libxml/parser.h"
#include "libxml/tree.h"
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
#include "iremote_object_mocker.h"

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
    xmlDocPtr docPtr = xmlParseMemory(xmlStr.c_str(), xmlStr.length());
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
} // namespace
class SceneSessionManagerTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest2::ssm_ = nullptr;

bool SceneSessionManagerTest2::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest2::callbackFunc_ =
    [](bool enable, const std::string& bundleName, GestureBackType type) { gestureNavigationEnabled_ = enable; };

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
    MockAccesstokenKit::ChangeMockStateToInit();
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: SetGestureNavigationEnabled
 * @tc.desc: SceneSessionManager set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, SetGestureNavigationEnabled, TestSize.Level1)
{
    ASSERT_NE(callbackFunc_, nullptr);

    WMError result00 = ssm_->SetGestureNavigationEnabled(true);
    ASSERT_EQ(result00, WMError::WM_OK);

    ssm_->SetGestureNavigationEnabledChangeListener(callbackFunc_);
    WMError result01 = ssm_->SetGestureNavigationEnabled(true);
    ASSERT_EQ(result01, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, true);

    WMError result02 = ssm_->SetGestureNavigationEnabled(false);
    ASSERT_EQ(result02, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, false);

    ssm_->SetGestureNavigationEnabledChangeListener(nullptr);
    WMError result03 = ssm_->SetGestureNavigationEnabled(true);
    ASSERT_EQ(result03, WMError::WM_OK);
}

/**
 * @tc.name: RegisterWindowManagerAgent01
 * @tc.desc: SceneSesionManager rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, RegisterWindowManagerAgent01, TestSize.Level1)
{
    sptr<IWindowManagerAgent> windowManagerAgent = sptr<WindowManagerAgent>::MakeSptr();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ssm_->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent));
}

/**
 * @tc.name: RegisterWindowManagerAgent02
 * @tc.desc: SceneSesionManager rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, RegisterWindowManagerAgent02, TestSize.Level1)
{
    sptr<IWindowManagerAgent> windowManagerAgent = sptr<WindowManagerAgent>::MakeSptr();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_PROPERTY;

    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ssm_->RegisterWindowManagerAgent(type, windowManagerAgent));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent));
}

/**
 * @tc.name: RegisterWindowPropertyChangeAgent01
 * @tc.desc: Register and unregister window property change agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, RegisterWindowPropertyChangeAgent01, TestSize.Level1)
{
    sptr<IWindowManagerAgent> windowManagerAgent = sptr<WindowManagerAgent>::MakeSptr();
    uint32_t interestInfo = static_cast<uint32_t>(WindowInfoKey::WINDOW_ID);
    ssm_->observedFlags_ = 0;
    ssm_->interestedFlags_ = 0;

    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION,
        ssm_->RegisterWindowPropertyChangeAgent(WindowInfoKey::DISPLAY_ID, interestInfo, windowManagerAgent));
    EXPECT_EQ(ssm_->observedFlags_, static_cast<uint32_t>(WindowInfoKey::DISPLAY_ID));
    EXPECT_EQ(ssm_->interestedFlags_, static_cast<uint32_t>(WindowInfoKey::WINDOW_ID));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION,
        ssm_->UnregisterWindowPropertyChangeAgent(WindowInfoKey::DISPLAY_ID, interestInfo, windowManagerAgent));
    EXPECT_EQ(0, ssm_->observedFlags_);
    EXPECT_EQ(0, ssm_->interestedFlags_);
}

/**
 * @tc.name: ConfigWindowSizeLimits01
 * @tc.desc: call ConfigWindowSizeLimits and check the systemConfig_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSizeLimits01, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect01, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
        "<shadowDark>"
        "<focused>"
        "<elevation>0</elevation>"
        "<color>#111111</color>"
        "<offsetX>2</offsetX>"
        "<offsetY>2</offsetY>"
        "<alpha>1</alpha>"
        "<radius>1</radius>"
        "</focused>"
        "</shadowDark>"
        "</appWindows>"
        "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadow_.radius_, 0.5);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadowDark_.alpha_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadowDark_.offsetX_, 2);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadowDark_.offsetY_, 2);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadowDark_.radius_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.focusedShadowDark_.color_, "#ff111111");
}

/**
 * @tc.name: ConfigWindowEffect02
 * @tc.desc: call ConfigWindowEffect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect02, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect03, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
        "<shadowDark>"
        "<unfocused>"
        "<elevation>0</elevation>"
        "<color>#111111</color>"
        "<offsetX>2</offsetX>"
        "<offsetY>2</offsetY>"
        "<alpha>1</alpha>"
        "<radius>1</radius>"
        "</unfocused>"
        "</shadowDark>"
        "</appWindows>"
        "</windowEffect>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.alpha_, 0);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetX_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.offsetY_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadow_.radius_, 0.5);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadowDark_.alpha_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadowDark_.offsetX_, 2);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadowDark_.offsetY_, 2);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadowDark_.radius_, 1);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.unfocusedShadowDark_.color_, "#ff111111");
}

/**
 * @tc.name: ConfigWindowEffect04
 * @tc.desc: call ConfigWindowEffect all
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect04, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect05, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect06, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect07, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowEffect08, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigDecor01, TestSize.Level1)
{
    std::string xmlStr1 =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"111\">"
        "<supportedMode>fullscreen</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>fullscreen</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorWindowModeSupportType_,
              static_cast<uint32_t>(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN));
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigDecor02, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorWindowModeSupportType_, WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor floating
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigDecor03, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>floating</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorWindowModeSupportType_, WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING);
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor pip
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigDecor04, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>pip</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorWindowModeSupportType_, WindowModeSupport::WINDOW_MODE_SUPPORT_PIP);
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor split
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigDecor05, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>split</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorWindowModeSupportType_,
              WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                  WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: call ConfigDecor default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigDecor06, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<decor enable=\"true\">"
        "<supportedMode>111</supportedMode>"
        "</decor>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.decorWindowModeSupportType_, WINDOW_MODE_SUPPORT_ALL);
}

/**
 * @tc.name: ConfigWindowSceneXml01
 * @tc.desc: call defaultWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml01, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>10</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>102</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.defaultWindowMode_, static_cast<WindowMode>(static_cast<uint32_t>(102)));
}

/**
 * @tc.name: ConfigWindowSceneXml02
 * @tc.desc: call defaultWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml02, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>1 1</defaultWindowMode>"
        "<uiType>phone</uiType>"
        "<backgroundScreenLock enable=\"true\"></backgroundScreenLock>"
        "<rotationMode>windowRotation</rotationMode>"
        "<supportTypeFloatWindow enable=\"true\"></supportTypeFloatWindow>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultWindowMode>1</defaultWindowMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.defaultWindowMode_, static_cast<WindowMode>(static_cast<uint32_t>(1)));
}

/**
 * @tc.name: ConfigWindowSceneXml03
 * @tc.desc: call defaultMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml03, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>1 1</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>1</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(SceneSession::maximizeMode_, static_cast<MaximizeMode>(static_cast<uint32_t>(1)));
}

/**
 * @tc.name: ConfigWindowSceneXml04
 * @tc.desc: call defaultMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml04, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>111</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<defaultMaximizeMode>0</defaultMaximizeMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(SceneSession::maximizeMode_, static_cast<MaximizeMode>(static_cast<uint32_t>(0)));
}

/**
 * @tc.name: ConfigWindowSceneXml05
 * @tc.desc: call maxFloatingWindowSize
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml05, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxFloatingWindowSize>1</maxFloatingWindowSize>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();

    std::string xmlStr1 =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<maxFloatingWindowSize>1</maxFloatingWindowSize>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr1);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->systemConfig_.maxFloatingWindowSize_, static_cast<uint32_t>(1));
}

/**
 * @tc.name: ConfigWindowSceneXml07
 * @tc.desc: call backgroundScreenLock
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml07, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml08, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<rotationMode>rotation</rotationMode>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.rotationMode_, "rotation");
}

/**
 * @tc.name: ConfigWindowAnimation01
 * @tc.desc: call ConfigWindowAnimation default
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation01, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation02, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation03, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation04, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation05, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation06, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigWindowAnimation07, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation01, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation02, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation03, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation04, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigStartingWindowAnimation05, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale01, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale02, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale03, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale04, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigSnapshotScale05, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
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
HWTEST_F(SceneSessionManagerTest2, ConfigSystemUIStatusBar01, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<systemUIStatusBar>"
        "<showInLandscapeMode>1</showInLandscapeMode>"
        "<immersiveStatusBarBgColor>#4c000000</immersiveStatusBarBgColor>"
        "<immersiveStatusBarContentColor>#ffffee</immersiveStatusBarContentColor>"
        "</systemUIStatusBar>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    sptr<SceneSessionManager> sceneSessionManager = sptr<SceneSessionManager>::MakeSptr();
    sceneSessionManager->ConfigWindowSceneXml();
    ASSERT_EQ(sceneSessionManager->appWindowSceneConfig_.systemUIStatusBarConfig_.showInLandscapeMode_, 1);
    ASSERT_STREQ(sceneSessionManager->appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarBgColor_.c_str(),
                 "#4c000000");
    ASSERT_STREQ(
        sceneSessionManager->appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarContentColor_.c_str(),
        "#ffffee");
}

/**
 * @tc.name: Init
 * @tc.desc: SceneSesionManager init
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, Init, TestSize.Level1)
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
HWTEST_F(SceneSessionManagerTest2, LoadWindowSceneXml, TestSize.Level1)
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
HWTEST_F(SceneSessionManagerTest2, UpdateRecoveredSessionInfo, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    std::vector<int32_t> recoveredPersistentIds;
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    recoveredPersistentIds.push_back(0);
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({ 0, sceneSession });
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    ssm_->sceneSessionMap_.erase(0);
}

/**
 * @tc.name: UpdateRecoveredSessionInfo02
 * @tc.desc: Test if failRecoverPersistentSet exist or not exist
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, UpdateRecoveredSessionInfo02, TestSize.Level1)
{
    int ret = 0;
    ASSERT_NE(ssm_, nullptr);
    std::vector<int32_t> recoveredPersistentIds;
    recoveredPersistentIds.push_back(0);
    recoveredPersistentIds.push_back(1);
    SessionInfo info;
    info.abilityName_ = "UpdateRecoveredSessionInfo02";
    info.bundleName_ = "SceneSessionManagerTest2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->failRecoveredPersistentIdSet_.insert(0);
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    ssm_->failRecoveredPersistentIdSet_.erase(0);
    ssm_->sceneSessionMap_.erase(1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifyCreateSubSession
 * @tc.desc: Test if createSubSessionFuncMap_ exist
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, NotifyCreateSubSession, TestSize.Level1)
{
    int ret = 0;
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "NotifyCreateSubSession";
    info.bundleName_ = "SceneSessionManagerTest2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    NotifyCreateSubSessionFunc func;
    ssm_->createSubSessionFuncMap_.insert({ 1, func });
    ssm_->NotifyCreateSubSession(1, sceneSession, 256);
    ssm_->createSubSessionFuncMap_.erase(1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigWindowSceneXml
 * @tc.desc: SceneSesionManager config window scene xml run
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigWindowSceneXml, TestSize.Level1)
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
HWTEST_F(SceneSessionManagerTest2, SetSessionContinueState, TestSize.Level1)
{
    MessageParcel* data = new MessageParcel();
    sptr<IRemoteObject> token = data->ReadRemoteObject();
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
HWTEST_F(SceneSessionManagerTest2, SetSessionContinueState002, TestSize.Level1)
{
    MessageParcel* data = new MessageParcel();
    sptr<IRemoteObject> token = data->ReadRemoteObject();
    auto continueState = static_cast<ContinueState>(0);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    if (sceneSession == nullptr) {
        delete data;
        return;
    }
    ssm_->sceneSessionMap_.insert({ 1000, sceneSession });
    ssm_->SetSessionContinueState(token, continueState);
    ASSERT_NE(sceneSession, nullptr);
    delete data;
}

/**
 * @tc.name: StartWindowInfoReportLoop
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, StartWindowInfoReportLoop, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->StartWindowInfoReportLoop();
    ssm_->eventHandler_ = nullptr;
    ssm_->StartWindowInfoReportLoop();
    ssm_->isReportTaskStart_ = true;
    ssm_->StartWindowInfoReportLoop();
}

/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetFocusWindowInfo, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    FocusChangeInfo info;
    ssm_->GetFocusWindowInfo(info);
}

/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetFocusWindowInfo2, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    FocusChangeInfo fcinfo;
    ssm_->GetFocusWindowInfo(fcinfo);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 0, sceneSession });
    ssm_->GetFocusWindowInfo(fcinfo);
}

/**
 * @tc.name: SetSessionLabel
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, SetSessionLabel, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetSessionLabel(nullptr, "test");

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->SetSessionLabel(nullptr, "test");
}

/**
 * @tc.name: SetSessionIcon
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, SetSessionIcon, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetSessionIcon(nullptr, nullptr);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->SetSessionIcon(nullptr, nullptr);
}

/**
 * @tc.name: InitWithRenderServiceAdded
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, InitWithRenderServiceAdded, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->InitWithRenderServiceAdded();
}

/**
 * @tc.name: PendingSessionToForeground
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, PendingSessionToForeground, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->PendingSessionToForeground(nullptr);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->PendingSessionToForeground(nullptr);
}

/**
 * @tc.name: GetFocusSessionToken
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetFocusSessionToken, TestSize.Level1)
{
    WSError ret;
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ret = ssm_->GetFocusSessionToken(token);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PERMISSION, ret);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ret = ssm_->GetFocusSessionToken(token);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: GetFocusSessionElement
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetFocusSessionElement, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    AppExecFwk::ElementName element;
    ssm_->GetFocusSessionElement(element);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->GetFocusSessionElement(element);
}

/**
 * @tc.name: GetAllAbilityInfos
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetAllAbilityInfos, TestSize.Level1)
{
    WSError ret;
    AAFwk::Want want;
    AppExecFwk::ElementName elementName = want.GetElement();
    int32_t userId = 1;
    std::vector<SCBAbilityInfo> scbAbilityInfos;

    ret = ssm_->GetAllAbilityInfos(want, userId, scbAbilityInfos);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);

    elementName.bundleName_ = "test";
    ret = ssm_->GetAllAbilityInfos(want, userId, scbAbilityInfos);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);

    elementName.abilityName_ = "test";
    ret = ssm_->GetAllAbilityInfos(want, userId, scbAbilityInfos);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);

    elementName.bundleName_ = "";
    ret = ssm_->GetAllAbilityInfos(want, userId, scbAbilityInfos);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);

    ssm_->bundleMgr_ = nullptr;
    ret = ssm_->GetAllAbilityInfos(want, userId, scbAbilityInfos);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: GetIsLayoutFullScreen
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetIsLayoutFullScreen, TestSize.Level1)
{
    WSError ret;
    bool isLayoutFullScreen = true;
    ret = ssm_->GetIsLayoutFullScreen(isLayoutFullScreen);
    ASSERT_EQ(WSError::WS_OK, ret);

    isLayoutFullScreen = false;
    ret = ssm_->GetIsLayoutFullScreen(isLayoutFullScreen);
    ASSERT_EQ(WSError::WS_OK, ret);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    isLayoutFullScreen = true;
    ret = ssm_->GetIsLayoutFullScreen(isLayoutFullScreen);
    ASSERT_EQ(WSError::WS_OK, ret);

    isLayoutFullScreen = false;
    ret = ssm_->GetIsLayoutFullScreen(isLayoutFullScreen);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: UpdateSessionAvoidAreaListener
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, UpdateSessionAvoidAreaListener, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    {
        std::unique_lock<std::shared_mutex> lock(ssm_->sceneSessionMapMutex_);
        ssm_->sceneSessionMap_.clear();
    }
    int32_t persistentId = 100;
    ssm_->UpdateSessionAvoidAreaListener(persistentId, true);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->UpdateSessionAvoidAreaListener(persistentId, true);
    ssm_->UpdateSessionAvoidAreaListener(persistentId, false);
}

/**
 * @tc.name: UpdateSessionTouchOutsideListener
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, UpdateSessionTouchOutsideListener, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    {
        std::unique_lock<std::shared_mutex> lock(ssm_->sceneSessionMapMutex_);
        ssm_->sceneSessionMap_.clear();
    }
    int32_t persistentId = 100;
    ssm_->UpdateSessionTouchOutsideListener(persistentId, true);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->UpdateSessionTouchOutsideListener(persistentId, true);

    ssm_->UpdateSessionTouchOutsideListener(persistentId, false);
}

/**
 * @tc.name: GetSessionSnapshotById
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetSessionSnapshotById, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    SessionSnapshot snapshot;
    ssm_->GetSessionSnapshotById(100, snapshot);
}

/**
 * @tc.name: ClearSession
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ClearSession, TestSize.Level1)
{
    MockAccesstokenKit::MockAccessTokenKitRet(-1);
    auto ret = ssm_->ClearSession(100);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: ClearAllSessions
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ClearAllSessions, TestSize.Level1)
{
    MockAccesstokenKit::MockAccessTokenKitRet(-1);
    auto ret = ssm_->ClearAllSessions();
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: GetTopWindowId
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetTopWindowId, TestSize.Level1)
{
    WMError ret;
    {
        std::unique_lock<std::shared_mutex> lock(ssm_->sceneSessionMapMutex_);
        ssm_->sceneSessionMap_.clear();
    }
    uint32_t persistentId = 100;
    uint32_t topWinId = 200;
    ret = ssm_->GetTopWindowId(persistentId, topWinId);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ret = ssm_->GetTopWindowId(persistentId, topWinId);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
}

/**
 * @tc.name: InitPersistentStorage
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, InitPersistentStorage, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    {
        std::unique_lock<std::shared_mutex> lock(ssm_->sceneSessionMapMutex_);
        ssm_->sceneSessionMap_.clear();
    }
    ssm_->InitPersistentStorage();
}

/**
 * @tc.name: GetAccessibilityWindowInfo
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, GetAccessibilityWindowInfo, TestSize.Level1)
{
    WMError ret;
    {
        std::unique_lock<std::shared_mutex> lock(ssm_->sceneSessionMapMutex_);
        ssm_->sceneSessionMap_.clear();
    }
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    ret = ssm_->GetAccessibilityWindowInfo(infos);
    ASSERT_EQ(WMError::WM_OK, ret);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ret = ssm_->GetAccessibilityWindowInfo(infos);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: OnScreenshot
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, OnScreenshot, TestSize.Level1)
{
    ASSERT_NE(nullptr, ssm_);
    {
        std::unique_lock<std::shared_mutex> lock(ssm_->sceneSessionMapMutex_);
        ssm_->sceneSessionMap_.clear();
    }
    DisplayId displayId = 0;
    ssm_->OnScreenshot(displayId);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->OnScreenshot(displayId);

    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->OnScreenshot(displayId);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->OnScreenshot(displayId);
    sceneSession->SetSessionState(SessionState::STATE_DISCONNECT);
    ssm_->OnScreenshot(displayId);
    sceneSession->SetSessionState(SessionState::STATE_END);
    ssm_->OnScreenshot(displayId);
}

/**
 * @tc.name: ProcessSubSessionForeground
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ProcessSubSessionForeground, TestSize.Level1)
{
    {
        std::unique_lock<std::shared_mutex> lock(ssm_->sceneSessionMapMutex_);
        ssm_->sceneSessionMap_.clear();
    }
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->ProcessSubSessionForeground(sceneSession);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->ProcessSubSessionForeground(sceneSession);

    sptr<SceneSession> sub1 = nullptr;
    sptr<SceneSession> sub2 = sptr<SceneSession>::MakeSptr(info, nullptr);
    std::vector<sptr<SceneSession>> subs;
    std::vector<sptr<SceneSession>> dialogs;
    subs.push_back(sub1);
    subs.push_back(sub2);
    dialogs.push_back(sub1);
    dialogs.push_back(sub2);
    sceneSession->subSession_ = subs;
    ssm_->ProcessSubSessionForeground(sceneSession);
    sptr<SceneSession> sub3 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sub3->state_ = SessionState::STATE_FOREGROUND;
    subs.push_back(sub3);
    dialogs.push_back(sub3);
    sceneSession->subSession_ = subs;
    ssm_->ProcessSubSessionForeground(sceneSession);
    sptr<SceneSession> sub4 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sub4->state_ = SessionState::STATE_FOREGROUND;
    sub4->persistentId_ = 100;
    subs.push_back(sub4);
    dialogs.push_back(sub4);
    sceneSession->subSession_ = subs;
    ssm_->ProcessSubSessionForeground(sceneSession);

    ssm_->sceneSessionMap_.insert({ 0, sceneSession });
    ssm_->sceneSessionMap_.insert({ 100, sceneSession });
    ssm_->ProcessSubSessionForeground(sceneSession);
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;
    ssm_->ProcessSubSessionForeground(sceneSession);
    ASSERT_NE(nullptr, ssm_);
}

/**
 * @tc.name: ConfigSystemUIStatusBar
 * @tc.desc: call ConfigSystemUIStatusBar default.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ConfigSystemUIStatusBar02, TestSize.Level1)
{
    std::string xmlStr =
        "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<systemUIStatusBar>"
        "</systemUIStatusBar>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    sptr<SceneSessionManager> sceneSessionManager = sptr<SceneSessionManager>::MakeSptr();
    ASSERT_NE(sceneSessionManager, nullptr);
    sceneSessionManager->ConfigWindowSceneXml();
}

/**
 * @tc.name: ClosePipWindowIfExist
 * @tc.desc: ClosePipWindowIfExist
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ClosePipWindowIfExist, TestSize.Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    ssm_->ClosePipWindowIfExist(WindowType::WINDOW_TYPE_PIP);

    SessionInfo info;
    info.sessionState_ = { 1 };
    Rect reqRect = { 0, 0, 10, 10 };
    property->SetRequestRect(reqRect);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    ASSERT_EQ(false, ssm_->IsEnablePiPCreate(property));
}

/**
 * @tc.name: RecoverAndConnectSpecificSession
 * @tc.desc: RecoverAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, RecoverAndConnectSpecificSession, TestSize.Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetParentId(1);
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    sptr<ISession> session;
    sptr<IRemoteObject> token;
    auto result =
        ssm_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, surfaceNode, property, session, token);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: RecoverAndConnectSpecificSession02
 * @tc.desc: RecoverAndConnectSpecificSession02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, RecoverAndConnectSpecificSession02, TestSize.Level1)
{
    SessionInfo sessionInfo;
    sessionInfo.abilityName_ = "RecoverAndConnectSpecificSession02";
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_END);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetSessionInfo(sessionInfo);
    property->SetPersistentId(1);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    sptr<ISession> session;
    sptr<IRemoteObject> token;
    ASSERT_NE(ssm_, nullptr);
    auto result =
        ssm_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, surfaceNode, property, session, token);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: SetAlivePersistentIds
 * @tc.desc: SetAlivePersistentIds
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, SetAlivePersistentIds, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<int32_t> recoveredPersistentIds = { 0, 1, 2 };
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    ASSERT_EQ(ssm_->alivePersistentIds_, recoveredPersistentIds);
}

/**
 * @tc.name: NotifyCreateToastSession
 * @tc.desc: NotifyCreateToastSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, NotifyCreateToastSession, TestSize.Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    ssm_->NotifyCreateToastSession(1, nullptr);
    SessionInfo Info;
    Info.persistentId_ = 1;
    int32_t persistentId = Info.persistentId_;
    Info.abilityName_ = "testInfo1a";
    Info.bundleName_ = "testInfo1b";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(Info, nullptr);
    ssm_->NotifyCreateToastSession(persistentId, session);
}

/**
 * @tc.name: RecoverCachedDialogSession
 * @tc.desc: RecoverCachedDialogSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, RecoverCachedDialogSession, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property;
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(sceneSession, nullptr);

    int32_t parentPersistentId = 1;
    ssm_->RecoverCachedDialogSession(parentPersistentId);
    ASSERT_EQ(ssm_->recoverDialogSessionCacheMap_[parentPersistentId].size(), 0);
    ssm_->recoverDialogSessionCacheMap_[parentPersistentId].emplace_back(sceneSession);
    ASSERT_EQ(ssm_->recoverDialogSessionCacheMap_[parentPersistentId].size(), 1);
    ssm_->recoverDialogSessionCacheMap_[parentPersistentId].emplace_back(sceneSession);
    ASSERT_EQ(ssm_->recoverDialogSessionCacheMap_[parentPersistentId].size(), 2);
    ssm_->RecoverCachedDialogSession(parentPersistentId);
    ASSERT_EQ(ssm_->recoverDialogSessionCacheMap_[parentPersistentId].size(), 0);
}

/**
 * @tc.name: ExtractSupportWindowModeFromMetaData
 * @tc.desc: ExtractSupportWindowModeFromMetaData
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ExtractSupportWindowModeFromMetaData, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property;
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(sceneSession, nullptr);

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    AppExecFwk::AbilityInfo abilityInfo;
    int ret = 0;
    std::vector<AppExecFwk::SupportWindowMode> updateWindowModes =
        ssm_->ExtractSupportWindowModeFromMetaData(std::make_shared<OHOS::AppExecFwk::AbilityInfo>(abilityInfo));
    ASSERT_EQ(ret, 0);

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ssm_->systemConfig_.freeMultiWindowEnable_ = false;
    updateWindowModes =
        ssm_->ExtractSupportWindowModeFromMetaData(std::make_shared<OHOS::AppExecFwk::AbilityInfo>(abilityInfo));
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ParseWindowModeFromMetaData
 * @tc.desc: ParseWindowModeFromMetaData
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest2, ParseWindowModeFromMetaData, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property;
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(sceneSession, nullptr);

    std::vector<AppExecFwk::SupportWindowMode> updateWindowModes = { AppExecFwk::SupportWindowMode::FULLSCREEN,
                                                                     AppExecFwk::SupportWindowMode::SPLIT,
                                                                     AppExecFwk::SupportWindowMode::FLOATING };
    ASSERT_EQ(updateWindowModes, ssm_->ParseWindowModeFromMetaData("fullscreen,split,floating"));
}
} // namespace
} // namespace Rosen
} // namespace OHOS

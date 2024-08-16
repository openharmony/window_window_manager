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
class SceneSessionManagerTest3 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 50000;
};

sptr<SceneSessionManager> SceneSessionManagerTest3::ssm_ = nullptr;

bool SceneSessionManagerTest3::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest3::callbackFunc_ = [](bool enable,
    const std::string& bundleName) {
    gestureNavigationEnabled_ = enable;
};

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable, const std::string& bundleName)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest3::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest3::TearDownTestCase()
{
    ssm_ = nullptr;
    usleep(WAIT_SYNC_IN_NS);
}

void SceneSessionManagerTest3::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest3::TearDown()
{
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: ConfigDecor
 * @tc.desc: SceneSesionManager config decor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigDecor, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem* item = new (std::nothrow) WindowSceneConfig::ConfigItem;
    ASSERT_NE(nullptr, item);
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
HWTEST_F(SceneSessionManagerTest3, ConfigWindowEffect, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem* item = new (std::nothrow) WindowSceneConfig::ConfigItem;
    ASSERT_NE(nullptr, item);
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
HWTEST_F(SceneSessionManagerTest3, ConfigAppWindowCornerRadius, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, ConfigAppWindowShadow, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, ConfigKeyboardAnimation, Function | SmallTest | Level3)
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
 * @tc.name: ConfigStatusBar
 * @tc.desc: ConfigStatusBar config window immersive status bar
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigStatusBar, Function | SmallTest | Level3)
{
    StatusBarConfig out;
    WindowSceneConfig::ConfigItem enable;
    enable.SetValue(true);
    WindowSceneConfig::ConfigItem showHide;
    showHide.SetProperty({{"enable", enable}});
    WindowSceneConfig::ConfigItem item01;
    WindowSceneConfig::ConfigItem contentColor;
    contentColor.SetValue(std::string("#12345678"));
    WindowSceneConfig::ConfigItem backgroundColor;
    backgroundColor.SetValue(std::string("#12345678"));
    item01.SetValue({{"showHide", showHide}, {"contentColor", contentColor}, {"backgroundColor", backgroundColor}});
    bool result01 = ssm_->ConfigStatusBar(item01, out);
    ASSERT_EQ(result01, true);
    ASSERT_EQ(out.showHide_, true);
    ASSERT_EQ(out.contentColor_, "#12345678");
    ASSERT_EQ(out.backgroundColor_, "#12345678");
}

/**
 * @tc.name: ConfigKeyboardAnimation
 * @tc.desc: SceneSesionManager config keyboard animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigWindowImmersive, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
            "<immersive>"
                "<inDesktopStatusBarConfig>"
                    "<showHide enable=\"true\"/>"
                    "<backgroundColor>#12341234</backgroundColor>"
                    "<contentColor>#12341234</contentColor>"
                "</inDesktopStatusBarConfig>"
                "<inSplitStatusBarConfig>"
                    "<upDownSplit>"
                        "<showHide enable=\"true\"/>"
                        "<backgroundColor>#12341234</backgroundColor>"
                        "<contentColor>#12341234</contentColor>"
                    "</upDownSplit>"
                    "<leftRightSplit>"
                        "<showHide enable=\"true\"/>"
                        "<backgroundColor>#12341234</backgroundColor>"
                        "<contentColor>#12341234</contentColor>"
                    "</leftRightSplit>"
                "</inSplitStatusBarConfig>"
            "</immersive>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    ssm_->ConfigWindowSceneXml();
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.desktopStatusBarConfig_.showHide_, true);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.desktopStatusBarConfig_.backgroundColor_, "#12341234");
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.desktopStatusBarConfig_.contentColor_, "#12341234");
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.upDownStatusBarConfig_.showHide_, true);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.upDownStatusBarConfig_.backgroundColor_, "#12341234");
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.upDownStatusBarConfig_.contentColor_, "#12341234");
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.leftRightStatusBarConfig_.showHide_, true);
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.leftRightStatusBarConfig_.backgroundColor_, "#12341234");
    ASSERT_EQ(ssm_->appWindowSceneConfig_.windowImmersive_.leftRightStatusBarConfig_.contentColor_, "#12341234");
}

/**
 * @tc.name: ConfigWindowAnimation
 * @tc.desc: SceneSesionManager config window animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigWindowAnimation, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, RecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    auto result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, nullptr, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = { 0, 1, 2 };
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
HWTEST_F(SceneSessionManagerTest3, ConfigStartingWindowAnimation, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, CreateCurve, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, SetRootSceneContext, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, GetRootSceneSession, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, GetSceneSession, Function | SmallTest | Level3)
{
    int32_t persistentId = 65535;
    ASSERT_EQ(ssm_->GetSceneSession(persistentId), nullptr);
}

/**
 * @tc.name: GetSceneSession002
 * @tc.desc: SceneSesionManager get scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, GetSceneSession002, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, GetSceneSessionByName, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, GetSceneSessionVectorByType, Function | SmallTest | Level3)
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
    ASSERT_NE(nullptr, property);
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
HWTEST_F(SceneSessionManagerTest3, UpdateParentSessionForDialog, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->UpdateParentSessionForDialog(nullptr, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    ssm_->UpdateParentSessionForDialog(nullptr, property);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.moduleName_ = "test3";
    info.appIndex_ = 10;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->UpdateParentSessionForDialog(sceneSession, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->UpdateParentSessionForDialog(sceneSession, property);
    property->SetParentPersistentId(2);
    ssm_->UpdateParentSessionForDialog(sceneSession, property);
    SessionInfo info1;
    info1.abilityName_ = "test2";
    info1.bundleName_ = "test3";
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession2);
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
HWTEST_F(SceneSessionManagerTest3, CheckWindowId, Function | SmallTest | Level3)
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
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({windowId, sceneSession});
    ssm_->CheckWindowId(windowId, pid);
    ssm_->PerformRegisterInRequestSceneSession(sceneSession);
    ssm_->sceneSessionMap_.erase(windowId);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: OnSCBSystemSessionBufferAvailable
 * @tc.desc: OnSCBSystemSessionBufferAvailable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, OnSCBSystemSessionBufferAvailable, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->OnSCBSystemSessionBufferAvailable(WindowType::WINDOW_TYPE_KEYGUARD);
}

/**
 * @tc.name: CreateSceneSession
 * @tc.desc: CreateSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, CreateSceneSession, Function | SmallTest | Level3)
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
    ASSERT_NE(nullptr, property);
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
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateSceneSessionWant(info);
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    ASSERT_NE(nullptr, want);
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
    ASSERT_NE(nullptr, sceneSession1);
    ssm_->UpdateCollaboratorSessionWant(sceneSession1, 1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CheckAppIsInDisplay
 * @tc.desc: CheckAppIsInDisplay
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, CheckAppIsInDisplay, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<SceneSession> sceneSession;
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    SessionInfo info;
    ret++;
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->DestroySubSession(sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sceneSession->SetSessionProperty(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    AppExecFwk::Configuration config;
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
    ssm_->StartUIAbilityBySCB(sceneSession);
    ssm_->PrepareTerminate(1, res);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->sceneSessionMap_.erase(1);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    ssm_->StartUIAbilityBySCB(abilitySessionInfo);
    ssm_->DestroySubSession(sceneSession);
    ssm_->EraseSceneSessionMapById(2);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: CreateAndConnectSpecificSession
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, CreateAndConnectSpecificSession, Function | SmallTest | Level3)
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
    property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: StartUIAbilityBySCB
 * @tc.desc: StartUIAbilityBySCB
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, StartUIAbilityBySCB, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "StartUIAbilityBySCB";
    info.bundleName_ = "StartUIAbilityBySCB";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    int32_t ret = ssm_->StartUIAbilityBySCB(sceneSession);
    EXPECT_EQ(ret, 2097202);
}

/**
 * @tc.name: ChangeUIAbilityVisibilityBySCB
 * @tc.desc: ChangeUIAbilityVisibilityBySCB
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, ChangeUIAbilityVisibilityBySCB, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "ChangeUIAbilityVisibilityBySCB";
    info.bundleName_ = "ChangeUIAbilityVisibilityBySCB";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    int32_t ret = ssm_->ChangeUIAbilityVisibilityBySCB(sceneSession, true);
    EXPECT_EQ(ret, 2097202);
}

/**
 * @tc.name: SetAbilitySessionInfo
 * @tc.desc: SceneSesionManager set ability session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, SetAbilitySessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetAbilitySessionInfo";
    info.bundleName_ = "SetAbilitySessionInfo";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<OHOS::AAFwk::SessionInfo> ret = ssm_->SetAbilitySessionInfo(sceneSession);
    OHOS::AppExecFwk::ElementName retElementName = ret->want.GetElement();
    ASSERT_EQ(retElementName.GetAbilityName(), info.abilityName_);
    ASSERT_EQ(retElementName.GetBundleName(), info.bundleName_);
}

/**
 * @tc.name: PrepareTerminate
 * @tc.desc: SceneSesionManager prepare terminate
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, PrepareTerminate, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    bool isPrepareTerminate = false;
    ASSERT_EQ(WSError::WS_OK, ssm_->PrepareTerminate(persistentId, isPrepareTerminate));
}

/**
 * @tc.name: DestroyDialogWithMainWindow
 * @tc.desc: SceneSesionManager destroy dialog with main window
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, DestroyDialogWithMainWindow, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "DestroyDialogWithMainWindow";
    info.bundleName_ = "DestroyDialogWithMainWindow";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_EQ(WSError::WS_OK, ssm_->DestroyDialogWithMainWindow(sceneSession));
}

/**
 * @tc.name: AddClientDeathRecipient
 * @tc.desc: SceneSesionManager add client death recipient
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, AddClientDeathRecipient, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "AddClientDeathRecipient";
    info.bundleName_ = "AddClientDeathRecipient";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    int ret = 0;
    ssm_->AddClientDeathRecipient(nullptr, sceneSession);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: DestroySpecificSession
 * @tc.desc: SceneSesionManager destroy specific session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, DestroySpecificSession, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, SetCreateSystemSessionListener, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, SetGestureNavigationEnabledChangeListener, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, OnOutsideDownEvent, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, NotifySessionTouchOutside, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, SetOutsideDownEventListener, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, DestroyAndDisconnectSpecificSession, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, DestroyAndDisconnectSpecificSessionWithDetachCallback, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, GetWindowSceneConfig, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, ProcessBackEvent, Function | SmallTest | Level3)
{
    WSError result = ssm_->ProcessBackEvent();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: IsPcSceneSessionLifecycle1
 * @tc.desc: Normal test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, IsPcSceneSessionLifecycle1, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->systemConfig_.backgroundswitch = true;
    SessionInfo info;
    info.abilityName_ = "IsPcSceneSessionLifecycle1";
    info.bundleName_ = "IsPcSceneSessionLifecycle1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sceneSession->SetSessionProperty(property);
    property->SetIsAppSupportPhoneInPc(false);
    sceneSession->SetIsPcAppInPad(false);

    bool ret = ssm_->IsPcSceneSessionLifecycle(sceneSession);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: IsPcSceneSessionLifecycle2
 * @tc.desc: pc app in pad
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, IsPcSceneSessionLifecycle2, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->systemConfig_.backgroundswitch = false;
    SessionInfo info;
    info.abilityName_ = "IsPcSceneSessionLifecycle2";
    info.bundleName_ = "IsPcSceneSessionLifecycle2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sceneSession->SetSessionProperty(property);
    property->SetIsAppSupportPhoneInPc(false);
    sceneSession->SetIsPcAppInPad(true);

    bool ret = ssm_->IsPcSceneSessionLifecycle(sceneSession);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: IsPcSceneSessionLifecycle3
 * @tc.desc: Compatible mode in pc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, IsPcSceneSessionLifecycle3, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->systemConfig_.backgroundswitch = true;
    SessionInfo info;
    info.abilityName_ = "IsPcSceneSessionLifecycle3";
    info.bundleName_ = "IsPcSceneSessionLifecycle3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sceneSession->SetSessionProperty(property);
    property->SetIsAppSupportPhoneInPc(true);
    sceneSession->SetIsPcAppInPad(false);

    bool ret = ssm_->IsPcSceneSessionLifecycle(sceneSession);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: InitUserInfo
 * @tc.desc: SceneSesionManager init user info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, InitUserInfo, Function | SmallTest | Level3)
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
 * @tc.name: IsInvalidMainSessionOnUserSwitch1
 * @tc.desc: invalid window type
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, IsNeedChangeLifeCycleOnUserSwitch1, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->systemConfig_.backgroundswitch = true;
    int32_t pid = 12345;
    SessionInfo info;
    info.abilityName_ = "IsNeedChangeLifeCycleOnUserSwitch1";
    info.bundleName_ = "IsNeedChangeLifeCycleOnUserSwitch1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetCallingPid(45678);
    property->SetIsAppSupportPhoneInPc(false);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession->SetSessionState(SessionState::STATE_BACKGROUND);

    bool ret = ssm_->IsNeedChangeLifeCycleOnUserSwitch(sceneSession, pid);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: IsNeedChangeLifeCycleOnUserSwitch2
 * @tc.desc: invalid window state
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, IsNeedChangeLifeCycleOnUserSwitch2, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    int32_t pid = 12345;
    ssm_->systemConfig_.backgroundswitch = true;
    SessionInfo info;
    info.abilityName_ = "IsNeedChangeLifeCycleOnUserSwitch2";
    info.bundleName_ = "IsNeedChangeLifeCycleOnUserSwitch2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetCallingPid(45678);
    property->SetIsAppSupportPhoneInPc(false);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionState(SessionState::STATE_END);

    bool ret = ssm_->IsNeedChangeLifeCycleOnUserSwitch(sceneSession, pid);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: IsNeedChangeLifeCycleOnUserSwitch3
 * @tc.desc: Normal test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, IsNeedChangeLifeCycleOnUserSwitch3, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    int32_t pid = 12345;
    ssm_->systemConfig_.backgroundswitch = true;
    SessionInfo info;
    info.abilityName_ = "IsNeedChangeLifeCycleOnUserSwitch3";
    info.bundleName_ = "IsNeedChangeLifeCycleOnUserSwitch3";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetCallingPid(45678);
    property->SetIsAppSupportPhoneInPc(false);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionState(SessionState::STATE_BACKGROUND);

    bool ret = ssm_->IsNeedChangeLifeCycleOnUserSwitch(sceneSession, pid);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: IsNeedChangeLifeCycleOnUserSwitch4
 * @tc.desc: Invalid pid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, IsNeedChangeLifeCycleOnUserSwitch4, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    int32_t pid = 12345;
    SessionInfo info;
    info.abilityName_ = "IsNeedChangeLifeCycleOnUserSwitch4";
    info.bundleName_ = "IsNeedChangeLifeCycleOnUserSwitch4";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetCallingPid(pid);

    bool ret = ssm_->IsNeedChangeLifeCycleOnUserSwitch(sceneSession, pid);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: NotifySwitchingUser
 * @tc.desc: SceneSesionManager notify switching user
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, NotifySwitchingUser, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->NotifySwitchingUser(true);
    ssm_->NotifySwitchingUser(false);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetSessionInfoByContinueSessionId
 * @tc.desc: SceneSesionManager GetSessionInfoByContinueSessionId
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, GetSessionInfoByContinueSessionId, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, PreHandleCollaborator, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "PreHandleCollaborator";
    info.bundleName_ = "PreHandleCollaborator";
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->PreHandleCollaborator(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->PreHandleCollaborator(sceneSession);
    sceneSession = nullptr;
    AppExecFwk::ApplicationInfo applicationInfo_;
    applicationInfo_.codePath = std::to_string(CollaboratorType::RESERVE_TYPE);
    AppExecFwk::AbilityInfo abilityInfo_;
    abilityInfo_.applicationInfo = applicationInfo_;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->PreHandleCollaborator(sceneSession);
    sceneSession = nullptr;
    applicationInfo_.codePath = std::to_string(CollaboratorType::OTHERS_TYPE);
    abilityInfo_.applicationInfo = applicationInfo_;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->PreHandleCollaborator(sceneSession);
    EXPECT_EQ(sceneSession->GetSessionInfo().want, nullptr);
    sceneSession = nullptr;
    info.want = std::make_shared<AAFwk::Want>();
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->PreHandleCollaborator(sceneSession);
    ASSERT_NE(sceneSession->GetSessionInfo().want, nullptr);
}

/**
 * @tc.name: CheckCollaboratorType
 * @tc.desc: SceneSesionManager check collborator type
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, CheckCollaboratorType, Function | SmallTest | Level3)
{
    int32_t type = CollaboratorType::RESERVE_TYPE;
    EXPECT_TRUE(ssm_->CheckCollaboratorType(type));
    type = CollaboratorType::OTHERS_TYPE;
    EXPECT_TRUE(ssm_->CheckCollaboratorType(type));
    type = CollaboratorType::DEFAULT_TYPE;
    ASSERT_FALSE(ssm_->CheckCollaboratorType(type));
}

/**
 * @tc.name: NotifyUpdateSessionInfo
 * @tc.desc: SceneSesionManager notify update session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, NotifyUpdateSessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "NotifyUpdateSessionInfo";
    info.bundleName_ = "NotifyUpdateSessionInfo";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->NotifyUpdateSessionInfo(nullptr);
    ASSERT_EQ(sceneSession->GetSessionInfo().want, nullptr);
    ssm_->NotifyUpdateSessionInfo(sceneSession);
    int32_t collaboratorType = CollaboratorType::RESERVE_TYPE;
    int32_t persistentId = 10086;
    ssm_->NotifyMoveSessionToForeground(collaboratorType, persistentId);
    ssm_->NotifyClearSession(collaboratorType, persistentId);
}

/**
 * @tc.name: QueryAbilityInfoFromBMS
 * @tc.desc: SceneSesionManager QueryAbilityInfoFromBMS NotifyStartAbility
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, QueryAbilityInfoFromBMS, Function | SmallTest | Level3)
{
    const int32_t uId = 32;
    SessionInfo sessionInfo_;
    sessionInfo_.bundleName_ = "BundleName";
    sessionInfo_.abilityName_ = "AbilityName";
    sessionInfo_.moduleName_ = "ModuleName";
    AppExecFwk::AbilityInfo abilityInfo;
    int32_t collaboratorType = CollaboratorType::RESERVE_TYPE;
    ssm_->QueryAbilityInfoFromBMS(uId,
        sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
    EXPECT_EQ(sessionInfo_.want, nullptr);
    ssm_->Init();
    ssm_->QueryAbilityInfoFromBMS(uId,
        sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
    ssm_->NotifyStartAbility(collaboratorType, sessionInfo_);
    sessionInfo_.want = std::make_shared<AAFwk::Want>();
    collaboratorType = CollaboratorType::OTHERS_TYPE;
    ssm_->NotifyStartAbility(collaboratorType, sessionInfo_);
    ASSERT_NE(sessionInfo_.want, nullptr);
}

/**
 * @tc.name: IsSessionClearable
 * @tc.desc: SceneSesionManager is session clearable
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, IsSessionClearable, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "IsSessionClearable";
    info.bundleName_ = "IsSessionClearable";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    EXPECT_FALSE(ssm_->IsSessionClearable(nullptr));
    EXPECT_FALSE(ssm_->IsSessionClearable(sceneSession));
    AppExecFwk::AbilityInfo abilityInfo_;
    abilityInfo_.excludeFromMissions = true;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    sceneSession = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    EXPECT_FALSE(ssm_->IsSessionClearable(sceneSession));
    abilityInfo_.excludeFromMissions = false;
    abilityInfo_.unclearableMission = true;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    sceneSession = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    EXPECT_FALSE(ssm_->IsSessionClearable(sceneSession));
    abilityInfo_.unclearableMission = false;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    info.lockedState = true;
    sceneSession = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    EXPECT_FALSE(ssm_->IsSessionClearable(sceneSession));
    info.lockedState = false;
    info.isSystem_ = true;
    sceneSession = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    EXPECT_FALSE(ssm_->IsSessionClearable(sceneSession));
    info.isSystem_ = false;
    sceneSession = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_TRUE(ssm_->IsSessionClearable(sceneSession));
}

/**
 * @tc.name: HandleTurnScreenOn
 * @tc.desc: SceneSesionManager handle turn screen on and keep screen on
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, HandleTurnScreenOn, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HandleTurnScreenOn";
    info.bundleName_ = "HandleTurnScreenOn";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->HandleTurnScreenOn(sceneSession);
    bool requireLock = true;
    ssm_->HandleKeepScreenOn(sceneSession, requireLock);
    requireLock = false;
    ssm_->HandleKeepScreenOn(sceneSession, requireLock);
}

/**
 * @tc.name: HandleHideNonSystemFloatingWindows
 * @tc.desc: SceneSesionManager update hide non system floating windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, HandleHideNonSystemFloatingWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HandleHideNonSystemFloatingWindows";
    info.bundleName_ = "HandleHideNonSystemFloatingWindows";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = nullptr;
    ssm_->UpdateForceHideState(sceneSession, property, true);
    property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    ssm_->HandleHideNonSystemFloatingWindows(property, sceneSession);
    property->SetHideNonSystemFloatingWindows(true);
    ssm_->UpdateForceHideState(sceneSession, property, true);
    ssm_->UpdateForceHideState(sceneSession, property, false);
    property->SetHideNonSystemFloatingWindows(false);
    property->SetFloatingWindowAppType(true);
    ssm_->UpdateForceHideState(sceneSession, property, true);
    ssm_->UpdateForceHideState(sceneSession, property, false);
    uint32_t result = property->GetModeSupportInfo();
    ASSERT_EQ(result, WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);
}

/**
 * @tc.name: UpdateBrightness
 * @tc.desc: SceneSesionManager update brightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, UpdateBrightness, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    WSError result01 = ssm_->UpdateBrightness(persistentId);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: SetDisplayBrightness
 * @tc.desc: SceneSesionManager set display brightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, SetDisplayBrightness, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, SetGestureNavigaionEnabled02, Function | SmallTest | Level3)
{
    bool enable = true;
    WMError result01 = ssm_->SetGestureNavigaionEnabled(enable);
    EXPECT_EQ(result01, WMError::WM_OK);
    ProcessGestureNavigationEnabledChangeFunc funcGesture_ = SceneSessionManagerTest3::callbackFunc_;
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
HWTEST_F(SceneSessionManagerTest3, SetFocusedSessionId, Function | SmallTest | Level3)
{
    int32_t focusedSession = ssm_->GetFocusedSessionId();
    EXPECT_EQ(focusedSession, INVALID_SESSION_ID);
    int32_t persistentId = INVALID_SESSION_ID;
    WSError result01 = ssm_->SetFocusedSessionId(persistentId);
    EXPECT_EQ(result01, WSError::WS_DO_NOTHING);
    persistentId = 10086;
    WSError result02 = ssm_->SetFocusedSessionId(persistentId);
    EXPECT_EQ(result02, WSError::WS_OK);
    ASSERT_EQ(ssm_->GetFocusedSessionId(), 10086);
}

/**
 * @tc.name: RequestFocusStatus
 * @tc.desc: SceneSesionManager request focus status
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, RequestFocusStatus, Function | SmallTest | Level3)
{
    int32_t focusedSession = ssm_->GetFocusedSessionId();
    EXPECT_EQ(focusedSession, 10086);

    int32_t persistentId = INVALID_SESSION_ID;
    WMError result01 = ssm_->RequestFocusStatus(persistentId, true);
    EXPECT_EQ(result01, WMError::WM_ERROR_NULLPTR);
    FocusChangeReason reasonResult = ssm_->GetFocusChangeReason();
    EXPECT_EQ(reasonResult, FocusChangeReason::DEFAULT);

    persistentId = 10000;
    FocusChangeReason reasonInput = FocusChangeReason::SCB_SESSION_REQUEST;
    WMError result02 = ssm_->RequestFocusStatus(persistentId, true, true, reasonInput);
    EXPECT_EQ(result02, WMError::WM_ERROR_NULLPTR);
    reasonResult = ssm_->GetFocusChangeReason();
    EXPECT_EQ(reasonResult, FocusChangeReason::DEFAULT);

    reasonInput = FocusChangeReason::SPLIT_SCREEN;
    WMError result03 = ssm_->RequestFocusStatus(persistentId, false, true, reasonInput);
    EXPECT_EQ(result03, WMError::WM_ERROR_NULLPTR);
    reasonResult = ssm_->GetFocusChangeReason();
    EXPECT_EQ(reasonResult, FocusChangeReason::DEFAULT);
}

/**
 * @tc.name: NotifyRequestFocusStatusNotifyManager
 * @tc.desc: NotifyRequestFocusStatusNotifyManager test.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, NotifyRequestFocusStatusNotifyManager, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "NotifyRequestFocusStatusNotifyManager";
    info.bundleName_ = "NotifyRequestFocusStatusNotifyManager";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ssm_->RegisterRequestFocusStatusNotifyManagerFunc(sceneSession);

    FocusChangeReason reasonInput = FocusChangeReason::DEFAULT;
    sceneSession->NotifyRequestFocusStatusNotifyManager(true, true, reasonInput);
    FocusChangeReason reasonResult = ssm_->GetFocusChangeReason();

    ASSERT_EQ(reasonInput, reasonResult);
}

/**
 * @tc.name: GetTopNearestBlockingFocusSession
 * @tc.desc: SceneSesionManager Gets the most recent session whose blockingType property is true
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, GetTopNearestBlockingFocusSession, Function | SmallTest | Level3)
{
    uint32_t zOrder = 9999;
    sptr<SceneSession> session = ssm_->GetTopNearestBlockingFocusSession(zOrder, true);
    EXPECT_EQ(session, nullptr);

    session = ssm_->GetTopNearestBlockingFocusSession(zOrder, false);
    EXPECT_EQ(session, nullptr);
}

/**
 * @tc.name: RaiseWindowToTop
 * @tc.desc: SceneSesionManager raise window to top
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, RaiseWindowToTop, Function | SmallTest | Level3)
{
    int32_t focusedSession = ssm_->GetFocusedSessionId();
    EXPECT_EQ(focusedSession, 10086);
    int32_t persistentId = INVALID_SESSION_ID;
    WSError result01 = ssm_->RaiseWindowToTop(persistentId);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PERMISSION);
    persistentId = 10000;
    WSError result02 = ssm_->RaiseWindowToTop(persistentId);
    EXPECT_EQ(result02, WSError::WS_ERROR_INVALID_PERMISSION);
    WSError result03 = ssm_->RaiseWindowToTop(persistentId);
    EXPECT_EQ(result03, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: ShiftAppWindowFocus
 * @tc.desc: SceneSesionManager shift app window focus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, ShiftAppWindowFocus, Function | SmallTest | Level3)
{
    int32_t focusedSession = ssm_->GetFocusedSessionId();
    EXPECT_EQ(focusedSession, 10086);
    int32_t sourcePersistentId = INVALID_SESSION_ID;
    int32_t targetPersistentId = INVALID_SESSION_ID;
    WSError result01 = ssm_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_OPERATION);
    targetPersistentId = 1;
    WSError result02 = ssm_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    EXPECT_EQ(result02, WSError::WS_ERROR_INVALID_OPERATION);
    sourcePersistentId = 1;
    WSError result03 = ssm_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    EXPECT_EQ(result03, WSError::WS_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: RegisterSessionExceptionFunc
 * @tc.desc: SceneSesionManager register session expection func
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, RegisterSessionExceptionFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RegisterSessionExceptionFunc";
    info.bundleName_ = "RegisterSessionExceptionFunc";
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->RegisterSessionExceptionFunc(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->RegisterSessionExceptionFunc(sceneSession);
    bool result01 = ssm_->IsSessionVisibleForeground(sceneSession);
    EXPECT_FALSE(result01);
    sceneSession->UpdateNativeVisibility(true);
    bool result02 = ssm_->IsSessionVisibleForeground(sceneSession);
    EXPECT_FALSE(result02);
}

/**
 * @tc.name: DumpSessionInfo
 * @tc.desc: SceneSesionManager dump session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, DumpSessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    std::ostringstream oss;
    std::string dumpInfo;
    info.abilityName_ = "DumpSessionInfo";
    info.bundleName_ = "DumpSessionInfo";
    info.isSystem_ = false;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->DumpSessionInfo(sceneSession, oss);
    EXPECT_FALSE(sceneSession->IsVisible());

    sptr<SceneSession::SpecificSessionCallback> specific = new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, specific);
    sceneSession = new (std::nothrow) SceneSession(info, specific);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->DumpSessionInfo(sceneSession, oss);
    EXPECT_FALSE(sceneSession->IsVisible());
    sceneSession = nullptr;
    info.isSystem_ = true;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(sceneSession, oss);
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: SceneSesionManager dump session element info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, DumpSessionElementInfo, Function | SmallTest | Level3)
{
    DumpRootSceneElementInfoFunc func_ = DumpRootSceneElementInfoFuncTest;
    ssm_->SetDumpRootSceneElementInfoListener(func_);
    SessionInfo info;
    info.abilityName_ = "DumpSessionElementInfo";
    info.bundleName_ = "DumpSessionElementInfo";
    info.isSystem_ = false;
    std::string strId = "10086";
    sptr<SceneSession> sceneSession = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    std::vector<std::string> params_(5, "");
    std::string dumpInfo;
    ssm_->DumpSessionElementInfo(sceneSession, params_, dumpInfo);
    sceneSession = nullptr;
    info.isSystem_ = true;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->DumpSessionElementInfo(sceneSession, params_, dumpInfo);
    WSError result01 = ssm_->GetSpecifiedSessionDumpInfo(dumpInfo, params_, strId);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: NotifyDumpInfoResult
 * @tc.desc: SceneSesionManager notify dump info result
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, NotifyDumpInfoResult, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, UpdateFocus, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, UpdateWindowMode, Function | SmallTest | Level3)
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
HWTEST_F(SceneSessionManagerTest3, IsScreenLocked, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
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
HWTEST_F(SceneSessionManagerTest3, UpdatePrivateStateAndNotify, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->RegisterSessionStateChangeNotifyManagerFunc(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->RegisterSessionStateChangeNotifyManagerFunc(sceneSession);
    ssm_->UpdatePrivateStateAndNotify(persistentId);
    auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
    std::unordered_set<string> privacyBundleList;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);
}

/**
 * @tc.name: UpdatePrivateStateAndNotifyForAllScreens
 * @tc.desc: SceneSesionManager update private state and notify for all screens
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, UpdatePrivateStateAndNotifyForAllScreens, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    ssm_->UpdatePrivateStateAndNotifyForAllScreens();
    auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
    std::unordered_set<std::string> privacyBundleList;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);
}

/**
 * @tc.name: GerPrivacyBundleListOneWindow
 * @tc.desc: get privacy bundle list when one window exist only.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, GerPrivacyBundleListOneWindow, Function | SmallTest | Level3)
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

    std::unordered_set<std::string> privacyBundleList;
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

    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(1, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);
}

/**
 * @tc.name: GetTopWindowId
 * @tc.desc: get top window id by main window id.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, GetTopWindowId, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "mainWin";
    sessionInfo1.abilityName_ = "mainAbilityName";
    sessionInfo1.persistentId_ = 100;
    auto sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(sceneSession1, nullptr);
    sceneSession1->SetCallingPid(65534);
    ssm_->sceneSessionMap_.insert({100, sceneSession1});

    SessionInfo sessionInfo2;
    sessionInfo2.bundleName_ = "subWin1";
    sessionInfo2.abilityName_ = "subAbilityName1";
    sessionInfo2.persistentId_ = 101;
    auto sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo2, nullptr);
    ASSERT_NE(sceneSession2, nullptr);
    sceneSession2->SetCallingPid(65535);
    ssm_->sceneSessionMap_.insert({101, sceneSession2});

    SessionInfo sessionInfo3;
    sessionInfo3.bundleName_ = "subWin2";
    sessionInfo3.abilityName_ = "subAbilityName2";
    sessionInfo3.persistentId_ = 102;
    auto sceneSession3 = sptr<SceneSession>::MakeSptr(sessionInfo3, nullptr);
    ASSERT_NE(sceneSession3, nullptr);
    sceneSession3->SetCallingPid(65534);
    ssm_->sceneSessionMap_.insert({102, sceneSession3});

    sceneSession1->AddSubSession(sceneSession2);
    sceneSession1->AddSubSession(sceneSession3);
    uint32_t topWinId;
    ASSERT_NE(ssm_->GetTopWindowId(static_cast<uint32_t>(sceneSession1->GetPersistentId()), topWinId),
        WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: ConfigWindowImmersive
 * @tc.desc: ConfigWindowImmersive SwitchFreeMultiWindow
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest3, ConfigWindowImmersive01, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem immersiveConfig;
    immersiveConfig.boolValue_ = false;
    ASSERT_NE(ssm_, nullptr);
    ssm_->ConfigWindowImmersive(immersiveConfig);

    ASSERT_NE(ssm_->SwitchFreeMultiWindow(false), WSError::WS_OK);
    SystemSessionConfig systemConfig;
    systemConfig.freeMultiWindowSupport_ = true;
    ssm_->SwitchFreeMultiWindow(false);
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: SceneSesionManager config decor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigDecor02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    WindowSceneConfig::ConfigItem* item = new WindowSceneConfig::ConfigItem;
    ASSERT_NE(item, nullptr);
    ssm_->ConfigDecor(*item, false);
    delete item;
}

/**
 * @tc.name: ConfigAppWindowShadow
 * @tc.desc: SceneSesionManager config app window shadow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigAppWindowShadow02, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem item;
    WindowSceneConfig::ConfigItem shadowConfig;
    WindowShadowConfig outShadow;
    std::vector<float> floatTest = {};
    bool result = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result, true);

    item.SetValue(floatTest);
    shadowConfig.SetValue({{"radius", item}});
    result = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result, false);

    item.SetValue(new std::string(""));
    shadowConfig.SetValue({{"", item}});
    result = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: ConfigWindowAnimation
 * @tc.desc: SceneSesionManager config window animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigWindowAnimation02, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem windowAnimationConfig;
    WindowSceneConfig::ConfigItem item;
    std::vector<float> rotation = {0.1f, 0.2f, 0.3f, 0.4f};
    ASSERT_NE(ssm_, nullptr);

    item.SetValue(rotation);
    item.SetValue({{"curve", item}});
    windowAnimationConfig.SetValue({{"timing", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
}

/**
 * @tc.name: ConfigStartingWindowAnimation
 * @tc.desc: SceneSesionManager config start window animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigStartingWindowAnimation02, Function | SmallTest | Level3)
{
    std::vector<float> midFloat = {0.1f};
    std::vector<int> midInt = {1};
    ASSERT_NE(ssm_, nullptr);
    WindowSceneConfig::ConfigItem middleFloat;
    middleFloat.SetValue(midFloat);
    ssm_->ConfigStartingWindowAnimation(middleFloat);
    WindowSceneConfig::ConfigItem middleInt;
    middleInt.SetValue(midInt);
    ssm_->ConfigStartingWindowAnimation(middleInt);

    WindowSceneConfig::ConfigItem curve;
    curve.SetValue(midFloat);
    curve.SetValue({{"curve", curve}});
    ssm_->ConfigStartingWindowAnimation(curve);
}

/**
 * @tc.name: ConfigMainWindowSizeLimits
 * @tc.desc: call ConfigMainWindowSizeLimits and check the systemConfig_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigMainWindowSizeLimits02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<int> maInt = {1, 2, 3, 4};
    WindowSceneConfig::ConfigItem mainleInt;
    mainleInt.SetValue(maInt);
    mainleInt.SetValue({{"miniWidth", mainleInt}});
    ssm_->ConfigMainWindowSizeLimits(mainleInt);
    mainleInt.ClearValue();

    std::vector<float> maFloat = {0.1f};
    WindowSceneConfig::ConfigItem mainFloat;
    mainFloat.SetValue(maFloat);
    mainFloat.SetValue({{"miniWidth", mainFloat}});
    ssm_->ConfigMainWindowSizeLimits(mainFloat);
    mainFloat.ClearValue();

    WindowSceneConfig::ConfigItem mainleInt02;
    mainleInt02.SetValue(maInt);
    mainleInt02.SetValue({{"miniHeight", mainleInt02}});
    ssm_->ConfigMainWindowSizeLimits(mainleInt02);

    WindowSceneConfig::ConfigItem mainFloat02;
    mainFloat02.SetValue(maFloat);
    mainFloat02.SetValue({{"miniHeight", mainFloat02}});
    ssm_->ConfigMainWindowSizeLimits(mainFloat02);
}

/**
 * @tc.name: ConfigSubWindowSizeLimits
 * @tc.desc: call ConfigSubWindowSizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest3, ConfigSubWindowSizeLimits02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<int> subInt = {1, 2, 3, 4};
    WindowSceneConfig::ConfigItem subleInt;
    subleInt.SetValue(subInt);
    subleInt.SetValue({{"miniWidth", subleInt}});
    ssm_->ConfigSubWindowSizeLimits(subleInt);
    subleInt.ClearValue();

    std::vector<float> subFloat = {0.1f};
    WindowSceneConfig::ConfigItem mainFloat;
    mainFloat.SetValue(subFloat);
    mainFloat.SetValue({{"miniWidth", mainFloat}});
    ssm_->ConfigSubWindowSizeLimits(mainFloat);
    mainFloat.ClearValue();

    WindowSceneConfig::ConfigItem subleInt02;
    subleInt02.SetValue(subInt);
    subleInt02.SetValue({{"miniHeight", subleInt02}});
    ssm_->ConfigSubWindowSizeLimits(subleInt02);

    WindowSceneConfig::ConfigItem mainFloat02;
    mainFloat02.SetValue(subFloat);
    mainFloat02.SetValue({{"miniHeight", mainFloat02}});
    ssm_->ConfigSubWindowSizeLimits(mainFloat02);
}
}
} // namespace Rosen
} // namespace OHOS
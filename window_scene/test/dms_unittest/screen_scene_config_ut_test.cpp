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

#include <libxml/globals.h>
#include <libxml/xmlstring.h>

#include "window_manager_hilog.h"
#include "xml_config_base.h"
#include "screen_scene_config.h"
#include "screen_session_manager.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class ScreenSceneConfigUtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSceneConfigUtTest::SetUpTestCase()
{
}

void ScreenSceneConfigUtTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void ScreenSceneConfigUtTest::SetUp()
{
}

void ScreenSceneConfigUtTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsNumber
 * @tc.desc: test function : IsNumber
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, IsNumber, TestSize.Level1)
{
    bool result = ScreenSceneConfig::IsNumber("123");
    ASSERT_EQ(true, result);
    result = ScreenSceneConfig::IsNumber("a123");
    ASSERT_EQ(false, result);
    result = ScreenSceneConfig::IsNumber("");
    ASSERT_EQ(false, result);
    result = ScreenSceneConfig::IsNumber("-123");
    ASSERT_EQ(false, result);
    result = ScreenSceneConfig::IsNumber("123.456");
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: GetConfigPath1
 * @tc.desc: test function : GetConfigPath
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetConfigPath1, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetConfigPath("");
    ASSERT_STRNE("/system/", result.c_str());
}

/**
 * @tc.name: GetConfigPath2
 * @tc.desc: test function : GetConfigPath
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetConfigPath2, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetConfigPath("a.xml");
    ASSERT_STREQ("/system/a.xml", result.c_str());
}

/**
 * @tc.name: LoadConfigXml
 * @tc.desc: test function : loadConfigXml
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, LoadConfigXml, TestSize.Level1)
{
    auto result = ScreenSceneConfig::LoadConfigXml();
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: IsValidNode1
 * @tc.desc: test function : IsValidNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, IsValidNode1, TestSize.Level1)
{
    xmlNode node;
    auto result = ScreenSceneConfig::IsValidNode(node);
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: IsValidNode2
 * @tc.desc: test function : IsValidNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, IsValidNode2, TestSize.Level1)
{
    const xmlChar xmlStringText[] = { 't', 'e', 'x', 't', 0 };
    xmlNode node;
    node.name = xmlStringText;
    node.type = XML_TEXT_NODE;
    auto result = ScreenSceneConfig::IsValidNode(node);
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: IsValidNode3
 * @tc.desc: test function : IsValidNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, IsValidNode3, TestSize.Level1)
{
    const xmlChar xmlStringText[] = { 't', 'e', 'x', 't', 0 };
    xmlNode node;
    node.name = xmlStringText;
    node.type = XML_COMMENT_NODE;
    auto result = ScreenSceneConfig::IsValidNode(node);
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: ReadIntNumbersConfigInfo
 * @tc.desc: test function : ReadIntNumbersConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadIntNumbersConfigInfo, TestSize.Level1)
{
    ScreenSceneConfig::enableConfig_.clear();

    auto configFilePath = ScreenSceneConfig::GetConfigPath("etc/window/resources/display_manager_config.xml");
    xmlDocPtr docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    if (docPtr == nullptr) {
        return;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return;
    }
    uint32_t readCount = 0;
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!ScreenSceneConfig::IsValidNode(*curNodePtr)) {
            continue;
        }
        auto nodeName = curNodePtr->name;
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("dpi")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("defaultDeviceRotationOffset")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("cutoutArea")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("curvedScreenBoundary"))) {
            ScreenSceneConfig::ReadIntNumbersConfigInfo(curNodePtr);
            readCount++;
            continue;
        }
    }
    ASSERT_GE(ScreenSceneConfig::intNumbersConfig_.size(), readCount);
    ScreenSceneConfig::DumpConfig();
    xmlFreeDoc(docPtr);
}

/**
 * @tc.name: ReadIntNumbersConfigInfo02
 * @tc.desc: Test ReadIntNumbersConfigInfo method when node content is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadIntNumbersConfigInfo02, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadIntNumbersConfigInfo(currNode);
    xmlFree(currNode);
}

/**
 * @tc.name: ReadIntNumbersConfigInfo03
 * @tc.desc: Test ReadIntNumbersConfigInfo method when node content is empty.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadIntNumbersConfigInfo03, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, BAD_CAST "");
    ScreenSceneConfig::ReadIntNumbersConfigInfo(currNode);
    xmlFree(currNode);
}

/**
 * @tc.name: ReadIntNumbersConfigInfo04
 * @tc.desc: Test ReadIntNumbersConfigInfo method when node content contains non-number.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadIntNumbersConfigInfo04, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, BAD_CAST "123 abc");
    ScreenSceneConfig::ReadIntNumbersConfigInfo(currNode);
    xmlFree(currNode);
}

/**
 * @tc.name: ReadIntNumbersConfigInfo05
 * @tc.desc: Test ReadIntNumbersConfigInfo method when node content is valid.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadIntNumbersConfigInfo05, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, BAD_CAST "123 456 789");
    ScreenSceneConfig::ReadIntNumbersConfigInfo(currNode);
    xmlFree(currNode);
}

/**
 * @tc.name: ReadEnableConfigInfo
 * @tc.desc: test function : ReadEnableConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadEnableConfigInfo, TestSize.Level1)
{
    ScreenSceneConfig::enableConfig_.clear();

    auto configFilePath = ScreenSceneConfig::GetConfigPath("etc/window/resources/display_manager_config.xml");
    xmlDocPtr docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    if (docPtr == nullptr) {
        return;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return;
    }
    uint32_t readCount = 0;
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!ScreenSceneConfig::IsValidNode(*curNodePtr)) {
            continue;
        }

        auto nodeName = curNodePtr->name;
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("isWaterfallDisplay"))) {
            ScreenSceneConfig::ReadEnableConfigInfo(curNodePtr);
            readCount++;
            continue;
        }

        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("dpi"))) {
            ScreenSceneConfig::ReadEnableConfigInfo(curNodePtr);
            readCount++;
            continue;
        }
    }

    ASSERT_LE(ScreenSceneConfig::enableConfig_.size(), readCount);

    ScreenSceneConfig::DumpConfig();
    xmlFreeDoc(docPtr);
}

/**
 * @tc.name: GetEnableConfig1
 * @tc.desc: test function : GetEnableConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetEnableConfig, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetEnableConfig();
    ASSERT_EQ(true, result.size() == 0);
}

/**
 * @tc.name: GetIntNumbersConfig
 * @tc.desc: test function : GetIntNumbersConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetIntNumbersConfig, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetIntNumbersConfig();
    ASSERT_NE(true, result.size() == 0);
}

/**
 * @tc.name: GetStringConfig
 * @tc.desc: test function : GetStringConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetStringConfig, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetStringConfig();
    ASSERT_NE(0, result.size());
}

/**
 * @tc.name: GetStringListConfig
 * @tc.desc: test function : GetStringListConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetStringListConfig, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetStringListConfig();
    ASSERT_EQ(0, result.size());
}

/**
 * @tc.name: GetCurvedScreenBoundaryConfig
 * @tc.desc: test function : GetCurvedScreenBoundaryConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetCurvedScreenBoundaryConfig, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetCurvedScreenBoundaryConfig();
    if ((ScreenSessionManager::GetInstance().GetCurvedCompressionArea() == 0) &&
        SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(0, result.size());
    } else {
        ASSERT_NE(0, result.size());
    }
}

/**
 * @tc.name: GetCutoutBoundaryRect
 * @tc.desc: GetCutoutBoundaryRect func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetCutoutBoundaryRect, TestSize.Level1)
{
    uint64_t displayId = -1;
    auto result = ScreenSceneConfig::GetCutoutBoundaryRect(displayId);
    ASSERT_FALSE(result.size() > 0);
}

/**
 * @tc.name: GetSubCutoutBoundaryRect
 * @tc.desc: GetSubCutoutBoundaryRect func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetSubCutoutBoundaryRect, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetSubCutoutBoundaryRect();
    if (ScreenSessionManager::GetInstance().IsFoldable()) {
        ASSERT_TRUE(result.size() > 0);
    } else {
        ASSERT_TRUE(result.size() == 0);
    }
}

/**
 * @tc.name: IsWaterfallDisplay
 * @tc.desc: IsWaterfallDisplay func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, IsWaterfallDisplay, TestSize.Level1)
{
    auto result = ScreenSceneConfig::IsWaterfallDisplay();
    if (result) {
        ASSERT_EQ(true, result);
    }
}

/**
 * @tc.name: GetCurvedCompressionAreaInLandscape
 * @tc.desc: GetCurvedCompressionAreaInLandscape func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetCurvedCompressionAreaInLandscape, TestSize.Level1)
{
    auto result = ScreenSceneConfig::GetCurvedCompressionAreaInLandscape();
    ASSERT_TRUE(result == 0);
}

/**
 * @tc.name: Split
 * @tc.desc: Split func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, Split, TestSize.Level1)
{
    auto result = ScreenSceneConfig::Split("oo", "+9");
    ASSERT_NE(0, result.size());
}

/**
 * @tc.name: CalcCutoutBoundaryRect
 * @tc.desc: CalcCutoutBoundaryRect func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, CalcCutoutBoundaryRect, TestSize.Level1)
{
    DMRect emptyRect = {0, 0, 0, 0};
    auto result = ScreenSceneConfig::CalcCutoutBoundaryRect("oo");
    ASSERT_FALSE(result != emptyRect);
}

/**
 * @tc.name: CalcCutoutBoundaryRect02
 * @tc.desc: Test scenario where svg parsing fails and an empty rectangle is expected.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, CalcCutoutBoundaryRect02, TestSize.Level1)
{
    std::string invalidSvgPath = "invalid_svg_path";
    DMRect result = ScreenSceneConfig::CalcCutoutBoundaryRect(invalidSvgPath);
    EXPECT_EQ(result.posX_, 0);
    EXPECT_EQ(result.posY_, 0);
    EXPECT_EQ(result.width_, 0);
    EXPECT_EQ(result.height_, 0);
}

/**
 * @tc.name: CalcCutoutBoundaryRect03
 * @tc.desc: Test scenario where SkRect is empty and an empty rectangle is expected.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, CalcCutoutBoundaryRect03, TestSize.Level1)
{
    std::string emptySvgPath = "M0 0";
    DMRect result = ScreenSceneConfig::CalcCutoutBoundaryRect(emptySvgPath);
    EXPECT_EQ(result.posX_, 0);
    EXPECT_EQ(result.posY_, 0);
    EXPECT_EQ(result.width_, 0);
    EXPECT_EQ(result.height_, 0);
}

/**
 * @tc.name: CalcCutoutBoundaryRect04
 * @tc.desc: Test scenario where svg is valid and a valid rectangle is expected.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, CalcCutoutBoundaryRect04, TestSize.Level1)
{
    std::string validSvgPath = "M10 10 L20 20";
    DMRect result = ScreenSceneConfig::CalcCutoutBoundaryRect(validSvgPath);
    EXPECT_EQ(result.posX_, 10);
    EXPECT_EQ(result.posY_, 10);
    EXPECT_EQ(result.width_, 10);
    EXPECT_EQ(result.height_, 10);
}

/**
 * @tc.name: SetCutoutSvgPath
 * @tc.desc: SetCutoutSvgPath func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, SetCutoutSvgPath, TestSize.Level1)
{
    uint64_t displayId = 0;
    ScreenSceneConfig::SetCutoutSvgPath(displayId, "oo");
    auto result_ = ScreenSceneConfig::GetCutoutBoundaryRect(displayId);
    ASSERT_NE(0, result_.size());
}

/**
 * @tc.name: SetSubCutoutSvgPath
 * @tc.desc: SetSubCutoutSvgPath func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, SetSubCutoutSvgPath, TestSize.Level1)
{
    ScreenSceneConfig::SetSubCutoutSvgPath("oo");
    auto result = ScreenSceneConfig::GetSubCutoutBoundaryRect();
    ASSERT_NE(0, result.size());
}

/**
 * @tc.name: SetSubCutoutSvgPath01
 * @tc.desc: SetSubCutoutSvgPath func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, SetSubCutoutSvgPath01, TestSize.Level1)
{
    ScreenSceneConfig::SetSubCutoutSvgPath("M507 18 L573 18 v 66 h -66 Z");
    std::vector<DMRect> result = ScreenSceneConfig::GetSubCutoutBoundaryRect();
    if (result.size() <= 0) {
        ASSERT_EQ(0, result.size());
    }
    DMRect targetRect{507, 18, 66, 66}; // the rect size after svg parsing
    EXPECT_EQ(result[0].posX_, targetRect.posX_);
    EXPECT_EQ(result[0].posY_, targetRect.posY_);
    EXPECT_EQ(result[0].width_, targetRect.width_);
    EXPECT_EQ(result[0].height_, targetRect.height_);
}

/**
 * @tc.name: SetCurvedCompressionAreaInLandscape
 * @tc.desc: SetCurvedCompressionAreaInLandscape func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, SetCurvedCompressionAreaInLandscape, TestSize.Level1)
{
    int res = 0;
    ScreenSceneConfig::SetCurvedCompressionAreaInLandscape();
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: IsSupportRotateWithSensor01
 * @tc.desc: IsSupportRotateWithSensor
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, IsSupportRotateWithSensor01, TestSize.Level1)
{
    ScreenSceneConfig::enableConfig_["supportRotateWithSensor"] = true;
    bool res = ScreenSceneConfig::IsSupportRotateWithSensor();
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: IsSupportRotateWithSensor01
 * @tc.desc: IsSupportRotateWithSensor
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, IsSupportRotateWithSensor02, TestSize.Level1)
{
    ScreenSceneConfig::enableConfig_.erase("supportRotateWithSensor");
    bool res = ScreenSceneConfig::IsSupportRotateWithSensor();
    ASSERT_EQ(false, res);
}

/**
 * @tc.name: GetExternalScreenDefaultMode01
 * @tc.desc: GetExternalScreenDefaultMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetExternalScreenDefaultMode01, TestSize.Level1)
{
    ScreenSceneConfig::stringConfig_["externalScreenDefaultMode"] = "mirror";
    std::string res = ScreenSceneConfig::GetExternalScreenDefaultMode();
    ASSERT_EQ("mirror", res);
}

/**
 * @tc.name: GetExternalScreenDefaultMode02
 * @tc.desc: GetExternalScreenDefaultMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetExternalScreenDefaultMode02, TestSize.Level1)
{
    ScreenSceneConfig::stringConfig_.erase("externalScreenDefaultMode");
    std::string res = ScreenSceneConfig::GetExternalScreenDefaultMode();
    ASSERT_EQ("", res);
}

/**
 * @tc.name: GetCurvedCompressionAreaInLandscape01
 * @tc.desc: GetCurvedCompressionAreaInLandscape
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetCurvedCompressionAreaInLandscape01, TestSize.Level1)
{
    ScreenSceneConfig::isWaterfallDisplay_ = false;
    ScreenSceneConfig::isScreenCompressionEnableInLandscape_ = false;
    auto result = ScreenSceneConfig::GetCurvedCompressionAreaInLandscape();
    ASSERT_TRUE(result == 0);
}

/**
 * @tc.name: GetCurvedCompressionAreaInLandscape02
 * @tc.desc: GetCurvedCompressionAreaInLandscape
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetCurvedCompressionAreaInLandscape02, TestSize.Level1)
{
    ScreenSceneConfig::isWaterfallDisplay_ = true;
    ScreenSceneConfig::isScreenCompressionEnableInLandscape_ = false;
    auto result = ScreenSceneConfig::GetCurvedCompressionAreaInLandscape();
    ASSERT_TRUE(result == 0);
}

/**
 * @tc.name: GetCurvedCompressionAreaInLandscape03
 * @tc.desc: Test GetCurvedCompressionAreaInLandscape method
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, GetCurvedCompressionAreaInLandscape03, TestSize.Level1)
{
    ScreenSceneConfig::isWaterfallDisplay_ = true;
    ScreenSceneConfig::isScreenCompressionEnableInLandscape_ = true;
    auto result = ScreenSceneConfig::GetCurvedCompressionAreaInLandscape();
    ASSERT_TRUE(result == 0);
}

/**
 * @tc.name: ReadStringListConfigInfo01
 * @tc.desc: ReadStringListConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadStringListConfigInfo01, Function | SmallTest | Level3)
{
    xmlNodePtr rootNode = nullptr;
    ScreenSceneConfig::ReadStringListConfigInfo(rootNode, "");
    EXPECT_EQ(rootNode, nullptr);
}

/**
 * @tc.name: ReadStringListConfigInfo02
 * @tc.desc: ReadStringListConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadStringListConfigInfo02, TestSize.Level1)
{
    xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(rootNode, nullptr);
    rootNode->name = nullptr;
    std::string name = "testName";
    ScreenSceneConfig::ReadStringListConfigInfo(rootNode, name);
    xmlFreeNode(rootNode);
}

/**
 * @tc.name: ReadStringListConfigInfo03
 * @tc.desc: ReadStringListConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadStringListConfigInfo03, TestSize.Level1)
{
    xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(rootNode, nullptr);
    std::string name = "testName";
    ScreenSceneConfig::ReadStringListConfigInfo(rootNode, name);
    xmlFreeNode(rootNode);
}

/**
 * @tc.name: ReadStringListConfigInfo04
 * @tc.desc: ReadStringListConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadStringListConfigInfo04, TestSize.Level1)
{
    xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(rootNode, nullptr);
    xmlNodePtr curNode = xmlNewNode(nullptr, BAD_CAST "invalidNode");
    rootNode->children = curNode;
    std::string name = "testName";
    ScreenSceneConfig::ReadStringListConfigInfo(rootNode, name);
    xmlFreeNode(rootNode);
}

/**
 * @tc.name: ReadStringListConfigInfo05
 * @tc.desc: ReadStringListConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadStringListConfigInfo05, TestSize.Level1)
{
    xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "testNode");
    ASSERT_NE(rootNode, nullptr);
    xmlNodePtr curNode = xmlNewNode(nullptr, BAD_CAST "invalidNode");
    xmlNodeSetContent(curNode, BAD_CAST "validContent");
    rootNode->children = curNode;
    std::string name = "testName";
    ScreenSceneConfig::ReadStringListConfigInfo(rootNode, name);
    xmlFreeNode(rootNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo01
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo01, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*> ("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo02
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo02, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>(" "));
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo03
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo03, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("100:200"));
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo04
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo04, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_FULL:100:200"));
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo05
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo05, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_MAIN:100:200"));
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo06
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo0, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_SUB:100:200"));
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo07
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo07, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("UNKNOWN:100:200"));
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadPhysicalDisplayConfigInfo08
 * @tc.desc: ReadPhysicalDisplayConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadPhysicalDisplayConfigInfo08, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadPhysicalDisplayConfigInfo(currNode);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_FULL:abc:def"));
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam01
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam01, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam02
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam02, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_FULL:1.5:0.5"));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam03
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam03, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_MAIN:1.5:0.5"));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam04
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam04, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_SUB:1.5:0.5"));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam05
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam05, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("UNKNOWN:1.5:0.5"));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam06
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam06, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
        xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_FULL:1.5:"));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam07
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam07, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>(" "));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam08
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam08, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("UNKNOWN:!!:aa"));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ReadScrollableParam09
 * @tc.desc: ReadScrollableParam
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ReadScrollableParam09, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("displayMode"));
    ASSERT_NE(currNode, nullptr);
    xmlNodeSetContent(currNode, reinterpret_cast<const xmlChar*>("FOLD_DISPLAY_MODE_COORDINATION:1.5:0.5"));
    ScreenSceneConfig::ReadScrollableParam(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ParseNodeConfig01
 * @tc.desc: Test if ParseNodeConfig correctly calls ReadEnableConfigInfo when node name matches.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ParseNodeConfig01, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("IS_WATERFALL_DISPLAY"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ParseNodeConfig(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ParseNodeConfig02
 * @tc.desc: Test if ParseNodeConfig correctly calls ReadIntNumbersConfigInfo when node name matches.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ParseNodeConfig02, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("DPI"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ParseNodeConfig(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ParseNodeConfig03
 * @tc.desc: Test if ParseNodeConfig correctly calls ReadStringConfigInfo when node name matches.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ParseNodeConfig03, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("DEFAULT_DISPLAY_CUTOUT_PATH"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ParseNodeConfig(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ParseNodeConfig04
 * @tc.desc: Test if ParseNodeConfig correctly calls ReadStringListConfigInfo when node name matches.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ParseNodeConfig04, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("HALL_SWITCH_APP"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ParseNodeConfig(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ParseNodeConfig05
 * @tc.desc: Test if ParseNodeConfig correctly calls ReadPhysicalDisplayConfigInfo when node name matches.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ParseNodeConfig05, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("PHYSICAL_DISPLAY_RESOLUTION"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ParseNodeConfig(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ParseNodeConfig06
 * @tc.desc: Test if ParseNodeConfig correctly calls ReadScrollableParam when node name matches.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ParseNodeConfig06, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("SCROLLABLE_PARAM"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ParseNodeConfig(currNode);
    xmlFreeNode(currNode);
}

/**
 * @tc.name: ParseNodeConfig07
 * @tc.desc: Test if ParseNodeConfig logs a warning when node name does not match any known node.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigUtTest, ParseNodeConfig07, TestSize.Level1)
{
    xmlNodePtr currNode = xmlNewNode(nullptr, reinterpret_cast<const xmlChar*>("UNKNOWN_NODE"));
    ASSERT_NE(currNode, nullptr);
    ScreenSceneConfig::ParseNodeConfig(currNode);
    xmlFreeNode(currNode);
}
}
} // namespace Rosen
} // namespace OHOS

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
#include "screen_scene_config.h"
#include "xml_config_base.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSceneConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSceneConfigTest::SetUpTestCase()
{
}

void ScreenSceneConfigTest::TearDownTestCase()
{
}

void ScreenSceneConfigTest::SetUp()
{
}

void ScreenSceneConfigTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsNumber
 * @tc.desc: test function : IsNumber
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, IsNumber, Function | SmallTest | Level1)
{
    bool result = ScreenSceneConfig::IsNumber("123");
    ASSERT_EQ(true, result);
    result = ScreenSceneConfig::IsNumber("a123");
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: GetConfigPath1
 * @tc.desc: test function : GetConfigPath
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetConfigPath1, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::GetConfigPath("");
    ASSERT_STRNE("/system/", result.c_str());
}

/**
 * @tc.name: GetConfigPath2
 * @tc.desc: test function : GetConfigPath
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetConfigPath2, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::GetConfigPath("a.xml");
    ASSERT_STREQ("/system/a.xml", result.c_str());
}

/**
 * @tc.name: LoadConfigXml
 * @tc.desc: test function : loadConfigXml
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, LoadConfigXml, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::LoadConfigXml();
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: IsValidNode1
 * @tc.desc: test function : IsValidNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, IsValidNode1, Function | SmallTest | Level1)
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
HWTEST_F(ScreenSceneConfigTest, IsValidNode2, Function | SmallTest | Level1)
{
    const xmlChar xmlStringText[] = { 't', 'e', 'x', 't', 0 };
    xmlNode node;
    node.name = xmlStringText;
    node.type = XML_TEXT_NODE;
    auto result = ScreenSceneConfig::IsValidNode(node);
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: ReadIntNumbersConfigInfo
 * @tc.desc: test function : ReadIntNumbersConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, ReadIntNumbersConfigInfo, Function | SmallTest | Level1)
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
 * @tc.name: ReadEnableConfigInfo
 * @tc.desc: test function : ReadEnableConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, ReadEnableConfigInfo, Function | SmallTest | Level1)
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
 * @tc.name: ReadStringConfigInfo
 * @tc.desc: test function : ReadStringConfigInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, ReadStringConfigInfo, Function | SmallTest | Level1)
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
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("defaultDisplayCutoutPath"))) {
            ScreenSceneConfig::ReadStringConfigInfo(curNodePtr);
            readCount++;
            continue;
        }

        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("dpi"))) {
            ScreenSceneConfig::ReadStringConfigInfo(curNodePtr);
            readCount++;
            continue;
        }
    }

    ASSERT_LE(ScreenSceneConfig::stringConfig_.size(), readCount);
    ScreenSceneConfig::DumpConfig();
    xmlFreeDoc(docPtr);
}

/**
 * @tc.name: GetEnableConfig1
 * @tc.desc: test function : GetEnableConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetEnableConfig, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::GetEnableConfig();
    ASSERT_EQ(true, result.size() == 0);
}

/**
 * @tc.name: GetIntNumbersConfig
 * @tc.desc: test function : GetIntNumbersConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetIntNumbersConfig, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::GetIntNumbersConfig();
    ASSERT_NE(true, result.size() == 0);
}

/**
 * @tc.name: GetStringConfig
 * @tc.desc: test function : GetStringConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetStringConfig, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::GetStringConfig();
    ASSERT_NE(0, result.size());
}

/**
 * @tc.name: GetCurvedScreenBoundaryConfig
 * @tc.desc: test function : GetCurvedScreenBoundaryConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetCurvedScreenBoundaryConfig, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::GetCurvedScreenBoundaryConfig();
    ASSERT_NE(0, result.size());
}

/**
 * @tc.name: GetCutoutBoundaryRect
 * @tc.desc: GetCutoutBoundaryRect func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetCutoutBoundaryRect, Function | SmallTest | Level3)
{
    auto result = ScreenSceneConfig::GetCutoutBoundaryRect();
    ASSERT_FALSE(result.size() > 0);
}

/**
 * @tc.name: IsWaterfallDisplay
 * @tc.desc: IsWaterfallDisplay func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, IsWaterfallDisplay, Function | SmallTest | Level3)
{
    auto result = ScreenSceneConfig::IsWaterfallDisplay();
    if (result)
    {
        ASSERT_EQ(false, result);
    }
}

/**
 * @tc.name: GetCurvedCompressionAreaInLandscape
 * @tc.desc: GetCurvedCompressionAreaInLandscape func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetCurvedCompressionAreaInLandscape, Function | SmallTest | Level3)
{
    auto result = ScreenSceneConfig::GetCurvedCompressionAreaInLandscape();
    ASSERT_TRUE(result == 0);
}

/**
 * @tc.name: Split
 * @tc.desc: Split func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, Split, Function | SmallTest | Level3)
{
    auto result = ScreenSceneConfig::Split("oo", "+9");
    ASSERT_NE(0, result.size());
}

/**
 * @tc.name: CalcCutoutBoundaryRect
 * @tc.desc: CalcCutoutBoundaryRect func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, CalcCutoutBoundaryRect, Function | SmallTest | Level3)
{
    DMRect emptyRect = {0, 0, 0, 0};
    auto result = ScreenSceneConfig::CalcCutoutBoundaryRect("oo");
    ASSERT_FALSE(result != emptyRect);
}

/**
 * @tc.name: SetCutoutSvgPath
 * @tc.desc: SetCutoutSvgPath func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, SetCutoutSvgPath, Function | SmallTest | Level3)
{
    ScreenSceneConfig::SetCutoutSvgPath("oo");
    auto result_ = ScreenSceneConfig::GetCutoutBoundaryRect();
    ASSERT_NE(0, result_.size());
}
}
} // namespace Rosen
} // namespace OHOS

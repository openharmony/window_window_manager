/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSceneConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
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
 * @tc.name: GetConfigPath
 * @tc.desc: test function : GetConfigPath
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSceneConfigTest, GetConfigPath, Function | SmallTest | Level1)
{
    auto result = ScreenSceneConfig::GetConfigPath("");
    ASSERT_STRNE("/system/", result.c_str());

    result = ScreenSceneConfig::GetConfigPath("a.xml");
    ASSERT_STREQ("/system/a.xml", result.c_str());
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

}
} // namespace Rosen
} // namespace OHOS

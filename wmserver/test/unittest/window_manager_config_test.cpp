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
#include "window_manager_config.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using ConfigItem = WindowManagerConfig::ConfigItem;
const std::string XML_STR = R"(<?xml version='1.0' encoding="utf-8"?>
<Configs>
    <windowAnimation>
        <timing>
            <duration>350</duration>
            <curve name="easeOut"></curve>
        </timing>
        <scale>0.7 0.7</scale>
        <rotation>0 0 1 0</rotation>
        <translate>0 0</translate>
        <opacity>0</opacity>
    </windowAnimation>
    <!--keyboard animation config-->
    <keyboardAnimation>
        <timing>
            <durationIn>500</durationIn>
            <durationOut>300</durationOut>
            <curve name="cubic">0.2 0.0 0.2 1.0</curve>
        </timing>
    </keyboardAnimation>
</Configs>
)";
class WindowManagerConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    ConfigItem config_;
};

void WindowManagerConfigTest::SetUpTestCase()
{
}

void WindowManagerConfigTest::TearDownTestCase()
{
}

void WindowManagerConfigTest::SetUp()
{
    xmlDocPtr docPtr = xmlParseMemory(XML_STR.c_str(), XML_STR.length() + 1);
    if (docPtr == nullptr) {
        return;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        xmlFreeDoc(docPtr);
        return;
    }

    std::map<std::string, ConfigItem> configMap;
    config_.SetValue(configMap);
    WindowManagerConfig::ReadConfig(rootPtr, *config_.mapValue_);

    xmlFreeDoc(docPtr);
}

void WindowManagerConfigTest::TearDown()
{
}
namespace {
/**
 * @tc.name: AnimationConfig
 * @tc.desc: animation config test
 * @tc.type: FUNC
 * @tc.require issueI5N26H
 */
HWTEST_F(WindowManagerConfigTest, AnimationConfig, Function | SmallTest | Level2)
{
    ConfigItem item = config_["windowAnimation"];
    ASSERT_EQ(true, item.IsMap());
    item = config_["windowAnimation"]["timing"]["duration"];
    ASSERT_EQ(true, item.IsInts());
    auto value = *item.intsValue_;
    ASSERT_EQ(true, value.size() == 1);
    ASSERT_EQ(350, value[0]);
    item = config_["windowAnimation"]["timing"]["curve"].GetProp("name");
    ASSERT_EQ(true, item.IsString());
    ASSERT_EQ("easeOut", item.stringValue_);
    item = config_["windowAnimation"]["scale"];
    ASSERT_EQ(true, item.IsFloats());
    item = config_["windowAnimation"]["rotation"];
    ASSERT_EQ(true, item.IsFloats() && (item.floatsValue_->size() == 4));
    item = config_["windowAnimation"]["translate"];
    ASSERT_EQ(true, item.IsFloats());
    item = config_["windowAnimation"]["opacity"];
    ASSERT_EQ(true, item.IsFloats());
}
}
}
}
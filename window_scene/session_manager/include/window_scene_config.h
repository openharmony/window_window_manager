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

#ifndef OHOS_ROSEN_WINDOW_SCENE_CONFIG_H
#define OHOS_ROSEN_WINDOW_SCENE_CONFIG_H

#include <refbase.h>

#include "xml_config_base.h"

struct _xmlNode;
typedef struct _xmlNode xmlNode;
typedef xmlNode* xmlNodePtr;

namespace OHOS {
namespace Rosen {
class WindowSceneConfig : public RefBase, public XmlConfigBase {
public:
    WindowSceneConfig() = delete;
    ~WindowSceneConfig() = default;

    static bool LoadConfigXml();
    static const ConfigItem& GetConfig()
    {
        return config_;
    }
    static void DumpConfig(const std::map<std::string, ConfigItem>& config);

private:
    static ConfigItem config_;
    static const std::map<std::string, ValueType> configItemTypeMap_;

    static bool IsValidNode(const xmlNode& currNode);
    static std::map<std::string, ConfigItem> ReadProperty(const xmlNodePtr& currNode);
    static std::vector<int> ReadIntNumbersConfigInfo(const xmlNodePtr& currNode);
    static std::vector<std::string> ReadStringsConfigInfo(const xmlNodePtr& currNode);
    static std::vector<float> ReadFloatNumbersConfigInfo(const xmlNodePtr& currNode, bool allowNeg);
    static std::string ReadStringConfigInfo(const xmlNodePtr& currNode);
    static void ReadConfig(const xmlNodePtr& rootPtr, std::map<std::string, ConfigItem>& mapValue);
    static std::string GetConfigPath(const std::string& configFileName);
    static std::vector<std::string> SplitNodeContent(const xmlNodePtr& node, const std::string& pattern = " ");
};

} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SCENE_CONFIG_H

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

#include "window_manager_config.h"
#include "config_policy_utils.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerConfig"};
}

std::map<std::string, WindowManagerConfig::ConfigItem> WindowManagerConfig::config_;

std::string WindowManagerConfig::GetConfigPath(const std::string& configFileName)
{
    char buf[PATH_MAX + 1];
    char* configPath = GetOneCfgFile(configFileName.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = { 0 };
    if (!configPath || strlen(configPath) == 0 || strlen(configPath) > PATH_MAX || !realpath(configPath, tmpPath)) {
        WLOGFI("[WmConfig] can not get customization config file");
        return "/system/" + configFileName;
    }
    return std::string(tmpPath);
}

void WindowManagerConfig::ReadConfig(const xmlNodePtr& rootPtr, std::map<std::string, ConfigItem>& mapValue)
{
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!IsValidNode(*curNodePtr)) {
            WLOGFE("[WmConfig]: invalid node!");
            continue;
        }
        auto nodeName = curNodePtr->name;
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("maxAppWindowNumber")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("modeChangeHotZones")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("duration")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("durationIn")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("durationOut"))) {
            std::vector<int> v;
            ReadIntNumbersConfigInfo(curNodePtr, v);
            mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
            continue;
        }
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("windowAnimation")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("keyboardAnimation")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("timing"))) {
            std::map<std::string, ConfigItem> v;
            ReadConfig(curNodePtr, v);
            mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
            continue;
        }
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("curve")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("splitRatios")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("exitSplitRatios")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("scale")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("rotation")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("translate")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("opacity")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("decor")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("minimizeByOther")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("stretchable")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("remoteAnimation"))) {
            std::map<std::string, ConfigItem> p;
            ReadProperty(curNodePtr, p);
            if (p.size() > 0) {
                mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetProperty(p);
            }
            std::vector<float> v;
            ReadFloatNumbersConfigInfo(curNodePtr, v);
            mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
            continue;
        }
    }
}

bool WindowManagerConfig::LoadConfigXml()
{
    auto configFilePath = GetConfigPath("etc/window/resources/window_manager_config.xml");
    xmlDocPtr docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    WLOGFI("[WmConfig] filePath: %{public}s", configFilePath.c_str());
    if (docPtr == nullptr) {
        WLOGFE("[WmConfig] load xml error!");
        return false;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        WLOGFE("[WmConfig] get root element failed!");
        xmlFreeDoc(docPtr);
        return false;
    }

    ReadConfig(rootPtr, config_);

    xmlFreeDoc(docPtr);
    return true;
}

bool WindowManagerConfig::IsValidNode(const xmlNode& currNode)
{
    if (currNode.name == nullptr || currNode.type == XML_COMMENT_NODE) {
        return false;
    }
    return true;
}

void WindowManagerConfig::ReadProperty(const xmlNodePtr& currNode,
    std::map<std::string, ConfigItem>& property)
{
    xmlChar* prop = xmlGetProp(currNode, reinterpret_cast<const xmlChar*>("enable"));
    if (prop != nullptr) {
        if (!xmlStrcmp(prop, reinterpret_cast<const xmlChar*>("true"))) {
            property["enable"].SetValue(true);
        } else if (!xmlStrcmp(prop, reinterpret_cast<const xmlChar*>("false"))) {
            property["enable"].SetValue(false);
        }
        xmlFree(prop);
    }

    prop = xmlGetProp(currNode, reinterpret_cast<const xmlChar*>("name"));
    if (prop != nullptr) {
        property["name"].SetValue(std::string(reinterpret_cast<const char*>(prop)));
        xmlFree(prop);
    }
}

void WindowManagerConfig::ReadIntNumbersConfigInfo(const xmlNodePtr& currNode, std::vector<int>& intsValue)
{
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        WLOGFE("[WmConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::string numbersStr = reinterpret_cast<const char*>(context);
    if (numbersStr.size() == 0) {
        xmlFree(context);
        return;
    }
    auto numbers = WindowHelper::Split(numbersStr, " ");
    for (auto& num : numbers) {
        if (!WindowHelper::IsNumber(num)) {
            WLOGFE("[WmConfig] read int number error: nodeName:(%{public}s)", currNode->name);
            xmlFree(context);
            return;
        }
        intsValue.push_back(std::stoi(num));
    }
    xmlFree(context);
}

void WindowManagerConfig::ReadFloatNumbersConfigInfo(const xmlNodePtr& currNode,
    std::vector<float>& floatsValue)
{
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        WLOGFE("[WmConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }
    std::string numbersStr = reinterpret_cast<const char*>(context);
    if (numbersStr.size() == 0) {
        xmlFree(context);
        return;
    }
    auto numbers = WindowHelper::Split(numbersStr, " ");
    for (auto& num : numbers) {
        if (!WindowHelper::IsFloatingNumber(num)) {
            WLOGFE("[WmConfig] read float number error: nodeName:(%{public}s)", currNode->name);
            xmlFree(context);
            return;
        }
        floatsValue.push_back(std::stof(num));
    }
    xmlFree(context);
}

void WindowManagerConfig::DumpConfig(const std::map<std::string, ConfigItem>& config)
{
    for (auto& conf : config) {
        WLOGFI("[WmConfig] %{public}s", conf.first.c_str());
        std::map<std::string, ConfigItem> propMap;
        if (conf.second.property_) {
            propMap = *conf.second.property_;
        }
        for (auto prop : propMap) {
            switch (prop.second.type_) {
                case ValueType::BOOL:
                    WLOGFI("[WmConfig] Prop: %{public}s %{public}u", prop.first.c_str(), prop.second.boolValue_);
                    break;
                case ValueType::STRING:
                    WLOGFI("[WmConfig] Prop: %{public}s %{public}s", prop.first.c_str(),
                        prop.second.stringValue_.c_str());
                    break;
                default:
                    break;
            }
        }
        switch (conf.second.type_) {
            case ValueType::MAP:
                if (conf.second.mapValue_) {
                    DumpConfig(*conf.second.mapValue_);
                }
                break;
            case ValueType::BOOL:
                WLOGFI("[WmConfig] %{public}u", conf.second.boolValue_);
                break;
            case ValueType::STRING:
                WLOGFI("[WmConfig] %{public}s", conf.second.stringValue_.c_str());
                break;
            case ValueType::INTS:
                if (conf.second.intsValue_) {
                    for (auto& num : *conf.second.intsValue_) {
                        WLOGFI("[WmConfig] Num: %{public}d", num);
                    }
                }
                break;
            case ValueType::FLOATS:
                if (conf.second.floatsValue_) {
                    for (auto& num : *conf.second.floatsValue_) {
                        WLOGFI("[WmConfig] Num: %{public}f", num);
                    }
                }
                break;
            default:
                break;
        }
    }
}
} // namespace Rosen
} // namespace OHOS

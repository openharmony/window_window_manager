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

WindowManagerConfig::ConfigItem WindowManagerConfig::config_;
const std::map<std::string, WindowManagerConfig::ValueType> WindowManagerConfig::configItemTypeMap_ = {
    { "maxAppWindowNumber",                           WindowManagerConfig::ValueType::INTS },
    { "modeChangeHotZones",                           WindowManagerConfig::ValueType::INTS },
    { "duration",                                     WindowManagerConfig::ValueType::INTS },
    { "durationIn",                                   WindowManagerConfig::ValueType::INTS },
    { "durationOut",                                  WindowManagerConfig::ValueType::INTS },
    { "defaultWindowMode",                            WindowManagerConfig::ValueType::INTS },
    { "dragFrameGravity",                             WindowManagerConfig::ValueType::INTS },
    { "floatingBottomPosY",                           WindowManagerConfig::ValueType::INTS },
    { "defaultFloatingWindow",                        WindowManagerConfig::ValueType::INTS },
    { "maxMainFloatingWindowNumber",                  WindowManagerConfig::ValueType::INTS },
    { "maxFloatingWindowSize",                        WindowManagerConfig::ValueType::INTS },
    { "defaultMaximizeMode",                          WindowManagerConfig::ValueType::INTS },
    { "windowAnimation",                              WindowManagerConfig::ValueType::MAP },
    { "keyboardAnimation",                            WindowManagerConfig::ValueType::MAP },
    { "timing",                                       WindowManagerConfig::ValueType::MAP },
    { "windowEffect",                                 WindowManagerConfig::ValueType::MAP },
    { "appWindows",                                   WindowManagerConfig::ValueType::MAP },
    { "cornerRadius",                                 WindowManagerConfig::ValueType::MAP },
    { "shadow",                                       WindowManagerConfig::ValueType::MAP },
    { "focused",                                      WindowManagerConfig::ValueType::MAP },
    { "unfocused",                                    WindowManagerConfig::ValueType::MAP },
    { "decor",                                        WindowManagerConfig::ValueType::MAP },
    { "startWindowTransitionAnimation",               WindowManagerConfig::ValueType::MAP },
    { "curve",                                        WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "splitRatios",                                  WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "exitSplitRatios",                              WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "scale",                                        WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "opacity",                                      WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "opacityStart",                                 WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "opacityEnd",                                   WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "elevation",                                    WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "alpha",                                        WindowManagerConfig::ValueType::POSITIVE_FLOATS },
    { "rotation",                                     WindowManagerConfig::ValueType::FLOATS },
    { "translate",                                    WindowManagerConfig::ValueType::FLOATS },
    { "offsetX",                                      WindowManagerConfig::ValueType::FLOATS },
    { "offsetY",                                      WindowManagerConfig::ValueType::FLOATS },
    { "radius",                                       WindowManagerConfig::ValueType::FLOATS },
    { "fullScreen",                                   WindowManagerConfig::ValueType::STRING },
    { "split",                                        WindowManagerConfig::ValueType::STRING },
    { "float",                                        WindowManagerConfig::ValueType::STRING },
    { "color",                                        WindowManagerConfig::ValueType::STRING },
    { "supportedMode",                                WindowManagerConfig::ValueType::STRINGS },
    { "minimizeByOther",                              WindowManagerConfig::ValueType::UNDIFINED },
    { "stretchable",                                  WindowManagerConfig::ValueType::UNDIFINED },
    { "remoteAnimation",                              WindowManagerConfig::ValueType::UNDIFINED },
    { "configMainFloatingWindowAbove",                WindowManagerConfig::ValueType::UNDIFINED },
};

std::vector<std::string> WindowManagerConfig::SplitNodeContent(const xmlNodePtr& node, const std::string& pattern)
{
    xmlChar* content = xmlNodeGetContent(node);
    if (content == nullptr) {
        WLOGFE("[WmConfig] read xml node error: nodeName:(%{public}s)", node->name);
        return std::vector<std::string>();
    }

    std::string contentStr = reinterpret_cast<const char*>(content);
    xmlFree(content);
    if (contentStr.size() == 0) {
        return std::vector<std::string>();
    }
    return WindowHelper::Split(contentStr, pattern);
}

std::string WindowManagerConfig::GetConfigPath(const std::string& configFileName)
{
    char buf[PATH_MAX + 1];
    char* configPath = GetOneCfgFile(configFileName.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = { 0 };
    if (!configPath || strlen(configPath) == 0 || strlen(configPath) > PATH_MAX || !realpath(configPath, tmpPath)) {
        WLOGI("[WmConfig] can not get customization config file");
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
        std::string nodeName = reinterpret_cast<const char*>(curNodePtr->name);
        if (configItemTypeMap_.count(nodeName)) {
            std::map<std::string, ConfigItem> p = ReadProperty(curNodePtr);
            if (p.size() > 0) {
                mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetProperty(p);
            }
            switch (configItemTypeMap_.at(nodeName)) {
                case ValueType::INTS: {
                    std::vector<int> v = ReadIntNumbersConfigInfo(curNodePtr);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::POSITIVE_FLOATS: {
                    std::vector<float> v = ReadFloatNumbersConfigInfo(curNodePtr, false);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::FLOATS: {
                    std::vector<float> v = ReadFloatNumbersConfigInfo(curNodePtr, true);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::MAP: {
                    std::map<std::string, ConfigItem> v;
                    ReadConfig(curNodePtr, v);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::STRING: {
                    std::string v = ReadStringConfigInfo(curNodePtr);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::STRINGS: {
                    std::vector<std::string> v = ReadStringsConfigInfo(curNodePtr);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

bool WindowManagerConfig::LoadConfigXml()
{
    auto configFilePath = GetConfigPath("etc/window/resources/window_manager_config.xml");
    xmlDocPtr docPtr = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    }
    WLOGI("[WmConfig] filePath: %{public}s", configFilePath.c_str());
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

    std::map<std::string, ConfigItem> configMap;
    config_.SetValue(configMap);
    ReadConfig(rootPtr, *config_.mapValue_);

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

std::map<std::string, XmlConfigBase::ConfigItem> WindowManagerConfig::ReadProperty(const xmlNodePtr& currNode)
{
    std::map<std::string, ConfigItem> property;
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

    return property;
}

std::vector<int> WindowManagerConfig::ReadIntNumbersConfigInfo(const xmlNodePtr& currNode)
{
    std::vector<int> intsValue;
    auto numbers = SplitNodeContent(currNode);
    for (auto& num : numbers) {
        if (!WindowHelper::IsNumber(num)) {
            WLOGFE("[WmConfig] read int number error: nodeName:(%{public}s)", currNode->name);
            return {};
        }
        intsValue.push_back(std::stoi(num));
    }
    return intsValue;
}

std::vector<std::string> WindowManagerConfig::ReadStringsConfigInfo(const xmlNodePtr& currNode)
{
    return SplitNodeContent(currNode);
}

std::vector<float> WindowManagerConfig::ReadFloatNumbersConfigInfo(const xmlNodePtr& currNode, bool allowNeg)
{
    std::vector<float> floatsValue;
    auto numbers = SplitNodeContent(currNode);
    for (auto& num : numbers) {
        if (!WindowHelper::IsFloatingNumber(num, allowNeg)) {
            WLOGFE("[WmConfig] read float number error: nodeName:(%{public}s)", currNode->name);
            return {};
        }
        floatsValue.push_back(std::stof(num));
    }
    return floatsValue;
}

std::string WindowManagerConfig::ReadStringConfigInfo(const xmlNodePtr& currNode)
{
    std::string stringValue;
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        WLOGFE("[WmConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return {};
    }

    stringValue = std::string(reinterpret_cast<const char*>(context));
    xmlFree(context);
    return stringValue;
}

void WindowManagerConfig::DumpConfig(const std::map<std::string, ConfigItem>& config)
{
    for (auto& conf : config) {
        WLOGI("[WmConfig] %{public}s", conf.first.c_str());
        std::map<std::string, ConfigItem> propMap;
        if (conf.second.property_) {
            propMap = *conf.second.property_;
        }
        for (auto prop : propMap) {
            switch (prop.second.type_) {
                case ValueType::BOOL:
                    WLOGI("[WmConfig] Prop: %{public}s %{public}u", prop.first.c_str(), prop.second.boolValue_);
                    break;
                case ValueType::STRING:
                    WLOGI("[WmConfig] Prop: %{public}s %{public}s", prop.first.c_str(),
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
                WLOGI("[WmConfig] %{public}u", conf.second.boolValue_);
                break;
            case ValueType::STRING:
                WLOGI("[WmConfig] %{public}s", conf.second.stringValue_.c_str());
                break;
            case ValueType::INTS:
                for (auto& num : *conf.second.intsValue_) {
                    WLOGI("[WmConfig] Num: %{public}d", num);
                }
                break;
            case ValueType::FLOATS:
                for (auto& num : *conf.second.floatsValue_) {
                    WLOGI("[WmConfig] Num: %{public}f", num);
                }
                break;
            default:
                break;
        }
    }
}
} // namespace Rosen
} // namespace OHOS

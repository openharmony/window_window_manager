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
#include "screen_scene_config.h"

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <libxml/globals.h>
#include <libxml/xmlstring.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "config_policy_utils.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathMeasure.h"
#include "include/utils/SkParsePath.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t NO_WATERFALL_DISPLAY_COMPRESSION_SIZE = 0;
enum XmlNodeElement {
    DPI = 0,
    SUB_DPI,
    IS_WATERFALL_DISPLAY,
    CURVED_SCREEN_BOUNDARY,
    CURVED_AREA_IN_LANDSCAPE,
    IS_CURVED_COMPRESS_ENABLED,
    BUILD_IN_DEFAULT_ORIENTATION,
    DEFAULT_DEVICE_ROTATION_OFFSET,
    DEFAULT_DISPLAY_CUTOUT_PATH,
    SUB_DISPLAY_CUTOUT_PATH,
    ROTATION_POLICY,
    HALL_SWITCH_APP,
    PACKAGE_NAME,
    SCREEN_SNAPSHOT_BUNDLE_NAME,
    SCREEN_SNAPSHOT_ABILITY_NAME,
    IS_RIGHT_POWER_BUTTON,
};
}

std::map<std::string, bool> ScreenSceneConfig::enableConfig_;
std::map<std::string, std::vector<int>> ScreenSceneConfig::intNumbersConfig_;
std::map<std::string, std::string> ScreenSceneConfig::stringConfig_;
std::map<std::string, std::vector<std::string>> ScreenSceneConfig::stringListConfig_;
std::map<uint64_t, std::vector<DMRect>> ScreenSceneConfig::cutoutBoundaryRectMap_;
std::vector<DMRect> ScreenSceneConfig::subCutoutBoundaryRect_;
bool ScreenSceneConfig::isWaterfallDisplay_ = false;
bool ScreenSceneConfig::isScreenCompressionEnableInLandscape_ = false;
uint32_t ScreenSceneConfig::curvedAreaInLandscape_ = 0;
std::map<int32_t, std::string> ScreenSceneConfig::xmlNodeMap_ = {
    {DPI, "dpi"},
    {SUB_DPI, "subDpi"},
    {IS_WATERFALL_DISPLAY, "isWaterfallDisplay"},
    {CURVED_SCREEN_BOUNDARY, "curvedScreenBoundary"},
    {CURVED_AREA_IN_LANDSCAPE, "waterfallAreaCompressionSizeWhenHorzontal"},
    {IS_CURVED_COMPRESS_ENABLED, "isWaterfallAreaCompressionEnableWhenHorizontal"},
    {BUILD_IN_DEFAULT_ORIENTATION, "buildInDefaultOrientation"},
    {DEFAULT_DEVICE_ROTATION_OFFSET, "defaultDeviceRotationOffset"},
    {DEFAULT_DISPLAY_CUTOUT_PATH, "defaultDisplayCutoutPath"},
    {SUB_DISPLAY_CUTOUT_PATH, "subDisplayCutoutPath"},
    {ROTATION_POLICY, "rotationPolicy"},
    {HALL_SWITCH_APP, "hallSwitchApp"},
    {PACKAGE_NAME, "packageName"},
    {SCREEN_SNAPSHOT_BUNDLE_NAME, "screenSnapshotBundleName"},
    {SCREEN_SNAPSHOT_ABILITY_NAME, "screenSnapshotAbilityName"},
    {IS_RIGHT_POWER_BUTTON, "isRightPowerButton"},
};


std::vector<std::string> ScreenSceneConfig::Split(std::string str, std::string pattern)
{
    std::vector<std::string> result;
    str += pattern;
    int32_t length = static_cast<int32_t>(str.size());
    for (int32_t i = 0; i < length; i++) {
        int32_t position = static_cast<int32_t>(str.find(pattern, i));
        if (position < length) {
            std::string tmp = str.substr(i, position - i);
            result.push_back(tmp);
            i = position + static_cast<int32_t>(pattern.size()) - 1;
        }
    }
    return result;
}

bool ScreenSceneConfig::IsNumber(std::string str)
{
    if (str.size() == 0) {
        return false;
    }
    for (int32_t i = 0; i < static_cast<int32_t>(str.size()); i++) {
        if (str.at(i) < '0' || str.at(i) > '9') {
            return false;
        }
    }
    return true;
}

std::string ScreenSceneConfig::GetConfigPath(const std::string& configFileName)
{
    char buf[PATH_MAX + 1];
    char* configPath = GetOneCfgFile(configFileName.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = { 0 };
    if (!configPath || strlen(configPath) == 0 || strlen(configPath) > PATH_MAX || !realpath(configPath, tmpPath)) {
        TLOGI(WmsLogTag::DMS, "[SsConfig] can not get customization config file");
        return "/system/" + configFileName;
    }
    return std::string(tmpPath);
}

bool ScreenSceneConfig::LoadConfigXml()
{
    auto configFilePath = GetConfigPath("etc/window/resources/display_manager_config.xml");
    xmlDocPtr docPtr = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    }
    TLOGI(WmsLogTag::DMS, "[SsConfig] filePath: %{public}s", configFilePath.c_str());
    if (docPtr == nullptr) {
        TLOGE(WmsLogTag::DMS, "[SsConfig] load xml error!");
        return false;
    }
    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        TLOGE(WmsLogTag::DMS, "[SsConfig] get root element failed!");
        xmlFreeDoc(docPtr);
        return false;
    }
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!IsValidNode(*curNodePtr)) {
            TLOGE(WmsLogTag::DMS, "SsConfig]: invalid node!");
            continue;
        }
        ParseNodeConfig(curNodePtr);
    }
    xmlFreeDoc(docPtr);
    return true;
}

void ScreenSceneConfig::ParseNodeConfig(const xmlNodePtr& currNode)
{
    std::string nodeName(reinterpret_cast<const char*>(currNode->name));
    bool enableConfigCheck = (xmlNodeMap_[IS_WATERFALL_DISPLAY] == nodeName) ||
        (xmlNodeMap_[IS_CURVED_COMPRESS_ENABLED] == nodeName) ||
        (xmlNodeMap_[IS_RIGHT_POWER_BUTTON] == nodeName);
    bool numberConfigCheck = (xmlNodeMap_[DPI] == nodeName) ||
        (xmlNodeMap_[SUB_DPI] == nodeName) ||
        (xmlNodeMap_[CURVED_SCREEN_BOUNDARY] == nodeName) ||
        (xmlNodeMap_[CURVED_AREA_IN_LANDSCAPE] == nodeName) ||
        (xmlNodeMap_[BUILD_IN_DEFAULT_ORIENTATION] == nodeName) ||
        (xmlNodeMap_[DEFAULT_DEVICE_ROTATION_OFFSET] == nodeName);
    bool stringConfigCheck = (xmlNodeMap_[DEFAULT_DISPLAY_CUTOUT_PATH] == nodeName) ||
        (xmlNodeMap_[SUB_DISPLAY_CUTOUT_PATH] == nodeName) ||
        (xmlNodeMap_[ROTATION_POLICY] == nodeName) ||
        (xmlNodeMap_[SCREEN_SNAPSHOT_BUNDLE_NAME] == nodeName) ||
        (xmlNodeMap_[SCREEN_SNAPSHOT_ABILITY_NAME] == nodeName);
    if (enableConfigCheck) {
        ReadEnableConfigInfo(currNode);
    } else if (numberConfigCheck) {
        ReadIntNumbersConfigInfo(currNode);
    } else if (stringConfigCheck) {
        ReadStringConfigInfo(currNode);
    } else if (xmlNodeMap_[HALL_SWITCH_APP] == nodeName) {
        ReadStringListConfigInfo(currNode, nodeName);
    } else {
        TLOGI(WmsLogTag::DMS, "xml config node name is not match, nodeName:%{public}s", nodeName.c_str());
    }
}

bool ScreenSceneConfig::IsValidNode(const xmlNode& currNode)
{
    if (currNode.name == nullptr || currNode.type == XML_COMMENT_NODE) {
        return false;
    }
    return true;
}

void ScreenSceneConfig::ReadIntNumbersConfigInfo(const xmlNodePtr& currNode)
{
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        TLOGE(WmsLogTag::DMS, "[SsConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::vector<int> numbersVec;
    std::string numbersStr = reinterpret_cast<const char*>(context);
    if (numbersStr.empty()) {
        xmlFree(context);
        return;
    }
    auto numbers = Split(numbersStr, " ");
    for (auto& num : numbers) {
        if (!IsNumber(num)) {
            TLOGE(WmsLogTag::DMS, "[SsConfig] read number error: nodeName:(%{public}s)", currNode->name);
            xmlFree(context);
            return;
        }
        numbersVec.emplace_back(std::stoi(num));
    }

    std::string nodeName = reinterpret_cast<const char *>(currNode->name);
    intNumbersConfig_[nodeName] = numbersVec;
    xmlFree(context);
}

void ScreenSceneConfig::ReadEnableConfigInfo(const xmlNodePtr& currNode)
{
    xmlChar* enable = xmlGetProp(currNode, reinterpret_cast<const xmlChar*>("enable"));
    if (enable == nullptr) {
        TLOGE(WmsLogTag::DMS, "[SsConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::string nodeName = reinterpret_cast<const char *>(currNode->name);
    if (!xmlStrcmp(enable, reinterpret_cast<const xmlChar*>("true"))) {
        enableConfig_[nodeName] = true;
        if (xmlNodeMap_[IS_WATERFALL_DISPLAY] == nodeName) {
            isWaterfallDisplay_ = true;
        } else if (xmlNodeMap_[IS_CURVED_COMPRESS_ENABLED] == nodeName) {
            isScreenCompressionEnableInLandscape_ = true;
        }
    } else {
        enableConfig_[nodeName] = false;
    }
    xmlFree(enable);
}

void ScreenSceneConfig::ReadStringConfigInfo(const xmlNodePtr& currNode)
{
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        TLOGE(WmsLogTag::DMS, "[SsConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::string inputString = reinterpret_cast<const char*>(context);
    std::string nodeName = reinterpret_cast<const char*>(currNode->name);
    stringConfig_[nodeName] = inputString;
    xmlFree(context);
}

void ScreenSceneConfig::ReadStringListConfigInfo(const xmlNodePtr& rootNode, std::string name)
{
    xmlChar* rootContext = xmlNodeGetContent(rootNode);
    if (rootNode == nullptr || rootNode->name == nullptr) {
        TLOGE(WmsLogTag::DMS, "[SsConfig] get root element failed!");
        xmlFree(rootContext);
        return;
    }
    std::vector<std::string> stringVec;
    for (xmlNodePtr curNodePtr = rootNode->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!IsValidNode(*curNodePtr)) {
            TLOGE(WmsLogTag::DMS, "SsConfig]: invalid node!");
            continue;
        }
        xmlChar* context = xmlNodeGetContent(curNodePtr);
        std::string str = reinterpret_cast<const char*>(context);
        stringVec.emplace_back(str);
        xmlFree(context);
    }
    stringListConfig_[name] = stringVec;
    xmlFree(rootContext);
}

const std::map<std::string, bool>& ScreenSceneConfig::GetEnableConfig()
{
    return enableConfig_;
}

const std::map<std::string, std::vector<int>>& ScreenSceneConfig::GetIntNumbersConfig()
{
    return intNumbersConfig_;
}

const std::map<std::string, std::string>& ScreenSceneConfig::GetStringConfig()
{
    return stringConfig_;
}

const std::map<std::string, std::vector<std::string>>& ScreenSceneConfig::GetStringListConfig()
{
    return stringListConfig_;
}

void ScreenSceneConfig::DumpConfig()
{
    for (auto& enable : enableConfig_) {
        TLOGI(WmsLogTag::DMS, "[SsConfig] Enable: %{public}s %{public}u", enable.first.c_str(), enable.second);
    }
    for (auto& numbers : intNumbersConfig_) {
        TLOGI(WmsLogTag::DMS, "[SsConfig] Numbers: %{public}s %{public}zu",
            numbers.first.c_str(), numbers.second.size());
        for (auto& num : numbers.second) {
            TLOGI(WmsLogTag::DMS, "[SsConfig] Num: %{public}d", num);
        }
    }
    for (auto& string : stringConfig_) {
        TLOGI(WmsLogTag::DMS, "[SsConfig] String: %{public}s", string.first.c_str());
    }
}

void ScreenSceneConfig::SetCutoutSvgPath(uint64_t displayId, const std::string& svgPath)
{
    cutoutBoundaryRectMap_.clear();
    cutoutBoundaryRectMap_[displayId].emplace_back(CalcCutoutBoundaryRect(svgPath));
}

void ScreenSceneConfig::SetSubCutoutSvgPath(const std::string& svgPath)
{
    subCutoutBoundaryRect_.clear();
    subCutoutBoundaryRect_.emplace_back(CalcCutoutBoundaryRect(svgPath));
}

DMRect ScreenSceneConfig::CalcCutoutBoundaryRect(std::string svgPath)
{
    DMRect emptyRect = { 0, 0, 0, 0 };
    SkPath skCutoutSvgPath;
    if (!SkParsePath::FromSVGString(svgPath.c_str(), &skCutoutSvgPath)) {
        TLOGE(WmsLogTag::DMS, "Parse svg string path failed.");
        return emptyRect;
    }
    SkRect skRect = skCutoutSvgPath.computeTightBounds();
    if (skRect.isEmpty()) {
        TLOGW(WmsLogTag::DMS, "Get empty skRect");
        return emptyRect;
    }
    SkIRect skiRect = skRect.roundOut();
    if (skiRect.isEmpty()) {
        TLOGW(WmsLogTag::DMS, "Get empty skiRect");
        return emptyRect;
    }
    int32_t left = static_cast<int32_t>(skiRect.left());
    int32_t top = static_cast<int32_t>(skiRect.top());
    uint32_t width = static_cast<uint32_t>(skiRect.width());
    uint32_t height = static_cast<uint32_t>(skiRect.height());
    TLOGI(WmsLogTag::DMS,
        "calc cutout boundary rect - left: [%{public}d top: %{public}d width: %{public}u height: %{public}u]",
        left, top, width, height);
    DMRect cutoutMinOuterRect = {
        .posX_ = left,
        .posY_ = top,
        .width_ = width,
        .height_ = height
    };
    return cutoutMinOuterRect;
}

std::vector<DMRect> ScreenSceneConfig::GetCutoutBoundaryRect(uint64_t displayId)
{
    if (cutoutBoundaryRectMap_.count(displayId) == 0) {
        return {};
    }
    return cutoutBoundaryRectMap_[displayId];
}

std::vector<DMRect> ScreenSceneConfig::GetSubCutoutBoundaryRect()
{
    return subCutoutBoundaryRect_;
}

bool ScreenSceneConfig::IsWaterfallDisplay()
{
    return isWaterfallDisplay_;
}

void ScreenSceneConfig::SetCurvedCompressionAreaInLandscape()
{
    if (intNumbersConfig_[xmlNodeMap_[CURVED_AREA_IN_LANDSCAPE]].size() > 0) {
        curvedAreaInLandscape_ = static_cast<uint32_t>(intNumbersConfig_[xmlNodeMap_[CURVED_AREA_IN_LANDSCAPE]][0]);
    } else {
        TLOGW(WmsLogTag::DMS, "waterfallAreaCompressionSizeWhenHorzontal value is not exist");
    }
}

std::vector<int> ScreenSceneConfig::GetCurvedScreenBoundaryConfig()
{
    return intNumbersConfig_[xmlNodeMap_[CURVED_SCREEN_BOUNDARY]];
}

uint32_t ScreenSceneConfig::GetCurvedCompressionAreaInLandscape()
{
    if (!isWaterfallDisplay_ || !isScreenCompressionEnableInLandscape_) {
        TLOGW(WmsLogTag::DMS, "not waterfall screen or waterfall compression is not enabled");
        return NO_WATERFALL_DISPLAY_COMPRESSION_SIZE;
    }
    return curvedAreaInLandscape_;
}
} // namespace OHOS::Rosen

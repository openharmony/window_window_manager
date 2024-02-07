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
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DMS_SCREEN_SESSION_MANAGER, "ScreenSceneConfig"};
constexpr char IS_WATERFALL_DISPLAY[] = "isWaterfallDisplay";
constexpr char CURVED_SCREEN_BOUNDARY[] = "curvedScreenBoundary";
constexpr char CURVED_AREA_IN_LANDSCAPE[] = "waterfallAreaCompressionSizeWhenHorzontal";
constexpr char IS_CURVED_COMPRESS_ENABLED[] = "isWaterfallAreaCompressionEnableWhenHorizontal";
constexpr uint32_t NO_WATERFALL_DISPLAY_COMPRESSION_SIZE = 0;
}

std::map<std::string, bool> ScreenSceneConfig::enableConfig_;
std::map<std::string, std::vector<int>> ScreenSceneConfig::intNumbersConfig_;
std::map<std::string, std::string> ScreenSceneConfig::stringConfig_;
std::vector<DMRect> ScreenSceneConfig::cutoutBoundaryRect_;
bool ScreenSceneConfig::isWaterfallDisplay_ = false;
bool ScreenSceneConfig::isScreenCompressionEnableInLandscape_ = false;
uint32_t ScreenSceneConfig::curvedAreaInLandscape_ = 0;

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
        WLOGFI("[SsConfig] can not get customization config file");
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
    WLOGFI("[SsConfig] filePath: %{public}s", configFilePath.c_str());
    if (docPtr == nullptr) {
        WLOGFE("[SsConfig] load xml error!");
        return false;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        WLOGFE("[SsConfig] get root element failed!");
        xmlFreeDoc(docPtr);
        return false;
    }

    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!IsValidNode(*curNodePtr)) {
            WLOGFE("SsConfig]: invalid node!");
            continue;
        }

        auto nodeName = curNodePtr->name;
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>(IS_WATERFALL_DISPLAY)) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>(IS_CURVED_COMPRESS_ENABLED))) {
            ReadEnableConfigInfo(curNodePtr);
            continue;
        }
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("dpi")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("defaultDeviceRotationOffset")) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>(CURVED_SCREEN_BOUNDARY)) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>(CURVED_AREA_IN_LANDSCAPE)) ||
            !xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("buildInDefaultOrientation"))) {
            ReadIntNumbersConfigInfo(curNodePtr);
            continue;
        }
        if (!xmlStrcmp(nodeName, reinterpret_cast<const xmlChar*>("defaultDisplayCutoutPath"))) {
            ReadStringConfigInfo(curNodePtr);
            continue;
        }
    }
    xmlFreeDoc(docPtr);
    return true;
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
        WLOGFE("[SsConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
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
            WLOGFE("[SsConfig] read number error: nodeName:(%{public}s)", currNode->name);
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
        WLOGFE("[SsConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::string nodeName = reinterpret_cast<const char *>(currNode->name);
    if (!xmlStrcmp(enable, reinterpret_cast<const xmlChar*>("true"))) {
        enableConfig_[nodeName] = true;
        if (IS_WATERFALL_DISPLAY == nodeName) {
            isWaterfallDisplay_ = true;
        } else if (IS_CURVED_COMPRESS_ENABLED == nodeName) {
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
        WLOGFE("[SsConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return;
    }

    std::string inputString = reinterpret_cast<const char*>(context);
    std::string nodeName = reinterpret_cast<const char*>(currNode->name);
    stringConfig_[nodeName] = inputString;
    xmlFree(context);
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

void ScreenSceneConfig::DumpConfig()
{
    for (auto& enable : enableConfig_) {
        WLOGFI("[SsConfig] Enable: %{public}s %{public}u", enable.first.c_str(), enable.second);
    }
    for (auto& numbers : intNumbersConfig_) {
        WLOGFI("[SsConfig] Numbers: %{public}s %{public}zu", numbers.first.c_str(), numbers.second.size());
        for (auto& num : numbers.second) {
            WLOGFI("[SsConfig] Num: %{public}d", num);
        }
    }
    for (auto& string : stringConfig_) {
        WLOGFI("[SsConfig] String: %{public}s", string.first.c_str());
    }
}

void ScreenSceneConfig::SetCutoutSvgPath(const std::string& svgPath)
{
    cutoutBoundaryRect_.clear();
    cutoutBoundaryRect_.emplace_back(CalcCutoutBoundaryRect(svgPath));
}

DMRect ScreenSceneConfig::CalcCutoutBoundaryRect(std::string svgPath)
{
    DMRect emptyRect = { 0, 0, 0, 0 };
    SkPath skCutoutSvgPath;
    if (!SkParsePath::FromSVGString(svgPath.c_str(), &skCutoutSvgPath)) {
        WLOGFE("Parse svg string path failed.");
        return emptyRect;
    }
    SkRect skRect = skCutoutSvgPath.computeTightBounds();
    if (skRect.isEmpty()) {
        WLOGFW("Get empty skRect");
        return emptyRect;
    }
    SkIRect skiRect = skRect.roundOut();
    if (skiRect.isEmpty()) {
        WLOGFW("Get empty skiRect");
        return emptyRect;
    }
    int32_t left = static_cast<int32_t>(skiRect.left());
    int32_t top = static_cast<int32_t>(skiRect.top());
    uint32_t width = static_cast<uint32_t>(skiRect.width());
    uint32_t height = static_cast<uint32_t>(skiRect.height());
    WLOGFI("calc cutout boundary rect - left: [%{public}d top: %{public}d width: %{public}u height: %{public}u]", left,
        top, width, height);
    DMRect cutoutMinOuterRect = {
        .posX_ = left,
        .posY_ = top,
        .width_ = width,
        .height_ = height
    };
    return cutoutMinOuterRect;
}

std::vector<DMRect> ScreenSceneConfig::GetCutoutBoundaryRect()
{
    return cutoutBoundaryRect_;
}

bool ScreenSceneConfig::IsWaterfallDisplay()
{
    return isWaterfallDisplay_;
}

void ScreenSceneConfig::SetCurvedCompressionAreaInLandscape()
{
    if (intNumbersConfig_[CURVED_AREA_IN_LANDSCAPE].size() > 0) {
        curvedAreaInLandscape_ = static_cast<uint32_t>(intNumbersConfig_[CURVED_AREA_IN_LANDSCAPE][0]);
    } else {
        WLOGFW("waterfallAreaCompressionSizeWhenHorzontal value is not exist");
    }
}

std::vector<int> ScreenSceneConfig::GetCurvedScreenBoundaryConfig()
{
    return intNumbersConfig_[CURVED_SCREEN_BOUNDARY];
}

uint32_t ScreenSceneConfig::GetCurvedCompressionAreaInLandscape()
{
    if (!isWaterfallDisplay_ || !isScreenCompressionEnableInLandscape_) {
        WLOGFW("not waterfall screen or waterfall compression is not enabled");
        return NO_WATERFALL_DISPLAY_COMPRESSION_SIZE;
    }
    return curvedAreaInLandscape_;
}
} // namespace OHOS::Rosen

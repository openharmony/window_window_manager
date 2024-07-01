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

#include "typec_port_info.h"

#include <string>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>

namespace OHOS::Rosen {
static std::vector<std::string> getDirList()
{
    DIR* dir = opendir("sys/class/thermal");
    if (dir == nullptr) {
        return std::vector<std::string>();
    }

    std::vector<std::string> allPath;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name.find("thermal_zone") == 0) {
            std::string thermalDir = "sys/class/thermal/" + name;
            allPath.emplace_back(thermalDir);
        }
    }
    closedir(dir);
    return allPath;
}

static bool IsStringNumeric(const char* buffer)
{
    auto begin = buffer;
    while (*begin) {
        if (!std::isdigit(*begin)) {
            return false;
        }
        begin++;
    }
    return true;
}

static bool GetTypeCFileNode(const std::vector<std::string>& allDirName, std::string& typeCMappingPath)
{
    constexpr int bufferSize = 20;
    for (const auto& dir : allDirName) {
        std::string absoluteTypePath = dir + "/type";
        char buffer[bufferSize] = {0};
        std::ifstream srcFile(absoluteTypePath.c_str(), std::ifstream::in);
        if (!srcFile.is_open()) {
            continue;
        }
        srcFile.getline(buffer, bufferSize);
        if (strcmp(buffer, "usb_port") == 0) {
            typeCMappingPath = dir + "/temp";
            return true;
        }
    }
    return false;
}

bool TypeCPortInfo::GetTypeCThermal(int32_t& thermal)
{
    static std::string typeCMappingPath;
    if (typeCMappingPath.empty()) {
        std::vector<std::string> allDirName = getDirList();
        if (allDirName.empty()) {
            return false;
        }
        if (!GetTypeCFileNode(allDirName, typeCMappingPath)) {
            return false;
        }
    }

    constexpr int bufferSize = 20;
    char buffer[bufferSize] = {0};

    std::ifstream typeCThermalFile(typeCMappingPath.c_str(), std::ifstream::in);
    if (!typeCThermalFile.is_open()) {
        return false;
    }
    typeCThermalFile.getline(buffer, bufferSize);
    if (!IsStringNumeric(buffer)) {
        return false;
    }
    thermal = std::stoi(buffer);
    return true;
}
} // namespace OHOS::Rosen
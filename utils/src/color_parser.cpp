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

#include "color_parser.h"

#include <cstdlib>

namespace OHOS {
namespace Rosen {
bool ColorParser::Parse(const std::string& colorStr, uint32_t& colorValue)
{
    if (colorStr.empty()) {
        return false;
    }

    if (colorStr[0] == '#') { // start with '#'
        std::string color = colorStr.substr(1);
        if (!IsValidHexString(color)) {
            return false;
        }
        constexpr int HEX = 16;
        colorValue = std::strtoul(color.c_str(), 0, HEX); // convert hex string to number
        if (colorStr.size() == 7) { // 7 is color string length.#RRGGBB: RRGGBB -> AARRGGBB
            colorValue |= 0xff000000;
            return true;
        }
        if (colorStr.size() == 9) { // 9 is color string length.#AARRGGBB
            return true;
        }
    }
    return false;
}

bool ColorParser::IsValidHexString(const std::string& colorStr)
{
    if (colorStr.empty()) {
        return false;
    }
    for (char ch : colorStr) {
        if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
            continue;
        }
        return false;
    }
    return true;
}

// check color string, format:#008EF5 or #FF008EF5. Alpha must be 0xff or none.
bool ColorParser::IsValidColorNoAlpha(const std::string& colorStr)
{
    uint32_t colorValue = 0;
    if (Parse(colorStr, colorValue)) {
        return (colorValue & 0xff000000) == 0xff000000;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS

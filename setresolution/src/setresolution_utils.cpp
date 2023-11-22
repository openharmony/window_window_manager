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

#include "setresolution_utils.h"

#include <getopt.h>
#include <iostream>
#include <string>

namespace OHOS {
void SetResolutionUtils::PrintUsage(const std::string &cmdLine)
{
    std::cout << "usage: " << cmdLine.c_str() <<
        " [-w width] [-h height] [-d dpi] [-m]" << std::endl;
}

bool SetResolutionUtils::ProcessArgs(int argc, char * const argv[], CmdArgments &cmdArgments)
{
    int opt = 0;
    const struct option longOption[] = {
        { "width", required_argument, nullptr, 'w' },
        { "height", required_argument, nullptr, 'h' },
        { "dpi", required_argument, nullptr, 'd' },
        { "help", required_argument, nullptr, 'm' },
        { nullptr, 0, nullptr, 0 }
    };
    while ((opt = getopt_long(argc, argv, "w:h:d:m", longOption, nullptr)) != -1) {
        switch (opt) {
            case 'w': // output width
                cmdArgments.width = atoi(optarg);
                cmdArgments.isWidthSet = true;
                break;
            case 'h': // output height
                cmdArgments.height = atoi(optarg);
                cmdArgments.isHeightSet = true;
                break;
            case 'd': // output dpi
                cmdArgments.dpi = atoi(optarg);
                cmdArgments.isDpiSet = true;
                break;
            case 'm': // help
            default:
                SetResolutionUtils::PrintUsage(argv[0]);
                return false;
        }
    }
    return true;
}
}
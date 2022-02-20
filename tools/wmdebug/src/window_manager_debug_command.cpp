/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_manager_debug_command.h"
#include <getopt.h>
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int MIN_ARG_NUMBER = 2;
    const std::string SHORT_OPTIONS = "hatc";
    const struct option LONG_OPTIONS[] = {
        {"help", no_argument, nullptr, 'h'},
        {"all", no_argument, nullptr, 'a'},
        {"tree", no_argument, nullptr, 't'},
        {"clear", no_argument, nullptr, 'c'},
    };
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerDebugCommand"};
} // namespace

WindowManagerDebugCommand::WindowManagerDebugCommand(int argc, char *argv[])
{
    opterr = 0;
    argc_ = argc;
    argv_ = argv;;
    cmd_ = argc < MIN_ARG_NUMBER ? "help" : argv[1]; // 1 command of cmd line
    for (int i = 2; i < argc; i++) {
        args_.push_back(argv[i]);
    }
    if (Init() != WMError::WM_OK) {
        WLOGFE("init window manager debug command failed.");
    }
}

WMError WindowManagerDebugCommand::Init()
{
    CreateCommandMap();
    return WMError::WM_OK;
}

void WindowManagerDebugCommand::CreateCommandMap()
{
    commandMap_ = {
        {"help", std::bind(&WindowManagerDebugCommand::RunHelpCommand, this)},
        {"dump", std::bind(&WindowManagerDebugCommand::RunDumpCommand, this)},
    };
}

std::string WindowManagerDebugCommand::RunCommand()
{
    auto commandFunc = commandMap_[cmd_];
    if (commandFunc == nullptr) {
        commandFunc = commandMap_["help"];
        result_.append(GetCommandErrorMsg());
    }
    if (commandFunc() != WMError::WM_OK) {
        result_ = "error: failed to run your command.\n";
    }
    return result_;
}

WMError WindowManagerDebugCommand::RunHelpCommand()
{
    WLOGI("run help command");
    result_.append(HELP_MSG);
    return WMError::WM_OK;
}

WMError WindowManagerDebugCommand::RunDumpCommand()
{
    WLOGI("run dump command");
    int option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);
    WLOGI("option: %{public}d, optopt: %{public}d, optind: %{public}d", option, optopt, optind);
    if (optind < 0 || optind > argc_) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isNeedDump = true;
    WindowDumpType type;
    switch (option) {
        case 'h': {
            // 'wmdebug dump -h'
            // 'wmdebug dump --help'
            result_.append(HELP_MSG_DUMP + "\n");
            isNeedDump = false;
            break;
        }
        case 'a' : {
            // 'wmdebug dump -a'
            // 'wmdebug dump --all'
            type = WindowDumpType::ALL;
            break;
        }
        case 't' : {
            // 'wmdebug dump -t'
            // 'wmdebug dump --tree'
            type = WindowDumpType::TREE;
            break;
        }
        case 'c' : {
            // 'wmdebug dump -c'
            // 'wmdebug dump --clear'
            type = WindowDumpType::CLEAR;
            break;
        }
        case '?' : {
            // 'wmdebug dump' with an unknown option: wmdebug dump --x
            // 'wmdebug dump' with an unknown option: wmdebug dump --xxx
            std::string unknownOptionMsg = GetUnknownOptionMsg();
            result_.append(unknownOptionMsg);
            isNeedDump = false;
            break;
        }
        default: {
            if (strcmp(argv_[optind], cmd_.c_str()) == 0) {
                // 'wmdebug dump' with no option: wmdebug dump
                // 'wmdebug dump' with a wrong argument: wmdebug dump xxx
                result_.append(HELP_MSG_NO_DUMP_OPTION + "\n");
                isNeedDump = false;
            }
            break;
        }
    }
    if (isNeedDump) {
        DumpResults(type);
    }
    return WMError::WM_OK;
}

void WindowManagerDebugCommand::DumpResults(WindowDumpType dumpType)
{
    std::vector<std::string> dumpResults;
    if (SingletonContainer::Get<WindowManager>().DumpWindowTree(dumpResults, dumpType) != WMError::WM_OK) {
        switch (dumpType) {
            case WindowDumpType::ALL: {
                result_ = DUMP_ALL_FAIL_MSG;
                break;
            }
            case WindowDumpType::TREE: {
                result_ = DUMP_TREE_FAIL_MSG;
                break;
            }
            case WindowDumpType::CLEAR: {
                result_ = CLEAR_DUMP_INFO_FAIL_MSG;
                break;
            }
        }
    } else {
        for (auto res : dumpResults) {
            result_ += res + "\n";
        }
        switch (dumpType) {
            case WindowDumpType::ALL: {
                result_ += DUMP_ALL_OK_MSG + "\n";
                break;
            }
            case WindowDumpType::TREE: {
                result_ += DUMP_TREE_OK_MSG + "\n";
                break;
            }
            case WindowDumpType::CLEAR: {
                result_ += CLEAR_DUMP_INFO_OK_MSG + "\n";
                break;
            }
        }
    }
}

std::string WindowManagerDebugCommand::GetCommandErrorMsg() const
{
    std::string commandErrorMsg = "wmdebug: " + cmd_ + " is not a valid command. See wmdebug help\n";
    return commandErrorMsg;
}

std::string WindowManagerDebugCommand::GetUnknownOptionMsg() const
{
    std::string result = "";

    if (optind < 0 || optind > argc_) {
        return result;
    }

    result.append("error: unknown option");
    result.append(".\n");

    return result;
}

} // namespace Rosen
} // namespace OHOS
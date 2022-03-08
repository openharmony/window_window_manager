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

#ifndef WINDO_MANAGER_DEBUG_COMMAND_H
#define WINDO_MANAGER_DEBUG_COMMAND_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "window_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string HELP_MSG = "usage: wmdebug <command> [<options>]\n"
                             "These are wmdebug commands list:\n"
                             "  help                         list available commands\n"
                             "  dump                         dump the info of window manager\n";

const std::string HELP_MSG_DUMP = "usage: wmdebug dump [<options>]\n"
                                  "options list:\n"
                                  "  -h, --help                   list available commands\n"
                                  "  -a, --all                    dump the all window manager history infos\n"
                                  "  -t, --tree                   dump current window tree\n"
                                  "  -c, --clear                  clear all history info of window manager";

const std::string HELP_MSG_NO_COMMAND = "error: you must specify an command name with 'help' or 'dump'.\n";
const std::string HELP_MSG_NO_DUMP_OPTION = "error: you must specify an option with '-a' or '-h' or '-t' or '-c'.\n";

const std::string CLEAR_DUMP_INFO_OK_MSG = "wmdebug: clear history of window manager successfully.\n";
const std::string CLEAR_DUMP_INFO_FAIL_MSG = "error: failed to clear history of window manager.\n";

const std::string DUMP_ALL_OK_MSG = "wmdebug: dump all history info of window manager successfully.\n";
const std::string DUMP_ALL_FAIL_MSG = "error: failed to dump all history info of window manager.\n";

const std::string DUMP_TREE_OK_MSG = "wmdebug: dump current window tree successfully.\n";
const std::string DUMP_TREE_FAIL_MSG = "error: failed to current window tree.\n";
}  // namespace

class WindowManagerDebugCommand {
public:
    WindowManagerDebugCommand(int argc, char *argv[]);
    ~WindowManagerDebugCommand() = default;
    std::string RunCommand();

private:
    void CreateCommandMap();
    WMError Init();

    WMError RunHelpCommand();
    WMError RunDumpCommand();
    void DumpResults(WindowDumpType dumpType);
    std::string GetCommandErrorMsg() const;
    std::string GetUnknownOptionMsg() const;
private:
    int argc_;
    char **argv_;
    std::string cmd_;
    std::vector<std::string> args_;
    std::string result_ = "";
    std::map<std::string, std::function<WMError()>> commandMap_;
};
}  // namespace Rosen
}  // namespace OHOS

#endif  // WINDO_MANAGER_DEBUG_COMMAND_H

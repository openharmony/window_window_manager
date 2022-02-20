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

#include "window_manager_record.h"

#include <ios>
#include <iomanip>
#include <sstream>


namespace OHOS {
namespace Rosen {
namespace {
    constexpr int MAX_WINDOW_RECORD_SIZE = 50;
    constexpr int DATA_LENGTH = 10;
    constexpr int TIME_LENGTH = 64;
    const std::vector<std::string> RECORD_NAMES = {"Name", "Id", "Type", "Mode", "Flag", "Zorder", "X", "Y", "W", "H"};
}

std::string WindowActivityRecorder::GetCurrentWindowTree()
{
    return curWindowTree_;
}

void WindowActivityRecorder::Clear()
{
    records_.clear();
}

void WindowActivityRecorder::AddNodeRecord(WindowManagerRecordInfo record)
{
    std::string historyRecord;
    int32_t number = records_.size();
    historyRecord += "\nNo." + std::to_string(number) + "\t[ACTIVITY]\n";
    char systime[TIME_LENGTH];
    strftime(systime, sizeof(char) * TIME_LENGTH, "%Y/%m/%d %H:%M:%S %p", &record.recordTime);
    std::string recordTime = "Time: " + std::string(systime) + "\n";
    historyRecord += recordTime;
    historyRecord += "\t";
    switch (record.reason) {
        case RecordReason::ADD_WINDOW: {
            historyRecord += "[+]:";
            break;
        }
        case RecordReason::REMOVE_WINDOW: {
            historyRecord += "[-]:";
            break;
        }
        case RecordReason::UPDATE_WINDOW: {
            historyRecord += "[U]:";
            break;
        }
        case RecordReason::LAYOUT_WINDOW: {
            historyRecord += "[L]:";
            break;
        }
        default:
            historyRecord += "[D]:";
            break;
    }
    Rect rect = record.rect;
    historyRecord += "Name: " + record.name + "\t";
    historyRecord += "Id: " + std::to_string(record.id) + "\t";
    historyRecord += "Yype: " + std::to_string(static_cast<int32_t>(record.type)) + "\t";
    historyRecord += "Mode: " + std::to_string(static_cast<int32_t>(record.mode)) + "\t";
    historyRecord += "Flag: " + std::to_string(record.flag) + "\t";
    historyRecord += "Location:[" + std::to_string(rect.posX_) + ", " + std::to_string(rect.posY_) +
                     ", " + std::to_string(rect.width_) + ", " + std::to_string(rect.height_) + "]\n";
    if (records_.size() == MAX_WINDOW_RECORD_SIZE) {
        records_.erase(records_.begin());
    }
    records_.emplace_back(historyRecord);
}

std::string WindowActivityRecorder::GetFormatString(std::string srcStr)
{
    if (srcStr.size() >= DATA_LENGTH) {
        srcStr = srcStr.substr(0, DATA_LENGTH - 1);
    }
    std::ostringstream oss;
    oss << std::left << std::setw(DATA_LENGTH) << srcStr;
    return oss.str();
}

void WindowActivityRecorder::AddTreeRecord(const std::vector<sptr<WindowNode>> &windowNodes)
{
    int32_t treeSize = static_cast<int32_t>(windowNodes.size());
    if (treeSize == 0) {
        return;
    }

    std::string treeInfo;
    std::string nodeInfo;
    std::ostringstream item;
    std::string horizontalLines(RECORD_NAMES.size()* DATA_LENGTH + 2, '-');

    int32_t number = records_.size();
    treeInfo += "\nNo." + std::to_string(number) + "\t[TREE]\n";

    treeInfo += (horizontalLines + "\n");
    treeInfo += "|";
    for (auto colName : RECORD_NAMES) {
        item << std::left << std::setw(DATA_LENGTH) << std::setfill(' ') << colName;
    }
    treeInfo += item.str();
    treeInfo += "|\n";

    treeInfo += (horizontalLines + "\n");

    for (auto node : windowNodes) {
        nodeInfo = "|";
        Rect rect = node->GetLayoutRect();
        nodeInfo += GetFormatString(node->GetWindowName());
        nodeInfo += GetFormatString(std::to_string(node->GetWindowId()));
        nodeInfo += GetFormatString(std::to_string(static_cast<int32_t>(node->GetWindowType())));
        nodeInfo += GetFormatString(std::to_string(static_cast<int32_t>(node->GetWindowMode())));
        nodeInfo += GetFormatString(std::to_string(node->GetWindowFlags()));
        nodeInfo += GetFormatString(std::to_string(--treeSize));
        nodeInfo += GetFormatString(std::to_string(rect.posX_));
        nodeInfo += GetFormatString(std::to_string(rect.posY_));
        nodeInfo += GetFormatString(std::to_string(rect.width_));
        nodeInfo += GetFormatString(std::to_string(rect.height_));
        nodeInfo += "|\n";
        treeInfo += nodeInfo;
    }
    treeInfo += (horizontalLines + "\n");
    curWindowTree_ = treeInfo;
    if (records_.size() == MAX_WINDOW_RECORD_SIZE) {
        records_.erase(records_.begin());
    }
    records_.emplace_back(treeInfo);
}

void WindowActivityRecorder::DumpWindowRecord(std::vector<std::string>& records)
{
    records = records_;
}

}
}
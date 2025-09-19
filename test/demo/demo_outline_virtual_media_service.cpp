/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <iostream>
#include <limits>
#include <refbase.h>
#include <sstream>
#include <string>
#include "common_test_utils.h"
#include "ipc_skeleton.h"
#include "window_manager.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace OHOS {
namespace Rosen {
class DemoOutlineVirtualMediaService {
public:
    void GetWindowIds()
    {
        windowInfos_.clear();
        WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfos_);
        std::cout << "Window infos length :" << windowInfos_.size() << std::endl;
        std::cout << "windowId -- windowType -- windowName" << std::endl;
        for (const auto& info : windowInfos_) {
            std::cout << "  " << info->wid_;
            std::cout << " -- " << static_cast<int32_t>(info->type_);
            std::cout << " -- " << info->bundleName_;
            std::cout << std::endl;
        }
    }
    void UpdateOutlines()
    {
        OutlineParams outlineParams;
        outlineParams.type_ = OutlineType::OUTLINE_FOR_WINDOW;
        std::cout << "Please input outline color:" << std::endl;
        std::cin >> outlineParams.outlineStyleParams_.outlineColor_;
        std::cout << "Please input outline width:" << std::endl;
        std::cin >> outlineParams.outlineStyleParams_.outlineWidth_;
        std::cout << "Please input outline shape:" << std::endl;
        uint32_t shape = 0;
        std::cin >> shape;
        outlineParams.outlineStyleParams_.outlineShape_ = static_cast<OutlineShape>(shape);
        std::cout << "Please input window id list:" << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        int32_t id;
        outlineParams.persistentIds_.clear();
        while (iss >> id) {
            outlineParams.persistentIds_.push_back(id);
        }
        if (remoteObject_ == nullptr) {
            std::cout << "Remote object is nullptr, pid" << IPCSkeleton::GetCallingPid() << std::endl;
            return;
        }
        std::cout << "Update outlines: curr pid: " << IPCSkeleton::GetCallingPid() << std::endl;
        std::cout << "Update outlines: the outline params: " << outlineParams.ToString().c_str() << std::endl;
        WMError ret = WindowManager::GetInstance().UpdateOutline(remoteObject_, outlineParams);
        if (ret != WMError::WM_OK) {
            std::cout << "Update outlines failed, ret:" <<
                         std::to_string(static_cast<int32_t>(ret)).c_str() << std::endl;
            return;
        }
        std::cout << "Update outlines succeed." << std::endl;
    }

    void ClearOutlines()
    {
        OutlineParams outlineParams;
        outlineParams.type_ = OutlineType::OUTLINE_FOR_WINDOW;
        std::cout << "Clear outlines: curr pid: " << IPCSkeleton::GetCallingPid() << std::endl;
        std::cout << "Clear outlines: the outline params: " << outlineParams.ToString().c_str() << std::endl;
        WMError ret = WindowManager::GetInstance().UpdateOutline(remoteObject_, outlineParams);
        if (ret != WMError::WM_OK) {
            std::cout << "Clear outlines failed,ret:" << std::to_string(static_cast<int32_t>(ret)).c_str() << std::endl;
            return;
        }
        std::cout << "Clear outlines succeed." << std::endl;
    }
private:
    std::vector<sptr<AccessibilityWindowInfo>> windowInfos_;
    sptr<IRemoteObject> remoteObject_ = IPCSkeleton::GetContextObject();
};
}
}

int main(int argc, char* argv[])
{
    std::cout << "===============Demo outline virtual media service start===============" << std::endl;
    std::cout << "Please input: " << std::endl;
    std::cout << "    1: Get the windows that can update outline;" << std::endl;
    std::cout << "    2: Update outline for windows;" << std::endl;
    std::cout << "    3: Clear all outline for windows;" << std::endl;
    std::cout << "    others: Exit this demo;" << std::endl;
    CommonTestUtils::GuaranteeFloatWindowPermission("demo_outline_virtual_media_service");

    uint32_t inputFlag = 0;
    std::unique_ptr<DemoOutlineVirtualMediaService> demo = std::make_unique<DemoOutlineVirtualMediaService>();
    while (true) {
        std::cout << "Please input flag: " << std::endl;
        std::cin >> inputFlag;
        bool needExit = false;
        switch (inputFlag) {
            case 1: { // 1: Get the windows that can update outline;
                demo->GetWindowIds();
                break;
            }
            case 2: { // 2: Update outline for windows;
                demo->UpdateOutlines();
                break;
            }
            case 3: { // 3: Clear all outline for windows;
                demo->ClearOutlines();
                break;
            }
            default: {
                needExit = true;
            }
        }
        if (needExit) {
            break;
        }
    };
    std::cout << "===============Demo outline virtual media service end===============" << std::endl;
    return 0;
}
